:Author: scuba82
:Email: {AuthorEmail}
:Date: 12/06/2023
:Revision: 1.0#
:License: Public Domain

= Project:  Open Hornet UFC v5.5 Test

Testing following features of Open Hornet UFC v5.5

- scratchpad -> DCS-BIOS integartion 
- OP Displays 1-5 -> DCS-BIOS integartion 
- COMM Displays 1 & 2 -> DCS-BIOS integartion 
- Rotary Encoder 1 & 2 -> Serial print a counter 
- Keypad Matrix -> Interrupt triggered;  Serial print button name; TODO rework PRESS / RELEASE detection
- ADF Switch  -> Interrupt triggered;  Serial print switch status; TODO rework PRESS / RELEASE detection
- BIT Switch  -> Interrupt triggered;  Serial print switch status; TODO rework PRESS / RELEASE detection
- VOL1, VOL2, BRT potis -> Serial print values every 2 sec
- GO / NOGO LED -> blink GO LED 5 sec; blink NOGO LED 2 sec

== Step 1: Installation

- Install the following DCS-BIOS FP for https://github.com/pavidovich/dcs-bios-arduino-library (enables DCS-BIOS  WIFI capabilities )
- Edit char ssid[] = "<SSID>";
- Edit char passwd[] = "<Password>";
- Install TM16xx library https://github.com/maxint-rd/TM16xx 
  - Modify character "V" in array "TM16XX_FONT_15SEG" in TM16xxFonts.h  to 0b100100000110000 since the original character is printed backwards
- Install Adafruit_TCA8418




=== Folder structure

....
 UFC_TEST_ESP32S2                => Arduino sketch folder
  ├── UFC_TEST_ESP32S2.ino       => main Arduino file
  ├── CHARSET.h                  => Custom charset for COMM displays
  ├── HT1621_OH.h .cpp           => Modififed HT1621 library for scratchpad
  └── ReadMe.adoc                => this file
....

