#include <Arduino.h>
#include <stdint.h>

class LightModbus
{
      public:
	void init(uint8_t address, uint8_t serialPortNumber);
	bool begin(uint32_t baudRate);
	// int querySlave(uint8_t busSlaveAddress, uint8_t *pdu, uint8_t pduLen);
	// int sendReplyToMaster(uint8_t *pdu, uint8_t pduLen);
      private:
	HardwareSerial *uart;
	uint8_t hardwareSerialPortNumber;
	uint8_t busAddress;
};
