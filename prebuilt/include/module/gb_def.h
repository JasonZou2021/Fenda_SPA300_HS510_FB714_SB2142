/**************************************************************************/
/**
*
* \file	gb_def.h
*
* \brief	 define module index£¬for load module
*
* \note  	Copyright (c) 2016 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   email@sunplus.com
* \version	 v0.01
* \date 	 2016/04/15
***************************************************************************/
#ifndef __GB_DEF_H
#define __GB_DEF_H

/*---------------------------------------------------------------------------*
* DATE TYPES
*---------------------------------------------------------------------------*/
//!define module index, Can't Large than 32
enum
{
	MODULE_BASE = 0,
	MODULE_IOP0, // normal: ir, cec
	MODULE_IOP1, // standby
	MODULE_IOP2, // Low Freq mode, risc alive
	MODULE_FS,
	MODULE_AUD,
	MODULE_AP,
	MODULE_AP_DEMUX,
	MODULE_FL,
	MODULE_FL_ID3,
	MODULE_BT,
	MODULE_NET,
	MODULE_AIRPLAY,
	MODULE_DLNA,
	MODULE_CLI,
	MODULE_USER,
	MODULE_OTHER,
	MODULE_Ns,
} MODULEINDEX;

extern unsigned char _text_drv_base[];
extern unsigned char _text_drv_fs[];
extern unsigned char _text_drv_aud[];
extern unsigned char _text_drv_ap[];
extern unsigned char _text_drv_ap_demux[];
extern unsigned char _text_drv_fl[];
extern unsigned char _text_drv_fl_id3[];
extern unsigned char _text_drv_bt[];
extern unsigned char _text_drv_net[];
extern unsigned char _text_drv_airplay[];
extern unsigned char _text_drv_dlna[];
extern unsigned char _text_drv_cli[];
extern unsigned char _text_drv_user[];
extern unsigned char _text_drv_other[];
extern unsigned char _text_drv_iop0[];
extern unsigned char _text_drv_iop1[];
extern unsigned char _text_drv_iop2[];

extern unsigned char _text_drv_base_end[];
extern unsigned char _text_drv_fs_end[];
extern unsigned char _text_drv_aud_end[];
extern unsigned char _text_drv_ap_end[];
extern unsigned char _text_drv_ap_demux_end[];
extern unsigned char _text_drv_fl_end[];
extern unsigned char _text_drv_fl_id3_end[];
extern unsigned char _text_drv_bt_end[];
extern unsigned char _text_drv_net_end[];
extern unsigned char _text_drv_airplay_end[];
extern unsigned char _text_drv_dlna_end[];
extern unsigned char _text_drv_cli_end[];
extern unsigned char _text_drv_user_end[];
extern unsigned char _text_drv_other_end[];
extern unsigned char _text_drv_iop0_end[];
extern unsigned char _text_drv_iop1_end[];
extern unsigned char _text_drv_iop2_end[];


#ifdef BUILT4ISP2
#define MODULE_TEXT_ADDRS {\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
	}

#define MODULE_TEXT_ADDRS_END {\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
		NULL,\
	}
#else
#define MODULE_TEXT_ADDRS {\
		_text_drv_base,\
		_text_drv_iop0,\
		_text_drv_iop1,\
		_text_drv_iop2,\
		_text_drv_fs,\
		_text_drv_aud,\
		_text_drv_ap,\
		_text_drv_ap_demux,\
		_text_drv_fl,\
		_text_drv_fl_id3,\
		_text_drv_bt,\
		_text_drv_net,\
		_text_drv_airplay,\
		_text_drv_dlna,\
		_text_drv_cli,\
		_text_drv_user,\
		_text_drv_other,\
	}
#define MODULE_TEXT_ADDRS_END {\
		_text_drv_base_end,\
		_text_drv_iop0_end,\
		_text_drv_iop1_end,\
		_text_drv_iop2_end,\
		_text_drv_fs_end,\
		_text_drv_aud_end,\
		_text_drv_ap_end,\
		_text_drv_ap_demux_end,\
		_text_drv_fl_end,\
		_text_drv_fl_id3_end,\
		_text_drv_bt_end,\
		_text_drv_net_end,\
		_text_drv_airplay_end,\
		_text_drv_dlna_end,\
		_text_drv_cli_end,\
		_text_drv_user_end,\
		_text_drv_other_end,\
	}

#endif

void LoadAppModule(UINT32 swIdModule);
void UnloadAppModule(UINT32 swIdModule);

#endif

