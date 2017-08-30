#include <Arduino.h>

#include "safe_boot.h"
#include "infarm_utils.h"
#include "relay_controller.h"
#include "light_controller.h"

void SafeBoot::init(LightController *c, RelayController *r)
{
	_lightCtrl = c;
	_relayCtrl = r;
}

void SafeBoot::allRelaysOff()
{
	_relayCtrl->allRelaysOff();
}

void SafeBoot::relaysR1toR5On()
{
	for (int i = 1; i <= 5; i++) {
		_relayCtrl->relayOn(i);
		fa_delay_seconds(5);
	}
}

bool SafeBoot::checkSchedule()
{
	if (!lightsScheduled())
		return false;

	_relayCtrl->relayOn(0);
	fa_delay_seconds(5);

	return true;
}

bool SafeBoot::lightsScheduled()
{
	/* TODO */
	return false;
}
