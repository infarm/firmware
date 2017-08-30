#pragma once

#include <stdint.h>
#include "basic_timeout.h"

class HeartBeat
{
      public:
	HeartBeat();
	HeartBeat(uint8_t pin, uint16_t intervalMs);
	void init(uint8_t pin, uint16_t intervalMs);
	bool pinIsOn();
	void tick();

      private:
	bool pinOn = false;
	uint8_t digitalPin;
	uint16_t intervalMs;
	BasicTimeout timer;
	void pinToggleState();
};
