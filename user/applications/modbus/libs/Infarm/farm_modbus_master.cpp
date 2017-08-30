#include "farm_modbus_master.h"

int FarmModbusMaster::slaveStartPump(enum DosingPump n, uint16_t onTimeMs)
{
	slaveRegisters->pumpSetTimerValue(n, onTimeMs);
	return slaveUpdatePumpTimer(n);
}

int FarmModbusMaster::slaveSendWriteRegisterModbusPacket(uint16_t address, uint16_t *reg)
{
	modbusPacket.u8id = slaveBusAddress;
	modbusPacket.u8fct = MB_FC_WRITE_REGISTER;
	modbusPacket.u16RegAdd = address;
	modbusPacket.u16CoilsNo = 1;
	modbusPacket.au16reg = reg;
	return slaveSendModbusPacket();
}

int FarmModbusMaster::slaveUpdatePumpTimer(enum DosingPump n)
{
	return slaveSendWriteRegisterModbusPacket(slaveRegisters->pumpTimerRegisterAddress(n),
						  slaveRegisters->registerPumpTimer(n));
}

int FarmModbusMaster::slaveSetRealTimeClock(uint8_t hours, uint8_t minutes)
{
	slaveRegisters->realTimeClockValueSet(hours, minutes);
	return slaveSendWriteRegisterModbusPacket(slaveRegisters->realTimeClockRegisterAddress(),
						  slaveRegisters->registerRealTimeClock());
}

int FarmModbusMaster::slaveFeedWatchdog()
{
	slaveRegisters->feedTheWatchdog();
	return slaveSendWriteRegisterModbusPacket(slaveRegisters->watchdogFeedRegisterAddress(),
						  slaveRegisters->registerWatchdogFeed());
}

int FarmModbusMaster::slaveSendModbusPacket()
{
	waitingForSlaveReply = false;
	if (bus.getState() != COM_IDLE)
		return -FarmModbusErrorMasterBusy;

	int r = bus.query(modbusPacket);
	if (r < 0)
		return -FarmModbusErrorMasterWrongQuery;

	waitingForSlaveReply = true;
	slaveWaitForReplyMs = millis() + slaveReplyTimeoutMs;
	return FarmModbusErrorNoError;
}

void FarmModbusMaster::setSlaveBusAddress(uint8_t address)
{
	slaveBusAddress = address;
}

void FarmModbusMaster::setSlaveRegisters(FarmModbusControllinoRegisters *r)
{
	slaveRegisters = r;
}

bool FarmModbusMaster::intervalHasPassed(uint32_t interval)
{
	uint32_t now = millis();

	if (interval > now)
		return false;

	return true;
}

bool FarmModbusMaster::busIsIdle()
{
#ifdef WE_HAVE_MORE_TIME_AND_CAN_FIX_THIS_MESS_NOW
	return ((bus.getState() == COM_IDLE) && !busIsWaitingForSlaveReply());
#endif
	return bus.getState() == COM_IDLE;
}

bool FarmModbusMaster::busIsWaitingForSlaveReply()
{
	return waitingForSlaveReply;
}

int FarmModbusMaster::poll()
{
	return bus.poll();

#ifdef WE_HAVE_MORE_TIME_AND_CAN_FIX_THIS_MESS_NOW
	if (waitingForSlaveReply && intervalHasPassed(slaveWaitForReplyMs)) {
		/* TODO: retry? */
		waitingForSlaveReply = false;
		return -FarmModbusErrorSlaveReplyTimeout;
	}

	/* TODO: poll() needs more love for better error handling */
	if (bus.poll() != 0) {
		return -FarmModbusErrorMasterPollFailed;
	}

	int busState = bus.getState();
	if (waitingForSlaveReply && busState == COM_WAITING) {
		return FarmModbusErrorNoError;
	}

	if (waitingForSlaveReply && busState == COM_IDLE) {
		waitingForSlaveReply = false;
		return FarmModbusErrorNoError;
	}

	if (waitingForSlaveReply && (bus.getLastError() != 0)) {
		waitingForSlaveReply = false;
		return -FarmModbusErrorBusError;
	}

	return FarmModbusErrorNoError;
#endif
}
