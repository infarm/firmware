#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#ifdef ARDUINO_ARCH_AVR
#include <avr/pgmspace.h>
#endif

#include "infarm_utils.h"
#include "serial_debugger.h"

void SerialDebugger::enableDebug()
{
	debugEnabled = true;
}

void SerialDebugger::setSerialDebuggerLogPrefix(const char *prefix)
{
	memset(logPrefix, 0, sizeof(logPrefix));
	strncpy(logPrefix, prefix, sizeof(logPrefix) - 1);
}

void SerialDebugger::outputLogPrefix(enum SerialLogLevel logLevel)
{
	if (logPrefix)
		fa_serial_debug_print((enum fa_log_level)logLevel, logPrefix);
}

void SerialDebugger::debugError(const char *fmt, ...)
{
	va_list args;

	if (!debugEnabled)
		return;

	outputLogPrefix(LogLevelError);
	va_start(args, fmt);
	fa_serial_debug_printf(FA_LOG_LEVEL_ERROR, fmt, args);
	va_end(args);
}

void SerialDebugger::debugMessage(const __FlashStringHelper *fmt, ...)
{
	va_list args;

	if (!debugEnabled)
		return;

	outputLogPrefix(LogLevelInfo);
	va_start(args, fmt);
	fa_serial_debug_printf_P(FA_LOG_LEVEL_INFO, (const char *)fmt, args);
	va_end(args);
}

void SerialDebugger::debugMessage(const char *fmt, ...)
{
	va_list args;

	if (!debugEnabled)
		return;

	outputLogPrefix(LogLevelInfo);
	va_start(args, fmt);
	fa_serial_debug_printf(FA_LOG_LEVEL_INFO, fmt, args);
	va_end(args);
}

void SerialDebugger::debugMessage(enum SerialLogLevel logLevel, const char *fmt, ...)
{
	va_list args;

	if (!debugEnabled)
		return;

	outputLogPrefix(logLevel);
	va_start(args, fmt);
	fa_serial_debug_printf((enum fa_log_level)logLevel, fmt, args);
	va_end(args);
}
