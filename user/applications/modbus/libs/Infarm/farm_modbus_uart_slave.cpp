#include "farm_modbus_uart_slave.h"

void FarmModbusUARTSlave::init()
{
	setBusSlaveWithAddress(0x12);
}
