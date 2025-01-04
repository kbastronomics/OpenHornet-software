#ifndef STANDBY_AIRSPEED_H
#define STANDBY_AIRSPEED_H

/* 2A7A1A3 STANDBY_AIRSPEED */
#define AIRSPEED_LED_DOUT       
#define AIRSPEED_COIL1          58
#define AIRSPEED_COIL2          52
#define AIRSPEED_COIL3          48
#define AIRSPEED_COIL4          46
#define AIRSPEED_ZERO_DETECT    A0
#define AIRSPEED_ZERO_CAL_BYTE  0
#define AIRSPEED_STEP_CAL_SIZE  27

/**
 * SETUP FOR STANDBY AIRSPEED
 *
 */

Stepper airspeedStepper(VID29_STEPSPERREVOLUTION, AIRSPEED_COIL2, AIRSPEED_COIL1, AIRSPEED_COIL3 , AIRSPEED_COIL4); // COILS 1/2 ARE FLIPPED TO REVERSE DIRECTION

uint32_t airSpeedFromDcs[AIRSPEED_STEP_CAL_SIZE] = { 0,2427,4854,7281,9708,12136,14563,16990,19417,21845,24272,26699,29126,31553,33981,36408,38835,41262,43690,46117,48544,53398,55826,58253,60680,63107,65535 };  // VALUES FROM DCS 
uint32_t airSpeedToStepper[AIRSPEED_STEP_CAL_SIZE] = { 0,26,53,80,106,133,160,186,213,240,266,293,320,346,373,400,426,453,480,506,533,586,613,640,666,693,720 };                    // STEPS TO MOVE GAUGE NEEDLE TO MATCH DCS

// eeprom.write(AIRSPEED_ZERO_CAL_BYTE,b1);  //  addr,value
// eeprom.read(AIRSPEED_ZERO_CAL_BYTE)       //  addr


/**
* Type :        integer
* Address :     0x74f0
* Mask :        0xffff
* ShiftBy :     0
* Max Value :   65535
* Description : gauge position
* 
* Read more about Addresses.h macros from the DCS-BIOS Wiki.
*/
void onStbyAsiAirspeedChange(unsigned int newValue) 
{
	//uint32_t pos = multiMap<long>(newValue, airSpeedFromDcs, airSpeedToStepper, AIRSPEED_STEP_CAL_SIZE);
}
DcsBios::IntegerBuffer stbyAsiAirspeedBuffer(FA_18C_hornet_STBY_ASI_AIRSPEED, onStbyAsiAirspeedChange);


/**
* @todo
* 
*/
void airspeedZero() {
  Serial.println("Homing the stepper motor...");

  // Move towards the home switch (towards negative direction, assuming home switch is at the end)
  while (digitalRead(airspeed_ZERO_DETECT) == LOW) {    // Home switch not pressed (HIGH when not pressed)
    airspeedStepper.setSpeed(-200);                    // Move in reverse direction at a speed of 200 steps/sec
    airspeedStepper.runSpeed();                        // Continuously run the motor at the set speed
  }

  // When the home switch is pressed, stop the motor
  airspeedStepper.stop();
  delay(100);                                            // Small delay to ensure the switch is properly triggered

  // Move a tiny bit away from the home switch to prevent constant triggering
  airspeedStepper.setSpeed(100);                       // Move away from the switch
  airspeedStepper.runSpeed();                          // Move slightly away from the switch
  delay(100);                                            // Allow time for the motor to move

  // Reset the stepper motor position to zero
  airspeedStepper.setCurrentPosition(0);               // Set the current position as the "home" position
  isHomed = true;                                        // Set the homing flag to true
  Serial.println("Homing complete. Motor is now at home position.");
}

/**
* @todo
*
*/
void airspeedCalibrate() {
   Serial.println("Calibrating the airspeed...");
}

/**
* @todo
* 
*/
void airspeedSetuo() {
   // Initialize the airspeed stepper motor
   pinMode(airspeed_ZERO_DETECT,INPUT);
   airspeedStepper.setMaxSpeed(500.0);
   airspeedStepper.setAcceleration(100.0);

}


void airspeedLoop() {
//static uint32_t lastPos = 0;
   airspeedStepper.run();
}

#endif