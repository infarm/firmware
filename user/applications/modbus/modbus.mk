CPPSRC += \
	$(USRSRC)/main.cpp \
	$(USRSRC)/libs/Infarm/infarm_utils.cpp \
	$(USRSRC)/libs/Infarm/serial_debugger.cpp \
	$(USRSRC)/libs/InfarmModbus/ModbusRtu.cpp \
	$(USRSRC)/libs/InfarmModbus/farm_modbus.cpp \
	$(USRSRC)/libs/InfarmModbus/farm_modbus_controllino_registers.cpp \
	$(USRSRC)/libs/InfarmModbus/farm_modbus_master.cpp \
	$(USRSRC)/libs/InfarmModbus/farm_modbus_uart_master.cpp

CFLAGS += \
	  -Wall -Werror -Wextra \
	  -Wno-unused-parameter -Wno-missing-field-initializers -Wno-ignored-qualifiers \
	  -I$(USRSRC)/libs/Infarm \
	  -I$(USRSRC)/libs/InfarmModbus
