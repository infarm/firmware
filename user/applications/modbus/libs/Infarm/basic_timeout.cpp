#include <Arduino.h>

#include "basic_timeout.h"

BasicTimeout::BasicTimeout()
{
}

void BasicTimeout::init(uint32_t intervalMillis)
{
	interval = intervalMillis;
}

void BasicTimeout::init(uint32_t intervalMillis, void (*timeoutCallback)(BasicTimeout *t, void *data), void *data)
{
	callback = timeoutCallback;
	callbackData = data;
	init(intervalMillis);
}

BasicTimeout::BasicTimeout(uint32_t intervalMillis)
{
	init(intervalMillis);
}

BasicTimeout::BasicTimeout(uint32_t intervalMillis, void (*timeoutCallback)(BasicTimeout *t, void *data), void *data)
{
	init(intervalMillis, timeoutCallback, data);
}

bool BasicTimeout::isStopped()
{
	return stopped;
}

void BasicTimeout::stop()
{
	stopped = true;
}

void BasicTimeout::start()
{
	timeout = millis() + interval;
	stopped = false;
	tick();
}

uint32_t BasicTimeout::restart()
{
	uint32_t r;

	r = remainingMillis();
	start();

	return r;
}

uint32_t BasicTimeout::remainingMillis()
{
	uint32_t now = millis();
	if (now > timeout)
		return 0;

	return timeout - now;
}

uint32_t BasicTimeout::now()
{
	return millis();
}

bool BasicTimeout::timeoutHasPassed()
{
	if (timeout > now())
		return false;

	return true;
}

void BasicTimeout::tick()
{
	if (isStopped())
		return;

	if (!timeoutHasPassed())
		return;

	stop();

	if (!callback)
		return;

	callback(this, callbackData);
}
