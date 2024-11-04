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
 * @author Sandra Carroll, OH Community, sandra
 * @date 02.29.2024
 * @version 0.0.3
 * @copyright Copyright 2016-2024 OpenHornet. Licensed under the Apache License, Version 2.0.
 * @brief Controls the Standy Instruments (Airspeed, VVI, Altimeter, RWR, SARI).
 *
 * @details
 * 
 *  * **Reference Designator:** 2A7A1
 *  * **Intended Board:** STANDYBY_CONTROLLER
 *  * **RS485 Bus Address:** 5
 *  * **RS485 Bus Terminator:** Yes

 * 
 * **Wiring diagram:**
 * PIN | Function
 * --- | ---
 * A0  | AMPCD Rotary Brightness
 * A1  | AMPCD Off - No DCS Bios use
 * 8   | HDG +
 * 10  | HDG -
 * 4   | CRS +
 * 7   | CRS -
 * 9   | DDI Backlighting PWM, must be defined as digital pin #
 * 6   | AMPCD IRQ Pin
 * 
 *
 * @brief following #define tells DCS-BIOS that this is a RS-485 slave device.
 * It also sets the address of this slave device. The slave address should be
 * between 1 and 126 and must be unique among all devices on the same bus.
 *
 * @bug Currently does not work with the Pro Micro (32U4), Fails to compile
 *
 //#define DCSBIOS_RS485_SLAVE 2
 *
*/

/**
 * Check if we're on a Mega328 or Mega2560 and define the correct
 * serial interface
 * 
 */
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
#define DCSBIOS_IRQ_SERIAL  ///< This enables interrupt-driven serial communication for DCS-BIOS. (Only used with the ATmega328P or ATmega2560 microcontrollers.)
#else
#define DCSBIOS_DEFAULT_SERIAL  ///< This enables the default serial communication for DCS-BIOS. (Used with all other microcontrollers than the ATmega328P or ATmega2560.)
#endif

#ifdef __AVR__
#include <avr/power.h>
#endif


/**
 * The Arduino pin that is connected to the
 * RE and DE pins on the RS-485 transceiver.
*/
#define TXENABLE_PIN 5  ///< Sets TXENABLE_PIN to Arduino Pin 5
#define UART1_SELECT    ///< Selects UART1 on Arduino for serial communication

#include "DcsBios.h"
#include "Wire.h"

// Define pins per the OH Interconnect.
#define AMPCD_ROT_A A0       ///< AMPCD Rotary Brightness
#define AMPCD_ROT_OFF A1     ///< AMPCD Off  - No DCS Bios use.
#define HDG_P 8              ///< HDG +
#define HDG_M 10             ///< HDG -
#define CRS_P 4              ///< CRS +
#define CRS_M 7              ///< CRS -
#define AMPCD_BACK_LIGHT 9  ///< DDI Backlighting PWM, must be defined as digital pin #
#define AMPCD_IRQ 6 ///< AMPCD IRQ Pin


//Connect switches to DCS-BIOS


/**
 * @brief Setup DCS-BIOS CALL Back routines
 *
 */


/**
* Arduino Setup Function
*
* Arduino standard Setup Function. Code who should be executed
* only once at the program start, belongs in this function.
*/
void setup() {

  // Run DCS Bios setup function
  DcsBios::setup();
}

/**
* Arduino Loop Function
*
* Arduino standard Loop Function. Code who should be executed
* over and over in a loop, belongs in this function.
* 
* @attention If AMPCD button output flickers increase debounceDelay.
*/
void loop() {

  //Run DCS Bios loop function
  DcsBios::loop();

}