/**************************************************************************************
 *        ____                   _    _                       _
 *       / __ \                 | |  | |                     | |
 *      | |  | |_ __   ___ _ __ | |__| | ___  _ __ _ __   ___| |_
 *      | |  | | '_ \ / _ \ '_ \|  __  |/ _ \| '__| '_ \ / _ \ __|
 *      | |__| | |_) |  __/ | | | |  | | (_) | |  | | | |  __/ |_
 *       \____/| .__/ \___|_| |_|_|  |_|\___/|_|  |_| |_|\___|\__|
 *             | |
 *             |_|
 *   ----------------------------------------------------------------------------------
 *   Copyright 2016-2024 OpenHornet
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *   ----------------------------------------------------------------------------------
 *   Note: All other portions of OpenHornet not within the 'OpenHornet-Software' 
 *   GitHub repository is released under the Creative Commons Attribution -
 *   Non-Commercial - Share Alike License. (CC BY-NC-SA 4.0)
 *   ----------------------------------------------------------------------------------
 *   This Project uses Doxygen as a documentation generator.
 *   Please use Doxygen capable comments.
 **************************************************************************************/

/**
 * @file 1A8-UP_FRONT_CONTROL_PANEL.ino
 * @author Sandra Carroll, sandra
 * @date 11.03.2024
 * @version 0.0.1a
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the Up-Front Control Panel
 *
 * @details
 * 
 *  * **Reference Designator:** 1A8
 *  * **Intended Board:** UFC_Main PCB1
 *  * **RS485 Bus Address:** N/A
 * 
 * *##Wiring diagram:
 * PIN | Function
 * --- | ---
 * A0   | AOA Indexer
 * A1   | HUD Balance
 * A2   | HUD Black Level
 * A3   | HUD Brightness
 * 6    | HUD Symbology REJ2
 * 7    | Video Control W/B
 * 8    | ALT BARO 
*  9    | ATT STBY
*  10   | ATT INS
*  14   | Day Switch
*  15   | HUD Symbology NORM
*  16   | Video Control OFF
*
* @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
* It also sets the address of this slave device. The slave address should be
* between 1 and 126 and must be unique among all devices on the same bus.
*
* @bug Currently does not work with the Pro Micro (32U4), Fails to compile. 
*
*  #define DCSBIOS_RS485_SLAVE 1 ///DCSBios RS485 Bus Address, once bug resolved move line below comment.
*/

/**
 * Check if we're on a Mega328 or Mega2560 and define the correct
 * serial interface
 * 
 */
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
#define DCSBIOS_IRQ_SERIAL ///< This enables interrupt-driven serial communication for DCS-BIOS. (Only used with the ATmega328P or ATmega2560 microcontrollers.)
#else
#define DCSBIOS_DEFAULT_SERIAL ///< This enables the default serial communication for DCS-BIOS. (Used with all other microcontrollers than the ATmega328P or ATmega2560.)  
#endif

#ifdef __AVR__
#include <avr/power.h>
#endif

/**
 * The Arduino pin that is connected to the
 * RE and DE pins on the RS-485 transceiver.
*/
#define TXENABLE_PIN 5 ///< Sets TXENABLE_PIN to Arduino Pin 5
#define UART1_SELECT ///< Selects UART1 on Arduino for serial communication

/**
 * @todo
 *  1.  clean up code WIP
 *  2.  fix slow downs in execution WIP
 *  3.  implement RTOS Scheduling
 *  4.  Clean up Opt code
 *  5.  Clean up Cue Code
 *  7.  Clean up Scratchpad Code
 */

//#define USE_ADS1115

/**
 * @brief Includes
 */
#include <Adafruit_TCA8418.h>           //Key Matrix driver
#include <Adafruit_TCA8418_registers.h> //Key Matrix driver
#include <TM1640Anode.h>                //TM1640 for OP Display
#include <TM1640.h>                     //TM1640 for COMM Display
#include <TM16xxMatrix.h>               //TM1640 matrix for COMM Display
#include <TM16xxMatrix16.h>             //TM1640 matrix for COMM Displa
#include "HT1621_OH.h"                  //modified HT1621 lib used for scratchpad
#include <PCA95x5.h>                    //PCA9555 AP Keyboard
#include "CHARSET.h"                    //Custom charset
#include <FastLED.h>                    //WS2812B Driver
#if defined(USE_ADS1115)
  #include <Adafruit_ADS1X15.h>           //ADDS1115 Driver / ADC 16bit 4 channel
#endif

//-------------- DCS BIOS ---------------
#define DCSBIOS_DISABLE_SERVO
//#define DCSBIOS_DEFAULT_SERIAL
#include "DcsBios.h"

// ------------- PIN Assignments
#define SDApin          33
#define SCLpin          35

#define IRQPIN          15

#define NOGOLED         13
#define GOLED           14

#define BL_DATA_PIN     9

#define ENC1PB          40
#define ENC1A           38
#define ENC1B           36
#define ENC2PB          34
#define ENC2A           21
#define ENC2B           17

#define DIO_3_3V        7
#define OP1_CLK_3_3V    4
#define OP2_CLK_3_3V    5
#define OP3_CLK_3_3V    6
#define OP4_CLK_3_3V    8
#define OP5_CLK_3_3V    18
#define CUE_CLK_3_3V    37
#define COM1_CLK_3_3V   39
#define COM2_CLK_3_3V   10

#define VCC_SNS_3_3V    16

#define OP_DIGITS       4
#define COM1_DIGITS     1
#define COM2_DIGITS     1


/**
 * TCA8418 GPIO PINS
 */
#define UFC_IPSW      5
#define UFC_BITSW     12
#define UFC_EMCON     17
#define UFC_GO        14
#define UFC_NOGO      13
#define UFC_ADF1      15
#define UFC_ADF2      16


#define EXT5V true
#define USB5V false

bool DEBUG = false;

#define DCSRUNNING true
#define DSCSTOPPED false

//-------------- UFC Brightness ---------------
int UCF_brightness = 7;
bool UFC_active = true;

/**
 *  @brief DCS COMMON DATA
 */
typedef struct 
{
u_int8_t    MissionStart  =  DSCSTOPPED;
char        AircraftName[24];
u_int8_t    UpdateCounter = 0;
u_int16_t   UpdateSkipCounter = 0;
u_int16_t   AltMslFtChange = 0;
} DCSCommondata;


/* create references to Structures */
DCSCommondata DCSMetaData;

//-------------- I2C/Serial Devices ---------------

Adafruit_TCA8418 UFCkeypad;
PCA9555 APkeypad;
#if defined(USE_ADS1115)
  Adafruit_ADS1115 ads1115;
#endif 
HT1621_OH scratchpad;     

volatile bool TCA8418_event = false;
uint8_t apkeys[1][10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/**
 * backlighting
 */

#define UFC_BL_LEDS 117
#define AP_BL_LEDS (4*7)
#define BACKLIGHT_LEDS UFC_BL_LEDS + AP_BL_LEDS

// Define the array of leds
CRGB ws2812b[BACKLIGHT_LEDS];

// ----------- COMM Displays

#define COM_DIGITS 4

// ----------- Cue Symbols
enum { CUE1, CUE2, CUE3, CUE4, CUE5 };

//-------------- Option Displays ---------------
uint8_t op_order[4] = {3,0,1,2};

enum { OP1, OP2, OP3, OP4, OP5 };

TM1640Anode GN1640[6] {
  TM1640Anode(DIO_3_3V, OP1_CLK_3_3V, OP_DIGITS, true, UCF_brightness), //OP Display 1; data, clock, digit count, active, brightness
  TM1640Anode(DIO_3_3V, OP2_CLK_3_3V, OP_DIGITS, true, UCF_brightness), //OP Display 2; data, clock, digit count, active, brightness
  TM1640Anode(DIO_3_3V, OP3_CLK_3_3V, OP_DIGITS, true, UCF_brightness), //OP Display 3; data, clock, digit count, active, brightness
  TM1640Anode(DIO_3_3V, OP4_CLK_3_3V, OP_DIGITS, true, UCF_brightness), //OP Display 4; data, clock, digit count, active, brightness
  TM1640Anode(DIO_3_3V, OP5_CLK_3_3V, OP_DIGITS, true, UCF_brightness), //OP Display 5; data, clock, digit count, active, brightness
  TM1640Anode(DIO_3_3V, CUE_CLK_3_3V, COM_DIGITS, true, UCF_brightness) //CUEs		 ; data, clock, digit count, active, brightness
};

struct op_display_content {
  bool cueing = false;
  char content[5] = { ' ', ' ', ' ', ' ', ' ' };
};

op_display_content op_display[5];

/**
 * @brief
 */
TM1640Anode COMM1(DIO_3_3V, COM1_CLK_3_3V, COM1_DIGITS, true, UCF_brightness); // DA=4, CLK=7
TM1640Anode COMM2(DIO_3_3V, COM2_CLK_3_3V, COM2_DIGITS, true, UCF_brightness); // DA=4, CLK=7

//-------------- COMM Displays ---------------
/**
 * @breif 
 */
TM1640 GN1640_COMM[2] {
  TM1640(DIO_3_3V, COM1_CLK_3_3V),
  TM1640(DIO_3_3V, COM2_CLK_3_3V)
};

/**
 * @breif 
 */
TM16xxMatrix matrix_COMM[2] {
  TM16xxMatrix( & GN1640_COMM[0], 4, 4),
    TM16xxMatrix( & GN1640_COMM[1], 4, 4)
};

TM16xxMatrix matrix_COMM1( & GN1640_COMM[0], 4, 4);
TM16xxMatrix16 matrix_COMM2( & GN1640_COMM[1], 8, 16);

//-------------- COMM Displays ---------------
static char COMM2_CHAR[2];


//------------ CUE SYMBOL --------------------
TM1640Anode CUE123(DIO_3_3V, CUE_CLK_3_3V, 8, true, UCF_brightness);

/**
 * start of subroutines
 */

//-------------- ISR ---------------
void IRAM_ATTR TCA8418_irq() {
  TCA8418_event = true;
}

/**
 * 
 */
void debugMsg(char *msg) {
Serial.printf(msg);
}


void processUFCIO() {
uint8_t adf1, adf2, adfsw;
static uint8_t lastadf;

  if(DEBUG) UFCkeypad.digitalWrite(UFC_NOGO, HIGH);

  UFCkeypad.digitalRead(UFC_IPSW) ? UFCkeys[0][0] = 1 : UFCkeys[0][0] = 0; //UFC_IP
  UFCkeypad.digitalRead(UFC_EMCON) ? UFCkeys[0][1] = 1 : UFCkeys[0][1] = 0; //UFC_EMCON

  UFCkeypad.digitalRead(UFC_BITSW) ? HUDkeys[0][0] = 1 : HUDkeys[0][0] = 0; //HUD_VIDEO_BIT

  adf1 = UFCkeypad.digitalRead(UFC_ADF1);
  adf2 = UFCkeypad.digitalRead(UFC_ADF2);

  adfsw = (adf1 << 2) + (adf2 << 1);
  if (!(lastadf == adfsw)) {
    switch (adfsw) {
      case 2:
        DcsBios::tryToSendDcsBiosMessage("UFC_ADF", "2");
        break;
      case 4:
        DcsBios::tryToSendDcsBiosMessage("UFC_ADF", "0");
        break;
      case 6:
        DcsBios::tryToSendDcsBiosMessage("UFC_ADF", "1");
        break;
    }
  }
  lastadf = adfsw;
  if(DEBUG) UFCkeypad.digitalWrite(UFC_NOGO, LOW);
}


void processUFCKeypad() {
  int row;
  int col;
  bool state;
  int keyCode;
  int intStat;

  if(DEBUG) UFCkeypad.digitalWrite(UFC_GO, HIGH); 

    intStat = UFCkeypad.readRegister(TCA8418_REG_INT_STAT);
    if (intStat & 0x02) {
      //read GPIO reg to clear IRQ flag
      UFCkeypad.readRegister(TCA8418_REG_GPIO_INT_STAT_1);
      UFCkeypad.readRegister(TCA8418_REG_GPIO_INT_STAT_2);
      UFCkeypad.readRegister(TCA8418_REG_GPIO_INT_STAT_3);
      //  clear GPIO IRQ flag
      UFCkeypad.writeRegister(TCA8418_REG_INT_STAT, 2);
    }

    if (intStat & 0x01) {
      keyCode = UFCkeypad.getEvent();

      if (keyCode > 127) { // check if press/release
        state = true;
      } else {
        state = false;
      }
      //Serial.printf("RAW KEYCODE1(0x%04x) : State1(0x%02x)\n",keyCode,state);

      keyCode &= 0x7F; // remove press/release from keycode
      //Serial.printf("RAW KEYCODE2(0x%04x) : State1(0x%02x)\n",keyCode,state);

      //  process  matrix
      keyCode--;
      row = keyCode / 10;
      col = keyCode % 10;
      // check if pressed or release then update UFCkeys with correct state.
      switch (state) {
        case true: //pressed 
          UFCkeys[row][col] = 1;
          break;
        case false: //released
          UFCkeys[row][col] = 0;
          break;
        }

      //  clear the EVENT IRQ flag
      UFCkeypad.writeRegister(TCA8418_REG_INT_STAT, 1);
    }
  if(DEBUG) UFCkeypad.digitalWrite(UFC_GO, LOW);  
}

/**
 * @breif process the ap keyboard
 */
void processAPKeypad() {
  APkeypad.read(PCA95x5::Port::P13) ? apkeys[0][0] = true : apkeys[0][0] = false;
  APkeypad.read(PCA95x5::Port::P14) ? apkeys[0][1] = true : apkeys[0][1] = false;
  APkeypad.read(PCA95x5::Port::P15) ? apkeys[0][2] = true : apkeys[0][2] = false;
  APkeypad.read(PCA95x5::Port::P16) ? apkeys[0][3] = true : apkeys[0][3] = false;
  APkeypad.read(PCA95x5::Port::P03) ? apkeys[0][4] = true : apkeys[0][4] = false;
  APkeypad.read(PCA95x5::Port::P04) ? apkeys[0][5] = true : apkeys[0][5] = false;
  APkeypad.read(PCA95x5::Port::P05) ? apkeys[0][6] = true : apkeys[0][6] = false;
}

/**
 * @breif 
 */
void sendAsciiChar_OH(int op_display, byte pos, char c) {
  uint16_t uSegments = pgm_read_word(OP_DISPLAY_FONT + (c - 32));
  GN1640[op_display].setSegments16(uSegments, op_order[pos]);
}

/**
 * @breif 
 */
void update_op_display(int disp_num) {
  int disp_pos = 0;

  for (int digit_num = 0; digit_num < OP_DIGITS; digit_num++) {
    sendAsciiChar_OH(disp_num, digit_num, op_display[disp_num].content[digit_num]);
  }
}


/**
 * @breif 
 */
void display_comm_char(char * comm_char, int n_display) {
  int array_pos;
  //Serial.printf("display_comm_char(%s,%d)\n", comm_char, n_display, n_display);
  switch (comm_char[1]) {
  case '1':
    array_pos = 1;
    break; // 1
  case '2':
    array_pos = 2;
    break; // 2
  case '3':
    array_pos = 3;
    break; // 3
  case '4':
    array_pos = 4;
    break; // 4
  case '5':
    array_pos = 5;
    break; // 5
  case '6':
    array_pos = 6;
    break; // 6
  case '7':
    array_pos = 7;
    break; // 7
  case '8':
    array_pos = 8;
    break; // 8
  case '9':
    array_pos = 9;
    break; // 9
  case 'M':
    array_pos = 21;
    break; // M
  case 'G':
    array_pos = 22;
    break; // G
  case 'C':
    array_pos = 23;
    break; // C
  case 'S':
    array_pos = 24;
    break; // S   {0x02, 0x0B, 0x02, 0x0B}, //S
  case ' ':
    array_pos = 0;
    break; // BLANK
  }

  if (strcmp(comm_char, "10") == 0) array_pos = 10; // 10
  if (strcmp(comm_char, "11") == 0) array_pos = 11; // 11   
  if (strcmp(comm_char, "12") == 0) array_pos = 12; // 12
  if (strcmp(comm_char, "13") == 0) array_pos = 13; // 13
  if (strcmp(comm_char, "14") == 0) array_pos = 14; // 14
  if (strcmp(comm_char, "15") == 0) array_pos = 15; // 15
  if (strcmp(comm_char, "16") == 0) array_pos = 16; // 16
  if (strcmp(comm_char, "17") == 0) array_pos = 17; // 17
  if (strcmp(comm_char, "18") == 0) array_pos = 18; // 18
  if (strcmp(comm_char, "19") == 0) array_pos = 19; // 19  
  if (strcmp(comm_char, "20") == 0) array_pos = 20; // 20
  if (strcmp(comm_char, "<>") == 0) array_pos = 25; // COMM ACTIVE
  if (strcmp(comm_char, "==") == 0) array_pos = 27; // MIDS ACTIVE
  if (strcmp(comm_char, "++") == 0) array_pos = 28; // BOTH ACTIVE

  //matrix_COMM[n_display].setAll(false);
  uint8_t segment;
  for (int i = 0; i < 4; i++) {
    if (n_display == 1) {
      matrix_COMM[n_display].setColumn(i, (uint8_t) pgm_read_byte( & (COMM_CHAR2[array_pos][i])));
    } else {
      matrix_COMM[n_display].setColumn(i, (uint8_t) pgm_read_byte( & (COMM_CHAR1[array_pos][i])));
    }
  }

  strcpy(COMM2_CHAR, comm_char);
}

/**
 * @breif 
 */
void cue(uint8_t pos, bool state) {
  switch (pos) {
  case CUE1: // CUE 1
    if (state == true) {
      CUE123.setSegments16(0x0001, 0b0100); // POS/SEG
    } else {
      CUE123.setSegments16(0x0000, 0b0100);
    }
    break;
  case CUE2: // CUE 2
    if (state == true) {
      CUE123.setSegments16(0x0002, 0b0011);
    } else {
      CUE123.setSegments16(0x0000, 0b0011);
    }
    break;
  case CUE3: // CUE 3   
    if (state == true) {
      CUE123.setSegments16(0x0004, 0b0010);
    } else {
      CUE123.setSegments16(0x0000, 0b0010);
    }
    break;
  case CUE4: // CUE 4
    if (state == true) {
      COMM2.setSegments16(0x4000, 0b00000111); // POS/SEG
      display_comm_char(COMM2_CHAR, 1);
    } else {
      COMM2.setSegments16(0x0000, 0b00000111); // POS/SEG
      display_comm_char(COMM2_CHAR, 1);
    }
    break;
  case CUE5: // CUE 5
    if (state == true) {
      COMM2.setSegments16(0x8000, 0b00000110); // POS/SEG
      display_comm_char(COMM2_CHAR, 1);
    } else {
      COMM2.setSegments16(0x0000, 0b00000110); // POS/SEG
      display_comm_char(COMM2_CHAR, 1);
    }
    break;
  default:
    break;
  }
}

/**
 * @breif 
 */
void update_op_cue(int disp_num) {
  switch (disp_num) {
  case 1: //OP Display 1 CUE			
    op_display[disp_num].cueing ? GN1640[6].setSegments16(0x0001, 0b0100) : GN1640[6].setSegments16(0x0000, 0b0100); //0x10 -> SEG5 active, 0 -> GRID 1
    break;
  case 2: //OP Display 2 CUE
    op_display[disp_num].cueing ? GN1640[6].setSegments16(0x0002, 0b0011) : GN1640[6].setSegments16(0x0000, 0b0011); //0x08 -> SEG4 active, 1 -> GRID 2
    break;
  case 3: //OP Display 3 CUE
    op_display[disp_num].cueing ? GN1640[6].setSegments16(0x0004, 0b0010) : GN1640[6].setSegments16(0x0004, 0b0010); //0x04 -> SEG3 active, 2 -> GRID 3
    break;
  case 4: //OP Display 4 CUE
    op_display[disp_num].cueing ? matrix_COMM2.setColumn(14, 0x80, true) : matrix_COMM2.setColumn(14, 0, true); //0x80 -> SEG8 active, 14 -> GRID 15
    //matrix_COMM2.setColumn(14, 0x80, true) : matrix_COMM2.setColumn(14,0,true); //0x80 -> SEG8 active, 14 -> GRID 15
    break;
  case 5: //OP Display 5 CUE
    op_display[disp_num].cueing ? matrix_COMM2.setColumn(15, 0x64, true) : matrix_COMM2.setColumn(15, 0, true); //0x64 -> SEG7 active, 15 -> GRID 16
    break;
  }
}

/**
 * @BREIF DCS-BIOS CALL BACKS
 */

void UFC_OPTION_CUEING_1Change(char * newValue) {
  if ( * newValue == ':') {
    cue(CUE1, true);
  } else {
    cue(CUE1, false);
  }
}
DcsBios::StringBuffer < 1 > UFC_OPTION_CUEING_1(FA_18C_hornet_UFC_OPTION_CUEING_1_A, UFC_OPTION_CUEING_1Change);

void UFC_OPTION_CUEING_2Change(char * newValue) {
  if ( * newValue == ':') {
    cue(CUE2, true);
  } else {
    cue(CUE2, false);
  }
}
DcsBios::StringBuffer < 1 > UFC_OPTION_CUEING_2(FA_18C_hornet_UFC_OPTION_CUEING_2_A, UFC_OPTION_CUEING_2Change);

void UFC_OPTION_CUEING_3Change(char * newValue) {
  if ( * newValue == ':') {
    cue(CUE3, true);
  } else {
    cue(CUE3, false);
  }
}
DcsBios::StringBuffer < 1 > UFC_OPTION_CUEING_3(FA_18C_hornet_UFC_OPTION_CUEING_3_A, UFC_OPTION_CUEING_3Change);

void UFC_OPTION_CUEING_4Change(char * newValue) {
  if ( * newValue == ':') {
    //op_display[CUE4].cueing = true;
    cue(CUE4, true);
  } else {
    //op_display[CUE4].cueing = false;
    cue(CUE4, false);
  }
  //update_op_cue(CUE4);
}
DcsBios::StringBuffer < 1 > UFC_OPTION_CUEING_4(FA_18C_hornet_UFC_OPTION_CUEING_4_A, UFC_OPTION_CUEING_4Change);

void UFC_OPTION_CUEING_5Change(char * newValue) {
  if ( * newValue == ':') {
    //op_display[CUE5].cueing = true;
    cue(CUE5, true);
  } else {
    //op_display[CUE5].cueing = false;
    cue(CUE5, false);
  }
  //update_op_cue(CUE5);
}
DcsBios::StringBuffer < 1 > UFC_OPTION_CUEING_5(FA_18C_hornet_UFC_OPTION_CUEING_5_A, UFC_OPTION_CUEING_5Change);

//-------------- UFC OP Displays ---------------

void UFC_OPTION_DISPLAY_1Change(char * newValue) {
  for (int i = 0; i < 4; i++ ){
    op_display[OP1].content[i] = newValue[i];  
  }
  update_op_display(OP1);
}
DcsBios::StringBuffer < 4 > UFC_OPTION_DISPLAY_1(FA_18C_hornet_UFC_OPTION_DISPLAY_1_A, UFC_OPTION_DISPLAY_1Change);

void UFC_OPTION_DISPLAY_2Change(char * newValue) {
  for (int i = 0; i < 4; i++ ){
    op_display[OP2].content[i] = newValue[i];  
  }
  update_op_display(OP2);
}
DcsBios::StringBuffer < 4 > UFC_OPTION_DISPLAY_2(FA_18C_hornet_UFC_OPTION_DISPLAY_2_A, UFC_OPTION_DISPLAY_2Change);

void UFC_OPTION_DISPLAY_3Change(char * newValue) {
  for (int i = 0; i < 4; i++) {
    op_display[OP3].content[i] = newValue[i];
  }
  update_op_display(OP3);
}
DcsBios::StringBuffer < 4 > UFC_OPTION_DISPLAY_3(FA_18C_hornet_UFC_OPTION_DISPLAY_3_A, UFC_OPTION_DISPLAY_3Change);

void UFC_OPTION_DISPLAY_4Change(char * newValue) {
  for (int i = 0; i < 4; i++) {
    op_display[OP4].content[i] = newValue[i];
  }
  update_op_display(OP4);
}
DcsBios::StringBuffer < 4 > UFC_OPTION_DISPLAY_4(FA_18C_hornet_UFC_OPTION_DISPLAY_4_A, UFC_OPTION_DISPLAY_4Change);

void UFC_OPTION_DISPLAY_5Change(char * newValue) {
  for (int i = 0; i < 4; i++) {
    op_display[OP5].content[i] = newValue[i];
  }
  update_op_display(OP5);
}
DcsBios::StringBuffer < 4 > UFC_OPTION_DISPLAY_5(FA_18C_hornet_UFC_OPTION_DISPLAY_5_A, UFC_OPTION_DISPLAY_5Change);

//-------------- UFC COMM Display ---------------
void UFC_COMM_DISPLAY_1Change(char * newValue) {
  display_comm_char(newValue, 0);
}
DcsBios::StringBuffer < 2 > UFC_COMM_DISPLAY_1(FA_18C_hornet_UFC_COMM1_DISPLAY_A, UFC_COMM_DISPLAY_1Change);

void UFC_COMM_DISPLAY_2Change(char * newValue) {
  display_comm_char(newValue, 1);
}
DcsBios::StringBuffer < 2 > UFC_COMM_DISPLAY_2(FA_18C_hornet_UFC_COMM2_DISPLAY_A, UFC_COMM_DISPLAY_2Change);

//-------------- UFC Scratchpad ---------------

void onUfcScratchpadNumberDisplayChange(char * newValue) {
  scratchpad.print_num(newValue, false);
}
DcsBios::StringBuffer < 8 > UFC_SCRATCHPAD_NUMBER_DISPLAY(FA_18C_hornet_UFC_SCRATCHPAD_NUMBER_DISPLAY_A, onUfcScratchpadNumberDisplayChange);

void onUfcScratchpadString1DisplayChange(char * newValue) {
  scratchpad.print_str(newValue, false);
}
DcsBios::StringBuffer < 4 > onUfcScratchpadString1DisplayBuffer(FA_18C_hornet_UFC_SCRATCHPAD_STRING_1_DISPLAY_A, onUfcScratchpadString1DisplayChange);

void onUfcScratchpadString2DisplayChange(char * newValue) {
  /* your code here */
}
DcsBios::StringBuffer < 2 > ufcScratchpadString2DisplayBuffer(FA_18C_hornet_UFC_SCRATCHPAD_STRING_2_DISPLAY_A, onUfcScratchpadString2DisplayChange);

void UFC_BRTChange(unsigned int newValue) {
  UCF_brightness = map(newValue, 0, 65535, 0, 255);

  (UCF_brightness > 0) ? UFC_active = true : UFC_active = false;
  
  for (int k = 0; k <= 5; k++) {
    GN1640[k].setupDisplay(UFC_active, UCF_brightness);
  }
  
  for (int i = 0; i < 2; i++) {
    GN1640_COMM[i].setupDisplay(UFC_active, UCF_brightness);
  }
  
  scratchpad.set_backlight_brightness(map(newValue, 0, 65535, 0, 255));

}
DcsBios::IntegerBuffer UFC_BRT(FA_18C_hornet_UFC_BRT, UFC_BRTChange);

void onInstrIntLtChange(unsigned int newValue) {
  fill_solid(ws2812b, BACKLIGHT_LEDS, CRGB(0, 255, 0));
  FastLED.setBrightness(map(newValue, 0, 65535, 0, 255));
}
DcsBios::IntegerBuffer instrIntLtBuffer(FA_18C_hornet_INSTR_INT_LT, onInstrIntLtChange);

/**
 * @brief void onAcftNameChange(char* newValue)
 * Type: string
 * Address: 0x0000 
 * Max Length: 24
 * Description: Aircraft Name (or NONE), null-terminated
 * 
 * @param newValue 
 * Aircraft Name (or NONE), null-terminated
 */
void onAcftNameChange(char* newValue) { 
  strcpy(DCSMetaData.AircraftName,"\n");
    if (strcmp(newValue,"\0") != 0) {
      DCSMetaData.MissionStart = DCSRUNNING;      // Set flag that DCS is now running
      if (DEBUG) Serial.printf("DCS RUNNING...\n");
      strcpy(DCSMetaData.AircraftName,newValue);  // Copy Aircraft name 
    } else {
      if (DEBUG) Serial.printf("DCS NOT RUNNING...\n");
      DCSMetaData.MissionStart = DSCSTOPPED;      // Set flag that DCS has stopped running  
    }
}
DcsBios::StringBuffer<24> AcftNameBuffer(MetadataStart_ACFT_NAME_A, onAcftNameChange);

/**
 * @brief DCS-BIOS Mapping
 */

DcsBios::MatActionButtonSet ufc1("UFC_1", & UFCkeys[1][0], HIGH);
DcsBios::MatActionButtonSet ufc2("UFC_2", & UFCkeys[1][1], HIGH);
DcsBios::MatActionButtonSet ufc3("UFC_3", & UFCkeys[1][2], HIGH);

DcsBios::MatActionButtonSet ufc4("UFC_4", & UFCkeys[2][0], HIGH);
DcsBios::MatActionButtonSet ufc5("UFC_5", & UFCkeys[2][1], HIGH);
DcsBios::MatActionButtonSet ufc6("UFC_6", & UFCkeys[2][2], HIGH);

DcsBios::MatActionButtonSet ufc7("UFC_7", & UFCkeys[3][0], HIGH);
DcsBios::MatActionButtonSet ufc8("UFC_8", & UFCkeys[3][1], HIGH);
DcsBios::MatActionButtonSet ufc9("UFC_9", & UFCkeys[3][2], HIGH);
DcsBios::MatActionButtonSet ufc0("UFC_0", & UFCkeys[4][1], HIGH);

DcsBios::MatActionButtonSet ufcclr("UFC_CLR", & UFCkeys[4][0], HIGH);
DcsBios::MatActionButtonSet ufcent("UFC_ENT", & UFCkeys[4][2], HIGH);
DcsBios::MatActionButtonSet ufcip("UFC_IP", & UFCkeys[0][0], LOW);
DcsBios::MatActionButtonSet ufcencon("UFC_EMCON", & UFCkeys[0][1], LOW);

DcsBios::MatActionButtonSet ufcos1("UFC_OS1", & UFCkeys[0][3], HIGH);
DcsBios::MatActionButtonSet ufcos2("UFC_OS2", & UFCkeys[1][3], HIGH);
DcsBios::MatActionButtonSet ufcos3("UFC_OS3", & UFCkeys[2][3], HIGH);
DcsBios::MatActionButtonSet ufcos4("UFC_OS4", & UFCkeys[3][3], HIGH);
DcsBios::MatActionButtonSet ufcos5("UFC_OS5", & UFCkeys[4][3], HIGH);

DcsBios::MatActionButtonSet hud_bit("HUD_VIDEO_BIT", & HUDkeys[0][0], LOW);

typedef DcsBios::RotaryEncoderT < POLL_EVERY_TIME, DcsBios::TWO_STEPS_PER_DETENT > OHRotaryEncoder;

OHRotaryEncoder ufcComm1ChannelSelect("UFC_COMM1_CHANNEL_SELECT", "DEC", "INC", ENC1A, ENC1B);
OHRotaryEncoder ufcComm2ChannelSelect("UFC_COMM2_CHANNEL_SELECT", "DEC", "INC", ENC2A, ENC2B);
DcsBios::Switch2Pos ufcComm1Pull("UFC_COMM1_PULL", ENC1PB);
DcsBios::Switch2Pos ufcComm2Pull("UFC_COMM2_PULL", ENC2PB);

DcsBios::MatActionButtonSet UFC_AP("UFC_AP", & apkeys[0][0], HIGH);
DcsBios::MatActionButtonSet UFC_IFF("UFC_IFF", & apkeys[0][1], HIGH);
DcsBios::MatActionButtonSet UFC_TCN("UFC_TCN", & apkeys[0][2], HIGH);
DcsBios::MatActionButtonSet UFC_ILS("UFC_ILS", & apkeys[0][3], HIGH);
DcsBios::MatActionButtonSet UFC_DL("UFC_DL", & apkeys[0][4], HIGH);
DcsBios::MatActionButtonSet UFC_BCN("UFC_BCN", & apkeys[0][5], HIGH);
DcsBios::MatActionButtonSet UFC_ONOFF("UFC_ONOFF", & apkeys[0][6], HIGH);

//template <unsigned long pollIntervalMs = POLL_EVERY_TIME, unsigned int hysteresis = 128, unsigned int ewma_divisor = 5>

  /**
   * @TODO randomly crashes the controller.   
   */
#if not defined(USE_ADS1115)
  typedef DcsBios::PotentiometerEWMA < POLL_EVERY_TIME, 128, 20 > OHPotentiometer;
  OHPotentiometer ufcBrt("UFC_BRT", A2, false, 0, 8190);
  OHPotentiometer ufccom1vol("UFC_COMM1_VOL", A0, true, 0, 8190);
  OHPotentiometer ufccom2vol("UFC_COMM2_VOL", A1, false, 0, 8190);
#else
  typedef DcsBios::PotentiometerMAT < POLL_EVERY_TIME, 128, 20 > OHPotentiometer;
  OHPotentiometer ufcBrt("UFC_BRT", adc[0], false, 0, 65535);
  OHPotentiometer ufccom1vol("UFC_COMM1_VOL", adc[1], true, 0, 65535);
  OHPotentiometer ufccom2vol("UFC_COMM2_VOL", adc[2], false, 0, 65535);
#endif

/**
 * @breif Show Inital Boot Message on Optdisplay
 */
void bootMessage(bool noPause, bool noClear, bool noCue) {
  char newValue[3],scr[10];

  strcpy(newValue, " 1");
  UFC_COMM_DISPLAY_1Change(newValue);
  strcpy(newValue, " 2");
  UFC_COMM_DISPLAY_2Change(newValue);

  strcpy(op_display[OP1].content, "UFC ");
  op_display[OP1].cueing = true;
  strcpy(op_display[OP2].content, "FA18");
  op_display[OP2].cueing = true;
  strcpy(op_display[OP3].content, "OCT ");
  op_display[OP3].cueing = true;
  strcpy(op_display[OP4].content, "20  ");
  op_display[OP4].cueing = true;
  strcpy(op_display[OP5].content, "2024");
  op_display[OP5].cueing = true;

  strcpy(scr, " O H");
  scratchpad.print_str(scr, false);
  strcpy(scr, "  U610");
  scratchpad.print_num(scr, false);

  for (int display_num = 0; display_num < 5; display_num++) {
    update_op_display(display_num);
    update_op_cue(display_num);
    if (!noCue)
      cue(display_num, true);
  }

  if (!noPause)
    delay(10000);

  if (!noClear) {
    strcpy(op_display[OP1].content, "    ");
    op_display[OP1].cueing = false;
    strcpy(op_display[OP2].content, "    ");
    op_display[OP2].cueing = false;
    strcpy(op_display[OP3].content, "    ");
    op_display[OP3].cueing = false;
    strcpy(op_display[OP4].content, "    ");
    op_display[OP4].cueing = false;
    strcpy(op_display[OP5].content, "    ");
    op_display[OP5].cueing = false;

    strcpy(newValue, " ");
    display_comm_char(newValue, 0);
    strcpy(newValue, " ");
    display_comm_char(newValue, 1);
  }
  for (int display_num = 0; display_num < 5; display_num++) {
    update_op_display(display_num);
    if (!noClear || !noCue)
      cue(display_num, false);
  }
}

/**
 * 
 */
void i2cscan() {
  byte nDevices = 0;

  for (byte address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 0x10)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println();

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 0x10)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0)
    Serial.println("No I2C devices found");
  else
    Serial.println("Done");
}


/**
 * @brief  bool powerSource()
 *  return EXT5V if external power is connected
 *  return USB5V is USB power is connected.
 *  can not determine if both EXTERNAL and USB power is connected
 */
bool powerSource() {
  if (digitalRead(VCC_SNS_3_3V) == EXT5V) {
    Serial.printf("EXTERNAL POWER DETECTED\n");
    return EXT5V;
  }
  else {
    Serial.printf("VUSB POWER DETECTED\n");
    return USB5V;
  }
}

/**
 * @brief enable ADF switch to enable backlight when in DEBUG Mode.  Grean Backlight
 * ADF1 Full Brightness
 * ADF2 Very Dim
 * ADFOFF backlight off
 */
void adf2bl() {
  int adf1,adf2;

  adf1 = UFCkeypad.digitalRead(UFC_ADF1);
  adf2 = UFCkeypad.digitalRead(UFC_ADF2);


  if (!adf1) {
    fill_solid(ws2812b, BACKLIGHT_LEDS, CRGB(0, 255, 0));
    FastLED.setBrightness(255);
  }

  if (!adf2) {
    fill_solid(ws2812b, BACKLIGHT_LEDS, CRGB(0, 255, 0));
    FastLED.setBrightness(8);
  }

  if ( adf1 and adf2 ) {
    FastLED.clear();  
  }

  FastLED.show();
}

/**
 *  @brief  Sets the HUD BOX LEDS to the MIssion status
 * GO IN MISSION
 * NOGO NOT IN MISSION 
 */
void gonogo() {

  if (DCSMetaData.MissionStart == DCSRUNNING ) {
    UFCkeypad.digitalWrite(UFC_GO, HIGH); 
    UFCkeypad.digitalWrite(UFC_NOGO, LOW); 
  } else {
    UFCkeypad.digitalWrite(UFC_NOGO, HIGH);
    UFCkeypad.digitalWrite(UFC_GO, LOW);
  }
}

/**
 * @breif Arduino Main Setup
 */
void setup() {
  
  // HOLD COMM2 PUSH AT POWER UP TO PUT INTO DEBUG MODE
  pinMode(ENC2PB, INPUT_PULLUP);
  (digitalRead(ENC2PB) == LOW) ? DEBUG = true : DEBUG = false ;

  if (DEBUG) {
  Serial.begin(250000);
  while (!Serial); // wait for serial attach
  }

  // ---------- UFC Keyboard Setup
  if(DEBUG) Serial.printf("Wire.begin\n");
  Wire.begin(SDApin, SCLpin);

  if (DEBUG) i2cscan();


  if (DEBUG) Serial.printf("UFC Keyboard Setup\n");
  if (!UFCkeypad.begin(TCA8418_DEFAULT_ADDR, & Wire)) {
    Serial.println("UFCkeypad not found, check wiring & pullups!");
  } else {
    Serial.printf("UFCkeypad found at %02x\n", TCA8418_DEFAULT_ADDR);
  }

  pinMode(IRQPIN, OUTPUT);
  pinMode(VCC_SNS_3_3V,INPUT_PULLUP);

  UFCkeypad.matrix(TCA8418_ROWS, TCA8418_COLS);

  UFCkeypad.pinMode(NOGOLED, OUTPUT); // NOGO LED
  UFCkeypad.pinMode(GOLED, OUTPUT); // GO LED
  
  UFCkeypad.pinMode(UFC_IPSW, INPUT_PULLUP); //5
  UFCkeypad.pinMode(UFC_BITSW, INPUT_PULLUP); //12
  UFCkeypad.pinMode(UFC_EMCON, INPUT_PULLUP); //17
  UFCkeypad.flush();
  UFCkeypad.enableDebounce();
  
  // ----------- AP Keyboard Setup
  if (DEBUG) Serial.printf("AP Keyboard Setup\n");
  APkeypad.attach(Wire);
  if (APkeypad.i2c_error()) {
    Serial.println("APkeypad not found, check wiring & pullups!");
  } else {
    Serial.println("APkeypad found at 0x20");
  }
  APkeypad.polarity(PCA95x5::Polarity::INVERTED_ALL);
  APkeypad.direction(PCA95x5::Direction::IN_ALL);
  APkeypad.write(PCA95x5::Level::L_ALL);

  #if defined(USE_ADS1115)
  if (!ads1115.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
  #endif

  // ---------- Backlight Setup
  if (DEBUG) Serial.printf("Backlight Setup\n");
  FastLED.addLeds < WS2812B, BL_DATA_PIN, GRB > (ws2812b, BACKLIGHT_LEDS); // GRB ordering is typical
  FastLED.clear();
  fill_solid(ws2812b, BACKLIGHT_LEDS, CRGB(0, 255, 0));
  FastLED.setBrightness(128);
  FastLED.show();

  // ---------- Scratchpad Setup
  if (DEBUG) Serial.printf("scratchpad()\n");
  scratchpad.begin(14, 13, 12, 11); // (cs, wr, Data, backlight)
  scratchpad.clear(); // clear the screen

  //  --------- Analog Port Setup  
  if (DEBUG) Serial.printf("Analog Setup 13bit, 11db attenutation\n");
  analogSetAttenuation(ADC_11db);
  analogReadResolution(13);
  analogSetClockDiv(15);

  //  ---------- Shoow the boot message and keep it on display until sim connects
  if (DEBUG) Serial.printf("Show UFC Boot Message\n");
  bootMessage(true, true, true);

  //  ---------- turn off backlight to show we're done with setup
  if (DEBUG) Serial.printf("Turn Off Backlight\n");
  FastLED.clear();
  FastLED.show();

  if (DEBUG) Serial.printf("Check Power Source\n");
  powerSource();

  if (DEBUG) Serial.printf("Turn On Go/NoGo For 5 seconds\n");
  UFCkeypad.digitalWrite(UFC_GO, HIGH); 
  UFCkeypad.digitalWrite(UFC_NOGO, HIGH);
  delay(5000);

  if (DEBUG) Serial.printf("DcsBios::setup()\n");
  DcsBios::setup();

  if (DEBUG) Serial.printf("Starting Arduino Loop\n");
}

/**
 * @breif Arduino Main Loop
 */
void loop() {
  // update backlighting
  FastLED.show();
  //------------ Main DCS-BIOS LOOP -----------
  DcsBios::loop();

  //----------- Keypad Matrix ----------  
  processUFCKeypad();
  processAPKeypad();
  processUFCIO();

  #if defined(USE_ADS1115)
    processADC();
  #endif

  // hud bit leds showing if mission is running or not
  // GO = MISSION Running
  // NOGO = MISSION NOT RUNNING
  //gonogo();

  // ADF1 BRIGHT, ADF2 DIM, ADF OFF OFF if DEBUG
  if(DEBUG) adf2bl();
}