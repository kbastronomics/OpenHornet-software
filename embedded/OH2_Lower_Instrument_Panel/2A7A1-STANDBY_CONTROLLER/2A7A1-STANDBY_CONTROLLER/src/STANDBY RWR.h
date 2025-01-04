#ifndef STANDBY_RWR_H
#define STANDBY_RWR_H

/* 2A7A1A2 STANDBY RWR BRT*/
#define RWR_BRT_A             A3 

/**
 * SETUP FOR STANDBY RWR
 *
 */

//DcsBios::RotaryEncoder rwrRwrIntesity("RWR_RWR_INTESITY", "-3200", "+3200", RWR_CH_A, RWR_CH_B);
DcsBios::Potentiometer rwrRwrIntesity("RWR_RWR_INTESITY", RWR_BRT_A);

#endif