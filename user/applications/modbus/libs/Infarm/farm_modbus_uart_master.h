#pragma once

#include <stdint.h>

#include "farm_modbus.h"

class FarmModbusUARTMaster : public FarmModbus
{
      public:
	void init();
};
