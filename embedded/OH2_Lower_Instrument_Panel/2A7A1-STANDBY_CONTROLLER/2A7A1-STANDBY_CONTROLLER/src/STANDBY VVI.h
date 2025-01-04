#ifndef STANDBY_VVI_H
#define STANDBY_VVI_H

/* 2A7A1A3 STANDBY_VVI */
#define VVI_LED_DOUT          
#define VVI_COIL1             34
#define VVI_COIL2             36
#define VVI_COIL3             32
#define VVI_COIL4             30
#define VVI_ZERO_DETECT       A2
#define VVI_ZERO_CAL_BYTE     2
#define VVI_STEP_CAL_SIZE  	  42   // ZERO POINT IS AT 21  

/**
 * SETUP FOR STANDBY VVI
 *
 */

Stepper vviStepper(VID29_STEPSPERREVOLUTION, VVI_COIL2, VVI_COIL1, VVI_COIL3 , VVI_COIL4); // COILS 1/2 ARE FLIPPED TO REVERSE DIRECTION

uint32_t vviFromDcs[VVI_STEP_CAL_SIZE] = { 0,6553,13107,19660,26214,32767,39321,45874,52428,58981,65535 };  // VALUES FROM DCS 
uint32_t vviToStepper[VVI_STEP_CAL_SIZE] = { 0,72,144,216,288,360,432,504,576,648,720 };                    // STEPS TO MOVE GAUGE NEEDLE TO MATCH DCS

// eeprom.write(VVI_ZERO_CAL,b1);  //  addr,value
// eeprom.read(VVI_ZERO_CAL)       // addr

/**
* 
* Type :        integer
* Address :     0x7500
* Mask :        0xffff
* ShiftBy :     0
* Max Value :   65535
* Description : gauge position
* 
* Read more about Addresses.h macros from the DCS-BIOS Wiki.
*/
void onVsiChange(unsigned int newValue) 
{
  // uint32_t pos = multiMap<long>(newValue, vviFromDcs, vviToStepper, VVI_STEP_CAL_SIZE);
}
DcsBios::IntegerBuffer vsiBuffer(FA_18C_hornet_VSI, onVsiChange);


/**
* @todo
* 
*/
void vviZero() {
  Serial.println("Homing the stepper motor...");

  // Move towards the home switch (towards negative direction, assuming home switch is at the end)
  while (digitalRead(vvi_ZERO_DETECT) == LOW) {    // Home switch not pressed (HIGH when not pressed)
    vviStepper.setSpeed(-200);                    // Move in reverse direction at a speed of 200 steps/sec
    vviStepper.runSpeed();                        // Continuously run the motor at the set speed
  }

  // When the home switch is pressed, stop the motor
  vviStepper.stop();
  delay(100);                                            // Small delay to ensure the switch is properly triggered

  // Move a tiny bit away from the home switch to prevent constant triggering
  vviStepper.setSpeed(100);                       // Move away from the switch
  vviStepper.runSpeed();                          // Move slightly away from the switch
  delay(100);                                            // Allow time for the motor to move

  // Reset the stepper motor position to zero
  vviStepper.setCurrentPosition(0);               // Set the current position as the "home" position
  isHomed = true;                                        // Set the homing flag to true
  Serial.println("Homing complete. Motor is now at home position.");
}

/**
* @todo
*
*/
void vviCalibrate() {
   Serial.println("Calibrating the vvi...");
}

/**
* @todo
* 
*/
void vviSetuo() {
   // Initialize the vvi stepper motor
   pinMode(vvi_ZERO_DETECT,INPUT);
   vviStepper.setMaxSpeed(500.0);
   vviStepper.setAcceleration(100.0);

}


void vviLoop() {
//static uint32_t lastPos = 0;
   vviStepper.run();
}

#endif