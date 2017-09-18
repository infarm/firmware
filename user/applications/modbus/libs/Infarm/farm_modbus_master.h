#pragma once

#include <stdint.h>

#include "farm_modbus_uart_master.h"
#include "farm_modbus_controllino_registers.h"

class FarmModbusMaster : public FarmModbusUARTMaster
{
      public:
	void setSlaveBusAddress(uint8_t address);
	int slaveStartPump(enum DosingPump n, uint16_t onTimeMs);
	int slaveReadFirmwareVersion();
	int slaveReadWaterTankPressureSensor();
	int slaveReadMaintenanceModeState();
	int slaveEnableMaintenanceModeViaOverride();
	int slaveDisableMaintenanceModeViaOverride();
	int slaveFeedWatchdog();
	int slaveSetLightIntensityScheduleForHour(uint8_t hour, uint8_t intensity);
	int slaveRequestReadOfLightIntensityScheduleForHour(uint8_t hour);
	int slaveUpdateLightIntensityScheduleRegistersState();
	int slaveSetRealTimeClock(uint8_t hours, uint8_t minutes);
	int slaveUpdatePumpState(enum DosingPump n);
	int slaveSendModbusPacket();
	void setSlaveRegisters(FarmModbusControllinoRegisters *r);
	bool intervalHasPassed(uint32_t interval);
	bool busIsWaitingForSlaveReply();
	bool busIsIdle();
	int poll();

      protected:
	int slaveUpdatePumpTimer(enum DosingPump n);
	int slaveSendWriteRegisterModbusPacket(uint16_t address, uint16_t *reg, uint8_t regCount);
	int slaveSendReadRegisterModbusPacket(uint16_t address, uint16_t *reg, uint8_t regCount);

      private:
	modbus_t modbusPacket;
	uint8_t slaveBusAddress = 0x12;
	bool waitingForSlaveReply = false;
	uint32_t slaveWaitForReplyMs = 0;
	uint32_t slaveReplyTimeoutMs = 1000;
	FarmModbusControllinoRegisters *slaveRegisters;
};
