#pragma once

#include <stdint.h>
#include "farm_modbus.h"

class FarmModbusUARTSlave : public FarmModbus
{
      public:
	void init();
};
