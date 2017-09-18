#pragma once

#include <stdint.h>
#include <stdarg.h>

enum fa_log_level {
	FA_LOG_LEVEL_DEBUG,
	FA_LOG_LEVEL_INFO,
	FA_LOG_LEVEL_ERROR,
};

#define SDEBUG(fmt, ...) fa_serial_debug(FA_LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#define SINFO(fmt, ...) fa_serial_debug(FA_LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define SERROR(fmt, ...) fa_serial_debug(FA_LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)

#define MILLIS_IN_SECONDS 1000UL
#define MILLIS_IN_MINUTE 60UL * MILLIS_IN_SECONDS
#define MINUTES_TO_MILLIS(mins) mins *MILLIS_IN_MINUTE
#define SECONDS_TO_MILLIS(secs) secs *MILLIS_IN_SECONDS

void fa_led_blink(int led, int on, int off);
void fa_show_progress(const char *bar, int width);
void fa_delay_seconds(uint32_t seconds);
void fa_serial_debug(enum fa_log_level level, const char *fmt, ...);
void fa_serial_debug_print(enum fa_log_level level, const char *text);
void fa_serial_debug_printf(enum fa_log_level level, const char *fmt, va_list args);
void fa_serial_debug_printf_P(enum fa_log_level level, const char *fmt, va_list args);
void fa_set_serial_debug_log_level(enum fa_log_level level);
int fa_free_ram();
