#include <Arduino.h>
#include "Controllino.h"

#include "switch_controller.h"
#include "light_controller.h"
#include "relay_controller.h"
#include "farmbrain_watchdog.h"
#include "modbus_controller.h"

#if DEVICE_CONTROLLINO
#include <Controllino.h>
#define FB_PSU_PIN CONTROLLINO_SCREW_TERMINAL_DIGITAL_OUT_00
#define SENSOR_PRESSURE_WATER_TANK_PIN CONTROLLINO_PIN_HEADER_ANALOG_ADC_IN_05
#endif

#if DEVICE_ARDUINO_MEGA
#define FB_PSU_PIN 22
#define SENSOR_PRESSURE_WATER_TANK_PIN A5
#endif

#define FB_WATCHDOG_TIMEOUT MINUTES_TO_MILLIS(3)

void ModbusController::init(RelayController *r, LightController *d, DosingPumpController *p, SwitchController *s)
{
	setSerialDebuggerLogPrefix("MC: ");

	rc = r;
	dc = d;
	dp = p;
	sc = s;

	slave.enableDebug();
	slave.begin(115200);
	slave.setRegisters(&cr);

	wdt.init(FB_WATCHDOG_TIMEOUT, FB_PSU_PIN);
	wdt.enableDebug();
	wdt.feed();
}

void ModbusController::handleSinglePump(enum DosingPump n, uint16_t timeout)
{
	if (sc->isMaintenanceModeActive()) {
		debugMessage(F("Maintenance mode active, ignoring pump %s ON request"), cr.pumpName(n));
		return;
	}

	cr.pumpSetTimerValue(n, 0);
	dp->pumpOnAndOffAfterTimeoutWithLimits(n, timeout);
}

void ModbusController::handlePumps()
{
	for (int n = cr.pumpFirstIndex(); n <= cr.pumpLastIndex(); n++) {
		uint16_t timeout = cr.pumpGetTimerValue((enum DosingPump)n);
		if (timeout > 0) {
			handleSinglePump((enum DosingPump)n, timeout);
		}
	}
}

void ModbusController::handleRealTimeClock()
{
	uint8_t hours = 0;
	uint8_t minutes = 0;

	if (cr.realTimeClockValueGet(&hours, &minutes) > 0) {
		debugMessage(F("RTC setting time to %02d:%02d"), hours, minutes);
		/* TODO: Error checking?! */
		Controllino_SetTimeDate(2, 17, 8, 17, hours, minutes, 0);

		hours = 0;
		minutes = 0;
		cr.realTimeClockValueSet(hours, minutes);

		if (Controllino_ReadTimeDate(0, 0, 0, 0, &hours, &minutes, 0) >= 0) {
			debugMessage(F("RTC time is now %02d:%02d"), hours, minutes);
		}
	}
}

void ModbusController::handleWatchdog()
{
	if (cr.watchdogWasFed()) {
		debugMessage(F("Watchdog was fed"));
		wdt.feed();
		cr.makeWatchdogHungry();
	}
}

void ModbusController::handleLightSchedule()
{
	uint8_t hour = 0;
	uint8_t intensity = 0;

	if (cr.lightScheduleWriteIsRequested()) {
		cr.lightScheduleValueGet(&hour, &intensity);
		debugMessage(F("Setting light schedule hour=%02d intensity=%d%%"), hour, intensity);
		dc->setLightIntensityPercentForHourAndStore(hour, intensity);

		hour = 0;
		intensity = 0;
		cr.lightScheduleValueSet(hour, intensity);
		cr.clearLightScheduleWriteRequest();
	}

	if (cr.lightScheduleReadIsRequested()) {
		hour = 0;
		intensity = 0;
		cr.lightScheduleValueGet(&hour, &intensity);
		intensity = dc->lightIntensityPercentForHour(hour);
		debugMessage(F("Replying with light schedule hour=%02d intensity=%d%%"), hour, intensity);
		cr.lightScheduleValueSet(hour, intensity);
		cr.clearLightScheduleReadRequest();
		cr.setLightScheduleDataAreReady();
	}
}

uint16_t ModbusController::waterTankPressureSensorValue()
{
	return analogRead(SENSOR_PRESSURE_WATER_TANK_PIN);
}

void ModbusController::handleWaterTankPressureSensor()
{
	cr.waterTankPressureSensorValueSet(waterTankPressureSensorValue());
}

void ModbusController::handleMaintenanceModeState()
{
	if (sc->isMaintenanceModeActive())
		cr.maintenanceModeEnable();
	else
		cr.maintenanceModeDisable();
}

void ModbusController::handleMaintenanceModeStateOverride()
{
	if (cr.maintenanceModeShouldBeEnabledByOverride()) {
		cr.clearMaintenanceModeEnableViaOverride();
		if (sc->isMaintenanceModeActive())
			return;

		debugMessage(F("Invoking maintenance mode via override"));
		sc->maintenanceModeEnable();
	}

	if (cr.maintenanceModeShouldBeDisabledByOverride()) {
		cr.clearMaintenanceModeDisableViaOverride();
		if (!sc->isMaintenanceModeActive())
			return;

		debugMessage(F("Disabling maintenance mode via override"));
		sc->maintenanceModeDisable();
	}

	handleMaintenanceModeState();
}

bool ModbusController::newMessageReceivedOnModbus()
{
	int r = slave.poll();
	if (r == 0)
		return false;

	if (r < 4) {
		debugMessage(F("bus poll err r=%d"), r);
		return false;
	}

	return true;
}

void ModbusController::tick()
{
	wdt.tick();
	handleWaterTankPressureSensor();
	handleMaintenanceModeState();

	if (!newMessageReceivedOnModbus())
		return;

	handlePumps();
	handleWatchdog();
	handleRealTimeClock();
	handleLightSchedule();
	handleMaintenanceModeStateOverride();
}
