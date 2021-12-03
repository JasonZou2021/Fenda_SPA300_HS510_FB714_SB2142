/******************************************************************************/
/**
*
* \file	    Usersetting.h
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
#ifndef Usersetting_H
#define Usersetting_H

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include "settingmanager.h"
#include "userapp.h"
#include "bt_common.h"
#include "sdk_cfg.h"

/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/

#define UserSetting_printf(s...)       ((void)0)

//#define Usersetting_DBG

#define LOGUSERSET2(MSG...)  LOGDT(MSG);

#ifdef  Usersetting_DBG
	#undef  UserSetting_printf
	#define UserSetting_printf LOGUSERSET2
#endif

#define BT_DEVICE_NUM 10


typedef struct
{
	UINT16 Src ;
	UINT16 Volume;
#if 1 //def SB714
	UINT8 FTLR_vol;
	UINT8 FSLR_vol;
	UINT8 RTLR_vol;
#endif	
	UINT8 Bass_vol;
	UINT8 Treble_vol;
	UINT8 LR_vol;
	UINT8 Center_vol;
	UINT8 Top_vol;
	UINT8 Woofer_vol;
	UINT8 LsRs_vol;
	UINT8 dimmer;
	UINT8 VirtualSurround;
	
	UINT16 WIRELESS_Volume;
	UINT16 EarVolume;
	UINT8 EQ_State;
	UINT8 Surround_3D;
	UINT8 Repeat;
	UINT8 Shuffle;
	UINT16 KokBgm_Vol;
	UINT16 KokMic1_Vol;
	UINT16 KokMic2_Vol;
	UINT16 Kok_KeyShift;
	UINT16 Kok_Echo;
	UINT32 Kok_Threshold;
	UINT32 AUD_DPS_UserGain; //AUD_DPS_UPDATE_USRGAIN #dps
	UINT16 PairDevRecordIndex;
	BTPairDev PairDev[BT_DEVICE_NUM];
	BtAddr_t auto_addr;
#if (SUPPORT_VA)
	UINT8 AuxVA_Status;
	UINT8 BtVA_Status;
#endif
} UserSetting_T;

enum UserAppEQ
{
	eApEQ_Mode_0 = 0,
	eApEQ_Mode_1,
	eApEQ_Mode_2,
	eApEQ_Mode_3,
	eApEQ_Mode_4,     //custom       // {{Added by Heaven 
	eApEQ_Mode_Max = eApEQ_Mode_4,
	eApEQ_Mode_Num,
};

enum UserAppSurround
{
	eApSurround_Mode_0 = 0,
	eApSurround_Mode_1,
	eApSurround_Mode_2,
	eApSurround_Mode_3,
	eApSurround_Mode_Max = eApSurround_Mode_3,
	eApSurround_Mode_Num,
};

/*---------------------------------------------------------------------------*
 *                          FUNCTION   DECLARATIONS                          *
 *---------------------------------------------------------------------------*/
int UserSetting_SaveVolume(UINT16 volume);
int UserSetting_SaveEarVolume(UINT16 volume);
int UserSetting_SaveSrc(UINT16 Src);
int UserSetting_SaveEQ(UINT16 EQState);
int UserSetting_SaveSurround(UINT16 SurroundState);
int UserSetting_SaveDPS(UINT32 UserGain); //AUD_DPS_UPDATE_USRGAIN #dps
int UserSetting_ReadSrc(UINT16 *pSrc);
int UserSetting_ReadVolume(UINT16 *pVolume);
int UserSetting_SaveRepeat(UINT8 State);
int UserSetting_ReadRepeat(UINT8 *pRepeat);
int UserSetting_SaveShuffle(UINT8 State);
int UserSetting_ReadShuffle(UINT8 *pShuffle);
int UserSetting_SaveALL(void);
int UserSetting_ReadALL(UserSetting_T *pUserSetting);
int UserSetting_RestoreDefault(void);
int UserSetting_Load(void);
int UserSetting_Init(UserSetting_T *pUserSetting);
int UserSetting_SaveKokBgmVolume(int vol);
int UserSetting_SaveKokMic1Volume(int vol);
int UserSetting_SaveKokMic2Volume(int vol);
int UserSetting_SaveKokKey(int key);
int UserSetting_SaveKokEcho(int echo);
int UserSetting_SavePairDev(BTPairDev PairInfo,int order);
int UserSetting_ReadAllPairDev(BTPairDev * PairInfo);
int UserSetting_ClearAllPairDev(void);
int UserSetting_SavePairDevRecordIndex(UINT8 index);
int UserSetting_ReadPairDevRecordIndex(UINT8* pIndex);
int UserSetting_ClearBtAutoConDev(void);
int UserSetting_SaveBtAutoConDev(BtAddr_t auto_addr);
int UserSetting_ReadBtAutoConDev(BtAddr_t * auto_addr);
#if (SUPPORT_VA)
int UserSetting_SaveVAStatus(UINT8 aux_va_status, UINT8 bt_va_status);
#endif
int UserSetting_SaveBass(UINT8 volume);
int UserSetting_SaveTreble(UINT8 volume);
int UserSetting_SaveLRVol(UINT8 volume);
int UserSetting_SaveCenterVol(UINT8 volume);
int UserSetting_SaveTopVol(UINT8 volume);
int UserSetting_SaveWooferVol(UINT8 volume);
int UserSetting_SaveLsRs(UINT8 volume);
int UserSetting_SaveDimmer(UINT8 dimer);
#if 1 //def SB714
int UserSetting_SaveFTLRVol(UINT8 volume);
int UserSetting_SaveFSLRVol(UINT8 volume);
int UserSetting_SaveRTLRVol(UINT8 volume);
#endif
#endif

