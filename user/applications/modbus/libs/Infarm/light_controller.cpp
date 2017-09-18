#include <stdio.h>
#include <Controllino.h>
#include <EEPROM.h>

#include "relay_controller.h"
#include "light_controller.h"

#define PWM_LIGHT_DIMMING CONTROLLINO_SCREW_TERMINAL_DIGITAL_OUT_11
#define LED_OPERATION_MODE_RED CONTROLLINO_SCREW_TERMINAL_DIGITAL_OUT_09
#define LED_OPERATION_MODE_GREEN CONTROLLINO_SCREW_TERMINAL_DIGITAL_OUT_10

void LightController::init(RelayController *rc)
{
	relayController = rc;

	setSerialDebuggerLogPrefix("LC: ");
	Controllino_RTC_init();
	setLightDimInPercents(0);
	pinMode(PWM_LIGHT_DIMMING, OUTPUT);
	pinMode(LED_OPERATION_MODE_GREEN, OUTPUT);
	pinMode(LED_OPERATION_MODE_RED, OUTPUT);
	setLightDimInPercents(0);
	signalNormalOperationMode();
	timerIntensitySmoother.init(lightIntensitySmootherIntervalMs);
	timerScheduleCheck.init(lightScheduleCheckIntervalMs);
	timerSerialLogFloodProtection.init(serialLogMessageIntervalMs);
	loadLightSchedulesFromFlash();
}

void LightController::setLightDimInPercents(uint8_t percents)
{
	m_intensityPercents = percents;
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
	digitalWrite(LED_OPERATION_MODE_GREEN, HIGH);
	digitalWrite(LED_OPERATION_MODE_RED, HIGH);
	debugMessage(F("Signaling maintenance operation mode"));
}

void LightController::signalNormalOperationMode()
{
	maintenanceMode = false;
	digitalWrite(LED_OPERATION_MODE_RED, LOW);
	digitalWrite(LED_OPERATION_MODE_GREEN, HIGH);
	debugMessage(F("Signaling normal operation mode"));
}

void LightController::signalErrorOperationMode()
{
	maintenanceMode = false;
	digitalWrite(LED_OPERATION_MODE_RED, HIGH);
	digitalWrite(LED_OPERATION_MODE_GREEN, LOW);
	debugMessage(F("Signaling error operation mode"));
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

void LightController::setLightIntensitySmootherIntervalMs(uint32_t n)
{
	lightIntensitySmootherIntervalMs = n;
	timerIntensitySmoother.setIntervalMillis(n);
}

void LightController::setLightScheduleCheckIntervalMs(uint32_t n)
{
	lightScheduleCheckIntervalMs = n;
	timerScheduleCheck.setIntervalMillis(n);
}

uint8_t LightController::intensityPercents()
{
	return m_intensityPercents;
}

uint8_t LightController::intensityPercentsSmoothedEnd()
{
	return m_intensityPercentsSmoothedEnd;
}

uint8_t LightController::intensityPercentsSmoothedStep()
{
	return m_intensityPercentsSmoothedStep;
}

uint8_t LightController::intensityPercentsSmoothedStart()
{
	return m_intensityPercentsSmoothedStart;
}

int LightController::lightIntensityPercentForHour(uint8_t hour)
{
	if (hour > 23)
		return -1;

	return lightSchedule[hour];
}

bool LightController::setLightIntensityPercentForHourAndStore(uint8_t hour, uint8_t intensity)
{
	return setLightIntensityPercentForHour(hour, intensity, true);
}

bool LightController::setLightIntensityPercentForHour(uint8_t hour, uint8_t intensity)
{
	return setLightIntensityPercentForHour(hour, intensity, false);
}

bool LightController::setLightIntensityPercentForHour(uint8_t hour, uint8_t intensity, bool storeInFlash)
{
	if (hour > 23)
		return false;

	lightSchedule[hour] = intensity;

	if (!storeInFlash)
		return true;

	return storeLightSchedulesToFlash();
}

bool LightController::loadLightSchedulesFromFlash()
{
#define EEPROM_NOT_WRITTEN_YET 255
	for (int hour = 0; hour <= 23; hour++) {
		int intensity = EEPROM.read(hour);
		if (intensity != EEPROM_NOT_WRITTEN_YET) {
			debugMessage("Loaded light schedule from flash hour=%d intensity=%d%%", hour, intensity);
			setLightIntensityPercentForHour(hour, intensity);
		}
	}

	/* TODO: Error checking?! */
	return true;
}

bool LightController::storeLightSchedulesToFlash()
{
	for (int hour = 0; hour <= 23; hour++) {
		int intensity = lightIntensityPercentForHour(hour);
		EEPROM.update(hour, intensity);
	}

	/* TODO: Error checking?! */
	return true;
}

uint8_t LightController::lightsIntensityValueForCurrentSchedule()
{
	int intensity;
	uint8_t hour = 25;
	uint8_t minute = 61;

	int ret = Controllino_ReadTimeDate(NULL, NULL, NULL, NULL, &hour, &minute, NULL);
	if (ret < 0 || hour == 25 || minute == 61) {
		debugError("Unable to read RTC!");
		return 0;
	}

	intensity = lightIntensityPercentForHour(hour);
	if (intensity < 0) {
		debugMessage(F("ERROR getting light intensity for %dh"), hour);
		return 0;
	}

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

	int intensity = lightsIntensityValueForCurrentSchedule();
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
	if (m_intensityPercents == percents) {
		debugMessage(F("Intensity is already set to %d%%, ignoring setting"), percents);
		return;
	}

	if (m_intensityPercentsSmoothedEnd == percents) {
		if (m_intensityPercentsSmoothedStart != m_intensityPercentsSmoothedEnd) {
			debugMessage(F("We're already smoothing the intensity to %d%%, ignoring setting"), percents);
		}
		return;
	}

	m_intensityPercentsSmoothedEnd = percents;
	m_intensityPercentsSmoothedStart = m_intensityPercents;
	m_intensityPercentsSmoothedStep = 0;
	handleLightIntensitySmoothing();
}

void LightController::handleLightIntensitySmoothing()
{
	if (!timerIntensitySmoother.timeoutHasPassed())
		return;

	if (m_intensityPercents == m_intensityPercentsSmoothedEnd)
		return;

	m_intensityPercentsSmoothedStep += 1;
	uint8_t percents = map(m_intensityPercentsSmoothedStep, 0, 100, m_intensityPercentsSmoothedStart,
			       m_intensityPercentsSmoothedEnd);

	if (m_intensityPercentsSmoothedStep >= 100) {
		m_intensityPercentsSmoothedStep = 0;
		percents = m_intensityPercentsSmoothedEnd;
	}

	debugMessage(F("Smoothing intensity %% current: %d target: %d next step: %d"), m_intensityPercents,
		     m_intensityPercentsSmoothedEnd, percents);
	setLightDimInPercents(percents);
	timerIntensitySmoother.restart();
}

void LightController::tickTimers()
{
	timerScheduleCheck.tick();
	timerIntensitySmoother.tick();
	timerSerialLogFloodProtection.tick();
}

void LightController::tick()
{
	tickTimers();
	handleLightIntensitySmoothing();
	ensureStaticLightSchedulePeriodically();
}
