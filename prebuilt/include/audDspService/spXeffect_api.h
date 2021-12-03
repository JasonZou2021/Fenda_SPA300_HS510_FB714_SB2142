/******************************************************************************/
/**
*
* \file	 spXeffect.h
*
* \brief	8107 xeffect api .\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author    email@sunplus.com
* \version	 v0.01
* \date 	 2020/10/15
******************************************************************************/

#ifndef _SPXEFFECT_API_H_
#define _SPXEFFECT_API_H_

#include "types.h"
#include "AudUserControl.h"
#include "AudDspService.h"

//define return value
#define XEFFECT_OK					(0)
#define XEFFECT_FAIL				(1)
#define XEFFECT_ALLOC_FAIL			(2)
#define XEFFECT_CHANNEL_REUSE		(3)
#define XEFFECT_STRATEGY_FAIL		(4)
#define XEFFECT_DATABUF_CREATFEAIL  (5)
#define XEFFECT_PATHCTL_CREATFEAIL  (6)
#define XEFFECT_MAX_ERRVAL  		(10)

//xeffect mode
/*
	struct {
		UINT32  alloc_channel     : 4;  // 1 ~ 4 channel.
		UINT32  channel_reserv    :12;  // 5 ~ 16 channel.
		UINT32  channel_playmode  : 4;  //0: promptly  1: smooth
		UINT32  reserv            : 8;
		UINT32  strategy_id		  : 4;  //strategy
	}
*/
#define XEFFET_USE_PATH1				(1)
#define XEFFET_USE_PATH2				(1 << 1)
#define XEFFET_USE_PATH3				(1 << 2)
#define XEFFET_USE_PATH4				(1 << 3)
#define XEFFET_PROMPTLY_PALY			(1 << 16)
#define XEFFET_SMOOTH_PALY				(0 << 16)
#define XEFFET_PLAYMODE_SINGLEPATH		(1 << 28)

#define XEFFECT_L_CH 					(1 << L)
#define XEFFECT_R_CH 					(1 << R)
#define XEFFECT_LS_CH 					(1 << Ls)
#define XEFFECT_RS_CH 					(1 << Rs)
#define XEFFECT_C_CH 					(1 << C)
#define XEFFECT_SUB_CH 					(1 << Sub)
#define XEFFECT_LTF_CH 					(1 << Ltf)
#define XEFFECT_RTF_CH 					(1 << Rtf)
#define XEFFECT_LRS_CH 					(1 << Lrs)
#define XEFFECT_RRS_CH 					(1 << Rrs)
#define XEFFECT_LTR_CH 					(1 << Ltr)
#define XEFFECT_RTR_CH 					(1 << Rtr)
#define XEFFECT_LW_CH 					(1 << Lw)
#define XEFFECT_RW_CH 					(1 << Rw)
#define XEFFECT_LTM_CH 					(1 << Ltm)
#define XEFFECT_RTM_CH 					(1 << Rtm)

UINT32 spXeffect_Alloc(INT32 mode, UINT32 data_buf_size);
INT32 spXeffect_free(INT32 xef_handle);
void spXeffect_Play(INT32 xef_handle, UINT8 *pbData, UINT32 dSize);
void spXeffect_Stop(INT32 xef_handle);
void spXeffect_SetHintVol(INT32 xef_handle, double dB);
void spXeffect_SetDuckVol(INT32 xef_handle, double dB);
void spXeffect_ChSelect(int mode, UINT16 L_to_out_ch_num, UINT16 R_to_out_ch_num);

#endif //_SPXEFFECT_API_H_

