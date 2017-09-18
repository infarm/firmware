#include <Arduino.h>
#include "Controllino.h"

#include "dosing_pump_controller.h"
#include "infarm_utils.h"

#define MAX_PUMPS 6
#define MIN_PUMP_NUM 0
#define MAX_PUMP_NUM 5
#define TIMER_PUMPS_OFF_HANDLER_INTERVAL_MS 500

enum PumpPins {
	PumpPinWaterValveIntake = CONTROLLINO_D1,
	PumpPinDosing1 = CONTROLLINO_D2,
	PumpPinDosing2 = CONTROLLINO_D3,
	PumpPinDosing3 = CONTROLLINO_D4,
	PumpPinDosing4 = CONTROLLINO_D5,
	PumpPinDosing5 = CONTROLLINO_D6,
};

enum PumpOnLimits {
	PumpOnLimitWaterValveIntake = MINUTES_TO_MILLIS(3),
	PumpOnLimitDosing1 = SECONDS_TO_MILLIS(15),
	PumpOnLimitDosing2 = SECONDS_TO_MILLIS(15),
	PumpOnLimitDosing3 = SECONDS_TO_MILLIS(30),
	PumpOnLimitDosing4 = SECONDS_TO_MILLIS(30),
	PumpOnLimitDosing5 = SECONDS_TO_MILLIS(30),
};

const static uint8_t pump_pins[MAX_PUMPS] = {
	PumpPinWaterValveIntake, PumpPinDosing1, PumpPinDosing2, PumpPinDosing3, PumpPinDosing4, PumpPinDosing5,
};

const static uint32_t pump_on_limits_ms[MAX_PUMPS] = {
	PumpOnLimitWaterValveIntake, PumpOnLimitDosing1, PumpOnLimitDosing2,
	PumpOnLimitDosing3,	  PumpOnLimitDosing4, PumpOnLimitDosing5,
};

const static char *pump_names[MAX_PUMPS] = {
	"PumpWaterValveIntake", "PumpDosing1", "PumpDosing2", "PumpDosing3", "PumpDosing4", "PumpDosing5",
};

static uint32_t pump_off_timeouts[MAX_PUMPS] = { 0 };

void DosingPumpController::init()
{
	setSerialDebuggerLogPrefix("DPC: ");

	for (uint8_t i = pumpMinNumber(); i <= pumpMaxNumber(); i++) {
		pinMode(pumpDigitalPin(i), OUTPUT);
	}

	timerPumpsOffHandler.init(TIMER_PUMPS_OFF_HANDLER_INTERVAL_MS);
}

void DosingPumpController::allPumpsOff()
{
	for (uint8_t i = pumpMinNumber(); i <= pumpMaxNumber(); i++) {
		pumpOff(i);
	}
}

uint8_t DosingPumpController::pumpMinNumber()
{
	return MIN_PUMP_NUM;
}

uint8_t DosingPumpController::pumpMaxNumber()
{
	return MAX_PUMP_NUM;
}

const char *DosingPumpController::pumpName(enum DosingPump pump)
{
	return pumpName((uint8_t)pump);
}

const char *DosingPumpController::pumpName(uint8_t n)
{
	if (n > pumpMaxNumber() || n < pumpMinNumber())
		return NULL;

	return pump_names[n];
}

bool DosingPumpController::pumpOn(uint8_t n)
{
	if (n > pumpMaxNumber())
		return false;

	pump_off_timeouts[n] = 0;
	digitalWrite(pumpDigitalPin(n), HIGH);
	return true;
}

bool DosingPumpController::pumpOn(enum DosingPump n)
{
	return pumpOn((uint8_t)n);
}

bool DosingPumpController::pumpOff(enum DosingPump n)
{
	return pumpOff((uint8_t)n);
}

bool DosingPumpController::pumpOff(uint8_t n)
{
	if (n > pumpMaxNumber())
		return false;

	pump_off_timeouts[n] = 0;
	digitalWrite(pumpDigitalPin(n), LOW);
	return true;
}

bool DosingPumpController::pumpIsOn(uint8_t n)
{
	if (n > pumpMaxNumber())
		return false;

	return digitalRead(pumpDigitalPin(n));
}

bool DosingPumpController::pumpIsOn(enum DosingPump n)
{
	return pumpIsOn((uint8_t)n);
}

int DosingPumpController::pumpDigitalPin(uint8_t n)
{
	if (n > pumpMaxNumber())
		return -1;

	return pump_pins[n];
}

int DosingPumpController::pumpDigitalPin(enum DosingPump n)
{
	return pumpDigitalPin((uint8_t)n);
}

int DosingPumpController::pumpState(uint8_t n)
{
	if (n > pumpMaxNumber())
		return -1;

	return digitalRead(pumpDigitalPin(n));
}

int DosingPumpController::pumpState(enum DosingPump n)
{
	return pumpState((uint8_t)n);
}

void DosingPumpController::pumpDosingOneOn()
{
	pumpOn(PumpDosing1);
}

void DosingPumpController::pumpDosingOneOff()
{
	pumpOff(PumpDosing1);
}

void DosingPumpController::pumpDosingTwoOn()
{
	pumpOn(PumpDosing2);
}

void DosingPumpController::pumpDosingTwoOff()
{
	pumpOff(PumpDosing2);
}

void DosingPumpController::pumpDosingThreeOn()
{
	pumpOn(PumpDosing3);
}

void DosingPumpController::pumpDosingThreeOff()
{
	pumpOff(PumpDosing3);
}

void DosingPumpController::pumpDosingFourOn()
{
	pumpOn(PumpDosing4);
}

void DosingPumpController::pumpDosingFourOff()
{
	pumpOff(PumpDosing4);
}

void DosingPumpController::pumpDosingFiveOn()
{
	pumpOn(PumpDosing5);
}

void DosingPumpController::pumpDosingFiveOff()
{
	pumpOff(PumpDosing5);
}

void DosingPumpController::pumpWaterValveIntakeOn()
{
	pumpOn(PumpWaterValveIntake);
}

void DosingPumpController::pumpWaterValveIntakeOff()
{
	pumpOff(PumpWaterValveIntake);
}

bool DosingPumpController::pumpOffAfterTimeout(enum DosingPump n, uint32_t timeout)
{
	return pumpOffAfterTimeout((uint8_t)n, timeout);
}

bool DosingPumpController::pumpOnTimeoutWithinLimit(enum DosingPump n, uint32_t timeout)
{
	return pumpOnTimeoutWithinLimit((uint8_t)n, timeout);
}

bool DosingPumpController::pumpOnTimeoutWithinLimit(uint8_t n, uint32_t timeout)
{
	uint32_t limit = pump_on_limits_ms[n];
	if (timeout <= limit)
		return true;

	debugMessage(F("Pump's %s off timeout %lu is over %lu ms limit, ignoring"), pumpName(n), timeout, limit);
	return false;
}

bool DosingPumpController::pumpOffAfterTimeout(uint8_t n, uint32_t timeout)
{
	if (n > pumpMaxNumber())
		return false;

	pump_off_timeouts[n] = millis() + timeout;
	debugMessage(F("Will turn %s pump off after %lu ms of activity"), pumpName(n), timeout);
	return true;
}

bool DosingPumpController::pumpOnAndOffAfterTimeoutWithLimits(enum DosingPump n, uint32_t timeout)
{
	if (pumpIsOn(n)) {
		debugMessage(F("Not starting %s pump, it's already on, remaining ON time %u ms"), pumpName(n),
			     pumpOnRemainingTime(n));
		return false;
	}

	if (!pumpOnTimeoutWithinLimit(n, timeout))
		return false;

	debugMessage(F("Starting %s pump with off time %u ms"), pumpName(n), timeout);
	pumpOn(n);
	pumpOffAfterTimeout(n, timeout);

	return true;
}

void DosingPumpController::pumpDosingOneOffAfterTimeout(uint32_t timeout)
{
	pumpOffAfterTimeout(PumpPinDosing1, timeout);
}

void DosingPumpController::pumpDosingTwoOffAfterTimeout(uint32_t timeout)
{
	pumpOffAfterTimeout(PumpPinDosing2, timeout);
}

void DosingPumpController::pumpDosingThreeOffAfterTimeout(uint32_t timeout)
{
	pumpOffAfterTimeout(PumpPinDosing3, timeout);
}

void DosingPumpController::pumpDosingFourOffAfterTimeout(uint32_t timeout)
{
	pumpOffAfterTimeout(PumpPinDosing4, timeout);
}

void DosingPumpController::pumpDosingFiveOffAfterTimeout(uint32_t timeout)
{
	pumpOffAfterTimeout(PumpPinDosing5, timeout);
}

void DosingPumpController::pumpWaterValveIntakeOffAfterTimeout(uint32_t timeout)
{
	pumpOffAfterTimeout(PumpWaterValveIntake, timeout);
}

uint32_t DosingPumpController::pumpOnRemainingTime(enum DosingPump n)
{
	return pumpOnRemainingTime((uint8_t)n);
}

uint32_t DosingPumpController::pumpOnRemainingTime(uint8_t n)
{
	uint32_t timeout = pump_off_timeouts[n];
	if (timeout == 0)
		return 0;

	uint32_t now = millis();
	if (now > timeout)
		return 0;

	return timeout - now;
}

void DosingPumpController::handlePumpsOffAfterTimeout()
{
	if (!timerPumpsOffHandler.timeoutHasPassed())
		return;

	timerPumpsOffHandler.restart();

	uint32_t now = millis();
	for (uint8_t i = pumpMinNumber(); i <= pumpMaxNumber(); i++) {
		uint32_t timeout = pump_off_timeouts[i];
		if (timeout > 0 && now > timeout) {
			debugMessage(F("Turning %s pump off after timeout"), pumpName(i));
			pumpOff(i);
		}
	}
}

void DosingPumpController::tick()
{
	timerPumpsOffHandler.tick();
	handlePumpsOffAfterTimeout();
}
