#ifndef STANDBY_SARI_H
#define STANDBY_SARI_H


/* 2A7A1A1 SARI CAGE */
#define SARI_CAGE             22
#define SARI_CH_A             26
#define SARI_CH_B             24
#define SARI_TEST             28

/**
* SETUP FOR STANDBY SARI
*
*/

//TwoStepRotaryEncoder aiSet("SAI_SE", "DEC", "INC", SARI_CH_A, SARI_CH_B);
DcsBios::RotaryEncoder saiSet("SAI_SET", "-3200", "+3200", SARI_CH_A, SARI_CH_B);
DcsBios::Switch2Pos saiCage("SAI_CAGE", SARI_CAGE,false,25);
DcsBios::Switch2Pos saiTestBtn("SAI_TEST_BTN", SARI_TEST,false,25);
#endif