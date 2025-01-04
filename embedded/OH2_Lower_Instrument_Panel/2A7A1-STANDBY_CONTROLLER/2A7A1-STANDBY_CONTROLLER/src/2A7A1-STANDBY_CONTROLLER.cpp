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
 * @file 2A7A1-STANDBY_CONTROLLER.ino
 * @author Sandra Carroll, sandra
 * @date 11.04.2024
 * @version 0.0.1a
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the Stand by Instruments
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
 * 11   | Backlight In
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
/// \version 0.0 2024-11-14
///              Initial code creation, module layout and testing 
///
/// \author  Sandra Carroll (smgvbest@gmail.com) DO NOT CONTACT THE AUTHOR DIRECTLY: USE THE DISCORD CHANNEL
// Copyright (C) 2009-2024 Openhornet.com

/**
 * Check if we're on a Mega328 or Mega2560 and define the correct
 * serial interface
 * 
 */
// #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
// #define DCSBIOS_IRQ_SERIAL ///< This enables interrupt-driven serial communication for DCS-BIOS. (Only used with the ATmega328P or ATmega2560 microcontrollers.)
// #else
#define DCSBIOS_DEFAULT_SERIAL ///< This enables the default serial communication for DCS-BIOS. (Used with all other microcontrollers than the ATmega328P or ATmega2560.)  
// #endif

#ifdef __AVR__
  #include <avr/power.h>
#endif

/**
 * The Arduino pin that is connected to the
 * RE and DE pins on the RS-485 transceiver.
*/
// #define TXENABLE_PIN 5 ///< Sets TXENABLE_PIN to Arduino Pin 5
// #define UART1_SELECT ///< Selects UART1 on Arduino for serial communication

/*
  The following #define tells DCS-BIOS that this is a RS-485 slave device.
  It also sets the address of this slave device. The slave address should be
  between 1 and 126 and must be unique among all devices on the same bus.
*/
// #define DCSBIOS_RS485_SLAVE 5

/**
 * enable debugging messages on the serial port
 */
bool DEBUG = true;

/**
 * @brief Includes
 */
#include <Arduino.h>

//#include <EEPROM.h>                   // INCLUDE IF RUNNING ON A MEGA2650
//#include <FastLED.h>                    // WS2812B Driver
#include <Adafruit_NeoPixel.h>
#include "TCA9548.h"                    // I2C SWITCH AT ADDRESS 0X70
#include <AccelStepper.h>
#include <RotaryEncoder.h>
#include "MultiMap.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// FOR REFERENCE ONLY
#define STANDBY_3v3         10
#define STANDBY_5V0         12
#define STANDBY_GND         14    

// GLOBAL STANDBY INSTRUMENTS
#define STANDBY_SCL          21    // DEFAULT IS SCL,  THIS IS FOR REFERENCE ONLY
#define STANDBY_SDA          20    // DEFAULT IS SDA,  THIS IS FOR REFERENCE ONLY
#define STANDBY_LED_COUNT    6     // 2 WS2812 PER INSTRUMENT x 3
#define STANDBY_LED_PIN      11    // USED IF BACKLIGHTING SET TO INTERNAL (ALWAYS ACTIVE IN CODE)

//-------------- DCS BIOS ---------------
#define DCSBIOS_DISABLE_SERVO
#include "DcsBios.h"

// /**
//  *  @brief DCS COMMON DATA
//  */
#define DCSRUNNING true
#define DSCSTOPPED false

typedef struct 
{
uint8_t    MissionStart  =  DSCSTOPPED;
char       AircraftName[24];
uint8_t    UpdateCounter = 0;
uint16_t   UpdateSkipCounter = 0;
uint16_t   AltMslFtChange = 0;
} DCSCommondata;

/* create references to Structures */
DCSCommondata DCSMetaData;

/**
 * SETUP FOR STANDBY CONTROLLER
 * This is items that are not specific to a gauge
 */
// CRGB STANDBY_NEOPIXEL[STANDBY_LED_COUNT];
Adafruit_NeoPixel neoPixel(STANDBY_LED_COUNT, STANDBY_LED_PIN , NEO_GRB + NEO_KHZ800);

TCA9548 MP(0x70);
uint8_t channels = 0;

const int VID29_STEPSPERREVOLUTION = 720;  // ALL VID29'S HAVE SAME STEP COUNT TO MAKE IT A GLOBAL

unsigned int  isHomed = false;

/**
 * Type :        integer
 * Address :     0x7560
 * Mask :        0xffff
 * ShiftBy :     0
 * Max Value :   65535
 * Description : gauge position
 * 
 * Read more about Addresses.h macros from the DCS-BIOS Wiki.
 */
void onInstrIntLtChange(unsigned int newValue) 
{
  neoPixel.fill(neoPixel.Color(0,255,0),0,0);
  neoPixel.setBrightness(map(newValue,0,65535,0,255));
  neoPixel.show();
  // STANDBY_NEOPIXEL[0]=CRGB::Green;
  // FastLED.setBrightness(map(newValue,0,65535,9,255));
  // FastLED.show();
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
      if (DEBUG) Serial.println("DCS RUNNING...");
      strcpy(DCSMetaData.AircraftName,newValue);  // Copy Aircraft name 
    } else {
      if (DEBUG) Serial.println("DCS NOT RUNNING...");
      DCSMetaData.MissionStart = DSCSTOPPED;      // Set flag that DCS has stopped running  
    }
}
DcsBios::StringBuffer<24> AcftNameBuffer(MetadataStart_ACFT_NAME_A, onAcftNameChange);

/**
 * Seek the needle to the zero (home) position
 */
bool seekZero(AccelStepper  *_stepper, unsigned long _zeroPin) {
  Serial.println("Homing the stepper motor...");

  // Move towards the home switch (towards negative direction, assuming home switch is at the end)
  while (digitalRead(_zeroPin) == LOW) {    // Home switch not pressed (HIGH when not pressed)
    _stepper->setSpeed(-200);                    // Move in reverse direction at a speed of 200 steps/sec
    _stepper->runSpeed();                        // Continuously run the motor at the set speed
  }

  // When the home switch is pressed, stop the motor
  while(_stepper->isRunning()) {
    _NOP()
  };
  _stepper->stop();

  // Move a tiny bit away from the home switch to prevent constant triggering
  _stepper->setSpeed(100);                       // Move away from the switch
  _stepper->runSpeed();                          // Move slightly away from the switch
  while(_stepper->isRunning());

  // Reset the stepper motor position to zero
  _stepper->setCurrentPosition(0);               // Set the current position as the "home" position
  isHomed = true;                                        // Set the homing flag to true
  Serial.println("Homing complete. Motor is now at home position.");
  return((isHomed ? 1 : 0));
}

/**
 * SETUP FOR STANDBY INSTRUMENTS 
 *
 */

enum instrumentType{ AIRSPEED, ALTIMETER, VVI };

/**
 * @brief Setup for the standby instruments structure to read/save the current state of the instrument.
 * this is used to recall the mapping of the DCS value to the step counts as well as save the offset 
 * from the zero postion to the needle zero position
 */
typedef struct {
  uint32_t eepromStartByte;   // where in EEPROM to write the state
  uint32_t eepromSize;        // how big a buffer are we writing
  uint32_t mapIn[50];         // store up to 50 calibration points from DCS
  uint32_t mapOut[50];        // store up to 50 calibration points to step counts
  uint16_t mapSize;           // how many entrieds are actually in the map
  uint32_t zeroOffset;        // how far from the zero index postion to the needle zero position do we need to move.
} instrumentState;


void readState( int _instrumentType, instrumentState _state) {
  switch (_instrumentType)
  {
  case AIRSPEED:
    break;
  case ALTIMETER:
    break;
  case VVI:
    break;
  default:
    break;
  }
}

void saveState( int instrumentType) {
  switch (instrumentType)
  {
  case AIRSPEED:
    break;
  case ALTIMETER:
    break;
  case VVI:
    break;
  default:
    break;
  }
}


// typedef DcsBios::RotaryEncoderT<POLL_EVERY_TIME, DcsBios::TWO_STEPS_PER_DETENT> TwoStepRotaryEncoder;

//#include <STANDBY SARI.h>
//#include <STANDBY RWR.h>
//#include <STANDBY AIRSPEED.h>
//#include <STANDBY OLED.h>
#include <STANDBY ALTIMETER.h>
//#include <STANDBY VVI.h>

//bool altitudeHomed = false;

bool altitudeHomed = seekZero(&altimeterStepper, ALTIMETER_ZERO_DETECT);

/**
 * Main Setup Routine,  Called 1 time only at power on
 */
void setup() {

  //if (DEBUG) {
  Serial.begin(9600);
  while(!Serial.available()); // wait for serial attach
  //}

  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);

  Serial.println("Openhornet Standy Instruments Panel V 0.0.1...");


  neoPixel.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  neoPixel.show();            // Turn OFF all pixels ASAP
  neoPixel.setBrightness(0); // Set BRIGHTNESS to about 1/5 (max = 255)

  Serial.println("Wire.begin()");
  Wire.begin();

  Serial.println("Detecting TCA9548");
  if (MP.begin() == false) 
      Serial.println("COULD NOT CONNECT TO MULTIPLEXER");
  else {
      Serial.println("CONNECTED TO MULTIPLEXER");
      Serial.print("TCS9548 CHANNELS FOUND: ");
      Serial.println(MP.channelCount());
    }

  Serial.print("Scan the channels of the multiplexer for searchAddress: 0x");
  Serial.println(OLED_ADDR, HEX);

  // // oledCheck();

  int ch = MP.channelCount();
  Serial.print("Channel Count: ");
  Serial.println(ch);
  
  for (int chan = 0; chan < ch; chan++)
  {
    MP.selectChannel(chan);
    bool b = MP.isConnected(OLED_ADDR);
    Serial.print("Channel: ");
    Serial.print(chan);
    Serial.print(" ");
    Serial.println( b ? "found!" : "x");
  }
  Serial.println();

  altimeterSetuo();
  altimeterZero();


  // STANDBY_NEOPIXEL[0]=CRGB::Green;
  // FastLED.setBrightness(128);
  // FastLED.show();
  neoPixel.fill(neoPixel.Color(0,255,0),0,0);
  neoPixel.setBrightness(128);
  neoPixel.show();

  Serial.println("DcsBios::setup()\n");
  DcsBios::setup();
}

void loop() {
  DcsBios::loop();
  // altimeterEncoder.tick();
  altimeterLoop();

  neoPixel.show();
  // FastLED.show();
}
