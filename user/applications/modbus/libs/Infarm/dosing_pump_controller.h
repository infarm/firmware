#pragma once

#include <stdint.h>

#include "basic_timeout.h"
#include "serial_debugger.h"
#include "farm_modbus_controllino_registers.h" /* For enum DosingPump */

class DosingPumpController : public SerialDebugger
{
      public:
	void init();
	void allPumpsOff();
	bool pumpOn(uint8_t n);
	bool pumpOn(enum DosingPump pump);
	bool pumpOff(enum DosingPump pump);
	bool pumpOff(uint8_t n);
	bool pumpIsOn(uint8_t n);
	bool pumpIsOn(enum DosingPump pump);
	uint8_t pumpMinNumber();
	uint8_t pumpMaxNumber();
	int pumpState(uint8_t n);
	int pumpState(enum DosingPump pump);
	int pumpDigitalPin(uint8_t n);
	int pumpDigitalPin(enum DosingPump pump);
	const char *pumpName(uint8_t n);
	const char *pumpName(enum DosingPump pump);
	void pumpDosingOneOn();
	void pumpDosingOneOff();
	void pumpDosingOneOffAfterTimeout(uint32_t timeout);
	void pumpDosingTwoOn();
	void pumpDosingTwoOff();
	void pumpDosingTwoOffAfterTimeout(uint32_t timeout);
	void pumpDosingThreeOn();
	void pumpDosingThreeOff();
	void pumpDosingThreeOffAfterTimeout(uint32_t timeout);
	void pumpDosingFourOn();
	void pumpDosingFourOff();
	void pumpDosingFourOffAfterTimeout(uint32_t timeout);
	void pumpDosingFiveOn();
	void pumpDosingFiveOff();
	void pumpDosingFiveOffAfterTimeout(uint32_t timeout);
	void pumpWaterValveIntakeOn();
	void pumpWaterValveIntakeOff();
	void pumpWaterValveIntakeOffAfterTimeout(uint32_t timeout);
	bool pumpOnTimeoutWithinLimit(uint8_t n, uint32_t timeout);
	bool pumpOnTimeoutWithinLimit(enum DosingPump n, uint32_t timeout);
	uint32_t pumpOnRemainingTime(enum DosingPump n);
	uint32_t pumpOnRemainingTime(uint8_t n);
	bool pumpOffAfterTimeout(enum DosingPump n, uint32_t timeout);
	bool pumpOffAfterTimeout(uint8_t n, uint32_t timeout);
	bool pumpOnAndOffAfterTimeoutWithLimits(enum DosingPump n, uint32_t timeout);
	void handlePumpsOffAfterTimeout();
	void tick();

      private:
	BasicTimeout timerPumpsOffHandler;
};
