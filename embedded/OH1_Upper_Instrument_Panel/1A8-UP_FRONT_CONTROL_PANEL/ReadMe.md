:Author: Sandra Carroll
:Email: {AuthorEmail}
:Date: 11/03/2024
:Revision: 0.1.0A
:License: Public Domain

= Project:  Open Hornet UFC v6.1.0 Test

Testing following features of Open Hornet UFC v6.1

| TEST          | STATE         | NOTES |
| ------------- |:-------------:| -----:|
| OPTION CUE 1-5| WORKING       |       |
| col 2 is      | centered      |   $12 |
| zebra stripes | are neat      |    $1 |

- OPTION CUE 1-5  	Working
- OPTION DISPLAY 1-5 	WORKING
- COMM 1-2		WORKING
- SCRATCHPAD		WORKING
- SCRATCHPAD BACKLIGHT	WORKING
- AP KEYBOARD		WORKING
- UFC KEYBOARD		WORKING
- ADF ON-OFF-ON		WORKING
- BRIGHTNESS KNOB	*WORKING (Typedefs', causes random crashes of UFC, looking to move to better ADC ADS1115)
- COMM1 VOLUME		*WORKING (Typedefs', causes random crashes of UFC, looking to move to better ADC ADS1115)
- COMM2	VOLUME		*WORKING (Typedefs', causes random crashes of UFC, looking to move to better ADC ADS1115)
- HUD BIT TEST		WORKING (not used in DCS)
- HUD GO LED		WORKING (On if In Mission)
- HUD NOGO LED		WORKING (On if Not in Mission)
- BACKLIGHTING		WORKING
- POWER CIRCUIT		*BYPASS (bypassed for testing, see PR#1113)
- COMM1 CHANNEL		WORKING (Typedefs') 
- COMM2 CHANNEL		WORKING (Typedefs')
- COMM1 PULL		WORKING
- COMM2 PULL		WORKING

== Step 1: Installation

NOTE:   This is a Visual Studio Code + PlatformIO Project for now
