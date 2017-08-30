#include "farm_modbus_slave.h"

void FarmModbusSlave::setRegisters(FarmModbusControllinoRegisters *r)
{
	registers = r;
}

/* @return 0 if no query, 1..4 if communication error, >4 if correct query */
int FarmModbusSlave::poll()
{
	return bus.poll(registers->blockOfRegisters(), registers->blockOfRegistersSize());
}
