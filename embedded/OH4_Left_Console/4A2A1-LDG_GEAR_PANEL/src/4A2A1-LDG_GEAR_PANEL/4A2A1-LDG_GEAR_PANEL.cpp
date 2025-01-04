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
 * @file 4A2A1-LDG_GEAR_PANEL.ino
 * @author Arribe
 * @date 2/28/2024
 * @version 0.0.3
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the LDG GEAR panel.
 *
 * @details
 * 
 *  * **Reference Designator:** 4A2A1
 *  * **Intended Board:** ABSIS ALE /w Relay Module
 *  * **RS485 Bus Address:** 1
 * 
 * ### Wiring diagram:
 * PIN | Function
 * --- | ---
 * A1  | Landing Gear Emergency Rotate and Pull
 * A2  | Landing Gear Down Lock Override Button
 * A3  | Landing Gear Warning Silence Button
 * 2   | Landing Gear Down Lock Solenoid
 * 3   | Landing Gear Limit Switch (handle raise / lower)
 * 4   | Landing Gear Lollipop LED
 *
 *
 * @brief The following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 * @bug Currently does not work with the Pro Micro (32U4), Fails to compile. 
 *
 // #define DCSBIOS_RS485_SLAVE 1
*/

/**
 * Check if we're on a Mega328 or Mega2560 and define the correct
 * serial interface
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

#include "DcsBios.h"


// Define pins for DCS-BIOS per interconnect diagram.
#define EMERGENCY_GEAR_ROTATE A1 ///< Landing Gear Emergency Rotate and Pull
#define GEAR_DOWNLOCK_OVERRIDE_BTN A2 ///< Landing Gear Down Lock Override Button
#define GEAR_SILENCE_BTN A3 ///< Landing Gear Warning Silence Button
#define LG_LEVER_SOLENOID 2 ///< Landing Gear Down Lock Solenoid
#define GEAR_LEVER 3 ///< Landing Gear Limit Switch (handle raise / lower)
#define LANDING_GEAR_HANDLE_LT 4 ///< Landing Gear Lollipop LED

//Declare variables for down lock logic
bool EXT_WOW_LEFT = true;           ///< Initializing weight-on-wheel value for cold/ground start.
bool EXT_WOW_RIGHT = true;          ///< Initializing weight-on-wheel value for cold/ground start.
bool EXT_WOW_NOSE = true;           ///< Initializing weight-on-wheel value for cold/ground start.
bool EXT_WOW_NOSEGEAR_DOWNLOCK_OVERRIDE = false; ///< Initializing value for down lock override to not pressed.

// Connect switches to DCS-BIOS 
DcsBios::Switch2Pos emergencyGearRotate("EMERGENCY_GEAR_ROTATE", EMERGENCY_GEAR_ROTATE);
DcsBios::Switch2Pos gearEXT_WOW_NOSEGEAR_DOWNLOCK_OVERRIDEBtn("GEAR_DOWNLOCK_OVERRIDE_BTN", GEAR_DOWNLOCK_OVERRIDE_BTN);
DcsBios::Switch2Pos gearLever("GEAR_LEVER", GEAR_LEVER);
DcsBios::Switch2Pos gearSilenceBtn("GEAR_SILENCE_BTN", GEAR_SILENCE_BTN);
DcsBios::LED landingGearHandleLt(0x747e, 0x0800, LANDING_GEAR_HANDLE_LT);

// DCSBios reads to save airplane state information.
void onExtEXT_WOW_LEFTChange(unsigned int newValue) {
  EXT_WOW_LEFT = newValue;
} DcsBios::IntegerBuffer extEXT_WOW_LEFTBuffer(FA_18C_hornet_EXT_WOW_LEFT, onExtEXT_WOW_LEFTChange);

void onExtEXT_WOW_NOSEChange(unsigned int newValue) {
  EXT_WOW_NOSE = newValue;
} DcsBios::IntegerBuffer extEXT_WOW_NOSEBuffer(FA_18C_hornet_EXT_WOW_NOSE, onExtEXT_WOW_NOSEChange);

void onExtEXT_WOW_RIGHTChange(unsigned int newValue) {
  EXT_WOW_RIGHT = newValue;
} DcsBios::IntegerBuffer extEXT_WOW_RIGHTBuffer(FA_18C_hornet_EXT_WOW_RIGHT, onExtEXT_WOW_RIGHTChange);

void onGearDownlockOverrideBtnChange(unsigned int newValue) {
  EXT_WOW_NOSEGEAR_DOWNLOCK_OVERRIDE = newValue;
} DcsBios::IntegerBuffer gearDownlockOverrideBtnBuffer(FA_18C_hornet_GEAR_DOWNLOCK_OVERRIDE_BTN, onGearDownlockOverrideBtnChange);

/**
* Arduino Setup Function
*
* Code that should be executed only once at the program start, belongs in this function.
*/
void setup() {

  // Run DCS Bios setup function
  DcsBios::setup();

  pinMode(LG_LEVER_SOLENOID, OUTPUT);
  digitalWrite(LG_LEVER_SOLENOID, LOW);  //initialize solenoid to off

  digitalWrite(LANDING_GEAR_HANDLE_LT,true);
  delay(1000);
  digitalWrite(LANDING_GEAR_HANDLE_LT,false);
  delay(1000);
}

/**
* Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
*/
void loop() {

  //Run DCS Bios loop function
  DcsBios::loop();

/**
* ### Landing Gear Down Lock Logic
*  -# If landing gear handle in down position and lock override pushed, then activate solenoid to **unlock** handle. \n
*  -# If landing gear handle in down position and NO weight on wheels, then activate solenoid to **unlock** handle. \n
*  -# IF landing gear handle is down and there is weight on at least one wheel, then turn off solenoid to **lock** handle down. \n
*  -# If landing gear handle is up turn off solenoid, handle cannot physically be locked in up position. \n
*  \n
* @remark Digital reads of switch state will allow the landing gear handle to operate using the downlock override button
* without needing to have the sim running.
*/
  if (digitalRead(GEAR_LEVER) == 1) {                              //Switch closed, gear handle is down
    if (EXT_WOW_NOSEGEAR_DOWNLOCK_OVERRIDE == true || !digitalRead(GEAR_DOWNLOCK_OVERRIDE_BTN) == true) {  // Override switched pushed virtually in sim or physically in pit, turn on solenoid to unlock gear handle.
      digitalWrite(LG_LEVER_SOLENOID, HIGH);
    } else if ((EXT_WOW_LEFT == false) || (EXT_WOW_RIGHT == false) || (EXT_WOW_NOSE == false) ) {  //No weight on any wheel, turn on solenoid to unlock the gear handle
      digitalWrite(LG_LEVER_SOLENOID, HIGH);
    } else {  // gear handle is down and there is weight on at least one wheel, turn off solenoid to lock the handle down.
      digitalWrite(LG_LEVER_SOLENOID, LOW);
    }
  } else {  //gear handle up, turn off solenoid
    digitalWrite(LG_LEVER_SOLENOID, LOW);
  }
}
