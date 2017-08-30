#pragma once

#include "basic_timeout.h"
#include "serial_debugger.h"

class RelayController;

class LightController : public SerialDebugger
{
      public:
	void init(RelayController *rc);
	void setLightDimInPercents(uint8_t percents);
	void setLightDimInPercentsSmoothed(uint8_t percents);
	uint8_t lightsIntensityValueForCurrentSchedule();
	bool isMaintenanceModeBeingSignaled();
	bool isOpenDoorsModeOngoing();
	void ensureStaticLightSchedule();
	void ensureStaticLightSchedulePeriodically();
	void handleLightIntensitySmoothing();
	void signalMaintenanceOperationMode();
	void signalNormalOperationMode();
	void enableOpenDoorsMode();
	void disableOpenDoorsMode();
	void lightsOff();
	void lightsOn();
	void tick();

      private:
	RelayController *relayController;
	BasicTimeout timerScheduleCheck;
	BasicTimeout timerIntensitySmoother;
	BasicTimeout timerSerialLogFloodProtection;
	bool maintenanceMode = false;
	bool openDoorsMode = false;
	uint8_t pwmValue = 0;
	uint8_t intensityPercents = 0;
	uint8_t intensityPercentsSmoothedEnd = 0;
	uint8_t intensityPercentsSmoothedStep = 0;
	uint8_t intensityPercentsSmoothedStart = 0;
	uint32_t lightIntensitySmootherIntervalMs = 100;
	uint32_t lightScheduleCheckIntervalMs = 30 * 1000;
	uint32_t serialLogMessageIntervalMs = 5 * 1000;
};
