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
	FarmModbusControllinoRegisters();
	void enableModbusMasterView();
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

	/* light schedules */
	uint16_t *registerLightScheduleValue();
	uint8_t lightScheduleValueRegisterAddress();
	uint16_t *registerLightScheduleControl();
	uint8_t lightScheduleControlRegisterAddress();
	uint16_t lightScheduleValueSet(uint8_t hour, uint8_t intensity);
	uint16_t lightScheduleValueGet(uint8_t *hour, uint8_t *intensity);
	bool lightScheduleWriteIsRequested();
	bool lightScheduleReadIsRequested();
	bool lightScheduleDataAreReady();
	bool clearLightScheduleReadRequest();
	bool clearLightScheduleWriteRequest();
	bool clearLightScheduleDataAreReady();
	bool setLightScheduleWriteRequest();
	bool setLightScheduleReadRequest();
	bool setLightScheduleDataAreReady();

	/* firmware version */
	uint16_t *registerFirmwareVersion();
	uint8_t firmwareVersionRegisterAddress();
	uint16_t firmwareVersionValueGet();
	uint16_t firmwareVersionValueSet(uint16_t version);

	/* water tank pressure sensor */
	uint16_t *registerWaterTankPressureSensor();
	uint8_t waterTankPressureSensorRegisterAddress();
	uint16_t waterTankPressureSensorValueGet();
	uint16_t waterTankPressureSensorValueSet(uint16_t value);

	/* maintenance mode */
	uint16_t *registerPeripheryStatus();
	uint16_t *registerControlRegister();
	uint8_t peripheryStatusRegisterAddress();
	uint8_t controlRegisterAddress();
	/* push button related bits */
	bool maintenanceModeIsActive();
	bool maintenanceModeEnable();
	bool maintenanceModeDisable();
	/* mode overrides by FB/Cloud */
	bool enableMaintenanceModeViaOverride();
	bool maintenanceModeShouldBeEnabledByOverride();
	bool clearMaintenanceModeEnableViaOverride();
	bool disableMaintenanceModeViaOverride();
	bool maintenanceModeShouldBeDisabledByOverride();
	bool clearMaintenanceModeDisableViaOverride();

      protected:
	uint8_t pumpIndexToRegisterIndex(uint8_t start, enum DosingPump n);
	static const uint8_t lastRegisterAddress = 0x71;
	uint16_t regs[lastRegisterAddress + 1] = { 0 };

      private:
	bool usedOnModbusMaster = false;
};
