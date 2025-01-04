#pragma once

#ifndef STANDBY_ALTIMETER_H
#define STANDBY_ALTIMETER_H

//#include <STANDBY CHARACTERS.h>  // FONTS FOR THE ALTIMETER AND BAROMETER OLED DISPLAYS

/* 2A7A1A4 STANDBY_ALTIMETER */
#define ALTIMETER_LED_DOUT 
#define ALTIMETER_COIL1          42
#define ALTIMETER_COIL2          44
#define ALTIMETER_COIL3          38
#define ALTIMETER_COIL4          40
#define ALTIMETER_ZERO_DETECT    A1
#define ALTIMETER_CH_A           12
#define ALTIMETER_CH_B           13
#define ALTIMETER_ZERO_CAL       1
#define ALTIMETER_STEP_CAL_SIZE  21

/* SETUP FOR DUAL OLED THRU I2C SWITCH */
#define I2C_CH1_ADDR   0x70
#define I2C_CH2_ADDR   0x71
#define OLED_ADDR      0x3C

/**
 * SETUP FOR STANDBY ALTIMETER
 *
 */
AccelStepper altimeterStepper(AccelStepper::FULL4WIRE, ALTIMETER_COIL2, ALTIMETER_COIL1, ALTIMETER_COIL3, ALTIMETER_COIL4);
RotaryEncoder altimeterEncoder(ALTIMETER_CH_A, ALTIMETER_CH_B, RotaryEncoder::LatchMode::FOUR3);

instrumentState altimeterState;

uint32_t altFromDcs[ALTIMETER_STEP_CAL_SIZE] = { 0,3276,6553,9830,13107,16383,19660,22937,26214,29490,32767,36044,39321,42597,45874,49151,52428,55704,58981,62258,65535 };  // VALUES FROM DCS 
uint32_t altToStepper[ALTIMETER_STEP_CAL_SIZE] = { 0,36,72,108,144,180,216,252,288,324,360,396,432,468,504,540,576,612,648,684,720 };                    // STEPS TO MOVE GAUGE NEEDLE TO MATCH DCS

// eeprom.write(ALTIMETER_ZERO_CAL,b1);  //  addr,value
// eeprom.read(ALTIMETER_ZERO_CAL)       // addr

uint32_t altimeterSteps = 0;  // MAPPED VALUE WILL BE THE ABSOLUTION POSTION TO MOVE STEPPER TOO WITH 0 BEING THE HOME POSITION HOWEVER THIS MUST WORK IN COMPASS MODE
                              // COMPASS MODE IS WHERE THE NEEDLE KEEPS GOING PAST 0 AND REPEATS INSTEAD OF GOING BACK TO ZERO 
uint32_t altimeterHome = 0;
//bool  isHomed = false;
                           

/**
* Type :        integer
* Address :     0x74f6
* Mask :        0xffff
* ShiftBy :     0
* Max Value :   65535
* Description : gauge position
* 
* Read more about Addresses.h macros from the DCS-BIOS Wiki.
*/
// void onStbyAlt10000FtCntChange(unsigned int newValue) 
// {
//    /**
//    * @todo
//    * add OLED Update Here
//    */
// }
// DcsBios::IntegerBuffer stbyAlt10000FtCntBuffer(FA_18C_hornet_STBY_ALT_10000_FT_CNT, onStbyAlt10000FtCntChange);

/**
* Type :        integer
* Address :     0x74f8
* Mask :        0xffff
* ShiftBy :     0
* Max Value :   65535
* Description : gauge position 
* 
* Read more about Addresses.h macros from the DCS-BIOS Wiki.
*/
void onStbyAlt1000FtCntChange(unsigned int newValue) 
{
   /**
   * @todo
   * add OLED update here
   */
}
DcsBios::IntegerBuffer stbyAlt1000FtCntBuffer(FA_18C_hornet_STBY_ALT_1000_FT_CNT, onStbyAlt1000FtCntChange);

/**
 * Type :        integer
 * Address :     0x74f4
 * Mask :        0xffff
 * ShiftBy :     0
 * Max Value :   65535
 * Description : gauge position
 * 
 * Read more about Addresses.h macros from the DCS-BIOS Wiki.
*/
void onStbyAlt100FtPtrChange(unsigned int newValue) 
{
	altimeterSteps = multiMap<uint32_t>(newValue, altFromDcs, altToStepper, ALTIMETER_STEP_CAL_SIZE);
}
DcsBios::IntegerBuffer stbyAlt100FtPtrBuffer(FA_18C_hornet_STBY_ALT_100_FT_PTR, onStbyAlt100FtPtrChange);


/** 
* Interface :      VARIABLE_STEP
* Message :        STBY_PRESS_ALT <new_value>|-<decrease_by>|+<increase_by>
* Suggested Step : 3200
* Value Range :    0-65535
* Description :    turn the dial left or right
*/
DcsBios::RotaryEncoder stbyPressAlt("STBY_PRESS_ALT", "-3200", "+3200", ALTIMETER_CH_A, ALTIMETER_CH_B);
//TwoStepRotaryEncoder stbyPressAlt("STBY_PRE", "DEC", "INC", ALTIMETER_CH_A, ALTIMETER_CH_B);


/**
* @todo
* 
*/
void altimeterZero() {
  Serial.println("Homing the stepper motor...");

  // Move towards the home switch (towards negative direction, assuming home switch is at the end)
  while (digitalRead(ALTIMETER_ZERO_DETECT) == LOW) {    // Home switch not pressed (HIGH when not pressed)
    altimeterStepper.setSpeed(-200);                    // Move in reverse direction at a speed of 200 steps/sec
    altimeterStepper.runSpeed();                        // Continuously run the motor at the set speed
  }

  // When the home switch is pressed, stop the motor
  altimeterStepper.stop();
  delay(100);                                            // Small delay to ensure the switch is properly triggered

  // Move a tiny bit away from the home switch to prevent constant triggering
  altimeterStepper.setSpeed(100);                       // Move away from the switch
  altimeterStepper.runSpeed();                          // Move slightly away from the switch
  delay(100);                                            // Allow time for the motor to move

  // Reset the stepper motor position to zero
  altimeterStepper.setCurrentPosition(0);               // Set the current position as the "home" position
  isHomed = true;                                        // Set the homing flag to true
  Serial.println("Homing complete. Motor is now at home position.");
}

/**
* @todo
*
*/
void altimeterCalibrate() {
   Serial.println("Calibrating the altimeter...");
}

/**
* @todo
* 
*/
void altimeterSetuo() {
   Serial.println("One time setup of the altimeter...");
   // Initialize the altimeter stepper motor and zero
   pinMode(ALTIMETER_ZERO_DETECT,INPUT);
   altimeterStepper.setMaxSpeed(500.0);
   altimeterStepper.setAcceleration(100.0);

}


void altimeterLoop() {
//static uint32_t lastPos = 0;
   altimeterStepper.run();
}

#endif