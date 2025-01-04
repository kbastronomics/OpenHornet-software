#include <math.h>
#include "Arduino.h"
#include "clsPCA9555.h"

PCA9555 functionSelect(0x20);

namespace DcsBios {
	template <unsigned long pollIntervalMs = POLL_EVERY_TIME>
	class virtSwitch2PosT : PollingInput, public ResettableInput
	{
	private:
		const char* msg_;
		char pin_;
		char debounceSteadyState_;
		char lastState_;
		bool reverse_;
		unsigned long debounceDelay_;
		unsigned long lastDebounceTime = 0;

		void resetState()
		{
			lastState_ = (lastState_==0)?-1:0;
		}

		void pollInput() {
			char state = functionSelect.digitalRead(pin_);
			if (reverse_) state = !state;

			unsigned long now = millis();

			if (state != debounceSteadyState_) {
				lastDebounceTime = now;
				debounceSteadyState_ = state;
			}
			
			if ((now - lastDebounceTime) >= debounceDelay_) {
				if (debounceSteadyState_ != lastState_) {
					if (tryToSendDcsBiosMessage(msg_, state == HIGH ? "0" : "1")) {
						lastState_ = debounceSteadyState_;
					}
				}
			}			
		}
	public:
		virtSwitch2PosT(const char* msg, char pin, bool reverse = false, unsigned long debounceDelay = 50) :
			PollingInput(pollIntervalMs)
		{ 
			msg_ = msg;
			pin_ = pin;
			functionSelect.pinMode(pin_, INPUT_PULLUP);
			debounceDelay_ = debounceDelay;
			reverse_ = reverse;

			lastState_ = functionSelect.digitalRead(pin_);
			if (reverse_) lastState_ = !lastState_;			
		}
				
		void SetControl( const char* msg )
		{
			msg_ = msg;
		}
        
		void resetThisState()
		{
			this->resetState();
		}
	};
	typedef virtSwitch2PosT<> virtSwitch2Pos;
}