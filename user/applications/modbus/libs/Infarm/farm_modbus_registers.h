#pragma once

#include <stdint.h>

class FarmModbusRegisters
{
      public:
	virtual void init();
	virtual uint8_t lastPossibleRegisterAddress();
	virtual uint8_t firstPossibleRegisterAddress();
	virtual uint16_t *blockOfRegisters();
	uint8_t blockOfRegistersSize();

      protected:
	static const uint8_t lastRegisterAddress = 1;
	uint16_t regs[lastRegisterAddress] = { 0 };
};
