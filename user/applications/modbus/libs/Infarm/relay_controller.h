#pragma once

#include <stdint.h>
#include "serial_debugger.h"

class RelayController : public SerialDebugger
{
      public:
	void init();
	void allRelaysOff();
	bool relayOn(uint8_t n);
	bool relayOff(uint8_t n);
	uint8_t relayMinNumber();
	uint8_t relayMaxNumber();
	int relayState(uint8_t n);
	int relayDigitalPin(uint8_t n);
	void relayLightBallastOn();
	void relayLightBallastOff();
	void relayIrrigationPumpOn();
	void relayIrrigationPumpOff();
	void relayCirculationPumpOn();
	void relayCirculationPumpOff();
	void relayHotBoxFansOn();
	void relayHotBoxFansOff();
	void relayClimateFansOn();
	void relayClimateFansOff();
	void relayWaterCoolerOn();
	void relayWaterCoolerOff();
	bool relayMotorBallastOn();
	void relayMotorBallastOff();
	void enableOpenDoorsMode();
	void disableOpenDoorsMode();
	bool isOpenDoorsModeActive();

      private:
	bool openDoorsMode = false;
};
