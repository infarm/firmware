CPPSRC += \
	$(USRSRC)/main.cpp \
	$(USRSRC)/libs/ModbusRtu/ModbusRtu.cpp \
	$(USRSRC)/libs/Infarm/farm_modbus.cpp \
	$(USRSRC)/libs/Infarm/farm_modbus_controllino_registers.cpp \
	$(USRSRC)/libs/Infarm/farm_modbus_master.cpp \
	$(USRSRC)/libs/Infarm/farm_modbus_uart_master.cpp \
	$(USRSRC)/libs/Infarm/serial_debugger.cpp \
	$(USRSRC)/libs/Infarm/infarm_utils.cpp

CFLAGS += \
	  -Wall -Werror -Wextra \
	  -Wno-unused-parameter -Wno-missing-field-initializers -Wno-ignored-qualifiers \
	  -I$(USRSRC)/libs/ModbusRtu \
	  -I$(USRSRC)/libs/Infarm
