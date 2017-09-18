#include <Arduino.h>
#include "Controllino.h"

#include "switch_controller.h"
#include "light_controller.h"
#include "relay_controller.h"
#include "dosing_pump_controller.h"

#define PUSH_SWITCH_MAINTENANCE CONTROLLINO_A0
#define PUSH_SWITCH_OVERRIDE_SELECT CONTROLLINO_A1
#define PUSH_SWITCH_OVERRIDE_EXECUTE CONTROLLINO_A4
#define SWITCH_OVERRIDE_CIRCULATION CONTROLLINO_A2
#define SWITCH_OVERRIDE_IRRIGATION CONTROLLINO_A3
#define SWITCH_FLOW_CIRCULATION_LINE CONTROLLINO_A7
#define SWITCH_FLOW_IRRIGATION_LINE CONTROLLINO_A8
#define SWITCH_PANEL_STATE CONTROLLINO_A6
#define SWITCH_DOORS_STATE CONTROLLINO_A9
#define SWITCH_DRAWER_STATE CONTROLLINO_IN0

#define MAX_SWITCH_PINS 10

const static uint8_t switch_pins[MAX_SWITCH_PINS] = {
	PUSH_SWITCH_MAINTENANCE,     PUSH_SWITCH_OVERRIDE_SELECT, PUSH_SWITCH_OVERRIDE_EXECUTE,
	SWITCH_OVERRIDE_CIRCULATION, SWITCH_OVERRIDE_IRRIGATION,  SWITCH_FLOW_CIRCULATION_LINE,
	SWITCH_FLOW_IRRIGATION_LINE, SWITCH_PANEL_STATE,	  SWITCH_DOORS_STATE,
	SWITCH_DRAWER_STATE,
};

void SwitchController::init(RelayController *r, LightController *d, DosingPumpController *p)
{
	setSerialDebuggerLogPrefix("SC: ");

	rc = r;
	dc = d;
	dp = p;

	for (uint8_t i = 0; i < MAX_SWITCH_PINS; i++) {
		pinMode(switch_pins[i], INPUT);
	}

	timerSerialLogFloodProtection.init(serialLogMessageIntervalMs);
	timerDoorsModeDisable.init(openDoorsModeDisableIntervalMs, openDoorsModeDisableTimerCallback, this);
	timerSwitchDebouncer.init(switchDebouncerIntervalMs);
	timerSwitchDebouncerMaintenance.init(switchDebouncerIntervalMs);
	timerReturnToDefaultState.init(defaultStateIntervalMs, defaultStateTimerCallback, this);
}

bool SwitchController::maintenancePushSwitchIsPressed()
{
	return digitalRead(PUSH_SWITCH_MAINTENANCE) == HIGH;
}

bool SwitchController::overrideSelectionPushSwitchIsPressed()
{
	return digitalRead(PUSH_SWITCH_OVERRIDE_SELECT) == HIGH;
}

bool SwitchController::overrideExecutePushSwitchIsPressed()
{
	return digitalRead(PUSH_SWITCH_OVERRIDE_EXECUTE) == HIGH;
}

bool SwitchController::circullationOverrideSwitchIsPressedOn()
{
	return digitalRead(SWITCH_OVERRIDE_CIRCULATION) == HIGH;
}

bool SwitchController::irrigationOverrideSwitchIsPressedOn()
{
	return digitalRead(SWITCH_OVERRIDE_IRRIGATION) == HIGH;
}

bool SwitchController::flowCirculationLineSwitchIsPressedOn()
{
	return digitalRead(SWITCH_FLOW_CIRCULATION_LINE) == HIGH;
}

bool SwitchController::flowIrrigationLineSwitchIsPressedOn()
{
	return digitalRead(SWITCH_FLOW_IRRIGATION_LINE) == HIGH;
}

bool SwitchController::panelSwitchStateSignalsOpen()
{
	return digitalRead(SWITCH_PANEL_STATE) == HIGH;
}

bool SwitchController::doorsSwitchStateSignalsOpen()
{
	return digitalRead(SWITCH_DOORS_STATE) == LOW;
}

bool SwitchController::drawerSwtichStateSignalsOpen()
{
	return digitalRead(SWITCH_DRAWER_STATE) == HIGH;
}

void SwitchController::handlePumpOverrides()
{
	if (circullationOverrideSwitchIsPressedOn()) {
		rc->relayCirculationPumpOn();
	} else {
		rc->relayCirculationPumpOff();
	}

	if (irrigationOverrideSwitchIsPressedOn()) {
		rc->relayIrrigationPumpOn();
	} else {
		rc->relayIrrigationPumpOff();
	}
}

void SwitchController::setManualPinIndex(uint8_t newIndex)
{
	manualPinIndex = newIndex;
}

void SwitchController::defaultStateTimerCallback(BasicTimeout *t, void *data)
{
	(void)t;
	SwitchController *sc = (SwitchController *)data;
	sc->debugMessage(F("Return manual pump override to default state"));
	sc->setManualPinIndex(0);
}

void SwitchController::openDoorsModeDisableTimerCallback(BasicTimeout *t, void *data)
{
	(void)t;
	SwitchController *sc = (SwitchController *)data;
	sc->debugMessage(F("Forcing open doors mode to OFF"));
	sc->openDoorsModeDisable();
}

void SwitchController::handleManualOverrides()
{
	if (!timerSwitchDebouncer.timeoutHasPassed())
		return;

	timerSwitchDebouncer.restart();

	bool selectSwitchActive = overrideSelectionPushSwitchIsPressed();
	bool executeSwitchActive = overrideExecutePushSwitchIsPressed();

	if (selectSwitchActive && !executeSwitchActive) {
		manualPinIndex += 1;
		if (manualPinIndex > dp->pumpMaxNumber())
			manualPinIndex = 0;

		debugMessage(F("Selected pump is %s (%d)"), dp->pumpName(manualPinIndex), manualPinIndex);
		timerReturnToDefaultState.restart();
	}

	if (!executeSwitchActive && !manualPinIsActive)
		return;

	if (manualPinIsActive && !executeSwitchActive) {
		debugMessage(F("Switching OFF active pump %s (%d)"), dp->pumpName(activePinIndex), activePinIndex);
		dp->pumpOff(activePinIndex);
		manualPinIsActive = false;
		timerReturnToDefaultState.restart();
		return;
	}

	if (manualPinIsActive && (activePinIndex == manualPinIndex)) {
		timerReturnToDefaultState.restart();
		return;
	}

	activePinIndex = manualPinIndex;
	debugMessage(F("Switching ON active pump %s (%d)"), dp->pumpName(activePinIndex), activePinIndex);
	dp->pumpOn(activePinIndex);
	manualPinIsActive = true;
	timerReturnToDefaultState.restart();
}

bool SwitchController::isMaintenanceModeActive()
{
	return maintenanceMode == true;
}

bool SwitchController::isOpenDoorsModeActive()
{
	return openDoorsMode == true;
}

void SwitchController::maintenanceModeEnable()
{
	/* TODO: FlashMemoryController->markMaintenanceModeStarted(); */
	maintenanceMode = true;
	debugMessage(F("Invoking maintenance mode"));
	dc->signalMaintenanceOperationMode();
	dp->allPumpsOff();
}

void SwitchController::maintenanceModeDisable()
{
	maintenanceMode = false;
	debugMessage(F("Maintenance mode is finished"));
	dc->signalNormalOperationMode();
}

void SwitchController::handleMaintenanceMode()
{
	if (!timerSwitchDebouncerMaintenance.timeoutHasPassed())
		return;

	timerSwitchDebouncerMaintenance.restart();

	bool switchPressed = maintenancePushSwitchIsPressed();
	if (!switchPressed && !isMaintenanceModeActive())
		return;

	if (switchPressed) {
		if (isMaintenanceModeActive()) {
			maintenanceModeDisable();
		} else {
			maintenanceModeEnable();
		}
	}
}

void SwitchController::openDoorsModeEnable()
{
	if (isOpenDoorsModeActive())
		return;

	if (openDoorsModePreventOnAgain) {
		if (timerSerialLogFloodProtection.timeoutHasPassed()) {
			timerSerialLogFloodProtection.restart();
			debugMessage(F("Doors open mode forbidden, broken switch?!"));
		}
		return;
	}

	openDoorsMode = true;
	debugMessage(F("Doors are open"));
	rc->enableOpenDoorsMode();
	dc->enableOpenDoorsMode();
	timerDoorsModeDisable.restart();
}

void SwitchController::openDoorsModeDisable()
{
	if (doorsSwitchStateSignalsOpen()) {
		if (!timerSwitchDebouncer.timeoutHasPassed())
			return;
		timerSwitchDebouncer.restart();
		debugMessage(F("Doors are probably still left open, forcing open doors mode off =%d"),
			     doorsSwitchStateSignalsOpen());
		openDoorsModePreventOnAgain = true;
	} else {
		debugMessage(F("Doors are closed"));
		openDoorsModePreventOnAgain = false;
	}

	openDoorsMode = false;
	rc->disableOpenDoorsMode();
	dc->disableOpenDoorsMode();
	timerDoorsModeDisable.stop();
}

void SwitchController::handleOpenDoorsMode()
{
	bool doorsAreOpen = doorsSwitchStateSignalsOpen();
	if (!doorsAreOpen && !isOpenDoorsModeActive())
		return;

	if (!doorsAreOpen && isOpenDoorsModeActive()) {
		openDoorsModeDisable();
	}

	if (doorsAreOpen && !isOpenDoorsModeActive()) {
		openDoorsModeEnable();
		return;
	}
}

void SwitchController::tick()
{
	timerSwitchDebouncer.tick();
	timerSwitchDebouncerMaintenance.tick();
	timerDoorsModeDisable.tick();
	timerReturnToDefaultState.tick();
	timerSerialLogFloodProtection.tick();

	handleMaintenanceMode();
	// handleOpenDoorsMode();
	handleManualOverrides();
	handlePumpOverrides();
}
