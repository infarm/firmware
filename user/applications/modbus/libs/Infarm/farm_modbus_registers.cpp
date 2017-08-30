#include <string.h>

#include "farm_modbus_registers.h"

void FarmModbusRegisters::init()
{
	memset(regs, 0, sizeof(regs));
}

uint8_t FarmModbusRegisters::firstPossibleRegisterAddress()
{
	return 0;
}

uint8_t FarmModbusRegisters::lastPossibleRegisterAddress()
{
	return lastRegisterAddress - 1;
}

uint8_t FarmModbusRegisters::blockOfRegistersSize()
{
	return lastRegisterAddress;
}

uint16_t *FarmModbusRegisters::blockOfRegisters()
{
	return regs;
}
