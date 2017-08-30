#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <Arduino.h>

#include "infarm_utils.h"

static enum fa_log_level serial_log_level = FA_LOG_LEVEL_DEBUG;

void fa_led_blink(int led, int on, int off)
{
	digitalWrite(led, HIGH);
	delay(on);
	digitalWrite(led, LOW);
	delay(off);
}

void fa_show_progress(const char *bar, int width)
{
	static int i = 0;

	Serial.print(bar);
	if ((++i % width) == 0) {
		i = 0;
		Serial.write("\r\n", 2);
	}
}

void fa_delay_seconds(uint32_t s)
{
	delay(s * 1000);
}

void fa_set_serial_debug_log_level(enum fa_log_level level)
{
	serial_log_level = level;
}

void fa_serial_debug_print(enum fa_log_level level, const char *text)
{
	if (level < serial_log_level)
		return;

	Serial.print(text);
}

void fa_serial_debug(enum fa_log_level level, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	fa_serial_debug_printf((enum fa_log_level)level, fmt, args);
	va_end(args);
}

void fa_serial_debug_printf(enum fa_log_level level, const char *fmt, va_list args)
{
	static char buf[128] = { 0 };

	if (level < serial_log_level)
		return;

	vsnprintf(buf, sizeof(buf), fmt, args);
	Serial.println(buf);
}

void fa_serial_debug_printf_P(enum fa_log_level level, const char *fmt, va_list args)
{
	static char buf[128] = { 0 };

	if (level < serial_log_level)
		return;

#ifdef ARDUINO_ARCH_AVR
	vsnprintf_P(buf, sizeof(buf), fmt, args);
#else
	vsnprintf(buf, sizeof(buf), fmt, args);
#endif
	Serial.println(buf);
}

int fa_free_ram()
{
#ifdef ARDUINO_ARCH_AVR
	extern int __heap_start, *__brkval;
	int v;
	return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
#else
	return 0;
#endif
}
