#include <Arduino.h>
#include "infarm_utils.h"
#include "farmbrain_watchdog.h"

#define WDT_RESET_PIN_LOW_TIME_MS 2 * 1000

void FarmBrainWatchdog::init(uint32_t timeoutMs, uint8_t pin)
{
	resetPin = pin;
	setSerialDebuggerLogPrefix("WDT: ");
	pinMode(resetPin, OUTPUT);
	digitalWrite(resetPin, HIGH);
	timerWatchdogExpiredCheck.init(timeoutMs);
	timerPowerOnFarmBrainAgain.init(WDT_RESET_PIN_LOW_TIME_MS, farmBrainPowerOnAgainTimerCallback, this);
}

void FarmBrainWatchdog::farmBrainPowerOnAgainTimerCallback(BasicTimeout *t, void *data)
{
	(void)t;
	FarmBrainWatchdog *w = (FarmBrainWatchdog *)data;
	w->powerOnFarmBrain();
}

bool FarmBrainWatchdog::tick()
{
	timerWatchdogExpiredCheck.tick();
	timerPowerOnFarmBrainAgain.tick();

	if (!timerPowerOnFarmBrainAgain.isStopped())
		return true;

	if (!timerWatchdogExpiredCheck.timeoutHasPassed())
		return true;

	debugMessage(F("Timer expired, shutting down FarmBrain"));
	digitalWrite(resetPin, LOW);
	timerPowerOnFarmBrainAgain.restart();
	return false;
}

void FarmBrainWatchdog::feed()
{
	timerWatchdogExpiredCheck.restart();
}

void FarmBrainWatchdog::powerOnFarmBrain()
{
	debugMessage(F("Powering on FarmBrain again"));
	digitalWrite(resetPin, HIGH);
	feed();
}
