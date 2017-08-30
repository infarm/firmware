#include "lightmodbus.h"

void LightModbus::init(uint8_t addressOnBus, uint8_t serialPortNumber)
{
	busAddress = addressOnBus;
	hardwareSerialPortNumber = serialPortNumber;
}

bool LightModbus::begin(uint32_t baudRate)
{
	switch (hardwareSerialPortNumber) {
	case 0:
		uart = &Serial;
	case 1:
		uart = &Serial1;
	case 2:
		uart = &Serial2;
	default:
		return false;
	}

	uart->begin(baudRate);
	return true;
}

/*
int LightModbus::querySlave(uint8_t busSlaveAddress, uint8_t *pdu, uint8_t pduLen)
{
	return 0;
}

int LightModbus::sendReplyToMaster(uint8_t *pdu, uint8_t pduLen)
{
	return 0;
}
*/
