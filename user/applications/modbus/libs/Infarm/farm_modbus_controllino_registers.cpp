#include <string.h>
#include "farm_modbus_controllino_registers.h"

#ifndef SKETCH_VERSION_NUMBER
#define SKETCH_VERSION_NUMBER 100
#endif

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
	RegisterAddressWaterTankPressureSensor = 0x34,
	RegisterAddressControlRegister = 0x36,
	RegisterAddressFirmwareVersion = 0x6f,
	RegisterAddressPeripheryStatus = 0x42,
	/* light schedule */
	RegisterAddressLightScheduleControl = 0x70,
	RegisterAddressLightScheduleValue = 0x71,
};

enum WatchdogFeedingStatus {
	WatchdogIsHungry,
	WatchdogWasFed,
};

enum ControlRegisterBits {
	ControlRegisterBitOverrideMaintenanceModeEnable = 1 << 1,
	ControlRegisterBitOverrideMaintenanceModeDisable = 1 << 0,
};

enum PeripheryStatusRegisterBits {
	PeripheryStatusBitMaintenanceModeActive = 1 << 0,
};

enum LightScheduleControlRegisterBits {
	LightScheduleControlBitReadRequested = 1 << 0,
	LightScheduleControlBitWriteRequested = 1 << 1,
	LightScheduleControlBitDataAreReady = 1 << 2,
};

FarmModbusControllinoRegisters::FarmModbusControllinoRegisters()
{
	resetAll();
}

void FarmModbusControllinoRegisters::enableModbusMasterView()
{
	usedOnModbusMaster = true;
}

void FarmModbusControllinoRegisters::resetAll()
{
	memset(regs, 0, sizeof(regs));
	if (!usedOnModbusMaster)
		firmwareVersionValueSet(SKETCH_VERSION_NUMBER);
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
	return RegisterAddressLightScheduleValue;
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

uint16_t *FarmModbusControllinoRegisters::registerLightScheduleValue()
{
	return &regs[RegisterAddressLightScheduleValue];
}

uint16_t *FarmModbusControllinoRegisters::registerFirmwareVersion()
{
	return &regs[RegisterAddressFirmwareVersion];
}

uint16_t FarmModbusControllinoRegisters::lightScheduleValueSet(uint8_t hour, uint8_t intensity)
{
	uint16_t oldValue = *registerLightScheduleValue();
	uint16_t newValue = ((hour << 8) & 0xff00) | (intensity & 0xff);
	*registerLightScheduleValue() = newValue;
	return oldValue;
}

uint16_t FarmModbusControllinoRegisters::lightScheduleValueGet(uint8_t *hour, uint8_t *intensity)
{
	uint16_t c = *registerLightScheduleValue();
	*hour = (c >> 8) & 0xff;
	*intensity = c & 0xff;
	return c;
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

uint8_t FarmModbusControllinoRegisters::lightScheduleValueRegisterAddress()
{
	return RegisterAddressLightScheduleValue;
}

uint8_t FarmModbusControllinoRegisters::firmwareVersionRegisterAddress()
{
	return RegisterAddressFirmwareVersion;
}

uint16_t FarmModbusControllinoRegisters::firmwareVersionValueGet()
{
	return *registerFirmwareVersion();
}

uint16_t FarmModbusControllinoRegisters::firmwareVersionValueSet(uint16_t version)
{
	uint16_t oldValue = *registerFirmwareVersion();
	*registerFirmwareVersion() = version;
	return oldValue;
}

uint16_t *FarmModbusControllinoRegisters::registerWaterTankPressureSensor()
{
	return &regs[RegisterAddressWaterTankPressureSensor];
}

uint8_t FarmModbusControllinoRegisters::waterTankPressureSensorRegisterAddress()
{
	return RegisterAddressWaterTankPressureSensor;
}

uint16_t FarmModbusControllinoRegisters::waterTankPressureSensorValueGet()
{
	return *registerWaterTankPressureSensor();
}

uint16_t FarmModbusControllinoRegisters::waterTankPressureSensorValueSet(uint16_t value)
{
	uint16_t oldValue = *registerWaterTankPressureSensor();
	*registerWaterTankPressureSensor() = value;
	return oldValue;
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

uint16_t *FarmModbusControllinoRegisters::registerPeripheryStatus()
{
	return &regs[RegisterAddressPeripheryStatus];
}

uint16_t *FarmModbusControllinoRegisters::registerControlRegister()
{
	return &regs[RegisterAddressControlRegister];
}

uint8_t FarmModbusControllinoRegisters::peripheryStatusRegisterAddress()
{
	return RegisterAddressPeripheryStatus;
}

uint8_t FarmModbusControllinoRegisters::controlRegisterAddress()
{
	return RegisterAddressControlRegister;
}

bool FarmModbusControllinoRegisters::maintenanceModeIsActive()
{
	return (*registerPeripheryStatus() & PeripheryStatusBitMaintenanceModeActive) ==
	       PeripheryStatusBitMaintenanceModeActive;
}

bool FarmModbusControllinoRegisters::maintenanceModeEnable()
{
	return *registerPeripheryStatus() |= PeripheryStatusBitMaintenanceModeActive;
}

bool FarmModbusControllinoRegisters::maintenanceModeDisable()
{
	return *registerPeripheryStatus() &= ~PeripheryStatusBitMaintenanceModeActive;
}

bool FarmModbusControllinoRegisters::enableMaintenanceModeViaOverride()
{
	return *registerControlRegister() |= ControlRegisterBitOverrideMaintenanceModeEnable;
}

bool FarmModbusControllinoRegisters::maintenanceModeShouldBeEnabledByOverride()
{
	return (*registerControlRegister() & ControlRegisterBitOverrideMaintenanceModeEnable) ==
	       ControlRegisterBitOverrideMaintenanceModeEnable;
}

bool FarmModbusControllinoRegisters::clearMaintenanceModeEnableViaOverride()
{
	return *registerControlRegister() &= ~ControlRegisterBitOverrideMaintenanceModeEnable;
}

bool FarmModbusControllinoRegisters::disableMaintenanceModeViaOverride()
{
	return *registerControlRegister() |= ControlRegisterBitOverrideMaintenanceModeDisable;
}

bool FarmModbusControllinoRegisters::maintenanceModeShouldBeDisabledByOverride()
{
	return (*registerControlRegister() & ControlRegisterBitOverrideMaintenanceModeDisable) ==
	       ControlRegisterBitOverrideMaintenanceModeDisable;
}

bool FarmModbusControllinoRegisters::clearMaintenanceModeDisableViaOverride()
{
	return *registerControlRegister() &= ~ControlRegisterBitOverrideMaintenanceModeDisable;
}

uint16_t *FarmModbusControllinoRegisters::registerLightScheduleControl()
{
	return &regs[RegisterAddressLightScheduleControl];
}

uint8_t FarmModbusControllinoRegisters::lightScheduleControlRegisterAddress()
{
	return RegisterAddressLightScheduleControl;
}

bool FarmModbusControllinoRegisters::lightScheduleWriteIsRequested()
{
	return (*registerLightScheduleControl() & LightScheduleControlBitWriteRequested) ==
	       LightScheduleControlBitWriteRequested;
}

bool FarmModbusControllinoRegisters::lightScheduleReadIsRequested()
{
	return (*registerLightScheduleControl() & LightScheduleControlBitReadRequested) ==
	       LightScheduleControlBitReadRequested;
}

bool FarmModbusControllinoRegisters::lightScheduleDataAreReady()
{
	return (*registerLightScheduleControl() & LightScheduleControlBitDataAreReady) ==
	       LightScheduleControlBitDataAreReady;
}

bool FarmModbusControllinoRegisters::clearLightScheduleReadRequest()
{
	return *registerLightScheduleControl() &= ~LightScheduleControlBitReadRequested;
}

bool FarmModbusControllinoRegisters::clearLightScheduleWriteRequest()
{
	return *registerLightScheduleControl() &= ~LightScheduleControlBitWriteRequested;
}

bool FarmModbusControllinoRegisters::clearLightScheduleDataAreReady()
{
	return *registerLightScheduleControl() &= ~LightScheduleControlBitDataAreReady;
}

bool FarmModbusControllinoRegisters::setLightScheduleWriteRequest()
{
	return *registerLightScheduleControl() |= LightScheduleControlBitWriteRequested;
}

bool FarmModbusControllinoRegisters::setLightScheduleReadRequest()
{
	return *registerLightScheduleControl() |= LightScheduleControlBitReadRequested;
}

bool FarmModbusControllinoRegisters::setLightScheduleDataAreReady()
{
	return *registerLightScheduleControl() |= LightScheduleControlBitDataAreReady;
}
