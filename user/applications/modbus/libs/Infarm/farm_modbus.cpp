#include "farm_modbus.h"

void FarmModbus::init()
{
}

void FarmModbus::begin(uint32_t baudrate)
{
	uint32_t txenPin = ModbusTxenPinRS232;

	init();
	setSerialDebuggerLogPrefix("MB: ");

	if (busStandardIsRS485)
		txenPin = ModbusTxenPinRS485;

	debugMessage(F("bus address: 0x%x, serial port: %d, baudrate: %lu mode: %s"), busAddress, busSerialPortNumber,
		     baudrate, busStandardIsRS485 ? "RS485" : "UART");
	bus.init(busAddress, busSerialPortNumber, txenPin);
	bus.begin(baudrate);
}

void FarmModbus::setBusMaster()
{
	busAddress = 0;
}

void FarmModbus::setBusSerialPort(uint8_t portNumber)
{
	busSerialPortNumber = portNumber;
}

void FarmModbus::setBusSlave()
{
	setBusSlaveWithAddress(0x12);
}

void FarmModbus::setBusSlaveWithAddress(uint8_t address)
{
	busAddress = address;
}

void FarmModbus::setBusStandardRS485()
{
	busStandardIsRS485 = true;
}

void FarmModbus::setBusStandardUART()
{
	busStandardIsRS485 = false;
}

bool FarmModbus::defaultBusStandardIsRS485()
{
#ifdef MODBUS_STANDARD_RS485
	return true;
#else
	return false;
#endif
}

uint8_t FarmModbus::defaultBusSerialPortNumber()
{
#ifdef DEVICE_CONTROLLINO
#ifdef MODBUS_STANDARD_RS485
#define MODBUS_SERIAL_PORT 3
#else
#define MODBUS_SERIAL_PORT 2
#endif
#elif defined(DEVICE_ARDUINO_MEGA)
#define MODBUS_SERIAL_PORT 1
#elif defined(SPARK_PLATFORM)
#define MODBUS_SERIAL_PORT 1
#else
#error "Add support for your device"
#endif
	return MODBUS_SERIAL_PORT;
}
