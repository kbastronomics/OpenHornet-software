//#include "esp32-hal.h"
/*******************************************************************************

!!!!! This a modification of the original HT1621 arduino library adopted to Open Hornet needs !!!!



Copyright 2016-2018 anxzhu (github.com/anxzhu)
Copyright 2018-2020 Valerio Nappi (github.com/5N44P) (changes)
Based on segment-lcd-with-ht1621 from anxzhu (2016-2018)
(https://github.com/anxzhu/segment-lcd-with-ht1621)

Partially rewritten and extended by Valerio Nappi (github.com/5N44P) in 2018

This file is part of the HT1621 arduino library, and thus under the MIT license.
More info on the project and the license conditions on :
https://github.com/5N44P/ht1621-7-seg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#include <Arduino.h>
#include "HT1621_OH.h"

HT1621_OH::HT1621_OH(){
	_buffer[0] = 0x00;
	_buffer[1] = 0x00;
	_buffer[2] = 0x00;
	_buffer[3] = 0x00;
	_buffer[4] = 0x00;
	_buffer[5] = 0x00;
	_buffer[6] = 0x00;
	_buffer16[0] = 0x0000;
	_buffer16[1] = 0x0000;
}

void HT1621_OH::begin(int cs_p, int wr_p, int data_p, int backlight_p)
{
	pinMode(cs_p, OUTPUT);
	pinMode(wr_p, OUTPUT);
	pinMode(data_p, OUTPUT);
	pinMode(backlight_p, OUTPUT);
	_cs_p=cs_p;
	_wr_p=wr_p;
	_data_p=data_p;
	_backlight_p=backlight_p;
	_backlight_en=false;
  _brightness_p = 255;
	config();
}

void HT1621_OH::begin(int cs_p, int wr_p, int data_p)
{
	pinMode(cs_p, OUTPUT);
	pinMode(wr_p, OUTPUT);
	pinMode(data_p, OUTPUT);
	_cs_p=cs_p;
	_wr_p=wr_p;
	_data_p=data_p;
	_backlight_en = false;
	config();
}

void HT1621_OH::wrDATA(unsigned char data, unsigned char cnt) {
	unsigned char i;
	for (i = 0; i < cnt; i++) {
		digitalWrite(_wr_p, LOW);
		delayMicroseconds(4);
		if (data & 0x80) {
			digitalWrite(_data_p, HIGH);
		}
		else {
			digitalWrite(_data_p, LOW);
		}
		digitalWrite(_wr_p, HIGH);
		delayMicroseconds(4);
		data <<= 1;
	}
}
void HT1621_OH::wrclrdata(unsigned char addr, unsigned char sdata)
{
	addr <<= 2;
	digitalWrite(_cs_p, LOW);
	wrDATA(0xa0, 3);
	wrDATA(addr, 6);
	wrDATA(sdata, 8);
	digitalWrite(_cs_p, HIGH);
}

void HT1621_OH::display()
{
	wrCMD(LCDON);
}

void HT1621_OH::noDisplay()
{
	wrCMD(LCDOFF);
}

void HT1621_OH::wrone(unsigned char addr, unsigned char sdata)
{
	addr <<= 2;
	digitalWrite(_cs_p, LOW);
	wrDATA(0xa0, 3);
	wrDATA(addr, 6);
	wrDATA(sdata, 8);
	digitalWrite(_cs_p, HIGH);
}

void HT1621_OH::set_backlight_brightness(int brightness){
  _brightness_p = brightness;
  backlight();
}

void HT1621_OH::backlight()
{
   for ( int i=0; i < 7; i++){
     if ( _buffer[i] != 0x00 ) {
       _backlight_en = true;
       break;
      }
   }
   for ( int i=0; i < 2; i++){
     if ( _buffer16[i] != 0x00  ) {
       _backlight_en = true;
       break;
      }
   }
	if (_backlight_en){
		analogWrite(_backlight_p, _brightness_p);
  }
  else{
    analogWrite(_backlight_p, 0);
  }
	delay(1);
  _backlight_en = false;
}

void HT1621_OH::noBacklight()
{
	//if(_backlight_en)
		digitalWrite(_backlight_p, LOW);
	delay(1);
}

void HT1621_OH::wrCMD(unsigned char CMD) {  //100
	digitalWrite(_cs_p, LOW);
	wrDATA(0x80, 4);
	wrDATA(CMD, 8);
	digitalWrite(_cs_p, HIGH);
}

void HT1621_OH::config()
{
	wrCMD(BIAS);
	wrCMD(RC256);
	wrCMD(SYSDIS);
	wrCMD(WDTDIS1);
	wrCMD(SYSEN);
	wrCMD(LCDON);
}

void HT1621_OH::wrCLR(unsigned char len) {
	unsigned char addr = 0;
	unsigned char i;
	for (i = 0; i < len; i++) {
		wrclrdata(addr, 0x00);
		addr = addr + 2;
	}
}


void HT1621_OH::clear(){
	wrCLR(16);
}



// takes the buffer and puts it straight into the driver
void HT1621_OH::update(){
	// the buffer is backwards with respect to the lcd. could be improved
	wrone(0,  _buffer16[0]); // writing 16 bits to memory
	wrone(2,  _buffer16[1]); // writing 16 bits to memory
    wrone(4,  _buffer16[2]); // writing 16 bits to memory
    wrone(6,  _buffer16[3]); // writing 16 bits to memory
	wrone(8,  _buffer[0]);   // writing 8 bits to memory
	wrone(10, _buffer[1]);   // writing 8 bits to memory
	wrone(12, _buffer[2]);   // writing 8 bits to memory
	wrone(14, _buffer[3]);   // writing 8 bits to memory
	wrone(16, _buffer[4]);   // writing 8 bits to memory
	wrone(18, _buffer[5]);   // writing 8 bits to memory
	wrone(20, _buffer[6]);   // writing 8 bits to memory	
}



void HT1621_OH::print_str(const char* str, bool leftPadded){
  for(int i = 0; i < 4; i++){
      if ( i == 0 | i == 2){
        if (str[i] == ' ' && !isDigit(str[i+1]) ){
          _buffer16[i] =charTo16SegBits_Dig1(str[i+1]);
        }else if ( str[i+1] == '4' || str[i+1] == '5' || str[i+1] == '6' || str[i+1] == '8' || str[i+1] == '9' || str[i+1] == '0'  ){
           char tmp_seg = charTo16SegBits_Dig1(str[i]);
           tmp_seg |= 0x01;
          _buffer16[i] = tmp_seg;
        }else{
          _buffer16[i] = charTo16SegBits_Dig1(str[i]);
        }
      }else{
        _buffer16[i] = charTo16SegBits_Dig2(str[i]);
      }
  }
  backlight();
  update();
}

void HT1621_OH::print_num(const char* str, bool leftPadded){
	int chars = strlen(str);
	int padding = 8 - chars;
  int k=0;

	for(int i = 0; i < 8; i++){ // We're getting 8 chars with the first beeing "space" ... so chop of the first one
		//_buffer[i] &= 0x80; // mask the first bit, used by batter and decimal point
		char character = leftPadded
				 		 ? i < padding ? ' ' : str[i - padding]
				 		 : i >= chars ? ' ' : str[i+1];
		  _buffer[i] = charToSegBits(character);
	}
  backlight();
	update();
}




/*
  |-A1-A2-|
   |\  |  /|
   F H J K B
   |  \|/  |
   |-G1-G2-|
   |  /|\  |
   E N M L C 
   |/  |  \|
   |-D1-D2-| DP
        
                 1  1 1
   High Byte:  0bDEFANGHJ  
                   2 2  2
   Low  Byte:  0bMLGKDCBA  
   */
  

char HT1621_OH::charTo16SegBits_Dig1(char character) {
  	switch (character) {
      case '-':
        return 0b00000100;
      case '0': 
        return 0b00000001;
      case '1': 
        return 0b01100000;
      case '2': 
        return 0b11010101;
      case '3': 
        return 0b00000000;
      case '4': 
        return 0b00000001;
      case '5': 
        return 0b00000001;
      case '6': 
        return 0b00000001;
      case '7': 
        return 0b00000111;
      case '8': 
        return 0b00000001;
      case '9': 
        return 0b00100111;
      case 'A': 
        return 0b01110100;
	  case 'B':
	  	return 0b10010001;
      case 'C': 
        return 0b11110000;
	  case 'D':
	  	return 0b00000000;
	  case 'E':
	  	return 0b00000000;
	  case 'F':
	  	return 0b00000000;
      case 'G': 
        return 0b11110000;
	  case 'H':
	  	return 0b01100100;
      case 'I': 
        return 0b10010001;
      case 'M': 
        return 0b01100010;
      case 'N':    
        return 0b01100010;
      case 'O': 
        return 0b11110000;
      case 'P': 
        return 0b01110100;
      case 'S': 
        return 0b10110100;
      case 'X': 
        return 0b00001010;
      case 'Y': 
        return 0b00000010;
      case ' ':
      default:
		    return 0b00000000;
    }
}
char HT1621_OH::charTo16SegBits_Dig2(char character) {
  	switch (character) {
      case '-':
        return 0b00100000;
      case '0': 
        return 0b10001111;
      case '1': 
        return 0b00000110;
      case '2': 
        return 0b10101011;
      case '3': 
        return 0b00101111;
      case '4': 
        return 0b00100110;
      case '5': 
        return 0b00101101;
      case '6': 
        return 0b10101100;
      case '7': 
        return 0b00000111;
      case '8': 
        return 0b10101111;
      case '9': 
        return 0b00100111;
      case 'A': 
        return 0b00100111;
	  case 'B':
		return 0b10101111;
	  case 'C': 
        return 0b00001001;
	  case 'D':
	  	return 0b00000000;
	  case 'E':
	  	return 0b00000000;
	  case 'F':
	  	return 0b00000000;  
      case 'G': 
        return 0b00101101;
	  case 'H':
	  	return 0b00100110;	
      case 'I': 
        return 0b10001001;
      case 'M': 
        return 0b00010110;
      case 'N': 
        return 0b01000110;
      case 'O': 
        return 0b00001111;
      case 'P': 
        return 0b00100011;
      case 'S': 
        return 0b00101101;
      case 'X': 
        return 0b01010000;
      case 'Y': 
        return 0b10010000;
      
      case ' ':
      default:
		    return 0b00000000;
    }
}
/*
7 Segments
   |--A--|
   F	   B
   |--G--|
   E	   C
   |--D--|
    
  0b?EFADCGB 
  
  */

char HT1621_OH::charToSegBits(char character) {
	switch (character) {
  case '@': // For ° 
		return 0b00110011;
	case '|':
		return 0b01100000;
	case '-':
		return 0b00000010;
	case '_':
		return 0b00001000;
  case '.':
		return 0b01001110;
	case '0':
		return 0b01111101;
	case '1':
		return 0b00000101;
	case '2':
		return 0b01011011;
	case '3':
		return 0b00011111;
	case '4':
		return 0b00100111;
	case '5':
		return 0b00111110;
	case '6':
		return 0b01101110;
	case '7':
		return 0b00010101;
	case '8':
		return 0b01111111;
	case '9':
		return 0b00110111;
	case 'A':
	case 'a':
		return 0b01110111;
	case 'b':
	case 'B':
		return 0b01111111;
	case 'c':
	case 'C':
		return 0b01111000;
	case 'd':
	case 'D':
		return 0b01111101;
	case 'e':
	case 'E':
		return 0b01111000;
	case 'f':
	case 'F':
		return 0b01110010;
	case 'G':
	case 'g':
		return 0b01111100;
	case 'h':
	case 'H':
		return 0b01100111;
	case 'i':
	case 'I':
		return 0b00000101;
	case 'J':
	case 'j':
		return 0b00011101;
	case 'l':
	case 'L':
		return 0b01101000;
	case 'm':
	case 'M':
		return 0b1010100;
	case 'n':
	case 'N':
		return 0b01000110;
	case 'O': 
	 	return 0b01111101;
	case 'o':
		return 0b01111101;
	case 'P':
	case 'p':
		return 0b01111010; // DCS-Bios sends p for an E (in ERROR)
	case 'q':
	case 'Q':
		return 0b00110111;
	case 'r':
	case 'R':
		return 0b01110101;
	case 'S':
	case 's':
		return 0b00111110;
	case 't':
	case 'T':
		return 0b0001111;
	case 'u':
	//	return 0b1001100;
	case 'U':
		return 0b01101101;
    case 'w':
		return 0b01110111; // DCS-Bios sends w for an R (in ERROR)
	case 'Y':
	case 'y':
		return 0b1101011;
	case 'z':
	case 'Z':
		return 0b0111110;
	case ' ':
	default:
		return 0b0000000;
	}
}