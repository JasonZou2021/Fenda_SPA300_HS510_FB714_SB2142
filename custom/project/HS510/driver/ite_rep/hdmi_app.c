#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "pinmux.h"
//#include "i2c_driver.h"
#include "fsi_api.h"
//#include "std_interfaces.h"
#include "busy_delay.h"
#include "amp.h"
#include "exstd_interfaces.h"
#include "log_utils.h"
#include "hdmi_app.h"

HDMI_FUNC *hdmi_func = &ite_it6622;


int hdmi_cfg(BYTE mode, BYTE src, BYTE vol, BYTE volmax)
{
	int ret = -1;
	
	HDMI_FUNC *pfunc = hdmi_func;

	if(pfunc && pfunc->cfg)
	{
		ret = pfunc->cfg(mode, src, vol, volmax);
	}

	return ret;
}

int hdmi_init(BYTE mode, BYTE src)
{
	int ret = -1;
	
	HDMI_FUNC *pfunc = hdmi_func;

	if(pfunc && pfunc->init)
	{
		ret = pfunc->init(mode, src);
	}

	return ret;
}

int hdmi_get_status(BYTE mode)
{
	int ret = -1;
	HDMI_FUNC *pfunc = hdmi_func;

	if(pfunc && pfunc->status)
	{
		ret = pfunc->status(mode);
	}

	return ret;
}


int hdmi_handle(BYTE cmd, BYTE para1, BYTE para2)
{
	int ret = -1;
	HDMI_FUNC *pfunc = hdmi_func;

	if(pfunc && pfunc->handle)
	{
		ret = pfunc->handle(cmd, para1, para2);
	}

	return ret;
}

int hdmi_upg(void)
{
	int ret = -1;
	HDMI_FUNC *pfunc = hdmi_func;

	if(pfunc && pfunc->upg)
	{
		ret = pfunc->upg();
	}

	return ret;
}

int hdmi_CheckEARC(void)
{
	int ret = -1;
	HDMI_FUNC *pfunc = hdmi_func;

	if(pfunc && pfunc->check_eARC)
	{
		ret = pfunc->check_eARC();
	}

	return ret;
}


int hdmi_CheckADOType(void)
{
	int ret = -1;
	HDMI_FUNC *pfunc = hdmi_func;

	if(pfunc && pfunc->check_ADOType)
	{
		ret = pfunc->check_ADOType();
	}

	return ret;
}

int hdmi_GetVersion(void)
{
	int ret = -1;
	HDMI_FUNC *pfunc = hdmi_func;

	if(pfunc && pfunc->ver)
	{
		ret = pfunc->ver();
	}

	return ret;
}

