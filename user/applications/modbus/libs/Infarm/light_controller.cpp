#include <stdio.h>
#include <Controllino.h>

#include "relay_controller.h"
#include "light_controller.h"

#define PWM_LIGHT_DIMMING CONTROLLINO_SCREW_TERMINAL_DIGITAL_OUT_11
#define LED_OPERATION_MODE_MAINTENANCE CONTROLLINO_SCREW_TERMINAL_DIGITAL_OUT_09
#define LED_OPERATION_MODE_NORMAL CONTROLLINO_SCREW_TERMINAL_DIGITAL_OUT_10

void LightController::init(RelayController *rc)
{
	relayController = rc;

	setSerialDebuggerLogPrefix("LC: ");
	Controllino_RTC_init();
	setLightDimInPercents(0);
	pinMode(PWM_LIGHT_DIMMING, OUTPUT);
	pinMode(LED_OPERATION_MODE_NORMAL, OUTPUT);
	pinMode(LED_OPERATION_MODE_MAINTENANCE, OUTPUT);
	setLightDimInPercents(0);
	digitalWrite(LED_OPERATION_MODE_NORMAL, LOW);
	digitalWrite(LED_OPERATION_MODE_MAINTENANCE, LOW);
	timerIntensitySmoother.init(lightIntensitySmootherIntervalMs);
	timerScheduleCheck.init(lightScheduleCheckIntervalMs);
	timerSerialLogFloodProtection.init(serialLogMessageIntervalMs);
}

void LightController::setLightDimInPercents(uint8_t percents)
{
	intensityPercents = percents;
#ifdef DEVICE_ARDUINO_MEGA
	pwmValue = map(percents, 0, 100, 0, 255);
#elif DEVICE_CONTROLLINO
	/*
	 * This works as expected on Arduino Mega, but on Controllino you can
	 * expect following on pin D0, D1 (it doesn't work on pins D9 and D11):
	 *
	 * duty 0% = Off
	 *      1% = 16% PWM on output, Vpp=11.1V, f=490Hz
	 *     30% = 50% PWM on output, Vpp=11.3V, f=490Hz
	 *     50% = 70% PWM on output, Vpp=10.7V, f=490Hz
	 *     80% = 93% PWM on output, Vpp=5.52V, f=493Hz Unstable!
	 *
	 * with the fixed mapping you can expect following on Controllino D0:
	 *
	 * duty 0% = Off
	 *      1% = 12% PWM on output, Vpp=10.2V, f=490Hz
	 *     30% = 38% PWM on output, Vpp=11.4V, f=490Hz
	 *     50% = 51% PWM on output, Vpp=11.2V, f=490Hz
	 *     80% = 69% PWM on output, Vpp=10.7V, f=493Hz
	 *     100% = 80% PWM on output, Vpp=9.68V, f=490Hz
	 */
	pwmValue = map(percents, 100, 0, 0, (255 / 100) * 79);
#else
#error "Please add your device mapping, but test it very carefully! (TM)"
#endif

	analogWrite(PWM_LIGHT_DIMMING, pwmValue);
}

void LightController::lightsOn()
{
	relayController->relayLightBallastOn();
	relayController->relayMotorBallastOn();
}

void LightController::lightsOff()
{
	relayController->relayLightBallastOff();
	relayController->relayMotorBallastOff();
}

bool LightController::isMaintenanceModeBeingSignaled()
{
	return maintenanceMode == true;
}

void LightController::signalMaintenanceOperationMode()
{
	maintenanceMode = true;
	digitalWrite(LED_OPERATION_MODE_NORMAL, LOW);
	digitalWrite(LED_OPERATION_MODE_MAINTENANCE, HIGH);
	debugMessage(F("Signaling maintenance operation mode"));
}

void LightController::signalNormalOperationMode()
{
	maintenanceMode = false;
	digitalWrite(LED_OPERATION_MODE_MAINTENANCE, LOW);
	digitalWrite(LED_OPERATION_MODE_NORMAL, HIGH);
	debugMessage(F("Signaling normal operation mode"));
}

bool LightController::isOpenDoorsModeOngoing()
{
	return openDoorsMode == true;
}

void LightController::enableOpenDoorsMode()
{
	openDoorsMode = true;
	ensureStaticLightSchedule();
}

void LightController::disableOpenDoorsMode()
{
	openDoorsMode = false;
	ensureStaticLightSchedule();
}

uint8_t LightController::lightsIntensityValueForCurrentSchedule()
{
	uint8_t hour = 25;
	uint8_t minute = 61;
	uint8_t intensity;
	static const uint8_t schedule[24] = { 80,  0,  0,  0,   0,  0,  0,  80, 80, 80, 80, 80,
					      80, 80, 90, 100, 90, 80, 80, 80, 80, 80, 80, 80 };

	int ret = Controllino_ReadTimeDate(NULL, NULL, NULL, NULL, &hour, &minute, NULL);
	if (ret < 0 || hour == 25 || minute == 61) {
		debugError("Unable to read RTC!");
		return 0;
	}

	intensity = schedule[hour];
	debugMessage(F("Light intensity for %dh is %d%%"), hour, intensity);
	return intensity;
}

void LightController::ensureStaticLightSchedule()
{
	if (isOpenDoorsModeOngoing()) {
		setLightDimInPercentsSmoothed(50);
		if (timerSerialLogFloodProtection.timeoutHasPassed()) {
			timerSerialLogFloodProtection.restart();
			debugMessage(F("Not performing light scheduling, doors are open"));
		}
		return;
	}

	uint8_t intensity = lightsIntensityValueForCurrentSchedule();
	if (intensity > 0) {
		debugMessage(F("Lights on"));
		lightsOn();
		setLightDimInPercentsSmoothed(intensity);
		return;
	}

	debugMessage(F("Lights off"));
	lightsOff();
	setLightDimInPercentsSmoothed(0);
}

void LightController::ensureStaticLightSchedulePeriodically()
{
	if (!timerScheduleCheck.timeoutHasPassed())
		return;

	timerScheduleCheck.restart();
	ensureStaticLightSchedule();
}

void LightController::setLightDimInPercentsSmoothed(uint8_t percents)
{
	intensityPercentsSmoothedEnd = percents;
	intensityPercentsSmoothedStart = intensityPercents;
	handleLightIntensitySmoothing();
}

void LightController::handleLightIntensitySmoothing()
{
	if (!timerIntensitySmoother.timeoutHasPassed())
		return;

	if (intensityPercents == intensityPercentsSmoothedEnd)
		return;

	intensityPercentsSmoothedStep += 1;
	uint8_t percents =
	    map(intensityPercentsSmoothedStep, 0, 100, intensityPercentsSmoothedStart, intensityPercentsSmoothedEnd);

	if (intensityPercentsSmoothedStep >= 100) {
		intensityPercentsSmoothedStep = 0;
		percents = intensityPercentsSmoothedEnd;
	}

	debugMessage(F("Smoothing intensity %% current: %d target: %d next step: %d"), intensityPercents,
		     intensityPercentsSmoothedEnd, percents);
	setLightDimInPercents(percents);
	timerIntensitySmoother.restart();
}

void LightController::tick()
{
	timerScheduleCheck.tick();
	timerIntensitySmoother.tick();
	timerSerialLogFloodProtection.tick();
	handleLightIntensitySmoothing();
	ensureStaticLightSchedulePeriodically();
}
