/******************************************************************************/
/**
*
* \file	    Usersetting_Default.c
*
* \brief
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author
* \version	 v0.01
* \date 	 2016/09/7
******************************************************************************/

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include "Usersetting_Default.h"
#include "mf_player.h"
#include "Usersetting.h"
#include "vfd_seg_map.h"
#include "userapp.h"

/*---------------------------------------------------------------------------*
 *                            GLOBAL   VARIABLES                             *
 *---------------------------------------------------------------------------*/
//#define DEFAULT_APP 
#define USERAPP_VOL_DEFAULT 20
#define USERAPP_WIRELESS_VOL_DEFAULT 10
#define USERAPP_EAR_VOL_DEFAULT 40
#define USERAPP_EQ_DEFAULT eApEQ_Mode_0
#define USERAPP_SURROUND_DEFAULT eApSurround_Mode_0
#define USERAPP_REPEAT_DEFAULT REPEAT_MODE_NOTHING
#define USERAPP_SHUFFLE_DEFAULT PLAY_MODE_ORDER

#define KARAOKE_BGM_VOLUME_DEFAULT  3
#define KARAOKE_MIC1_VOLUME_DEFAULT 20  //0dB
#define KARAOKE_MIC2_VOLUME_DEFAULT 20  //0dB
#define KARAOKE_KEYSHIFT_DEFAULT    8   //keyshift   off
#define KARAOKE_ECHO_DEFAULT        3   //TBD
#define KARAOKE_THRESHOLD_DEFAULT 	0x0200

#define USERAPP_AUD_DPS_USERGAIN	0x80000	// AUD_DPS_UPDATE_USRGAIN #dps

#define USERAPP_Bass_DEFAULT 9
#define USERAPP_Dimmer_DEFAULT DIMMER_HIGH
#define USERAPP_Treble_DEFAULT 6
#define USERAPP_FTLR_DEFAULT 9
#define USERAPP_FSLR_DEFAULT 9
#define USERAPP_RTLR_DEFAULT 9
#define USERAPP_Center_DEFAULT 5
#define USERAPP_Top_DEFAULT 5
#define USERAPP_SL_DEFAULT 5
#define USERAPP_SR_DEFAULT 5
#define USERAPP_BT_VA_DEFAULT_STATUS VA_ON
#define USERAPP_AUX_VA_DEFAULT_STATUS VA_ON

static UserSetting_T gstDefaultUserSetting =
{
	.Src = APP_SRC_ARC,//DEFAULT_APP,
	.Volume = USERAPP_VOL_DEFAULT,
	.FTLR_vol = USERAPP_FTLR_DEFAULT,
	.FSLR_vol = USERAPP_FSLR_DEFAULT,
	.RTLR_vol = USERAPP_RTLR_DEFAULT,

	.Bass_vol = USERAPP_Bass_DEFAULT,
	.Treble_vol = USERAPP_Treble_DEFAULT,
	.LR_vol = 5,
	.Center_vol = USERAPP_Center_DEFAULT,
	.Top_vol = USERAPP_Top_DEFAULT,
	.Woofer_vol = 5,
	.LsRs_vol = USERAPP_SL_DEFAULT,
	.dimmer = USERAPP_Dimmer_DEFAULT,

	.VirtualSurround = 1,
	
	.WIRELESS_Volume = USERAPP_WIRELESS_VOL_DEFAULT,
	.EarVolume = USERAPP_EAR_VOL_DEFAULT,
	.EQ_State = USERAPP_EQ_DEFAULT,
	.Surround_3D = USERAPP_SURROUND_DEFAULT,
	.Repeat = USERAPP_REPEAT_DEFAULT,
	.Shuffle = USERAPP_SHUFFLE_DEFAULT,
	.KokBgm_Vol	= KARAOKE_BGM_VOLUME_DEFAULT,
	.KokMic1_Vol = KARAOKE_MIC1_VOLUME_DEFAULT,
	.KokMic2_Vol = KARAOKE_MIC2_VOLUME_DEFAULT,
	.Kok_KeyShift = KARAOKE_KEYSHIFT_DEFAULT,
	.Kok_Echo = KARAOKE_ECHO_DEFAULT,
	.Kok_Threshold = KARAOKE_THRESHOLD_DEFAULT,
	.AUD_DPS_UserGain = USERAPP_AUD_DPS_USERGAIN,	//AUD_DPS_UPDATE_USRGAIN  #dps
    .PairDevRecordIndex = 0,
    .auto_addr =  {{0}},

#if (SUPPORT_VA)
    .AuxVA_Status = USERAPP_AUX_VA_DEFAULT_STATUS,
    .BtVA_Status = USERAPP_BT_VA_DEFAULT_STATUS,
#endif
};

/*---------------------------------------------------------------------------*
 *                          FUNCTION   DECLARATIONS                          *
 *---------------------------------------------------------------------------*/

UserSetting_T *GetDefaultUserSetting(void)
{
	return &gstDefaultUserSetting;
}

