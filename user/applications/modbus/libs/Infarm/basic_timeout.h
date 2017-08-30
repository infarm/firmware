#pragma once

#include <stdint.h>

class BasicTimeout
{
      public:
	BasicTimeout();
	BasicTimeout(uint32_t intervalMillis);
	BasicTimeout(uint32_t intervalMillis, void (*timeoutCallback)(BasicTimeout *t, void *data), void *data);
	void init(uint32_t intervalMillis);
	void init(uint32_t intervalMillis, void (*timeoutCallback)(BasicTimeout *t, void *data), void *data);
	uint32_t remainingMillis();
	uint32_t restart();
	void start();
	void stop();
	void tick();
	bool isStopped();
	bool timeoutHasPassed();

      protected:
	uint32_t now();

      private:
	bool stopped = true;
	uint32_t interval = 0;
	uint32_t timeout = 0;
	void (*callback)(BasicTimeout *t, void *data) = nullptr;
	void *callbackData = nullptr;
};
