#include <Arduino.h>

#include "heart_beat.h"

HeartBeat::HeartBeat()
{
}

HeartBeat::HeartBeat(uint8_t pin, uint16_t intervalMs)
{
	init(pin, intervalMs);
}

void HeartBeat::init(uint8_t pin, uint16_t intervalMs)
{
	digitalPin = pin;
	pinMode(digitalPin, OUTPUT);
	timer.init(intervalMs);
}

bool HeartBeat::pinIsOn()
{
	return pinOn;
}

void HeartBeat::pinToggleState()
{
	if (pinIsOn()) {
		digitalWrite(digitalPin, LOW);
		pinOn = false;
	} else {
		digitalWrite(digitalPin, HIGH);
		pinOn = true;
	}
}

void HeartBeat::tick()
{
	if (!timer.timeoutHasPassed())
		return;

	timer.restart();
	pinToggleState();
}
