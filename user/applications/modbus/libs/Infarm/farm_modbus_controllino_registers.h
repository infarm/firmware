#pragma once

#include <stdint.h>

#include "farm_modbus_registers.h"

/* TODO: Where to move this?! It's shared with DosingPumpController */
enum DosingPump {
	PumpWaterValveIntake,
	PumpDosing1,
	PumpDosing2,
	PumpDosing3,
	PumpDosing4,
	PumpDosing5,
};

class FarmModbusControllinoRegisters
{
      public:
	void resetAll();
	uint8_t lastPossibleRegisterAddress();
	uint8_t firstPossibleRegisterAddress();
	uint16_t *blockOfRegisters();
	uint8_t blockOfRegistersSize();

	/* pumps */
	uint8_t pumpFirstIndex();
	uint8_t pumpLastIndex();
	void pumpStart(enum DosingPump n);
	void pumpStop(enum DosingPump n);
	const char *pumpName(enum DosingPump n);
	uint16_t pumpGetTimerValue(enum DosingPump n);
	uint16_t pumpSetTimerValue(enum DosingPump n, uint16_t newValue);
	uint8_t pumpTimerRegisterAddress(enum DosingPump n);
	uint8_t pumpStateRegisterAddress(enum DosingPump n);
	uint16_t *registerPumpTimer(enum DosingPump n);
	/* watchdog/time synchronization */
	uint16_t *registerWatchdogFeed();
	uint16_t *registerRealTimeClock();
	uint8_t realTimeClockRegisterAddress();
	uint8_t watchdogFeedRegisterAddress();
	bool feedTheWatchdog();
	bool watchdogWasFed();
	bool makeWatchdogHungry();
	uint16_t realTimeClockValueSet(uint8_t hours, uint8_t minutes);
	uint16_t realTimeClockValueGet(uint8_t *hours, uint8_t *minutes);

      protected:
	uint8_t pumpIndexToRegisterIndex(uint8_t start, enum DosingPump n);
	static const uint8_t lastRegisterAddress = 0x30;
	uint16_t regs[lastRegisterAddress+1] = { 0 };
};
