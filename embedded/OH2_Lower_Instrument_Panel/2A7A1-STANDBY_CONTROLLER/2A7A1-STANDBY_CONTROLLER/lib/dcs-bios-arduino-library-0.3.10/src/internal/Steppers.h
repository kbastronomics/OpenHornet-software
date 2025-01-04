#ifndef __DCSBIOS_STEPPERS_H
#define __DCSBIOS_STEPPERS_H

#include "Arduino.h"
#include "ExportStreamListener.h"
#if defined(AccelStepper_h)
	#include <AccelStepper.h>
#else
	#include <Stepper.h>
#endif
#if defined(MULTIMAP_LIB_VERSION)
	#include "MultiMap.h"
#endif

namespace DcsBios {
	class StepperOutput : public Int16Buffer : public AccelStepper {
		private:
			void onDcsBiosFrameSync();
            #if defined(AccelStepper_h)
                AccelStepper stepper_;
            #else
                Stepper stepper_;
            #endif
			char zeroPin_;
			int minPulseWidth_;
			int maxPulseWidth_;
			unsigned int (*map_function_)(unsigned int newValue);
		public:
            bool isHomed;

			StepperOutput(unsigned int address, char pin, int minPulseWidth, int maxPulseWidth) : Int16Buffer (address) {
				zeroPin_ = pin;
				minPulseWidth_ = minPulseWidth;
				maxPulseWidth_ = maxPulseWidth;
				map_function_ = NULL;
			}
			
            StepperOutput(unsigned int address, char pin) : Int16Buffer (address) {
				zeroPin_ = pin;
				minPulseWidth_ = 544;
				maxPulseWidth_ = 2400;
				map_function_ = NULL;
			}
			
            StepperOutput(unsigned int address, char pin, int minPulseWidth, int maxPulseWidth, unsigned int (*map_function)(unsigned int newValue)) : Int16Buffer(address) {
				zeroPin_ = pin;
				minPulseWidth_ = minPulseWidth;
				maxPulseWidth_ = maxPulseWidth;
				map_function_ = map_function;
			}
			
            virtual void loop() {
				if (!servo_.attached())
					servo_.attach(zeroPin_, minPulseWidth_, maxPulseWidth_);
				if (hasUpdatedData()) {
					servo_.writeMicroseconds(mapValue(getData()));
				}
			}
			
            unsigned int mapValue(unsigned int value) {
				if (map_function_) {
					return map_function_(value);
				} else {
					return map(value, 0, 65535, minPulseWidth_, maxPulseWidth_);
				}
			}
			
            unsigned int zero() {
0
                    stepper_.setSpeed(-200);  
                    stepper_.runSpeed();      
                }
                stepper_.stop();
                while(isRunning());

                while (digitalRead(zeroPin_) == LOW) { 
                    stepper_.setSpeed(5);  
                    stepper_.runSpeed();   
                }
                stepper_.setCurrentPosition(0);  
                return(isHomed = true);                 			
			}

	};
}

#endif