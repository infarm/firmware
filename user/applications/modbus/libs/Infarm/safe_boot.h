#pragma once

#include <stdint.h>
#include <stdbool.h>

class LightController;
class RelayController;

class SafeBoot
{
      public:
	void init(LightController *c, RelayController *r);
	void allRelaysOff();
	void relaysR1toR5On();
	bool lightsScheduled();
	bool checkSchedule();

      private:
	uint8_t _resetPin;
	LightController *_lightCtrl;
	RelayController *_relayCtrl;
};
