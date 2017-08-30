#pragma once

#include <stdint.h>

#include "ModbusRtu.h"
#include "serial_debugger.h"

enum FarmModbusError {
	FarmModbusErrorNoError,
	FarmModbusErrorNotImplemented,
	FarmModbusErrorBusError,
	FarmModbusErrorMasterBusy,
	FarmModbusErrorMasterWrongQuery,
	FarmModbusErrorMasterPollFailed,
	FarmModbusErrorMasterWrongState,
	FarmModbusErrorSlaveReplyTimeout,
};

class FarmModbus : public SerialDebugger
{
      public:
	void begin(uint32_t baudrate);
	void setBusSerialPort(uint8_t portNumber);
	void setBusStandardRS485();
	void setBusStandardUART();
	bool defaultBusStandardIsRS485();

      protected:
	Modbus bus;
	virtual void init();
	void setBusSlave();
	void setBusSlaveWithAddress(uint8_t address);
	void setBusMaster();
	uint8_t defaultBusSerialPortNumber();
	enum ModbusTxenPin { ModbusTxenPinRS232 = 1, ModbusTxenPinRS485 };

      private:
	uint8_t busAddress = 0;
	uint32_t busBaudrate = 0;
	bool busStandardIsRS485 = defaultBusStandardIsRS485();
	uint8_t busSerialPortNumber = defaultBusSerialPortNumber();
};
