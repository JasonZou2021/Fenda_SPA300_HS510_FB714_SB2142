#ifndef __BT_BACKGROUND_APP_C__
#define __BT_BACKGROUND_APP_C__
#include "user_bt.h"
#include "userapp.h"
#include "bt_background_app.h"
#include "Usersetting.h"
#include "AudDspService.h"
#include "amp.h"
#include "bt_mw_core.h"


static const UINT16 bDspSubVolTable[41] =
{
	0x0, 0x19F, 0x1AF, 0x1BF, 0x1CF, 0x1DF, 0x1EF, 0x1FF, 0x20F, 0x21F,
	0x22F, 0x237, 0x23F, 0x247, 0x24F, 0x257, 0x25F, 0x267, 0x26F, 0x277,
	0x27F, 0x287, 0x28F, 0x297, 0x29F, 0x2A7, 0x2AF, 0x2B7, 0x2BF, 0x2C7,
	0x2CF, 0x2D7, 0x2DF, 0x2E7, 0x2EF, 0x2F7, 0x2FF, 0x307, 0x30F, 0x317,
	0x31F,
};


static const UINT16 bDspVolTable[41] =
{
	0x00, 0x36d, 0x38b, 0x3a9, 0x3c7, 0x3e5, 0x403, 0x421, 0x43f, 0x45d,
	0x47b, 0x499, 0x4b7, 0x4d5, 0x4f3, 0x511, 0x52f, 0x54d, 0x56b, 0x589,
	0x5a7, 0x5c5, 0x5e3, 0x601, 0x61f, 0x63d, 0x65b, 0x679, 0x697, 0x6b5,
	0x6d3, 0x6f1, 0x70f, 0x72d, 0x74b, 0x769, 0x787, 0x7a5, 0x7c3, 0x7e1,
	0x7ff,
};

/*the value must match SrcType_e's order:
[0]:speakout delay times
[1]:waterlvel packet nums for sub.please do not adjust this ,if it's not necessary
*/
static const UINT8 bDelaySyncTable[MD_MAX][2] =
{
	{0, 0},
	{62, 3},
	{62, 3},
	{69, 3},
	{79, 2},
	{79, 2},
	{72, 2},
	{72, 2},
	{79, 2},
	{79, 2},
};

//static BtBackGroundQMSM *pstBtBackGroupApp = NULL;
static BtBackGroundQMSM stBtBackGroundApp;
stBtDevInfo remoteBtDevInfo;



QState BtBackGroupApp_Idle(BtBackGroundQMSM *const me, QEvt const *const e);
QState BtBackGroupApp_Intend(BtBackGroundQMSM *const me, QEvt const *const e) ;
int UserBtSetWssVolume(BtBackGroundQMSM *me, int val);
int UserBtSetVolumeSync(BtBackGroundQMSM *const me, int volume, DEVROLE_DEX_e role);

int BtSaveDevInfo(BTPairDev *stPairDev,DevInfoType_e type)
{
	//static UINT8 replace_order = 0;
	int i;
	BTPairDev Saved_PairInfo[BT_DEVICE_NUM];
    UserSetting_ReadAllPairDev(Saved_PairInfo);

	for(i=0;i<BT_DEVICE_NUM;i++)
	{
		if(memcmp(&stPairDev->bd_addr,&Saved_PairInfo[i].bd_addr,sizeof(BtAddr_t))==0)
		{
			LOGD("find same addr...i:%d...\n",i);
			break;
		}
	}

	if(i == BT_DEVICE_NUM)
	{
		for(i=0;i<BT_DEVICE_NUM;i++)
		{
			if(Saved_PairInfo[i].used == 0)
			{
				LOGD("find empty position...i:%d...\n",i);
				break;
			}
		}
	}


	if(i == BT_DEVICE_NUM)
	{
		UINT_8 *pIndex = &stBtBackGroundApp.PairDevRecordIndex;
		//UINT_8 *pIndex = &stBtBackGroupApp.PairDevRecordIndex;
		i = *pIndex;

		LOGD("replace the device info...i:%d...\n",i);
		if(*pIndex == BT_DEVICE_NUM-1)
		{
			*pIndex = 0;
		}
		else
		{
			*pIndex++;
		}

		UserSetting_SavePairDevRecordIndex(*pIndex);
	}

	switch(type)
	{
		case NAME:
		{
			memcpy(Saved_PairInfo[i].name,stPairDev->name,stPairDev->name_len);
			Saved_PairInfo[i].name_len = stPairDev->name_len;
		}
		break;
		case LINKKEY:
		{
			memcpy(Saved_PairInfo[i].linkKey,stPairDev->linkKey,16);
		}
		break;
	}

	memcpy(&Saved_PairInfo[i].bd_addr,&stPairDev->bd_addr,6);

	Saved_PairInfo[i].used = 1;
	UserSetting_SavePairDev(Saved_PairInfo[i],i);

	return SUCCESS;
}

int BtGetRemoteAddress(BtAddr_t *pAddress)
{
	BtAddr_t Addr = {{0}};
	UserSetting_ReadBtAutoConDev(&Addr);

	if(pAddress != NULL)
	{
		*pAddress = Addr;
		return SUCCESS;
	}

	return FAIL;
}


/******************************************************************************************/
/**
 * \fn		QActive * UserBtTargetAo(void)
 *
 * \brief		get target ao to post msg
 *
 * \param		void
 *
 * \return	      QActive *
 *
 ******************************************************************************************/
QActive *UserBtTargetAo(void)
{
	//BtBackGroundQMSM *const me = pstBtBackGroupApp;
	BtBackGroundQMSM * const me = &stBtBackGroundApp;

	if (me)
		return me->TargetAo;

	return NULL;
}
/******************************************************************************************/
/**
 * \fn		int UserAppVolumeNotifyToBt(INT16 Vol,INT8 MuteState)
 *
 * \brief		used to inform volume to bt ,witch run in background
 *
 * \param	INT16 Vol,INT8 MuteState
 *
 * \return	      0  , -1
 *
 ******************************************************************************************/
int UserAppVolumeNotifyToBt(INT16 Vol, INT8 MuteState)
{
	stSYS_AudioInfo_t *pAudioInfo = NULL;
	pAudioInfo = Q_NEW(stSYS_AudioInfo_t, BT_VOL_SYNC_SET_SIG);

	if (pAudioInfo != NULL)
	{
		pAudioInfo->MuteState = MuteState;
		pAudioInfo->Vol = Vol;
		pAudioInfo->MaxVol = USERAPP_VOL_MAX;
		pAudioInfo->MinVol = USERAPP_VOL_MIN;
		QACTIVE_POST(UserBtTargetAo(), (QEvt *)pAudioInfo, UserBtTargetAo());
		return SUCCESS;
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		int UserAppSetSubVolumeToBt(UINT8 Vol)
 *
 * \brief		used to inform volume to bt ,witch run in background
 *
 * \param	UINT8 Vol
 *
 * \return	      0  , -1
 *
 ******************************************************************************************/
int UserAppSetSubVolumeToBt(UINT8 Vol)
{
	stVolSet *pSubVol = NULL;
	pSubVol = Q_NEW(stVolSet, BT_VOL_SET_SUB_GAIN_SIG);

	if (pSubVol != NULL)
	{
		pSubVol->vol = Vol;
		QACTIVE_POST(UserBtTargetAo(), (QEvt *)pSubVol, UserBtTargetAo());
		return SUCCESS;
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		int UserBtInit(BtBackGroundQMSM * const me)
 *
 * \brief		used Bt  to register user cbk
 *
 * \param	UserApp * const me
 *
 * \return	    NULL
 *
 ******************************************************************************************/
int BtAPPInit(BtBackGroundQMSM *const me)
{
	(void)me;
	//Sunplus//BtInit();
	BtInitStart();
	return UserBtCbkRegister(UserBtTargetAo());
}

/******************************************************************************************/
/**
 * \fn		int UserBtSetWaterDelay(UINT8 iSrc)
 *
 * \brief		set waterlevel to sub and set speakout delay
 *
 * \param	UserApp * const me
 *
 * \return	    NULL
 *
 ******************************************************************************************/
int UserBtSetWaterDelay(UINT8 iSrc)
{
	SrcType_e type = MD_NONE;
	BtDelaySyncInfo DelaySyncInfo;
	ap_printf("UserBtSetWaterDelay iSrc : %d\n", iSrc);

	switch (iSrc)
	{
		case eAp_USB_Src:
			type = MD_USB;
			break;

		case eAp_CARD_Src:
			type = MD_CARD;
			break;

#if (!SUPPORT_VA)
		case eAp_AUX_Src:
			type = MD_AUXIN;
			break;
#endif

		case eAp_LINE_Src:
			type = MD_LINEIN;
			break;


		case eAp_SPDIF_Optical_Src:
			type = MD_SPDIFIN2;
			break;

#if defined(CFG_COAX_ENABLE) && (CFG_COAX_ENABLE == 1)

		case eAp_SPDIF_Coaxial_Src:
			type = MD_SPDIFIN3;
			break;
#endif

		case eAp_ARC_Src:
			type = MD_ARC;
			break;

		#ifdef USERAPP_SUPPORT_BT
		case eAp_Bt_Src:
			type = MD_BT;
			break;
		#endif

        #ifdef USERAPP_SUPPORT_HDMI_TX
		case eAp_HDMI_TX_Src:
			type = MD_HDMI_TX;
			break;
		#endif

		case eApUserAppSrc_Num:
			type = MD_NONE;
			break;

		default:
			type = MD_MAX;
			ap_printf("no match src !!! please check why? : %d\n", iSrc);
			return FAIL;
			break;
	}

	DelaySyncInfo.src = type;
	DelaySyncInfo.DelayTime = bDelaySyncTable[type][0];;
	DelaySyncInfo.WaterLevelPacketNums = bDelaySyncTable[type][1];
	return BtSetWssDelayLevel(DelaySyncInfo);
}

/******************************************************************************************/
/**
 * \fn		int UserBtSetWssVolume(BtBackGroundQMSM *me,UINT8 val)
 *
 * \brief		Set sub Volume,Volume val:0~40step (note val: 0~799, 0.125dB/step, 799=0dB )
 *
 * \param
 *              		val:Volume val
 *
 * \return	    void
 *
 ******************************************************************************************/
int UserBtSetWssVolume(BtBackGroundQMSM *me, int val)
{
	int res = FAIL;
	CFG_BT_T *pBtConf = GET_SDK_CFG(CFG_BT_T);
	CFG_BTMAIN_T *pBtSdkCfg = &(pBtConf->BtSdkCfg.BtMainCfg);
	(void)me;
	(void)val;

	if ((val > USERAPP_VOL_MAX) || (val < USERAPP_VOL_MIN))
	{
		ap_printf_E("FAIL: %d\n", val);
		return FAIL;
	}

	if (pBtSdkCfg->TwsWork == ON)
	{
		if (me->BtTwsMode == WSS)
		{
			ap_printf("wss Volume = %d,real_val=%d\n", val, bDspSubVolTable[val]);

			if (bDspSubVolTable[val] > 799)
			{
				ap_printf_E("val range:0~799\n");
				return FAIL;
			}

			//AudDspService_Set_VolumeChannel(UserBtTargetAo(), CH_SUB, bDspSubVolTable[val]);
		}
		else if (me->BtTwsMode == HEADPHONE)
		{
			if (bDspVolTable[val] > 2047)
			{
				ap_printf_E("val range:0~2047\n");
				return FAIL;
			}

			//AudDspService_Set_Volume(UserBtTargetAo(), bDspVolTable[val], 150);
		}
	}
	else
	{
		ap_printf_E("FAIL: %d\n", res);
		return FAIL;
	}

	return res;
}
/******************************************************************************************/
/**
 * \fn		int UserBtSetVolumeSync(BtBackGroundQMSM * const me,int volume, DEVROLE_DEX_e role)
 *
 * \brief		input Set  Volume,Volume val:0~USERAPP_VOL_MAX  ,role:phone or headphone
 *
 * \param
 *              		val:Volume val
 *
 * \return	    void
 *
 ******************************************************************************************/
int UserBtSetVolumeSync(BtBackGroundQMSM *const me, int volume, DEVROLE_DEX_e role)
{
	int res = FAIL;

	volume = volume * 0x7f / USERAPP_VOL_MAX;

	if (role == PHONE_ADDR)
	{
#ifdef USERAPP_SUPPORT_BT
		if (me->IsSourceDevVolumeSyncReg == TRUE && me->aUserSrc_tbl[me->iSrc] == eAp_Bt_Src)
#else
		if (me->IsSourceDevVolumeSyncReg == TRUE)
#endif
		{
			res = BtSetVolumeSync(volume, PHONE_ADDR);
		}

	}
	else if (role == HEADPHONE_ADDR)
	{
		if (me->IsSinkDevVolumeSyncReg == TRUE)
		{
			res = BtSetVolumeSync(volume, HEADPHONE_ADDR);
		}
	}

	return res;
}
/******************************************************************************************/
/**
 * \fn		int UserBtSwitchTwsMode(BtBackGroundQMSM * const me)
 *
 * \brief	    Switch Tws mode: bt sink dev of  sub/headphone
 *
 * \param
 *              		me: UserApp *
 *
 * \return	    void
 *
 ******************************************************************************************/
int UserBtSwitchTwsMode(BtBackGroundQMSM *const me)
{
	CFG_BT_T *pBtConf = GET_SDK_CFG(CFG_BT_T);
	CFG_BTMAIN_T *pBtSdkCfg = &(pBtConf->BtSdkCfg.BtMainCfg);

	if (pBtSdkCfg->TwsWork == ON)
	{
		if (!me->IsSurportTwsSwitch)
		{
			return FAIL;
		}

		if (me->BtTwsMode == HEADPHONE)
		{
			AudDspService_Set_Mute(UserBtTargetAo());

			if (BtSinkModeSwitch(WSS) == SUCCESS)
			{
				//AudDspService_Set_Parm_From_Earphone_To_Wss(UserBtTargetAo(), 1);
				return SUCCESS;
			}
		}
		else if (me->BtTwsMode == WSS)
		{
			AudDspService_Set_Mute(UserBtTargetAo());

			if (BtSinkModeSwitch(HEADPHONE) == SUCCESS)
			{
				//AudDspService_Set_Parm_From_Earphone_To_Wss(UserBtTargetAo(), 0);
				return SUCCESS;
			}
		}
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		int UserBtHintPlay(BtBackGroundQMSM * const me,BtHintType_e type)
 *
 * \brief		do bt soundhint play
 *
 * \param	UserApp * const me ,type:hintplay type
 *
 * \return	    0:success , -1: error
 *
 ******************************************************************************************/
int UserBtHintPlay(BtBackGroundQMSM *const me, BtHintType_e type)
{
	static int LastPlayReadyFlag = FALSE;
	(void)me;

	switch (type)
	{
		case BT_CONNECT_READY:
		{
			if (LastPlayReadyFlag == FALSE)
			{
				LastPlayReadyFlag = TRUE;
				QACTIVE_POST(UserBtTargetAo(), Q_NEW(QEvt, BT_READY_SOUNDHINT_SIG), UserBtTargetAo());
			}

			break;
		}
		case BT_CONNECT_SUCCESS:
		{
			LastPlayReadyFlag = FALSE;
			QACTIVE_POST(UserBtTargetAo(), Q_NEW(QEvt, BT_CONNECT_SOUNDHINT_SIG), UserBtTargetAo());
			break;
		}
		default:
		{
			break;
		}
	}

	return 0;
}

/******************************************************************************************/
/**
 * \fn		 int BtAppA2dpHandle(BtBackGroundQMSM * const me,BtAppCBKEvt *msg)
 *
 * \brief		bt a2dp&&avrcp connect handle
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              		BtAppCBKEvt msg:(input)
 *
 * \return	    void
 *
 ******************************************************************************************/
int IsAVDPConnect = FALSE;
extern int IsBtModeFlag;

int BtAppA2dpHandle(BtBackGroundQMSM *const me, BtAppCBKEvt *msg)
{
	stBtDevInfo *pDev = NULL;
	(void)pDev;
	//BT_PROFILE_STATE_e meA2dpProfileState = DISCONNECT_OK;

	if (msg->pdata != NULL)
	{
		pDev = (stBtDevInfo *)msg->pdata;
	}

	switch (msg->type)
	{
		case BT_APP_AVDP_CONNECT_ING_SIG:
		{
			ap_printf("%s BT_APP_AVDP_CONNECT_ING_SIG  \n", __FUNCTION__);
			break;
		}
		case BT_APP_AVDP_CONNECT_SUCCESS_SIG:
		{
			ap_printf("%s:BT_APP_AVDP_CONNECT_SUCCESS_SIG	\n", __FUNCTION__);

			/*BtAddr_t stBtAddr = pDev->bd_addr;
			//BtGetRemoteDevInfo(&stBtAddr, &remoteBtDevInfo);
			memcpy(&remoteBtDevInfo.bd_addr,&stBtAddr,SIZE_OF_MACADDR);*/

			/*if (pDev->DevRole == PHONE_ADDR && me->BtTwsMode != HEADPHONE)
			{
				UserBtHintPlay(me, BT_CONNECT_SUCCESS);
			}*/ //zehai.liu todo

			ap_printf("%s:IsAVDPConnect = %d.\n", __FUNCTION__,IsAVDPConnect);
			ap_printf("%s:IsBtModeFlag = %d.\n", __FUNCTION__,IsBtModeFlag);
			if(IsAVDPConnect == FALSE)
			{
			 	if (IsBtModeFlag == TRUE)
			 	{
					UserBtHintPlay(me, BT_CONNECT_SUCCESS);
			 	}
			 	IsAVDPConnect = TRUE;
			}

			me->IsDevConnected[pDev->DevRole] = TRUE;
			break;
		}
		case BT_APP_AVDP_DISCONNECT_ING_SIG:
		{
			ap_printf("%s:BT_APP_AVDP_DISCONNECT_ING_SIG  \n", __FUNCTION__);
			break;
		}
		case BT_APP_AVDP_DISCONNECT_SUCCESS_SIG:
		{
			ap_printf("%s BT_APP_AVDP_DISCONNECT_SUCCESS_SIG  \n", __FUNCTION__);

			/*if (pDev->DevRole == PHONE_ADDR && me->BtTwsMode != HEADPHONE)
			{
				UserBtHintPlay(me, BT_CONNECT_READY);
				//BtScanModeSet(BOTH_DISCOVERY_CONNECTABLE);
			}*/ //zehai.liu todo

			//BtScanModeSet(BOTH_DISCOVERY_CONNECTABLE); //zehai.liu todo


			/*sBtCheckAllDeviceProfileState(AVDTP,&meA2dpProfileState);

			ap_printf("%s:AVDTP status = %d.\n", __FUNCTION__,meA2dpProfileState);

			if (meA2dpProfileState == CONNECT_OK) break;    //for this BT_APP_AVDP_DISCONNECT_SUCCESS_SIG is not from the first linked remote BT device.

			ap_printf("%s:IsAVDPConnect = %d.\n", __FUNCTION__,IsAVDPConnect);
			ap_printf("%s:IsBtModeFlag = %d.\n", __FUNCTION__,IsBtModeFlag);

			if(IsAVDPConnect == TRUE)
			{
			    if (IsBtModeFlag == TRUE)
				{
					UserBtHintPlay(me, BT_CONNECT_READY);
				}
				IsAVDPConnect = FALSE;
			}*/

			me->IsDevConnected[pDev->DevRole] = FALSE;
			break;
		}
		default:
		{
			ap_printf("%s default  \n", __FUNCTION__);
			break;
		}

	}

	if (msg->pdata != NULL)
	{
		BtFree(msg->pdata);
		msg->pdata = NULL;
	}
	return 0;
}

/******************************************************************************************/
/**
 * \fn		int BtAppSppHandle(BtBackGroundQMSM * const me,BtAppCBKEvt *msg)
 *
 * \brief		bt spp connect handle
 *
 * \param
 *              		BtAppCBKEvt msg:(input)
 *
 * \return	    void
 *
 ******************************************************************************************/
int BtAppSppHandle(BtBackGroundQMSM *const me, BtAppCBKEvt *msg)
{
	stBtDevInfo *pDev = NULL;
	(void)pDev;

	if (msg->pdata != NULL)
	{
		pDev = (stBtDevInfo *)msg->pdata;
	}

	switch (msg->type)
	{
		case BT_APP_SPP_CONNECT_SUCCESS_SIG:
		{
			ap_printf("%s:BT_APP_SPP_CONNECT_SUCCESS_SIG	\n", __FUNCTION__);
			me->IsDevConnected[pDev->DevRole] = TRUE;

			if (pDev->DevRole == WSS_ADDR)
			{
				//UserBtSetWaterDelay(me->aUserSrc_tbl[me->iSrc]); //zehai.liu todo
			}

			break;
		}
		case BT_APP_SPP_DISCONNECT_SUCCESS_SIG:
		{
			ap_printf("%s:BT_APP_SPP_DISCONNECT_SUCCESS_SIG  \n", __FUNCTION__);
			me->IsDevConnected[pDev->DevRole] = FALSE;

			if (pDev->DevRole == WSS_ADDR)
			{
				//UserBtSetWaterDelay(eApUserAppSrc_Num); //zehai.liu todo
			}

			break;
		}
		default:
		{
			ap_printf("%s default  \n", __FUNCTION__);
			break;
		}
	}

	if (msg->pdata)
	{
		BtFree(msg->pdata);
		msg->pdata = NULL;
	}

	return 0;
}
/******************************************************************************************/
/**
 * \fn		int BtAppCtrlCbkHandle(BtBackGroundQMSM * const me,BtAppCBKEvt *pMsg)
 *
 * \brief		bt spp connect handle
 *
 * \param
 *              		BtAppCBKEvt msg:(input)
 *
 * \return	    void
 *
 ******************************************************************************************/
int BtAppCtrlCbkHandle(BtBackGroundQMSM *const me, BtAppCBKEvt *pMsg)
{
	QEvt *msg = NULL;

	switch (pMsg->type)
	{
		case BT_APP_VOLUME_SET_REQ_SIG:
		{
			BtVolumeInfo *pVolumeInfo = (BtVolumeInfo *)pMsg->pdata;
			stSYS_AudioInfo_t *pAudioInfo = NULL;
			pAudioInfo = Q_NEW(stSYS_AudioInfo_t, AUDIO_VOL_SET_SIG);

			if (me->BtTwsMode == HEADPHONE)
			{
				me->TwsRecodeHeadphoneVolume = USERAPP_VOL_MAX * pVolumeInfo->volume / 127;
				pAudioInfo->Vol = me->TwsRecodeHeadphoneVolume;
			}
			else
			{
				me->TwsRecodeWssVolume = USERAPP_VOL_MAX * pVolumeInfo->volume / 127;
				pAudioInfo->Vol = me->TwsRecodeWssVolume;
			}

			//post to set volume
			if (pAudioInfo != NULL)
			{
				QACTIVE_POST(UserBtTargetAo(), (QEvt *)pAudioInfo, UserBtTargetAo());
			}

			break;
		}
		case BT_APP_VOLUME_REG_REQ_SIG:
		{
			BtVolumeInfo *pVolumeInfo = (BtVolumeInfo *)pMsg->pdata;

			if (pVolumeInfo->role == HEADPHONE_ADDR)
			{
				me->IsSinkDevVolumeSyncReg = TRUE;
			}
			else if (pVolumeInfo->role == PHONE_ADDR)
			{
				me->IsSourceDevVolumeSyncReg = TRUE;
			}

			break;
		}
		case BT_APP_PLAY_REQ_SIG:
		{
			ap_printf(" BT_APP_PLAY_REQ_SIG \n");
			msg = Q_NEW(QEvt, BT_MUSIC_PLAY_REQ_SIG);
			QACTIVE_POST(UserBtTargetAo(), (QEvt *)msg, UserBtTargetAo());
			break;
		}
		case BT_APP_PAUSE_REQ_SIG:
		{
			ap_printf(" BT_APP_PAUSE_REQ_SIG \n");
			msg = Q_NEW(QEvt, PAUSE_RELEASE_SIG);
			QACTIVE_POST(UserBtTargetAo(), (QEvt *)msg, UserBtTargetAo());
			break;
		}
		case BT_APP_NEXT_PLAY_REQ_SIG:
		{
			ap_printf(" BT_APP_NEXT_PLAY_REQ_SIG \n");
			msg = Q_NEW(QEvt, BT_MUSIC_NEXT_REQ_SIG);
			QACTIVE_POST(UserBtTargetAo(), (QEvt *)msg, UserBtTargetAo());
			break;
		}
		case BT_APP_PREV_PLAY_REQ_SIG:
		{
			ap_printf(" BT_APP_PREV_PLAY_REQ_SIG \n");
			msg = Q_NEW(QEvt, BT_MUSIC_PREV_REQ_SIG);
			QACTIVE_POST(UserBtTargetAo(), (QEvt *)msg, UserBtTargetAo());
			break;
		}
		default:
		{
			break;
		}
	}

	if (pMsg->pdata)
	{
		BtFree(pMsg->pdata);
		pMsg->pdata = NULL;
	}

	return 0;
}
/******************************************************************************************/
/**
 * \fn		int BtAppPlayerStateCbkHandle(BtBackGroundQMSM * const me,BtAppCBKEvt *pMsg)
 *
 * \brief		bt spp connect handle
 *
 * \param
 *              		BtAppCBKEvt msg:(input)
 *
 * \return	    void
 *
 ******************************************************************************************/
int BtAppPlayerStateCbkHandle(BtBackGroundQMSM *const me, BtAppCBKEvt *pMsg)
{
	BtAppCBKEvt *msg = pMsg;
	(void)me;

	switch (msg->type)
	{
		case BT_APP_SOURCE_STATE_UPDATE_SIG:
		{
			ap_printf("%s:BT_APP_SOURCE_STATE_UPDATE_SIG \n", __FUNCTION__);

#ifdef USERAPP_SUPPORT_BT
			BtDevPlayState *pPlayerState = (BtDevPlayState *)msg->pdata;

			if (me->IsIgnoreSwitchSrc && me->iSrc != eAp_Bt_Src)
			{
				if (pPlayerState->state == STATUS_PLAY)
				{
					QACTIVE_POST(UserBtTargetAo(), Q_NEW(QEvt, SET_BT_SOURCE_SIG), UserBtTargetAo());
					me->iSrc = eAp_Bt_Src;
				}
			}
#endif

			break;
		}
		default:
		{
			break;
		}
	}

	if (pMsg->pdata)
	{
		BtFree(pMsg->pdata);
		pMsg->pdata = NULL;
	}

	return 0;
}
/******************************************************************************************/
/**
 * \fn		 int BtAppOsdCbkHandle(BtBackGroundQMSM * const me,BtAppCBKEvt *msg)
 *
 * \brief		bt a2dp&&avrcp connect handle
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              		BtAppCBKEvt msg:(input)
 *
 * \return	    void
 *
 ******************************************************************************************/

int BtAppOsdCbkHandle(BtBackGroundQMSM *const me, BtAppCBKEvt *msg)
{
    (void)me;
	static BYTE last_status;

	switch (msg->type)
	{
	    case BT_APP_REMOTE_DEVICE_INFO_SIG:
		{
			BTConnectRemoteInfo *pstConInfo = (BTConnectRemoteInfo *)msg->pdata;
			BtAddr_t *pBtAddr = (BtAddr_t *)(&pstConInfo->addr);
			ap_printf("[0x%02x][0x%02x][0x%02x][0x%02x][0x%02x][0x%02x]......\n",
			pBtAddr->address[0],pBtAddr->address[1],
			pBtAddr->address[2],pBtAddr->address[3],
			pBtAddr->address[4],pBtAddr->address[5]);
			ap_printf("Name:%s\n", pstConInfo->name);
			ap_printf("Name len:%d\n",pstConInfo->name_len);
			break;
		}
		case BT_APP_MUSIC_PLAY_STATUS_SIG:
		{
			t_btmusic_play_status *pstConInfo = (t_btmusic_play_status *)msg->pdata;
		    ap_printf("play_status:%d\n",pstConInfo->play_status);

			if(last_status != pstConInfo->play_status)
			{
				if(pstConInfo->play_status == 1) // BTMUSIC_STATUS_PLAYING
					QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, BT_APP_PLAY_STATUS), (void*)0);
				else
					QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, BT_APP_PAUSE_STATUS), (void*)0);

				last_status = pstConInfo->play_status ;
			}
			ap_printf("totalTime:%d,currentTime:%d\n",pstConInfo->totalTime,pstConInfo->currentTime);
			break;
		}
		case BT_APP_MUSIC_CODEC_INFO_SIG:
		{
			stBtAudioPlayInfo *pstConInfo = (stBtAudioPlayInfo *)msg->pdata;
			ap_printf("codec_type:%d,sample_rate:%d\n",pstConInfo->codec_type,pstConInfo->sample_rate);
			break;
		}
	    case BT_APP_MUSIC_INFO_SIG:
		{
			t_btmusic_musicinfo *pstConInfo = (t_btmusic_musicinfo *)msg->pdata;
			ap_printf("title:%s\n",pstConInfo->title.string);
			ap_printf("artist:%s\n",pstConInfo->artist.string);
			ap_printf("album:%s\n",pstConInfo->album.string);
			ap_printf("genre:%s\n",pstConInfo->genre.string);
			break;
		}
	    case BT_APP_REMOTE_DEVICE_RSSI_SIG:
		{
			stBtReadRssi *pstConInfo = (stBtReadRssi *)msg->pdata;
			ap_printf("rssi:%d\n", pstConInfo->rssi);
			break;
		}
		default:
		{
			ap_printf("%s default  \n", __FUNCTION__);
			break;
		}

	}

	if (msg->pdata != NULL)
	{
		BtFree(msg->pdata);
		msg->pdata = NULL;
	}
	return 0;
}

/******************************************************************************************/
/**
 * \fn		int BtAppUserActionHandle(BtBackGroundQMSM * const me,BtAppCBKEvt *pMsg)
 *
 * \brief		bt spp connect handle
 *
 * \param
 *              		BtAppCBKEvt msg:(input)
 *
 * \return	    void
 *
 ******************************************************************************************/
int BtAppUserActionHandle(BtBackGroundQMSM *const me, QEvt const *const e)
{

	switch (e->sig)
	{
		case BT_START_SEARCH_RELEASE_SIG:
		{
			ap_printf("BT_START_SEARCH_RELEASE_SIG:\n");

			if (me->BtState == BT_READY && me->BtTwsMode == HEADPHONE)
			{
				BtSearchStart();
			}

			break;
		}
		case BT_TWS_MODE_SWITCH_RELEASE_SIG:
		{
			ap_printf("BT_TWS_MODE_SWITCH_RELEASE_SIG:\n");

			if (me->BtState == BT_READY && me->IsSurportTwsSwitch)
			{
				UserBtSwitchTwsMode(me);
			}

			break;
		}
		case BT_VOL_SYNC_SET_SIG:
		{
			stSYS_AudioInfo_t *pstAudioInfo = ((stSYS_AudioInfo_t *)e);

			ap_printf("BT_VOL_SYNC_SET_SIG:\n");

			if (pstAudioInfo->MuteState == eMute_on)
			{
				UserBtSetWssVolume(me, 0);
			}
			else
			{
				UserBtSetWssVolume(me, pstAudioInfo->Vol);
			}

			if (me->BtTwsMode == HEADPHONE)
			{
				me->TwsRecodeHeadphoneVolume = pstAudioInfo->Vol;
				UserBtSetVolumeSync(me, me->TwsRecodeHeadphoneVolume, HEADPHONE_ADDR);
				UserSetting_SaveEarVolume(me->TwsRecodeHeadphoneVolume);
			}
			else
			{
				me->TwsRecodeWssVolume = pstAudioInfo->Vol;
				UserSetting_SaveVolume(me->TwsRecodeWssVolume);
				UserBtSetVolumeSync(me, pstAudioInfo->Vol, PHONE_ADDR);
			}

			break;
		}
		case BT_VOL_SET_SUB_GAIN_SIG:
		{
			if (me->TwsWork == ON && me->BtTwsMode == WSS)
			{
				stVolSet *pSubVol = (stVolSet *)e;
				UserBtSetWssVolume(me, pSubVol->vol);
				me->TwsRecodeWssVolume = pSubVol->vol;
				UserSetting_SaveVolume(me->TwsRecodeWssVolume);
			}

			break;
		}
		case SOURCE_SW_RELEASE_SIG:
		{
			me->iSrc++;
			ap_printf("\n ====index : %d, src: %d! ===== \n", me->iSrc, me->aUserSrc_tbl[me->iSrc]);

#ifdef USERAPP_SUPPORT_BT
			if (me->aUserSrc_tbl[me->iSrc] == eAp_Bt_Src)
			{
				if (me->IsSurportTwsSwitch && me->BtTwsMode == HEADPHONE)
				{
					me->iSrc++;
				}
			}
#endif
			if (me->aUserSrc_tbl[me->iSrc] == me->SrcEnd)
			{
				me->iSrc = 0;
			}

			//UserBtSetWaterDelay(me->aUserSrc_tbl[me->iSrc]); //zehai.liu todo
			break;
		}
		default:
		{
			break;
		}
	}

	return 0;
}
/******************************************************************************************/
/**
* \fn		void BtAppSigHandle(BtBackGroundQMSM * const me,QEvt const * const e)
*
* \brief		bt sig handle
*
* \param
*              		BtBackGroundQMSM * const me,QEvt const * const e
*
* \return	    void
*
******************************************************************************************/
static void BtAppSigHandle(BtBackGroundQMSM *const me, BtAppCBKEvt const *const e)
{
	BtAppCBKEvt *msg = (BtAppCBKEvt *)e;
	//BtAddr_t BtAddr = {{0}};   //zehai.liu todo
	//BtAddr_t TempAddr = {{0}};  //zehai.liu todo
	BT_PROFILE_STATE_e meA2dpProfileState = DISCONNECT_OK;

	switch (msg->type)
	{
		case BT_APP_READY_OK_SIG:
		{
			ap_printf("%s:BT_APP_READY_OK_SIG \n", __FUNCTION__);

			//me->BtState = BT_READY;  //zehai.liu todo
			//BtScanModeSet(BOTH_DISCOVERY_CONNECTABLE); //zehai.liu todo

			BASE_IF_WriteDevClass(BT_DEV_CLASS3);//0x240414

			int i;
			UINT_8   empty_linkKey[16] = {0};
			BTPairDev Saved_PairInfo[BT_DEVICE_NUM];
			UserSetting_ReadAllPairDev(Saved_PairInfo);



			for(i=0;i<BT_DEVICE_NUM;i++)
			{
				if(memcmp(empty_linkKey,Saved_PairInfo[i].linkKey,sizeof(Saved_PairInfo[i].linkKey))!=0)
				{
					LOGD("find linkkey...i:%d...\n",i);
					LOGD("Link_Key:0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
				Saved_PairInfo[i].linkKey[0],Saved_PairInfo[i].linkKey[1],Saved_PairInfo[i].linkKey[2],Saved_PairInfo[i].linkKey[3],
				Saved_PairInfo[i].linkKey[4],Saved_PairInfo[i].linkKey[5],Saved_PairInfo[i].linkKey[6],Saved_PairInfo[i].linkKey[7],
				Saved_PairInfo[i].linkKey[8],Saved_PairInfo[i].linkKey[9],Saved_PairInfo[i].linkKey[10],Saved_PairInfo[i].linkKey[11],
				Saved_PairInfo[i].linkKey[12],Saved_PairInfo[i].linkKey[13],Saved_PairInfo[i].linkKey[14],Saved_PairInfo[i].linkKey[15]);
					LOGD("BtDoAddBondDevaddr...0x%02x%02x%02x%02x%02x%02x...\n",
				Saved_PairInfo[i].bd_addr.address[0],Saved_PairInfo[i].bd_addr.address[1],
				Saved_PairInfo[i].bd_addr.address[2],Saved_PairInfo[i].bd_addr.address[3],
				Saved_PairInfo[i].bd_addr.address[4],Saved_PairInfo[i].bd_addr.address[5]);
					//LOGD("master:%d\n",Saved_PairInfo[i].master);
					//LOGD("cod:%d\n",Saved_PairInfo[i].cod);
					//LOGD("linkKeyType:%d\n",Saved_PairInfo[i].linkKeyType);
					BtAddBondDev(&Saved_PairInfo[i]);
				}
			}

			UserSetting_ReadPairDevRecordIndex(&me->PairDevRecordIndex);

			me->BtState = BT_READY;
			/*UserBtHintPlay(me, BT_CONNECT_READY);

#ifdef USERAPP_SUPPORT_BT
			if (me->aUserSrc_tbl[me->iSrc] == eAp_Bt_Src)
			{
				BtScanModeSet(BOTH_DISCOVERY_CONNECTABLE);
			}
			else
#endif
				if (me->TwsWork == ON && me->BtTwsMode == WSS)
				{
					BtGetAddrByDevRole(&BtAddr, WSS_ADDR);

					if (memcmp(&TempAddr, &BtAddr, sizeof(BtAddr_t)) != 0)
					{
						BtScanModeSet(BOTH_DISCOVERY_CONNECTABLE);
					}
				}*/  //zehai.liu todo

#ifdef USER_BT_DUT
			BtUdtTestOn();
#endif

			break;
		}
		case BT_APP_UNREADY_OK_SIG:
		{
			ap_printf("%s:BT_APP_UNREADY_OK_SIG \n", __FUNCTION__);
			me->BtState = BT_UNREADY;
			break;
		}
		case BT_APP_TWS_MODE_CHANGED_SIG:
		{
			ap_printf("BT_APP_TWS_MODE_CHANGED_SIG %d \n", *(SINK_MODE_e *)msg->pdata);
			stSYS_AudioInfo_t *pAudioInfo = NULL;
			pAudioInfo = Q_NEW(stSYS_AudioInfo_t, AUDIO_VOL_SET_SIG);

			me->BtTwsMode = *(SINK_MODE_e *)msg->pdata;

			if (me->BtTwsMode == HEADPHONE)
			{
				pAudioInfo->Vol = me->TwsRecodeHeadphoneVolume;
				AmpSet_MUTE_ON(&stAmpList);
				me->BtHeadPhoneMute = TRUE;
				AudDspService_Set_DeMute(UserBtTargetAo());
			}
			else
			{
				pAudioInfo->Vol = me->TwsRecodeWssVolume;
				//AudDspService_Set_Volume(UserBtTargetAo(), bDspVolTable[USERAPP_VOL_MAX], 400);
				me->BtHeadPhoneMute = FALSE;
				AmpSet_MUTE_OFF(&stAmpList);
				AudDspService_Set_DeMute(UserBtTargetAo());
			}

			if (pAudioInfo != NULL)
			{
				QACTIVE_POST(UserBtTargetAo(), (QEvt *)pAudioInfo, UserBtTargetAo());
			}

			break;
		}
		case BT_APP_SEARCH_RESULT_SIG:
		{
			ap_printf("%s:BT_APP_SEARCH_RESULT_SIG \n", __FUNCTION__);

			break;
		}
		case BT_APP_SEARCH_FINISH_SIG:
		{
			ap_printf("%s:BT_APP_SEARCH_FINISH_SIG	\n", __FUNCTION__);

			break;
		}
		case BT_APP_ACL_CONNECT_SIG:
		{
			ap_printf("%s:BT_APP_ACL_CONNECT_SIG	\n", __FUNCTION__);
			break;
		}
		case BT_APP_PAIRING_ING_SIG:
		{
			ap_printf("%s:BT_APP_PAIRING_ING_SIG	\n", __FUNCTION__);

			break;
		}
		case BT_APP_PAIRED_OK_SIG:
		{
			ap_printf("%s:BT_APP_PAIRED_OK_SIG	\n", __FUNCTION__);


			BTPairDev *pPairDev = (BTPairDev *) msg->pdata;


			//UserSetting_SavePairDev(*pPairDev);
			BtSaveDevInfo(pPairDev,LINKKEY);

			break;
		}
		/*case BT_APP_REMOTE_DEVICE_NAME_SIG:
		{
			ap_printf("%s:BT_APP_REMOTE_DEVICE_NAME_SIG \n",__FUNCTION__);
			BTPairDev *pDeviceInfo = (BTPairDev *) msg->pdata;
			ap_printf("name:%s,name len:%d\n",pDeviceInfo->name,pDeviceInfo->name_len);
			BtSaveDevInfo(pDeviceInfo,NAME);
			break;
		}*/
		case BT_APP_UNPAIRED_OK_SIG:
		{
			ap_printf("%s:BT_APP_UNPAIRED_OK_SIG \n", __FUNCTION__);
			break;
		}
		case BT_APP_ACL_DISCONNECT_OK_SIG:
		{
			ap_printf("%s:BT_APP_ACL_DISCONNECT_OK_SIG \n", __FUNCTION__);


			BtCheckAllDeviceProfileState(AVDTP,&meA2dpProfileState);

			ap_printf("%s:AVDTP status = %d.\n", __FUNCTION__,meA2dpProfileState);

			if (meA2dpProfileState == CONNECT_OK) break;

			ap_printf("%s:IsAVDPConnect = %d.\n", __FUNCTION__,IsAVDPConnect);
			ap_printf("%s:IsBtModeFlag = %d.\n", __FUNCTION__,IsBtModeFlag);

			if(IsAVDPConnect == TRUE)
			{
			    if (IsBtModeFlag == TRUE)
				{
					UserBtHintPlay(me, BT_CONNECT_READY);
				}
				IsAVDPConnect = FALSE;
			}

			break;
		}
		case BT_APP_AVDP_SIG...BT_APP_AVDP_MAX_SIG:
		{
			BtAppA2dpHandle(me, msg);
			break;
		}
		case BT_APP_SPP_SIG...BT_APP_SPP_MAX_SIG:
		{
			BtAppSppHandle(me, msg);
			break;
		}
		case BT_APP_CTR_REQ_SIG...BT_APP_CTR_REQ_MAX_SIG:
		{
			BtAppCtrlCbkHandle(me, msg);
			break;
		}
		case BT_APP_SOURCE_STATE_UPDATE_SIG:
		{
			BtAppPlayerStateCbkHandle(me, msg);
			break;
		}
		case BT_APP_REMOTE_DEVICE_INFO_SIG...BT_APP_REMOTE_DEVICE_RSSI_SIG:
		{
			BtAppOsdCbkHandle(me, msg);
			break;
		}
		default:
		{
			ap_printf("%s default  \n", __FUNCTION__);
			break;
		}
	}

	if (msg->pdata)
	{
		BtFree(msg->pdata);
		msg->pdata = NULL;
	}

}
/******************************************************************************************/
/**
 * \fn		QState BtBackGroupApp_Idle(BtBackGroundQMSM * const me, QEvt const * const e)
 *
 * \brief		trans mitter idle state
 *
 * \param	me:BtBackGroundQMSM  				e:QEvt
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
QState BtBackGroupApp_Idle(BtBackGroundQMSM *const me, QEvt const *const e)
{
	(void)e; /*!< avoid compiler warning about unused parameter */
	(void)me;
	QState dStatus;

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGI("%s: Q_ENTRY_SIG \n", __FUNCTION__);
			dStatus = Q_HANDLED();
			break;

		}
		case Q_EXIT_SIG:
		{
			LOGI("%s: Q_EXIT_SIG \n", __FUNCTION__);
			dStatus = Q_HANDLED();
			break;

		}
		case Q_INIT_SIG:
		{
			LOGI("%s: Q_INIT_SIG \n", __FUNCTION__);

			if ((me->BtAutoWork == ON) && (me->BtState == BT_UNREADY))
			{
				BtAPPInit(me);
				//UserBtSetWaterDelay(me->aUserSrc_tbl[me->iSrc]);  //zehai.liu todo
			}

			dStatus = Q_HANDLED();
			break;

		}
		case BT_APP_SIG:
		{
			BtAppCBKEvt *msg = (BtAppCBKEvt *)e;

			if (msg->type == BT_APP_READY_OK_SIG)
			{
				BtAppSigHandle(me, msg);
				dStatus = Q_TRAN(&BtBackGroupApp_Intend);
			}
			else
			{
				dStatus = Q_HANDLED();
			}

			break;
		}
		case SOURCE_SW_RELEASE_SIG:
		case BT_VOL_SYNC_SET_SIG:
		{
			BtAppUserActionHandle(me, e);
			dStatus = Q_HANDLED();
			break;
		}
		default:
		{
			dStatus = Q_SUPER(&QHsm_top);
			break;
		}

	}

	return dStatus;
}
/******************************************************************************************/
/**
 * \fn		QState BtBackGroupApp_Intend(BtBackGroundQMSM * const me, QEvt const * const e)
 *
 * \brief		trans mitter idle state
 *
 * \param	me:BtBackGroundQMSM  				e:QEvt
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
QState BtBackGroupApp_Intend(BtBackGroundQMSM *const me, QEvt const *const e)
{
	(void)e; /*!< avoid compiler warning about unused parameter */
	(void)me;
	QState dStatus;

	switch (e->sig)
	{
		case Q_INIT_SIG:
		{
			ap_printf("%s: Q_INIT_SIG \n", __FUNCTION__);
			dStatus = Q_HANDLED();
			break;

		}
		case BT_APP_SIG:
		{
			BtAppCBKEvt *msg = (BtAppCBKEvt *)e;
			BtAppSigHandle(me, msg);

			if (msg->type == BT_APP_UNREADY_OK_SIG)
			{
				dStatus = Q_TRAN(&BtBackGroupApp_Idle);
			}
			else
			{
				dStatus = Q_HANDLED();
			}

			break;
		}
		case BT_START_SEARCH_RELEASE_SIG:
		case BT_TWS_MODE_SWITCH_RELEASE_SIG:
		case BT_VOL_SYNC_SET_SIG:
		case SOURCE_SW_RELEASE_SIG:
		{
			BtAppUserActionHandle(me, e);
			dStatus = Q_HANDLED();
			break;
		}
		default:
		{
			dStatus = Q_SUPER(&QHsm_top);
			break;
		}

	}

	return dStatus;
}
/******************************************************************************************/
/**
 * \fn		QState Transmit_Initial(BtTransActive * const me, QEvt const * const e)
 *
 * \brief		trans mitter init
 *
 * \param	me : BtTransActive  		 e:QEvt
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
QState BtBackGroupApp_Initial(BtBackGroundQMSM *const me, QEvt const *const e)
{
	(void)me;
	(void)e;
	QState dStatus;

	dStatus = Q_TRAN(&BtBackGroupApp_Idle);
	return dStatus;
}


void BtBackGroup_Ctor(BtBackGroundQMSM **pBtQmsm, QActive *TargetAo)
{
	UserSetting_T UserSetting;
	CFG_BT_T *pBtConf = GET_SDK_CFG(CFG_BT_T);
	CFG_BTMAIN_T *pBtSdkCfg = &(pBtConf->BtSdkCfg.BtMainCfg);
	//BtBackGroundQMSM *const me = pBtQmsm;
	BtBackGroundQMSM * const me = &stBtBackGroundApp;

	if (pBtConf->BtAutoWork == ON && me != NULL)
	{
		//pstBtBackGroupApp = pBtQmsm;
		UserSetting_Init(&UserSetting);
		me->TargetAo = TargetAo;
		me->BtAutoWork = pBtConf->BtAutoWork;
		me->TwsWork = pBtSdkCfg->TwsWork;
		me->BtTwsMode = pBtSdkCfg->SinkMode;
		me->IsIgnoreSwitchSrc = pBtSdkCfg->IsIgnoreSwitchSrc;
		me->TwsRecodeWssVolume = UserSetting.Volume;
		me->TwsRecodeHeadphoneVolume  = UserSetting.EarVolume;
		me->iSrc = UserSetting.Src;
		me->IsSourceDevVolumeSyncReg = FALSE;
		me->IsSinkDevVolumeSyncReg = FALSE;
		me->BtState = BT_UNREADY;
		memset(me->IsDevConnected, 0, sizeof(me->IsDevConnected[DEVNUM_MAX]));

		if (me->TwsWork  == ON && pBtSdkCfg->SinkMode == HEADPHONE)
		{
			me->IsSurportTwsSwitch = pBtConf->IsSurportTwsSwitch;
			me->BtHeadPhoneMute = TRUE;
			UserBtSetWssVolume(me, me->TwsRecodeHeadphoneVolume);
		}
		else if (me->TwsWork == ON)
		{
			me->IsSurportTwsSwitch = pBtConf->IsSurportTwsSwitch;
			me->BtHeadPhoneMute = FALSE;
			UserBtSetWssVolume(me, me->TwsRecodeWssVolume);
		}
		else
		{
			me->BtTwsMode = SINK_NONE;
			me->IsSurportTwsSwitch = FALSE;
			me->BtHeadPhoneMute = FALSE;
		}

		QTimeEvt_ctorX(&me->timeAutoPlayEvt, UserBtTargetAo(), BT_AUTO_PLAY_SIG, 0U);

		QHsm_ctor(&me->super, Q_STATE_CAST(& BtBackGroupApp_Initial));
		QMSM_INIT(&me->super, (QEvt *)0);

		*pBtQmsm = me;
	}
	else
	{
		me->TargetAo = NULL;
		*pBtQmsm = NULL;
	}
}


#endif //__BT_BACKGROUND_APP_C__

