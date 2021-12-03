#include <string.h>
#include "qpc.h"
#include "log_utils.h"
#include "custom_sig.h"
#include "types.h"

#include "wireless.h"

extern WIRELESS_FUNC wl_ev01se;

WIRELESS_FUNC *wireless = &wl_ev01se;

int wireless_init(void)
{
	int ret = -1;
	WIRELESS_FUNC *wl =  wireless;

	if(wl != NULL && wl->wl_Init != NULL)
		ret = wl->wl_Init();

	return ret;
}


int wireless_pair(BYTE status)
{
	int ret = -1;
	WIRELESS_FUNC *wl =  wireless;

	if(wl != NULL && wl->wl_pair != NULL)
		ret = wl->wl_pair(status);

	return ret;
}


int wireless_volume(unsigned char gain)
{
	int ret = -1;
	WIRELESS_FUNC *wl =  wireless;

	if(wl != NULL && wl->wl_vol != NULL)
		ret = wl->wl_vol(gain);

	return ret;
}


int wireless_woofer_vol(unsigned char gain)
{
	int ret = -1;
	WIRELESS_FUNC *wl =  wireless;

	if(wl != NULL && wl->wl_woofer_vol != NULL)
		ret = wl->wl_woofer_vol(gain);

	return ret;
}

int wireless_surround_vol(unsigned char gain)
{
	int ret = -1;
	WIRELESS_FUNC *wl =  wireless;

	if(wl != NULL && wl->wl_surround_vol != NULL)
		ret = wl->wl_surround_vol(gain);

	return ret;
}

int wireless_bass_vol(unsigned char gain)
{
	int ret = -1;
	WIRELESS_FUNC *wl =  wireless;

	if(wl != NULL && wl->wl_bass_vol != NULL)
		ret = wl->wl_bass_vol(gain);

	return ret;
}

int wireless_eq(unsigned char eq)
{
	int ret = -1;
	WIRELESS_FUNC *wl =  wireless;

	if(wl != NULL && wl->wl_eq != NULL)
		ret = wl->wl_eq(eq);

	return ret;
}

int wireless_mute(unsigned char mute)
{
	int ret = -1;
	WIRELESS_FUNC *wl =  wireless;

	if(wl != NULL && wl->wl_mute != NULL)
		ret = wl->wl_mute(mute);

	return ret;
}


int wireless_get_version(UINT8 *type)
{
	int ret = -1;
	WIRELESS_FUNC *wl =  wireless;

	if(wl != NULL && wl->wl_version != NULL)
		ret = wl->wl_version(type);

	return ret;
}


int wireless_polling(void)
{
	int ret = -1;
	WIRELESS_FUNC *wl =  wireless;

	if(wl != NULL && wl->wl_poll != NULL)
		ret = wl->wl_poll();

	return ret;
}

int wireless_get_syn(void)
{
	int ret = -1;
	WIRELESS_FUNC *wl =  wireless;

	if(wl != NULL && wl->wl_get_syn != NULL)
		ret = wl->wl_get_syn();

	return ret;
}

// type   0: RX    1:  TX
int wireless_upg(QActive* me, ARFunc_t func, eWirelessUpg type)
{
	int ret = -1;
	WIRELESS_FUNC *wl =  wireless;

	if(wl != NULL && wl->wl_upg != NULL)
		ret = wl->wl_upg(me, func, type);

	return ret;
}

int wireless_standby(void)
{
	int ret = -1;
	WIRELESS_FUNC *wl =  wireless;

	if(wl != NULL && wl->wl_standby!= NULL)
		ret = wl->wl_standby();

	return ret;
}

