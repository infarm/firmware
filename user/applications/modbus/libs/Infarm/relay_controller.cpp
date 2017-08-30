#include <Arduino.h>
#include "Controllino.h"

#include "relay_controller.h"

#define MAX_RELAYS 10
#define MIN_RELAY_NUM 0
#define MAX_RELAY_NUM 9

#define RELAY_LIGHT_BALLAST 0
#define RELAY_IRRIGATION_PUMP 1
#define RELAY_CIRCULATION_PUMP 2
#define RELAY_HOT_BOX_FANS 3
#define RELAY_EXHAUST_FANS 9
#define RELAY_VENTILATION_FANS 8
#define RELAY_WATER_COOLER 4
#define RELAY_MOTOR_BALLAST 5

const static uint8_t relay_pins[MAX_RELAYS] = { CONTROLLINO_RELAY_00, CONTROLLINO_RELAY_01, CONTROLLINO_RELAY_02,
						CONTROLLINO_RELAY_03, CONTROLLINO_RELAY_04, CONTROLLINO_RELAY_05,
						CONTROLLINO_RELAY_06, CONTROLLINO_RELAY_07, CONTROLLINO_RELAY_08,
						CONTROLLINO_RELAY_09 };

void RelayController::init()
{
	setSerialDebuggerLogPrefix("RC: ");

	for (uint8_t i = relayMinNumber(); i <= relayMaxNumber(); i++) {
		pinMode(relayDigitalPin(i), OUTPUT);
	}
}

void RelayController::allRelaysOff()
{
	for (uint8_t i = relayMinNumber(); i <= relayMaxNumber(); i++) {
		relayOff(i);
	}
}

uint8_t RelayController::relayMinNumber()
{
	return MIN_RELAY_NUM;
}

uint8_t RelayController::relayMaxNumber()
{
	return MAX_RELAY_NUM;
}

bool RelayController::relayOn(uint8_t n)
{
	if (n > relayMaxNumber())
		return false;

	if (isOpenDoorsModeActive() && (n == RELAY_MOTOR_BALLAST)) {
		debugMessage(F("Open doors mode, can't switch ON motor ballast"));
		return false;
	}

	digitalWrite(relayDigitalPin(n), HIGH);
	return true;
}

bool RelayController::relayOff(uint8_t n)
{
	if (n > relayMaxNumber())
		return false;

	digitalWrite(relayDigitalPin(n), LOW);
	return true;
}

int RelayController::relayDigitalPin(uint8_t n)
{
	if (n > relayMaxNumber())
		return -1;

	return relay_pins[n];
}

int RelayController::relayState(uint8_t n)
{
	if (n > relayMaxNumber())
		return -1;

	return digitalRead(relayDigitalPin(n));
}

void RelayController::relayLightBallastOn()
{
	relayOn(RELAY_LIGHT_BALLAST);
}

void RelayController::relayLightBallastOff()
{
	relayOff(RELAY_LIGHT_BALLAST);
}

void RelayController::relayIrrigationPumpOn()
{
	relayOn(RELAY_IRRIGATION_PUMP);
}

void RelayController::relayIrrigationPumpOff()
{
	relayOff(RELAY_IRRIGATION_PUMP);
}

void RelayController::relayCirculationPumpOn()
{
	relayOn(RELAY_CIRCULATION_PUMP);
}

void RelayController::relayCirculationPumpOff()
{
	relayOff(RELAY_CIRCULATION_PUMP);
}

void RelayController::relayHotBoxFansOn()
{
	relayOn(RELAY_HOT_BOX_FANS);
}

void RelayController::relayHotBoxFansOff()
{
	relayOff(RELAY_HOT_BOX_FANS);
}

void RelayController::relayClimateFansOn()
{
	relayOn(RELAY_EXHAUST_FANS);
	relayOn(RELAY_VENTILATION_FANS);
}

void RelayController::relayClimateFansOff()
{
	relayOff(RELAY_EXHAUST_FANS);
	relayOff(RELAY_VENTILATION_FANS);
}

void RelayController::relayWaterCoolerOn()
{
	relayOn(RELAY_WATER_COOLER);
}

void RelayController::relayWaterCoolerOff()
{
	relayOff(RELAY_WATER_COOLER);
}

bool RelayController::relayMotorBallastOn()
{
	if (isOpenDoorsModeActive()) {
		debugMessage(F("Open doors mode, can't switch ON motor ballast"));
		return false;
	}

	relayOn(RELAY_MOTOR_BALLAST);
	return true;
}

void RelayController::relayMotorBallastOff()
{
	relayOff(RELAY_MOTOR_BALLAST);
}

void RelayController::enableOpenDoorsMode()
{
	relayMotorBallastOff();
}

void RelayController::disableOpenDoorsMode()
{
	relayMotorBallastOn();
}

bool RelayController::isOpenDoorsModeActive()
{
	return openDoorsMode == true;
}
