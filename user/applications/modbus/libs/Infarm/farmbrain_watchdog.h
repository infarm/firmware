#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "basic_timeout.h"
#include "serial_debugger.h"

class FarmBrainWatchdog : public SerialDebugger
{
      public:
	void init(uint32_t timeoutMs, uint8_t resetPin);
	bool tick();
	void feed();

      private:
	uint8_t resetPin;
	void powerOnFarmBrain();
	BasicTimeout timerWatchdogExpiredCheck;
	BasicTimeout timerPowerOnFarmBrainAgain;
	static void farmBrainPowerOnAgainTimerCallback(BasicTimeout *t, void *data);
};
