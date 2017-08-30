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
#endif

#if DEVICE_ARDUINO_MEGA
#define FB_PSU_PIN 22
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
	cr.pumpSetTimerValue((enum DosingPump)n, 0);

	if (dp->pumpIsOn(n)) {
		debugMessage(F("Not starting %s pump, it's already on, remaining ON time %u ms"),
			     cr.pumpName((enum DosingPump)n), dp->pumpOnRemainingTime(n));
		return;
	}

	debugMessage(F("Starting %s pump with off time %u ms"), cr.pumpName((enum DosingPump)n), timeout);
	dp->pumpOn(n);
	dp->pumpOffAfterTimeout(n, timeout);
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

	if (!newMessageReceivedOnModbus())
		return;

	handlePumps();
	handleWatchdog();
	handleRealTimeClock();
}
