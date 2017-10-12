#include "application.h"
#include "infarm_utils.h"
#include "serial_debugger.h"
#include "farm_modbus_master.h"
#include "farm_modbus_controllino_registers.h"

SYSTEM_MODE(MANUAL);

SerialDebugger d;
FarmModbusMaster master;
FarmModbusControllinoRegisters cr;

void setup()
{
	d.enableDebug();
	Serial.begin(115200);

	while (!Serial.isConnected())
		Particle.process();

	master.enableDebug();
	master.begin(115200);
	cr.enableModbusMasterView();
	master.setSlaveRegisters(&cr);
}

void waitForSlaveReplyBlocking(const char *waiting_for)
{
	int rx;

	while (!master.busIsIdle()) {
		rx = master.poll();
		if (rx == 0)
			continue;

		if (rx < 7) {
			d.debugMessage(F("ERROR while waiting for %s, pool() error ret=%d"), waiting_for, rx);
			continue;
		}

		if (rx > 7) {
			d.debugMessage(F("Ok, got %s reply, pool() ret=%d"), waiting_for, rx);
			break;
		}

		delay(5);
	}
}

void testRealTimeClockWrite()
{
	int tx;
	uint8_t hour = random(0, 24);
	uint8_t minute = random(0, 60);

	tx = master.slaveSetRealTimeClock(hour, minute);
	if (tx != FarmModbusErrorNoError) {
		d.debugMessage(F("Sending command update RTC, ERROR=%d"), tx);
		return;
	}

	waitForSlaveReplyBlocking("set RTC");
	d.debugMessage(F("Ok, RTC should be now set on slave to %d:%d"), hour, minute);
}

void feedWatchdog()
{
	int tx;

	tx = master.slaveFeedWatchdog();
	if (tx != FarmModbusErrorNoError) {
		d.debugMessage(F("Sending command feed watchdog, ERROR=%d"), tx);
		return;
	}

	waitForSlaveReplyBlocking("feed watchdog");
	d.debugMessage(F("Ok, watchdog should be fed now on slave"));
}

void testFirmwareVersion()
{
	int tx;

	tx = master.slaveReadFirmwareVersion();
	if (tx != FarmModbusErrorNoError) {
		d.debugMessage(F("Sending command read firmware version, ERROR=%d"), tx);
		return;
	}

	waitForSlaveReplyBlocking("firmware version");
	d.debugMessage(F("Ok, firmware version on slave is %u"), cr.firmwareVersionValueGet());
}

void testWaterTankPressureSensor()
{
	int tx;

	tx = master.slaveReadWaterTankPressureSensor();
	if (tx != FarmModbusErrorNoError) {
		d.debugMessage(F("Sending command water tank pressure sensor read, ERROR=%d"), tx);
		return;
	}

	waitForSlaveReplyBlocking("water tank pressure sensor");
	d.debugMessage(F("Ok, ADC value of water tank pressure sensor on slave is %u"),
		       cr.waterTankPressureSensorValueGet());
}

void testMaintenanceModeState()
{
	int tx;

	/* first read the current state */
	tx = master.slaveReadMaintenanceModeState();
	if (tx != FarmModbusErrorNoError) {
		d.debugMessage(F("Sending command maintenance mode read, ERROR=%d"), tx);
		return;
	}

	waitForSlaveReplyBlocking("maintenance mode read");
	d.debugMessage(F("Ok, maintenance mode on slave is %s"), cr.maintenanceModeIsActive() ? "active" : "inactive");

	/* then flip it */
	if (!cr.maintenanceModeIsActive()) {
		tx = master.slaveEnableMaintenanceModeViaOverride();
	} else {
		tx = master.slaveDisableMaintenanceModeViaOverride();
	}

	if (tx != FarmModbusErrorNoError) {
		d.debugMessage(F("Sending command maintenance mode write, ERROR=%d"), tx);
		return;
	}

	waitForSlaveReplyBlocking("maintenance mode write");
	d.debugMessage(F("Ok, maintenance mode on slave should be set to %s"),
		       !cr.maintenanceModeIsActive() ? "active" : "inactive");
}

void writeLightScheduleForHour(uint8_t hour, uint8_t intensity)
{
	int tx = master.slaveSetLightIntensityScheduleForHour(hour, intensity);
	if (tx != FarmModbusErrorNoError) {
		d.debugMessage(F("Sending command write light intensity=%d%% hour=%d, ERROR=%d"), intensity, hour, tx);
		return;
	}

	waitForSlaveReplyBlocking("light intensity write");
	d.debugMessage(F("Ok, light intensity=%d%% hour=%d should be set"), intensity, hour);
}

void requestLightScheduleReadForHour(const uint8_t hour)
{
	int tx = master.slaveRequestReadOfLightIntensityScheduleForHour(hour);
	if (tx != FarmModbusErrorNoError) {
		d.debugMessage(F("Sending command read light intensity hour=%d, ERROR=%d"), hour, tx);
		return;
	}

	waitForSlaveReplyBlocking("light intensity");
	d.debugMessage(F("Ok, light schedule intensity for hour=%d was requested"), hour);
}

void readLightScheduleRegisters()
{
	int tx = master.slaveUpdateLightIntensityScheduleRegistersState();
	if (tx != FarmModbusErrorNoError) {
		d.debugMessage(F("Sending command read light intensity registers, ERROR=%d"), tx);
		return;
	}

	waitForSlaveReplyBlocking("light intensity registers");
}

void testLightSchedules()
{
#define INTENSITY_FIXED_HOUR_OFFSET 50
	uint8_t hour;
	uint8_t intensity;

	for (uint8_t h=0; h <= 23; h++) {
		writeLightScheduleForHour(h, h+INTENSITY_FIXED_HOUR_OFFSET);
	}

	for (uint8_t h=0; h <= 23; h++) {
		requestLightScheduleReadForHour(h);

		delay(10); /* TODO: replace with DATA READY checking */

		readLightScheduleRegisters();
		cr.lightScheduleValueGet(&hour, &intensity);

		d.debugMessage(F("Slave light schedule settings, hour=%d intensity=%d%%"), hour, intensity);

		if (hour+INTENSITY_FIXED_HOUR_OFFSET != intensity) {
			d.debugMessage(F("Slave light schedule settings ERROR, wanted intensity=%d%% received intensity=%d%%"),
					hour+INTENSITY_FIXED_HOUR_OFFSET, intensity);
		}
	}

	d.debugMessage(F("Waiting 30s to improve flash memory lifetime..."));
	delay(30000);
}

void loop()
{
	testRealTimeClockWrite();
	feedWatchdog();
	testFirmwareVersion();
	testWaterTankPressureSensor();
	testMaintenanceModeState();
	// testLightSchedules(); /* Disabled by default to not wear out flash memory */
	delay(1000);
}
