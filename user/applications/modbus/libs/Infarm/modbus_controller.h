#pragma once

#include "basic_timeout.h"
#include "serial_debugger.h"
#include "infarm_utils.h"
#include "farm_modbus_slave.h"
#include "farmbrain_watchdog.h"
#include "dosing_pump_controller.h"
#include "farm_modbus_controllino_registers.h"

class RelayController;
class LightController;
class DosingPumpController;
class SwitchController;

class ModbusController : public SerialDebugger
{
      public:
	void pollModbus();
	void handlePumps();
	void handleWatchdog();
	void handleRealTimeClock();
	void init(RelayController *rc, LightController *dc, DosingPumpController *dp, SwitchController *s);
	void tick();

      protected:
	void handleSinglePump(enum DosingPump n, uint16_t timeout);
	bool newMessageReceivedOnModbus();

      private:
	RelayController *rc;
	LightController *dc;
	SwitchController *sc;
	FarmModbusSlave slave;
	DosingPumpController *dp;
	FarmBrainWatchdog wdt;
	FarmModbusControllinoRegisters cr;
};
