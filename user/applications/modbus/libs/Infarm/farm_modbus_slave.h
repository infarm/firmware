#pragma once

#include <stdint.h>

#include "farm_modbus_uart_slave.h"
#include "farm_modbus_controllino_registers.h"

class FarmModbusSlave : public FarmModbusUARTSlave
{
      public:
	void setRegisters(FarmModbusControllinoRegisters *r);
	int poll();

      private:
	FarmModbusControllinoRegisters *registers;
};
