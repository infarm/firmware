#pragma once

class __FlashStringHelper;

enum SerialLogLevel {
	LogLevelDebug,
	LogLevelInfo,
	LogLevelError,
};

class SerialDebugger
{
      public:
	void enableDebug();
	void setSerialDebuggerLogPrefix(const char *prefix);
	void debugError(const char *fmt, ...);
	void debugMessage(const char *fmt, ...);
	void debugMessage(const __FlashStringHelper *fmt, ...);
	void debugMessage(enum SerialLogLevel logLevel, const char *fmt, ...);

      protected:
	void outputLogPrefix(enum SerialLogLevel logLevel);

      private:
	bool debugEnabled = false;
	char logPrefix[10] = { 0 };
};
