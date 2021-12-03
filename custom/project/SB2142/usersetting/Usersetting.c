/******************************************************************************/
/**
*
* \file	    Usersetting.c
*
* \brief
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author    tingwei
* \version	 v0.01
* \date 	 2016/09/7
******************************************************************************/
#include <string.h>
#include "std_interfaces.h"
#include "types.h"
#include "settingmanager.h"
#include "Usersetting.h"
#include "Usersetting_Default.h"
#define LOG_MODULE_ID ID_APUSR
#include "log_utils.h"

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/

static UserSetting_T *gpUserSettingPool = NULL;
extern void *memcpy(void *pDst, const void *pSrc, UINT32 uiLen);
extern void *memset(void *pDst, int iFillValue, UINT32 uiLen);


/******************************************************************************************/
/**
 * \fn      static INT32 UserSettingPool_Init(void)
 *
 * \brief   custom save partition data buffer init
 *
 * \param   void
 *
 * \return  0: success , other: fail.
 *
******************************************************************************************/
static INT32 UserSettingPool_Init(void)
{
	INT32 sdRet = SUCCESS;
#if !(defined(ROMTER) || defined(BUILT4ISP2))
	gpUserSettingPool = (UserSetting_T *)SaveContentGet(USERCONTENT);
	if (gpUserSettingPool == NULL)
	{
		gpUserSettingPool = (UserSetting_T *)malloc(sizeof(UserSetting_T));
		if (gpUserSettingPool != NULL)
		{
			UserSetting_T *pstUserSettingInit;

			pstUserSettingInit = (UserSetting_T *)GetDefaultUserSetting();

			memcpy((void *)gpUserSettingPool, (void *)pstUserSettingInit, sizeof(UserSetting_T));
			LOGI("build default setup value \n");
			SavePartitionAlloc(USERCONTENT, gpUserSettingPool, sizeof(UserSetting_T));
			sdRet = SUCCESS;
		}
	}
	else if(SettingSizeIsEqual(USERCONTENT, sizeof(UserSetting_T)) == 0)
	{
		UserSetting_T stInit;

		memset((void *)&stInit, 0x5a, sizeof(UserSetting_T));
		if (ResavePartition(((WRITETHROUGH << 8) | USERCONTENT), &stInit, sizeof(UserSetting_T)) == 0)
			gpUserSettingPool = (UserSetting_T *)SaveContentGet(USERCONTENT);

		sdRet = FAIL;
    }
	else
	{
		LOGI("[%s]get the save buf \n", __FUNCTION__);
		sdRet = SUCCESS;
	}
#else
	gpUserSettingPool = NULL;
	return FAIL;
#endif
	return sdRet;
}

#if 0
/******************************************************************************************/
/**
 * \fn      static UINT16 UserSettingPool_GetSetupStart()
 *
 * \brief   the item offset, which in the UserSetting_T struct
 *
 * \param   void
 *
 * \return  other: item start address, which in the UserSetting_T struct.
 *
******************************************************************************************/
static UINT16 UserSettingPool_GetSetupStart()
{
	return  ( UINT16 ) ( ( void * )gpUserSettingPool - ( void * )gpUserSettingPool );
}
#endif

/******************************************************************************************/
/**
 * \fn      static INT32 UserSettingPool_ReadInfo(INT32 addr, UINT8 *pBuf, INT32 sdLen)
 *
 * \brief   read the save data
 *
 * \param   addr :(input) the offset value.
 * \param   pBuf :(input) the read data buffer point.
 * \param   sdLen :(input) buffer length.
 *
 * \return  0: success , other: fail.
 *
******************************************************************************************/
static INT32 UserSettingPool_ReadInfo(INT32 addr, UINT8 *pBuf, INT32 sdLen)
{
	INT32 sdRet;

#if !(defined(ROMTER) || defined(BUILT4ISP2))
	sdRet = SavePartitionDataGet(USERCONTENT, addr, (void *)pBuf, (UINT32)sdLen);
#else
	(void)addr;
	(void)pBuf;
	(void)sdLen;
	sdRet = SUCCESS;
#endif

	return sdRet;
}

/******************************************************************************************/
/**
 * \fn      static INT32 UserSettingPool_SaveInfo(INT32 addr, UINT8 *pBuf, INT32 sdLen, INT32 sdWriteMode)
 *
 * \brief   write the save data into partition space
 *
 * \param   addr :(input) the offset value.
 * \param   pBuf :(input) the write data buffer point.
 * \param   sdLen :(input) write length.
 * \param   sdWriteMode :(input) write mode WRITEBACK or WRITETHROUGH.
 *
 * \return  0: success , other: fail.
 *
******************************************************************************************/
static INT32 UserSettingPool_SaveInfo(INT32 addr, UINT8 *pBuf, INT32 sdLen, INT32 sdWriteMode)
{
	INT32 sdRet;

#if !(defined(ROMTER) || defined(BUILT4ISP2))
	sdRet = SavePartitionUpdate((sdWriteMode << 8 | USERCONTENT), addr, (void *)pBuf, (UINT32)sdLen);
#else
	(void)addr;
	(void)pBuf;
	(void)sdLen;
	(void)sdWriteMode;
	sdRet = SUCCESS;
#endif

	return sdRet;
}

/******************************************************************************************/
/**
 * \fn			int UserSetting_SaveKokBgmVolume(UINT16 vol)
 *
 * \brief		       save customer  karaoke bgm setting volume value to save partition
 *
 *
 * \param		UINT16 vol
 *
 *
 * \return	    return : 1(SETTING_SAVE_INFO_SUCCESS) ; -1(SETTING_SAVE_INFO_FAIL)
 *
 *
 ******************************************************************************************/
int UserSetting_SaveKokBgmVolume(int vol)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stSaveBgmVolume;

	stSaveBgmVolume.KokBgm_Vol = vol;
	LOGI("stSaveBgmVolume.KokBgm_Vol = %d..\n", stSaveBgmVolume.KokBgm_Vol);
	LOGI("sizeof(stSaveBgmVolume.KokBgm_Vol) = %d..\n", sizeof(stSaveBgmVolume.KokBgm_Vol));
	Offset = (UINT16)((UINT8 *)(&stSaveBgmVolume.KokBgm_Vol) - (UINT8 *)(&stSaveBgmVolume));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stSaveBgmVolume.KokBgm_Vol), sizeof(stSaveBgmVolume.KokBgm_Vol), WRITEBACK);
	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}

/******************************************************************************************/
/**
 * \fn			int UserSetting_SaveKokMic1Volume(UINT16 vol)
 *
 * \brief		save customer  karaoke mic1 setting volume value to save partition
 *
 *
 * \param		UINT16 vol
 *
 *
 * \return	    return : 1(SETTING_SAVE_INFO_SUCCESS) ; -1(SETTING_SAVE_INFO_FAIL)
 *
 *
 ******************************************************************************************/
int UserSetting_SaveKokMic1Volume(int vol)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stSaveKokMic1Volume;

	stSaveKokMic1Volume.KokMic1_Vol = vol;
	LOGI("stSaveKokMic1Volume.KokMic1_Vol = %d..\n", stSaveKokMic1Volume.KokMic1_Vol);
	LOGI("sizeof(stSaveKokMic1Volume.KokMic1_Vol) = %d..\n", sizeof(stSaveKokMic1Volume.KokMic1_Vol));
	Offset = (UINT16)((UINT8 *)(&stSaveKokMic1Volume.KokMic1_Vol) - (UINT8 *)(&stSaveKokMic1Volume));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stSaveKokMic1Volume.KokMic1_Vol), sizeof(stSaveKokMic1Volume.KokMic1_Vol), WRITEBACK);
	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}

/******************************************************************************************/
/**
 * \fn			int UserSetting_SaveKokMic2Volume(UINT16 vol)
 *
 * \brief		save customer  karaoke mic2 setting volume value to save partition
 *
 *
 * \param		UINT16 vol
 *
 *
 * \return	    return : 1(SETTING_SAVE_INFO_SUCCESS) ; -1(SETTING_SAVE_INFO_FAIL)
 *
 *
 ******************************************************************************************/
int UserSetting_SaveKokMic2Volume(int vol)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stSaveKokMic2Volume;

	stSaveKokMic2Volume.KokMic2_Vol = vol;
	LOGI("stSaveKokMi2Volume.KokMic2_Vol = %d..\n", stSaveKokMic2Volume.KokMic2_Vol);
	LOGI("sizeof(stSaveKokMic2Volume.KokMic2_Vol) = %d..\n", sizeof(stSaveKokMic2Volume.KokMic2_Vol));
	Offset = (UINT16)((UINT8 *)(&stSaveKokMic2Volume.KokMic2_Vol) - (UINT8 *)(&stSaveKokMic2Volume));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stSaveKokMic2Volume.KokMic2_Vol), sizeof(stSaveKokMic2Volume.KokMic2_Vol), WRITEBACK);
	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}

/******************************************************************************************/
/**
 * \fn			int UserSetting_SaveKokKey(UINT16 key)
 *
 * \brief		save customer  karaoke keyshift setting  value to save partition
 *
 *
 * \param		UINT16 key
 *
 *
 * \return	    return : 1(SETTING_SAVE_INFO_SUCCESS) ; -1(SETTING_SAVE_INFO_FAIL)
 *
 *
 ******************************************************************************************/
int UserSetting_SaveKokKey(int key)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stSaveKokKey;

	stSaveKokKey.Kok_KeyShift = key;
	LOGI("stSaveKokKey.Kok_KeyShift = %d..\n", stSaveKokKey.Kok_KeyShift);
	LOGI("sizeof(stSaveKokEcho.Kok_KeyShift) = %d..\n", sizeof(stSaveKokKey.Kok_KeyShift));
	Offset = (UINT16)((UINT8 *)(&stSaveKokKey.Kok_KeyShift) - (UINT8 *)(&stSaveKokKey));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stSaveKokKey.Kok_KeyShift), sizeof(stSaveKokKey.Kok_KeyShift), WRITEBACK);
	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}

/******************************************************************************************/
/**
 * \fn			int UserSetting_SaveKokEcho(UINT16 echo)
 *
 * \brief		save customer  karaoke echo setting  value to save partition
 *
 *
 * \param		UINT16 echo
 *
 *
 * \return	    return : 1(SETTING_SAVE_INFO_SUCCESS) ; -1(SETTING_SAVE_INFO_FAIL)
 *
 *
 ******************************************************************************************/
int UserSetting_SaveKokEcho(int echo)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stSaveKokEcho;

	stSaveKokEcho.Kok_Echo = echo;
	LOGI("stSaveKokEcho.Kok_Echo = %d..\n", stSaveKokEcho.Kok_Echo);
	LOGI("sizeof(stSaveKokEcho.Kok_Echo) = %d..\n", sizeof(stSaveKokEcho.Kok_Echo));
	Offset = (UINT16)((UINT8 *)(&stSaveKokEcho.Kok_Echo) - (UINT8 *)(&stSaveKokEcho));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stSaveKokEcho.Kok_Echo), sizeof(stSaveKokEcho.Kok_Echo), WRITEBACK);
	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}

/******************************************************************************************/
/**
 * \fn			int UserSetting_SaveVolume(UINT16 volume)
 *
 * \brief		save customer setting volume value to save partition
 *
 *
 * \param		UINT16 volume
 *
 *
 * \return	    return : 1(SETTING_SAVE_INFO_SUCCESS) ; -1(SETTING_SAVE_INFO_FAIL)
 *
 *
 ******************************************************************************************/

int UserSetting_SaveVolume(UINT16 volume)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stSaveVolume;

	stSaveVolume.Volume = volume;
	LOGI("stSaveVolume.Volume = %d..\n", stSaveVolume.Volume);
	LOGI("sizeof(stSaveVolume.Volume) = %d..\n", sizeof(stSaveVolume.Volume));
	Offset = (UINT16)((UINT8 *)(&stSaveVolume.Volume) - (UINT8 *)(&stSaveVolume));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stSaveVolume.Volume), sizeof(stSaveVolume.Volume), WRITETHROUGH);
	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}
int UserSetting_SaveBass(UINT8 volume)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stSaveVolume;

	stSaveVolume.Bass_vol = volume;

	Offset = (UINT16)((UINT8 *)(&stSaveVolume.Bass_vol) - (UINT8 *)(&stSaveVolume));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stSaveVolume.Bass_vol), sizeof(stSaveVolume.Bass_vol), WRITETHROUGH);
	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}

int UserSetting_SaveTreble(UINT8 volume)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stSaveVolume;

	stSaveVolume.Treble_vol = volume;

	Offset = (UINT16)((UINT8 *)(&stSaveVolume.Treble_vol) - (UINT8 *)(&stSaveVolume));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stSaveVolume.Treble_vol), sizeof(stSaveVolume.Treble_vol), WRITETHROUGH);
	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}

#if 1 //def SB714
int UserSetting_SaveFTLRVol(UINT8 volume)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stSaveVolume;

	stSaveVolume.FTLR_vol = volume;

	Offset = (UINT16)((UINT8 *)(&stSaveVolume.FTLR_vol) - (UINT8 *)(&stSaveVolume));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stSaveVolume.FTLR_vol), sizeof(stSaveVolume.FTLR_vol), WRITETHROUGH);
	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}

int UserSetting_SaveFSLRVol(UINT8 volume)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stSaveVolume;

	stSaveVolume.FSLR_vol = volume;

	Offset = (UINT16)((UINT8 *)(&stSaveVolume.FSLR_vol) - (UINT8 *)(&stSaveVolume));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stSaveVolume.FSLR_vol), sizeof(stSaveVolume.FSLR_vol), WRITETHROUGH);
	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}

int UserSetting_SaveRTLRVol(UINT8 volume)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stSaveVolume;

	stSaveVolume.RTLR_vol = volume;

	Offset = (UINT16)((UINT8 *)(&stSaveVolume.RTLR_vol) - (UINT8 *)(&stSaveVolume));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stSaveVolume.RTLR_vol), sizeof(stSaveVolume.RTLR_vol), WRITETHROUGH);
	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}
#endif

int UserSetting_SaveLRVol(UINT8 volume)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stSaveVolume;

	stSaveVolume.LR_vol= volume;

	Offset = (UINT16)((UINT8 *)(&stSaveVolume.LR_vol) - (UINT8 *)(&stSaveVolume));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stSaveVolume.LR_vol), sizeof(stSaveVolume.LR_vol), WRITEBACK);
	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}

int UserSetting_SaveCenterVol(UINT8 volume)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stSaveVolume;

	stSaveVolume.Center_vol= volume;

	Offset = (UINT16)((UINT8 *)(&stSaveVolume.Center_vol) - (UINT8 *)(&stSaveVolume));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stSaveVolume.Center_vol), sizeof(stSaveVolume.Center_vol), WRITETHROUGH);
	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}

int UserSetting_SaveTopVol(UINT8 volume)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stSaveVolume;

	stSaveVolume.Top_vol = volume;

	Offset = (UINT16)((UINT8 *)(&stSaveVolume.Top_vol) - (UINT8 *)(&stSaveVolume));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stSaveVolume.Top_vol), sizeof(stSaveVolume.Top_vol), WRITETHROUGH);
	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}

int UserSetting_SaveWooferVol(UINT8 volume)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stSaveVolume;

	stSaveVolume.Woofer_vol= volume;

	Offset = (UINT16)((UINT8 *)(&stSaveVolume.Woofer_vol) - (UINT8 *)(&stSaveVolume));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stSaveVolume.Woofer_vol), sizeof(stSaveVolume.Woofer_vol), WRITEBACK);
	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}

int UserSetting_SaveLsRs(UINT8 volume)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stSaveVolume;

	stSaveVolume.LsRs_vol= volume;

	Offset = (UINT16)((UINT8 *)(&stSaveVolume.LsRs_vol) - (UINT8 *)(&stSaveVolume));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stSaveVolume.LsRs_vol), sizeof(stSaveVolume.LsRs_vol), WRITEBACK);
	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}

int UserSetting_SaveDimmer(UINT8 dimer)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stSaveVolume;

	stSaveVolume.dimmer= dimer;

	Offset = (UINT16)((UINT8 *)(&stSaveVolume.dimmer) - (UINT8 *)(&stSaveVolume));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stSaveVolume.dimmer), sizeof(stSaveVolume.dimmer), WRITETHROUGH);
	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}

int UserSetting_SaveVirtualSurround(UINT8 Surround)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stSaveVolume;

	stSaveVolume.VirtualSurround= Surround;

	Offset = (UINT16)((UINT8 *)(&stSaveVolume.VirtualSurround) - (UINT8 *)(&stSaveVolume));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stSaveVolume.VirtualSurround), sizeof(stSaveVolume.VirtualSurround), WRITEBACK);
	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}

/******************************************************************************************/
/**
 * \fn			int UserSetting_SaveEarVolume(UINT16 volume)
 *
 * \brief		save customer setting ear volume value to save partition
 *
 *
 * \param		UINT16 volume
 *
 *
 * \return	    return : 1(SETTING_SAVE_INFO_SUCCESS) ; -1(SETTING_SAVE_INFO_FAIL)
 *
 *
 ******************************************************************************************/

int UserSetting_SaveEarVolume(UINT16 volume)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stSaveVolume;

	stSaveVolume.EarVolume = volume;
	LOGI("stSaveVolume.EarVolume = %d..\n", stSaveVolume.EarVolume);
	LOGI("sizeof(stSaveVolume.EarVolume) = %d..\n", sizeof(stSaveVolume.EarVolume));
	Offset = (UINT16)((UINT8 *)(&stSaveVolume.EarVolume) - (UINT8 *)(&stSaveVolume));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stSaveVolume.EarVolume), sizeof(stSaveVolume.EarVolume), WRITEBACK);
	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}

/******************************************************************************************/
/**
 * \fn			int UserSetting_SaveSrc(UINT16 Src)
 *
 * \brief		save customer setting src value to save partition
 *
 *
 * \param		UINT16 Src
 *
 *
 * \return	    return : 1(SETTING_SAVE_INFO_SUCCESS) ; -1(SETTING_SAVE_INFO_FAIL)
 *
 *
 ******************************************************************************************/

int UserSetting_SaveSrc(UINT16 Src)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stSaveSrc;

	stSaveSrc.Src = Src;
	LOGI("stSaveSrc.Src = %d..\n", stSaveSrc.Src);
	LOGI("sizeof(stSaveSrc.Src) = %d..\n", sizeof(stSaveSrc.Src));
	Offset = (UINT16)((UINT8 *)(&stSaveSrc.Src) - (UINT8 *)(&stSaveSrc));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stSaveSrc.Src), sizeof(stSaveSrc.Src), WRITETHROUGH);

	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}



/******************************************************************************************/
/**
 * \fn			int UserSetting_SaveEQ(UINT16 EQState)
 *
 * \brief		save customer setting EQ value to save partition
 *
 *
 * \param		UINT16 EQState
 *
 *
 * \return	    return : 1(SETTING_SAVE_INFO_SUCCESS) ; -1(SETTING_SAVE_INFO_FAIL)
 *
 *
 ******************************************************************************************/

int UserSetting_SaveEQ(UINT16 EQState)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stUserSetting;

	stUserSetting.EQ_State = EQState;
	LOGI("stUserSetting.EQ_State = %d..\n", stUserSetting.EQ_State);
	Offset = (UINT16)((UINT8 *)(&stUserSetting.EQ_State) - (UINT8 *)(&stUserSetting));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stUserSetting.EQ_State), sizeof(stUserSetting.EQ_State), WRITEBACK);

	if (ret != SUCCESS)
	{
		LOGDT("UserSetting Error! %s : %d\n", __FUNCTION__, EQState);
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}

/******************************************************************************************/
/**
 * \fn			int UserSetting_SaveSurround(UINT16 SurroundState)
 *
 * \brief		save customer setting Surround value to save partition
 *
 *
 * \param		UINT16 SurroundState
 *
 *
 * \return	    return : 1(SETTING_SAVE_INFO_SUCCESS) ; -1(SETTING_SAVE_INFO_FAIL)
 *
 *
 ******************************************************************************************/

int UserSetting_SaveSurround(UINT16 SurroundState)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stUserSetting;

	stUserSetting.Surround_3D = SurroundState;
	LOGI("stUserSetting.Surround_3D = %d..\n", stUserSetting.Surround_3D);
	Offset = (UINT16)((UINT8 *)(&stUserSetting.Surround_3D) - (UINT8 *)(&stUserSetting));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stUserSetting.Surround_3D), sizeof(stUserSetting.Surround_3D), WRITEBACK);

	if (ret != SUCCESS)
	{
		LOGDT("UserSetting Error! %s : %d\n", __FUNCTION__, SurroundState);
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}

//AUD_DPS_UPDATE_USRGAIN  #dps
int UserSetting_SaveDPS(UINT32 UserGain)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stUserSetting;

	stUserSetting.AUD_DPS_UserGain = UserGain;
	LOGI("stUserSetting.AUD_DPS_UserGain = %d..\n", stUserSetting.AUD_DPS_UserGain);
	Offset = (UINT16)((UINT8 *)(&stUserSetting.AUD_DPS_UserGain) - (UINT8 *)(&stUserSetting));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stUserSetting.AUD_DPS_UserGain), sizeof(stUserSetting.AUD_DPS_UserGain), WRITEBACK);

	if (ret != SUCCESS)
	{
		LOGE("UserSetting Error! %s : %d\n", __FUNCTION__, UserGain);
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;

}



/******************************************************************************************/
/**
 * \fn			int UserSetting_SaveRepeat(UINT8 State)
 *
 * \brief		save customer setting Repeat value to save partition
 *
 *
 * \param		UINT8 State
 *
 *
 * \return	    return : 1(SETTING_SAVE_INFO_SUCCESS) ; -1(SETTING_SAVE_INFO_FAIL)
 *
 *
 ******************************************************************************************/

int UserSetting_SaveRepeat(UINT8 State)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stUserSetting;

	stUserSetting.Repeat = State;
	LOGI("stUserSetting.Repeat = %d..\n", stUserSetting.Repeat);
	Offset = (UINT16)((UINT8 *)(&stUserSetting.Repeat) - (UINT8 *)(&stUserSetting));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stUserSetting.Repeat), sizeof(stUserSetting.Repeat), WRITEBACK);

	if (ret != SUCCESS)
	{
		LOGDT("UserSetting Error! %s : %d\n", __FUNCTION__, State);
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}

/******************************************************************************************/
/**
 * \fn			int UserSetting_SaveShuffle(UINT16 State)
 *
 * \brief		save customer setting shuffle value to save partition
 *
 *
 * \param		UINT16 State
 *
 *
 * \return	    return : 1(SETTING_SAVE_INFO_SUCCESS) ; -1(SETTING_SAVE_INFO_FAIL)
 *
 *
 ******************************************************************************************/

int UserSetting_SaveShuffle(UINT8 State)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stUserSetting;

	stUserSetting.Shuffle = State;
	LOGI("stUserSetting.Repeat = %d..\n", stUserSetting.Shuffle);
	Offset = (UINT16)((UINT8 *)(&stUserSetting.Shuffle) - (UINT8 *)(&stUserSetting));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stUserSetting.Shuffle), sizeof(stUserSetting.Shuffle), WRITEBACK);

	if (ret != SUCCESS)
	{
		LOGDT("UserSetting Error! %s : %d\n", __FUNCTION__, State);
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}

/******************************************************************************************/
/**
 * \fn			int UserSetting_ReadRepeat(UINT8* pRepeat)
 *
 * \brief		read customer setting Repeat value from save partition
 *
 *
 * \param		UINT8* pRepeat
 *
 *
 * \return	    return : 1(SETTING_READ_INFO_SUCCESS) ; -1(SETTING_READ_INFO_FAIL)
 *
 *
 ******************************************************************************************/

int UserSetting_ReadRepeat(UINT8 *pRepeat)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stReadRepeat;

	Offset = (UINT16)((UINT8 *)(&stReadRepeat.Repeat) - (UINT8 *)(&stReadRepeat));

	ret = UserSettingPool_ReadInfo(Offset, (UINT8 *)(&stReadRepeat.Repeat), sizeof(stReadRepeat.Repeat));
	LOGUSERSET2("stReadSrc.Repeat = %d..\n", stReadRepeat.Repeat);
	if (ret != SUCCESS)
	{
		return SETTING_READ_INFO_FAIL;
	}
	*pRepeat = stReadRepeat.Repeat;
	return SETTING_READ_INFO_SUCCESS;
}

/******************************************************************************************/
/**
 * \fn			int UserSetting_ReadShuffle(UINT8* pShuffle)
 *
 * \brief		read customer setting shuffle value from save partition
 *
 *
 * \param		UINT8* pShuffle
 *
 *
 * \return	    return : 1(SETTING_READ_INFO_SUCCESS) ; -1(SETTING_READ_INFO_FAIL)
 *
 *
 ******************************************************************************************/

int UserSetting_ReadShuffle(UINT8 *pShuffle)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stReadShuffle;

	Offset = (UINT16)((UINT8 *)(&stReadShuffle.Shuffle) - (UINT8 *)(&stReadShuffle));

	ret = UserSettingPool_ReadInfo(Offset, (UINT8 *)(&stReadShuffle.Shuffle), sizeof(stReadShuffle.Shuffle));
	LOGI("stReadShuffle.Shuffle = %d..\n", stReadShuffle.Shuffle);
	if (ret != SUCCESS)
	{
		return SETTING_READ_INFO_FAIL;
	}
	*pShuffle = stReadShuffle.Shuffle;
	return SETTING_READ_INFO_SUCCESS;
}


/******************************************************************************************/
/**
 * \fn			int UserSetting_ReadSrc(UINT16 * pSrc)
 *
 * \brief		read customer setting src value from save partition
 *
 *
 * \param		UINT16 * pSrc
 *
 *
 * \return	    return : 1(SETTING_READ_INFO_SUCCESS) ; -1(SETTING_READ_INFO_FAIL)
 *
 *
 ******************************************************************************************/

int UserSetting_ReadSrc(UINT16 *pSrc)
{
	int ret = SUCCESS;
	UINT16 Offset ;
	UserSetting_T stReadSrc;

	Offset = (UINT16)((UINT8 *)(&stReadSrc.Src) - (UINT8 *)&stReadSrc);
	ret = UserSettingPool_ReadInfo(Offset, (UINT8 *)(&stReadSrc.Src), sizeof(stReadSrc.Src));
	LOGI("stReadSrc.Src = %d..\n", stReadSrc.Src);
	if (ret != SUCCESS)
	{
		return SETTING_READ_INFO_FAIL;
	}
	*pSrc = stReadSrc.Src;
	return SETTING_READ_INFO_SUCCESS;
}

/******************************************************************************************/
/**
 * \fn			int UserSetting_ReadVolume(UINT16 * pVolume)
 *
 * \brief		read customer setting volume value from save partition
 *
 *
 * \param		UINT16 * pVolume
 *
 *
 * \return	    return : 1(SETTING_READ_INFO_SUCCESS) ; -1(SETTING_READ_INFO_FAIL)
 *
 *
 ******************************************************************************************/

int UserSetting_ReadVolume(UINT16 *pVolume)
{
	int ret = SUCCESS;
	UINT16 Offset ;
	UserSetting_T stReadVolume;

	Offset = (UINT16)((UINT8 *)(&stReadVolume.Volume) - (UINT8 *)&stReadVolume);
	ret = UserSettingPool_ReadInfo(Offset, (UINT8 *)(&stReadVolume.Volume), sizeof(stReadVolume.Volume));
	LOGI("stReadVolume.Volume = %d..\n", stReadVolume.Volume);
	if (ret != SUCCESS)
	{
		return SETTING_READ_INFO_FAIL;
	}
	*pVolume = stReadVolume.Volume;
	return SETTING_READ_INFO_SUCCESS;
}

int UserSetting_SavePairDev(BTPairDev PairInfo,int i)
{
    UINT16 Offset ;
    int ret = SUCCESS;
    UserSetting_T stUserSetting;

    memcpy(&stUserSetting.PairDev[i],&PairInfo,sizeof(BTPairDev));
    //LOGI("stUserSetting.PairDev = %d..\n",stUserSetting.Repeat);
    Offset = (UINT16)((UINT8*)(&stUserSetting.PairDev[i]) - (UINT8*)(&stUserSetting));
    ret = UserSettingPool_SaveInfo(Offset, (UINT8*)(&stUserSetting.PairDev[i]),sizeof(BTPairDev),WRITETHROUGH);

    if(ret != SUCCESS)
    {
       // LOGDT("UserSetting Error! %s : %d\n", __FUNCTION__, State);
        return SETTING_SAVE_INFO_FAIL;
    }
    return SETTING_SAVE_INFO_SUCCESS;
}


int UserSetting_ReadAllPairDev(BTPairDev * PairInfo)
{
    int ret = SUCCESS;
    UINT16 Offset ;
    UserSetting_T stReadPairInfo;

    Offset = (UINT16)((UINT8*)(&stReadPairInfo.PairDev) - (UINT8*)&stReadPairInfo);
    ret = UserSettingPool_ReadInfo(Offset, (UINT8*)(&stReadPairInfo.PairDev),sizeof(stReadPairInfo.PairDev));
    if(ret != SUCCESS)
    {
        return SETTING_READ_INFO_FAIL;
    }
    memcpy(PairInfo,&stReadPairInfo.PairDev,sizeof(stReadPairInfo.PairDev));
    return SETTING_READ_INFO_SUCCESS;
}

int UserSetting_ClearAllPairDev()
{
    UINT16 Offset ;
    int ret = SUCCESS;
    UserSetting_T stUserSetting;

    memset(&stUserSetting.PairDev,0,sizeof(stUserSetting.PairDev));
    //LOGI("stUserSetting.PairDev = %d..\n",stUserSetting.Repeat);
    Offset = (UINT16)((UINT8*)(&stUserSetting.PairDev) - (UINT8*)(&stUserSetting));
    ret = UserSettingPool_SaveInfo(Offset, (UINT8*)(&stUserSetting.PairDev),sizeof(stUserSetting.PairDev),WRITETHROUGH);

    if(ret != SUCCESS)
    {
       // LOGDT("UserSetting Error! %s : %d\n", __FUNCTION__, State);
        return SETTING_SAVE_INFO_FAIL;
    }
    return SETTING_SAVE_INFO_SUCCESS;
}

int UserSetting_SavePairDevRecordIndex(UINT8 index)
{
    UINT16 Offset ;
    int ret = SUCCESS;
    UserSetting_T stUserSetting;

    stUserSetting.PairDevRecordIndex = index;
    LOGD("stUserSetting.PairDevRecordIndex = %d..\n",stUserSetting.PairDevRecordIndex);
    Offset = (UINT16)((UINT8*)(&stUserSetting.PairDevRecordIndex) - (UINT8*)(&stUserSetting));
    ret = UserSettingPool_SaveInfo(Offset, (UINT8*)(&stUserSetting.PairDevRecordIndex),sizeof(stUserSetting.PairDevRecordIndex),WRITETHROUGH);

    if(ret != SUCCESS)
    {
        LOGDT("UserSetting Error! %s : %d\n", __FUNCTION__, index);
        return SETTING_SAVE_INFO_FAIL;
    }
    return SETTING_SAVE_INFO_SUCCESS;
}

int UserSetting_ReadPairDevRecordIndex(UINT8* pIndex)
{
    UINT16 Offset ;
    int ret = SUCCESS;
    UserSetting_T stReadPairDevRecordIndex;

    Offset = (UINT16)((UINT8*)(&stReadPairDevRecordIndex.PairDevRecordIndex) - (UINT8*)(&stReadPairDevRecordIndex));

    ret = UserSettingPool_ReadInfo(Offset, (UINT8*)(&stReadPairDevRecordIndex.PairDevRecordIndex),sizeof(stReadPairDevRecordIndex.PairDevRecordIndex));
    LOGD("stReadPairDevRecordIndex.PairDevRecordIndex = %d..\n",stReadPairDevRecordIndex.PairDevRecordIndex);
    if(ret != SUCCESS)
    {
        return SETTING_READ_INFO_FAIL;
    }
    *pIndex = stReadPairDevRecordIndex.PairDevRecordIndex;
    return SETTING_READ_INFO_SUCCESS;
}

int UserSetting_ClearBtAutoConDev()
{
    UINT16 Offset ;
    int ret = SUCCESS;
    UserSetting_T stUserSetting;

    memset(&stUserSetting.auto_addr,0,sizeof(stUserSetting.auto_addr));
    //LOGI("stUserSetting.PairDev = %d..\n",stUserSetting.Repeat);
    Offset = (UINT16)((UINT8*)(&stUserSetting.auto_addr) - (UINT8*)(&stUserSetting));
    ret = UserSettingPool_SaveInfo(Offset, (UINT8*)(&stUserSetting.auto_addr),sizeof(stUserSetting.auto_addr),WRITETHROUGH);

    if(ret != SUCCESS)
    {
       // LOGDT("UserSetting Error! %s : %d\n", __FUNCTION__, State);
        return SETTING_SAVE_INFO_FAIL;
    }
    return SETTING_SAVE_INFO_SUCCESS;
}

int UserSetting_SaveBtAutoConDev(BtAddr_t auto_addr)
{
    UINT16 Offset ;
    int ret = SUCCESS;
    UserSetting_T stUserSetting;

    memcpy(&stUserSetting.auto_addr,&auto_addr,sizeof(auto_addr));
    //UserSetting_printf("stUserSetting.PairDev = %d..\n",stUserSetting.Repeat);
    Offset = (UINT16)((UINT8*)(&stUserSetting.auto_addr) - (UINT8*)(&stUserSetting));
    ret = UserSettingPool_SaveInfo(Offset, (UINT8*)(&stUserSetting.auto_addr),sizeof(auto_addr),WRITETHROUGH);

    if(ret != SUCCESS)
    {
       // LOGDT("UserSetting Error! %s : %d\n", __FUNCTION__, State);
        return SETTING_SAVE_INFO_FAIL;
    }
    return SETTING_SAVE_INFO_SUCCESS;
}

int UserSetting_ReadBtAutoConDev(BtAddr_t * auto_addr)
{
    int ret = SUCCESS;
    UINT16 Offset ;
    UserSetting_T stReadBtAutoAddr;

    Offset = (UINT16)((UINT8*)(&stReadBtAutoAddr.auto_addr) - (UINT8*)&stReadBtAutoAddr);
    ret = UserSettingPool_ReadInfo(Offset, (UINT8*)(&stReadBtAutoAddr.auto_addr),sizeof(stReadBtAutoAddr.auto_addr));
    if(ret != SUCCESS)
    {
        return SETTING_READ_INFO_FAIL;
    }
    memcpy(auto_addr,&stReadBtAutoAddr.auto_addr,sizeof(stReadBtAutoAddr.auto_addr));
    return SETTING_READ_INFO_SUCCESS;
}

#if (SUPPORT_VA)
/******************************************************************************************/
/**
 * \fn			int UserSetting_SaveVAStatus(UINT8 aux_va_status, UINT8 bt_va_status)
 *
 * \brief		save customer setting va status to save partition
 *
 *
 * \param		UINT8 aux_va_status, UINT8 bt_va_status
 *
 *
 * \return	    return : 1(SETTING_SAVE_INFO_SUCCESS) ; -1(SETTING_SAVE_INFO_FAIL)
 *
 *
 ******************************************************************************************/

int UserSetting_SaveVAStatus(UINT8 aux_va_status, UINT8 bt_va_status)
{
	UINT16 Offset ;
	int ret = SUCCESS;
	UserSetting_T stSaveVAStatus;

	stSaveVAStatus.AuxVA_Status = aux_va_status;
	stSaveVAStatus.BtVA_Status = bt_va_status;
	LOGI("stSaveVAStatus.AuxVA_Status = %d..\n", stSaveVAStatus.AuxVA_Status);
	LOGI("stSaveVAStatus.BtVA_Status = %d..\n", stSaveVAStatus.BtVA_Status);
	Offset = (UINT16)((UINT8 *)(&stSaveVAStatus.AuxVA_Status) - (UINT8 *)(&stSaveVAStatus));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stSaveVAStatus.AuxVA_Status), sizeof(stSaveVAStatus.AuxVA_Status), WRITEBACK);
	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}

	Offset = (UINT16)((UINT8 *)(&stSaveVAStatus.BtVA_Status) - (UINT8 *)(&stSaveVAStatus));
	ret = UserSettingPool_SaveInfo(Offset, (UINT8 *)(&stSaveVAStatus.BtVA_Status), sizeof(stSaveVAStatus.BtVA_Status), WRITEBACK);
	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}
#endif

/******************************************************************************************/
/**
 * \fn			int UserSetting_SaveALL(void)
 *
 * \brief		save all customer setting value to save partition
 *
 *
 * \param		void
 *
 *
 * \return	    return : 1(SETTING_READ_INFO_SUCCESS) ; -1(SETTING_READ_INFO_FAIL)
 *
 *
 ******************************************************************************************/

int UserSetting_SaveALL(void)
{
	int ret = SUCCESS;
	ret = SavePartitionDataSync(USERCONTENT);
	if (ret != SUCCESS)
	{
		return SETTING_SAVE_INFO_FAIL;
	}
	return SETTING_SAVE_INFO_SUCCESS;
}

/******************************************************************************************/
/**
 * \fn			int UserSetting_ReadALL(UserSetting_T* pUserSetting)
 *
 * \brief		read all customer setting data from save partition
 *
 *
 * \param		UserSetting_T* pUserSetting : user save structure pointer
 *
 *
 * \return	    return : 1(SETTING_READ_INFO_SUCCESS) ; -1(SETTING_READ_INFO_FAIL)
 *
 *
 ******************************************************************************************/

int UserSetting_ReadALL(UserSetting_T *pUserSetting)
{
	UserSetting_T stReadALL;
	int ret = SUCCESS;

	ret = UserSettingPool_ReadInfo(0, (UINT8 *)&stReadALL, sizeof(stReadALL));

	if (ret != SUCCESS)
	{
		return SETTING_READ_INFO_FAIL;
	}

	memcpy(pUserSetting, &stReadALL, sizeof(UserSetting_T));

	return SETTING_READ_INFO_SUCCESS;
}

/******************************************************************************************/
/**
 * \fn			int UserSetting_RestoreDefault(void)
 *
 * \brief		customer setting restore to default value
 *
 *
 * \param
 *
 *
 * \return	    return : 0(SETTING_Default_SUCCESS) ; -1(SETTING_Default_FAIL)
 *
 *
 ******************************************************************************************/

int UserSetting_RestoreDefault(void)
{
	int ret = SETTING_Default_SUCCESS;
	UserSetting_T *pstUserSettingInit;
	UserSetting_T stRestoreDefault;

	pstUserSettingInit = (UserSetting_T *)GetDefaultUserSetting();
	memcpy(&stRestoreDefault, pstUserSettingInit, sizeof(stRestoreDefault));

	ret = UserSettingPool_SaveInfo(0, (UINT8 *)(&stRestoreDefault), sizeof(stRestoreDefault), WRITETHROUGH);
	if (ret != SUCCESS)
	{
		return SETTING_Default_FAIL;
	}
	return SETTING_Default_SUCCESS ;

}

/******************************************************************************************/
/**
 * \fn			int UserSetting_Load(void)
 *
 * \brief		customer save partition data buffer(in flash) init
 *
 *
 * \param
 *
 *
 * \return	    none.
 *
 *
 ******************************************************************************************/
int UserSetting_Load(void)
{
	int Init_Check;

	Init_Check = UserSettingPool_Init();
	if (Init_Check != SUCCESS)
	{
		return SETTING_INIT_FAIL;
	}
	else
	{
		return SETTING_INIT_SUCCESS;
	}
	return SETTING_INIT_SUCCESS;
}

/******************************************************************************************/
/**
 * \fn			int UserSetting_Init(UserSetting_T* pstSetting)
 *
 * \brief		customer setting default data and init
 *
 *
 * \param		UserSetting_T* pstSetting : user save structure pointer
 *
 *
 * \return	    return : 1(SETTING_INIT_SUCCESS) ; -1(SETTING_INIT_FAIL)
 *
 *
 ******************************************************************************************/

int UserSetting_Init(UserSetting_T *pstSetting)
{
#ifdef BUILT4ISP2
	LOGD("%s, BUILT4ISP2=1, Init to 0\n", __FUNCTION__);
	memset(pstSetting, 0, sizeof(UserSetting_T));
	return SETTING_INIT_SUCCESS;
#else
	int ret ;

	ret = UserSetting_Load();
	if (ret != SETTING_INIT_SUCCESS)
	{
		UserSetting_T *pstUserSettingInit;
		UserSetting_T stInit;

		pstUserSettingInit = (UserSetting_T *)GetDefaultUserSetting();

		memset(pstUserSettingInit->PairDev,0,sizeof(pstUserSettingInit->PairDev)); //zehai.liu todo
		LOGD("UserSetting_Init:%d\n",sizeof(pstUserSettingInit->PairDev));

		memcpy(&stInit, pstUserSettingInit, sizeof(stInit));
		#if !(defined(ROMTER) || defined(BUILT4ISP2))
		ret = UserSettingPool_SaveInfo(0, (UINT8 *)(&stInit), sizeof(stInit), WRITETHROUGH);
		if (ret != SETTING_Default_SUCCESS)
		{
			return SETTING_INIT_FAIL;
		}
		#endif
		memcpy(pstSetting, &stInit, sizeof(UserSetting_T));

		LOGI("pUserSetting->Src = %d...error\n", pstSetting->Src);
		LOGI("pUserSetting->Volume = %d...error\n", pstSetting->Volume);
		//LOGI("pUserSetting->Volume = %d...error\n", pstSetting->WIRELESS_Volume);
		LOGI("pUserSetting->EQ_State = %d...error\n", pstSetting->EQ_State);
		LOGI("pUserSetting->Surround_3D = %d...error\n", pstSetting->Surround_3D);
		LOGI("pUserSetting->Repeat = %d...error\n", pstSetting->Repeat);
		LOGI("pUserSetting->Shuffle = %d...error\n", pstSetting->Shuffle);
		LOGI("pUserSetting->KokBgm_Vol = %d...error\n", pstSetting->KokBgm_Vol);
		LOGI("pUserSetting->KokMic1_Vol = %d...error\n", pstSetting->KokMic1_Vol);
		LOGI("pUserSetting->KokMic2_Vol = %d...error\n", pstSetting->KokMic2_Vol);
		LOGI("pUserSetting->Kok_KeyShift = %d...error\n", pstSetting->Kok_KeyShift);
		LOGI("pUserSetting->Kok_Echo = %d...error\n", pstSetting->Kok_Echo);
	}
	else
	{
		ret = UserSetting_ReadALL(pstSetting);
		LOGI("pUserSetting->Src = %d...success\n", pstSetting->Src);
		LOGI("pUserSetting->Volume = %d...success\n", pstSetting->Volume);
		//LOGI("pUserSetting->WIRELESS_Volume = %d...success\n", pstSetting->WIRELESS_Volume);
		LOGI("pUserSetting->EQ_State = %d...success\n", pstSetting->EQ_State);
		LOGI("pUserSetting->Surround_3D = %d...success\n", pstSetting->Surround_3D);
		LOGI("pUserSetting->Repeat = %d...success\n", pstSetting->Repeat);
		LOGI("pUserSetting->Shuffle = %d...success\n", pstSetting->Shuffle);
		LOGI("pUserSetting->KokBgm_Vol = %d...success\n", pstSetting->KokBgm_Vol);
		LOGI("pUserSetting->KokMic1_Vol = %d...success\n", pstSetting->KokMic1_Vol);
		LOGI("pUserSetting->KokMic2_Vol = %d...success\n", pstSetting->KokMic2_Vol);
		LOGI("pUserSetting->Kok_KeyShift = %d...success\n", pstSetting->Kok_KeyShift);
		LOGI("pUserSetting->Kok_Echo = %d...success\n", pstSetting->Kok_Echo);

		if (ret != SETTING_READ_INFO_SUCCESS)
		{
			return SETTING_INIT_FAIL;
		}
	}
	return SETTING_INIT_SUCCESS;
#endif
}


