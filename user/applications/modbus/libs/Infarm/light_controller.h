#pragma once

#include "basic_timeout.h"
#include "serial_debugger.h"

class RelayController;

class LightController : public SerialDebugger
{
      public:
	void init(RelayController *rc);
	void setLightIntensitySmootherIntervalMs(uint32_t n);
	void setLightScheduleCheckIntervalMs(uint32_t n);
	void setLightDimInPercents(uint8_t percents);
	void setLightDimInPercentsSmoothed(uint8_t percents);
	uint8_t lightsIntensityValueForCurrentSchedule();
	uint8_t intensityPercents();
	uint8_t intensityPercentsSmoothedEnd();
	uint8_t intensityPercentsSmoothedStep();
	uint8_t intensityPercentsSmoothedStart();
	bool isMaintenanceModeBeingSignaled();
	bool isOpenDoorsModeOngoing();
	void ensureStaticLightSchedule();
	void ensureStaticLightSchedulePeriodically();
	void handleLightIntensitySmoothing();
	void signalMaintenanceOperationMode();
	void signalNormalOperationMode();
	void signalErrorOperationMode();
	void enableOpenDoorsMode();
	void disableOpenDoorsMode();
	void lightsOff();
	void lightsOn();
	void tick();
	void tickTimers();
	bool loadLightSchedulesFromFlash();
	bool storeLightSchedulesToFlash();
	int lightIntensityPercentForHour(uint8_t hour);
	bool setLightIntensityPercentForHour(uint8_t hour, uint8_t intensity);
	bool setLightIntensityPercentForHour(uint8_t hour, uint8_t intensity, bool storeInFlash);
	bool setLightIntensityPercentForHourAndStore(uint8_t hour, uint8_t intensity);

      private:
	RelayController *relayController;
	BasicTimeout timerScheduleCheck;
	BasicTimeout timerIntensitySmoother;
	BasicTimeout timerSerialLogFloodProtection;
	bool maintenanceMode = false;
	bool openDoorsMode = false;
	uint8_t pwmValue = 0;
	uint8_t m_intensityPercents = 0;
	uint8_t m_intensityPercentsSmoothedEnd = 0;
	uint8_t m_intensityPercentsSmoothedStep = 0;
	uint8_t m_intensityPercentsSmoothedStart = 0;
	uint32_t lightIntensitySmootherIntervalMs = 100;
	uint32_t lightScheduleCheckIntervalMs = 30 * 1000;
	uint32_t serialLogMessageIntervalMs = 5 * 1000;
	uint8_t lightSchedule[24] = { 0,  0,  0,  0,  0,  95, 95, 95, 95, 95, 95, 95,
				      95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 0 };
};
