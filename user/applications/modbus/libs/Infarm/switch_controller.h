#pragma once

#include "basic_timeout.h"
#include "serial_debugger.h"
#include "infarm_utils.h"

class RelayController;
class LightController;
class DosingPumpController;

class SwitchController : public SerialDebugger
{
      public:
	void init(RelayController *rc, LightController *dc, DosingPumpController *dp);
	bool maintenancePushSwitchIsPressed();
	bool overrideSelectionPushSwitchIsPressed();
	bool overrideExecutePushSwitchIsPressed();
	bool circullationOverrideSwitchIsPressedOn();
	bool irrigationOverrideSwitchIsPressedOn();
	bool flowCirculationLineSwitchIsPressedOn();
	bool flowIrrigationLineSwitchIsPressedOn();
	bool panelSwitchStateSignalsOpen();
	bool doorsSwitchStateSignalsOpen();
	bool drawerSwtichStateSignalsOpen();
	void handlePumpOverrides();
	void handleManualOverrides();
	void handleOpenDoorsMode();
	void handleMaintenanceMode();
	void tick();
	void setManualPinIndex(uint8_t newIndex);
	bool isMaintenanceModeActive();
	bool isOpenDoorsModeActive();
	void openDoorsModeEnable();
	void openDoorsModeDisable();
	void maintenanceModeDisable();
	void maintenanceModeEnable();
	static void openDoorsModeDisableTimerCallback(BasicTimeout *t, void *data);
	static void maintenanceModeDisableTimerCallback(BasicTimeout *t, void *data);
	static void defaultStateTimerCallback(BasicTimeout *t, void *data);

      private:
	RelayController *rc;
	LightController *dc;
	DosingPumpController *dp;
	uint8_t manualPinIndex = 0;
	bool manualPinIsActive = false;
	bool maintenanceMode = false;
	bool openDoorsMode = false;
	bool openDoorsModePreventOnAgain = false;
	BasicTimeout timerDoorsModeDisable;
	BasicTimeout timerMaintenanceModeDisable;
	BasicTimeout timerReturnToDefaultState;
	BasicTimeout timerSwitchDebouncer;
	BasicTimeout timerSwitchDebouncerMaintenance;
	BasicTimeout timerSerialLogFloodProtection;
	uint32_t maintenanceModeDisableIntervalMs = MINUTES_TO_MILLIS(10);
	uint32_t openDoorsModeDisableIntervalMs = MINUTES_TO_MILLIS(30);
	uint32_t defaultStateIntervalMs = 3 * 1000;
	uint32_t switchDebouncerIntervalMs = 250;
	uint32_t serialLogMessageIntervalMs = 5 * 1000;
	uint8_t activePinIndex = 0;
};
