#include <string.h>
#include "farm_modbus_controllino_registers.h"

enum ControllinoRegisterAddress {
	/* pumps */
	RegisterAddressPumpWaterValveIntakeTimer = 0x10,
	RegisterAddressPumpDosing1Timer = 0x12,
	RegisterAddressPumpDosing2Timer = 0x14,
	RegisterAddressPumpDosing3Timer = 0x16,
	RegisterAddressPumpDosing4Timer = 0x18,
	RegisterAddressPumpDosing5Timer = 0x1A,
	/* watchdog/time sync */
	RegisterAddressRealTimeClock = 0x20,
	RegisterAddressWatchdogFeed = 0x30,
};

enum WatchdogFeedingStatus {
	WatchdogIsHungry,
	WatchdogWasFed,
};

void FarmModbusControllinoRegisters::resetAll()
{
	memset(regs, 0, sizeof(regs));
}

uint8_t FarmModbusControllinoRegisters::pumpFirstIndex()
{
	return PumpWaterValveIntake;
}

uint8_t FarmModbusControllinoRegisters::pumpLastIndex()
{
	return PumpDosing5;
}

/* TODO: common code with DosingPumpController detected */
const char *FarmModbusControllinoRegisters::pumpName(enum DosingPump n)
{
	const static char *pump_names[] = {
		"PumpWaterValveIntake", "PumpDosing1", "PumpDosing2", "PumpDosing3", "PumpDosing4", "PumpDosing5",
	};

	return pump_names[n];
}

uint8_t FarmModbusControllinoRegisters::firstPossibleRegisterAddress()
{
	return RegisterAddressPumpWaterValveIntakeTimer;
}

uint8_t FarmModbusControllinoRegisters::lastPossibleRegisterAddress()
{
	return RegisterAddressWatchdogFeed;
}

uint8_t FarmModbusControllinoRegisters::blockOfRegistersSize()
{
	return lastRegisterAddress;
}

uint16_t *FarmModbusControllinoRegisters::blockOfRegisters()
{
	return regs;
}

uint8_t FarmModbusControllinoRegisters::pumpIndexToRegisterIndex(uint8_t start, enum DosingPump n)
{
	return start + (2 * n);
}

uint16_t *FarmModbusControllinoRegisters::registerPumpTimer(enum DosingPump n)
{
	uint8_t registerIndex = pumpIndexToRegisterIndex(RegisterAddressPumpWaterValveIntakeTimer, n);
	return &regs[registerIndex];
}

uint8_t FarmModbusControllinoRegisters::pumpTimerRegisterAddress(enum DosingPump n)
{
	return pumpIndexToRegisterIndex(RegisterAddressPumpWaterValveIntakeTimer, n);
}

uint16_t FarmModbusControllinoRegisters::pumpGetTimerValue(enum DosingPump n)
{
	return *registerPumpTimer(n);
}

uint16_t FarmModbusControllinoRegisters::pumpSetTimerValue(enum DosingPump n, uint16_t newValue)
{
	uint16_t oldValue = pumpGetTimerValue(n);
	*registerPumpTimer(n) = newValue;
	return oldValue;
}

uint16_t *FarmModbusControllinoRegisters::registerRealTimeClock()
{
	return &regs[RegisterAddressRealTimeClock];
}

uint16_t *FarmModbusControllinoRegisters::registerWatchdogFeed()
{
	return &regs[RegisterAddressWatchdogFeed];
}

uint16_t FarmModbusControllinoRegisters::realTimeClockValueSet(uint8_t hours, uint8_t minutes)
{
	uint16_t oldValue = *registerRealTimeClock();
	uint16_t newValue = ((hours << 8) & 0xff00) | (minutes & 0xff);
	*registerRealTimeClock() = newValue;
	return oldValue;
}

uint16_t FarmModbusControllinoRegisters::realTimeClockValueGet(uint8_t *hours, uint8_t *minutes)
{
	uint16_t time = *registerRealTimeClock();
	*hours = (time >> 8) & 0xff;
	*minutes = time & 0xff;
	return time;
}

uint8_t FarmModbusControllinoRegisters::realTimeClockRegisterAddress()
{
	return RegisterAddressRealTimeClock;
}

uint8_t FarmModbusControllinoRegisters::watchdogFeedRegisterAddress()
{
	return RegisterAddressWatchdogFeed;
}

bool FarmModbusControllinoRegisters::feedTheWatchdog()
{
	bool oldValue = (*registerWatchdogFeed() == WatchdogWasFed);
	*registerWatchdogFeed() = WatchdogWasFed;
	return oldValue;
}

bool FarmModbusControllinoRegisters::watchdogWasFed()
{
	return *registerWatchdogFeed() == WatchdogWasFed;
}

bool FarmModbusControllinoRegisters::makeWatchdogHungry()
{
	bool oldValue = (*registerWatchdogFeed() == WatchdogWasFed);
	*registerWatchdogFeed() = WatchdogIsHungry;
	return oldValue;
}
