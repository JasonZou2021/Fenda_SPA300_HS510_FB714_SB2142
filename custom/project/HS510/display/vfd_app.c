#include <string.h>
#include <stdio.h>
#include "log_utils.h"
#include "cfg_sdk.h"
#include "vfd_seg_map.h"
#include "cfg_sdk.h"
#include "VFD.h"

#define VFD_LEN 5

VFD_CFG vfd_cfg = 
{
	.seg_tbl = VFD_14SEG_TBL,
	.vfd_func = &vfd_aip1944,
	.vfd_len = VFD_LEN,
};

int vfd_init(void)
{
	BYTE first_setup = 1;
	VFD_FUNC *pvfd_func = vfd_cfg.vfd_func;

	if(first_setup)
	{
		first_setup = 0;
		vfd_set_seg_table();
	}
	
	if(pvfd_func && pvfd_func->init)
	{
		pvfd_func->init();
	}
	
	return 0;
}


int vfd_standby_init(void)
{
	BYTE first_setup = 1;
	VFD_FUNC *pvfd_func = vfd_cfg.vfd_func;

	if(first_setup)
	{
		first_setup = 0;
		vfd_set_seg_table();
	}
	
	if(pvfd_func && pvfd_func->standby_init)
	{
		pvfd_func->standby_init();
	}
	
	return 0;
}


int vfd_show(void)
{
	VFD_FUNC *pvfd_func = vfd_cfg.vfd_func;

	if(pvfd_func && pvfd_func->show)
	{
		pvfd_func->show();
	}

	return 0;
}

int vfd_set_str(const char *s)
{
	VFD_FUNC *pvfd_func = vfd_cfg.vfd_func;

	if(pvfd_func && pvfd_func->set)
	{
		pvfd_func->set(s);
	}

	return 0;
}


int vfd_dimmer(BYTE level)
{
	VFD_FUNC *pvfd_func = vfd_cfg.vfd_func;

	if(pvfd_func && pvfd_func->dimmer)
	{
		pvfd_func->dimmer(level);
	}

	return 0;
}


int vfd_dot(UINT32 p1, UINT32 p2)
{
	VFD_FUNC *pvfd_func = vfd_cfg.vfd_func;
	if(pvfd_func && pvfd_func->dot)
	{
		pvfd_func->dot(p1, p2);
	}

	return 0;
}


UINT32 vfd_read_key(void)
{
	VFD_FUNC *pvfd_func = vfd_cfg.vfd_func;
	UINT32 key = 0;

	if(pvfd_func && pvfd_func->key)
	{
		key = pvfd_func->key();
	}

	return key;
}


int vfd_off(void)
{
	VFD_FUNC *pvfd_func = vfd_cfg.vfd_func;

	if(pvfd_func && pvfd_func->off)
	{
	 	pvfd_func->off();
	}

	return 0;
}

int vfd_get_vfd_len(void)
{
	return vfd_cfg.vfd_len;
}



VFDOP_T	 g_stVfdApp =
{
	vfd_init,
	vfd_dot,
	vfd_set_str,
	vfd_get_vfd_len,
	vfd_read_key,
	NULL,
};


