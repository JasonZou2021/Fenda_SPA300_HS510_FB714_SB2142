#ifndef __BT_AO_C__
#define __BT_AO_C__
/******************************************************************************/
/**
*
* \file	bt_ao.c
*
* \brief	8107 bt middle ware ao implement .\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author	 jie.jia@sunplus.com
* \version	 v0.01
* \date 	 2016/06/28
******************************************************************************/
/*---------------------------------------------------------------------------*
 *							  INCLUDE	DECLARATIONS						 *
 *---------------------------------------------------------------------------*/
#include "initcall.h"
#define LOG_EXTRA_STR  "[AO]"
#define LOG_MODULE_ID  ID_BT
#include "log_utils.h"
#include "std_interfaces.h"
#include "task_attribute_user.h"
#include "AudDspService.h"
#include "bt_ao.h"
#include "powerservice.h"
#include "cfg_sdk.h"
#include "bt_ao_audio.h"
#include "bt_ao_service_api.h"
#include "user_def.h"
Q_DEFINE_THIS_FILE;

/*---------------------------------------------------------------------------*
 *    MACRO DECLARATIONS
 *---------------------------------------------------------------------------*/
#define USER_CBK(P_CB, P_CBACK, ...)\
    if (P_CB && P_CB->P_CBACK) {            \
        P_CB->P_CBACK(__VA_ARGS__);         \
    }                                       \
    else {                                  \
        bt_ao_ntc_printf("user callback null!");  \
    }


#define BT_ACTION(P_CB, P_CBACK, ...)\
    if (P_CB && P_CB->P_CBACK) {            \
        P_CB->P_CBACK(__VA_ARGS__);         \
    }                                       \
    else {                                  \
        bt_ao_ntc_printf("bt_action null!");  \
    }

#define BT_AO_Queue_Cnt 15

/*---------------------------------------------------------------------------*
 *    GLOBAL VARIABLES
 *---------------------------------------------------------------------------*/
typedef enum
{
	DO_NONE,
	DO_SEARCH,
	DO_UNSEARCH,
	DO_PAIR,
	DO_UNPAIR,
	DO_CONNECT,
	DO_DISCONNECT,
}BT_USER_ACTION_e;


typedef enum
{
	BT_AO_NONE_SIG,
	BT_ENABLE_FINISH_SIG,
	BT_DISABLE_FINISH_SIG,
	BT_AUTO_CONNECT_START_SIG,
	BT_STANDBY_RES_SIG,
	BT_START_POWER_OFF_SIG,
	BT_AUDIO_SYNC_ENCODE_SIG,
	BT_AO_MAX_SIG,
}BTAOEVTSIG_e;


typedef struct
{
	QHsm super;
	QTimeEvt stSppTimeEvt;
	stBtTransAction *pTransModeAction;
}BtTransActive;


typedef int (*UpGradeCbk )(void);

typedef struct st_BtUpgrade
{
	char UpGradeFlag;
	UpGradeCbk pFunc;
}BtUpgradeInfo;

/**
*\brief the BTactive object
*/
typedef struct _BT_AO_e
{
    QActive super;   /*!< inherit QActive */
	QEQueue stBtAoQueue;
    QEvt const *pstBtAoQueueSto[3]; /*!< storage for deferred queue buffer */
	QTimeEvt timeEvt;
	SrcType_e  CurMode;
	int	PowerOffFlag;
	int StandByResFlag;
	int IsAutoRun;
	int AutoTimerWork;
	stBTInfo BtCurInfo;
	BtUpgradeInfo ApUpgrade;
	BtTransActive stSppDataService;
	BT_USER_ACTION_e BtUserAction;
	stBTAction *pBtAction;
	AO_USER_CBK_FUNC *pUserCBK;
	Connected_Device Connected_Device_List[Connected_Device_Max];
} BT_AO_t;



typedef struct stBT_AO_EVENT
{
	QEvent super;
	BTAOEVTSIG_e type;
	void *pdata;
	int  len;
}BtAoEvt;



static BT_AO_t gstBtAO;
static BtAudioMgrApi *pstBtAudio = NULL;
/*---------------------------------------------------------------------------*
 *    EXTERNAL REFERENCE
 *---------------------------------------------------------------------------*/
//extern void  SendData2TransLayer(BtAddr_t BtAddr,char *buf);
//extern void TransportLayerReceiverDataAnalys(BtAddr_t BtAddr,char *buf,UINT16 dLength);

/*---------------------------------------------------------------------------*
 *    FUNTION DECLARATIONS
 *---------------------------------------------------------------------------*/
/*!< hierarchical state machine ... */
QState BT_AO_Initial(BT_AO_t * const me, QEvt const * const e);
QState BT_AO_State_Idle(BT_AO_t * const me, QEvt const * const e);
QState BT_AO_State_Intend(BT_AO_t * const me, QEvt const * const e);
QState BT_AO_State_Active(BT_AO_t * const me, QEvt const * const e);
QState Transmit_Active(BtTransActive * const me, QEvt const * const e);
QState Transmit_Idle(BtTransActive * const me, QEvt const * const e);



/******************************************************************************************/
/**
 * \fn		BT_AO_t *_AoGet(void)
 *
 * \brief		get the bt ao struct
 *
 * \param	void
 *
 *
 * \return	    BT_AO_t struct
 *
 ******************************************************************************************/
BT_AO_t *_AoGet(void)
{
	return &gstBtAO;
}


Connected_Device *Connected_Device_List_Get(void)
{
	return gstBtAO.Connected_Device_List;
}

int Profile_Connected_handle(stBtDevInfo *pDev,Profile_Name_e Profile_Name)
{
	Connected_Device *stConnected_Device = NULL;
	int i,index=-1;

	Connected_Device *Connected_Device_List = Connected_Device_List_Get();

	for(i=0;i<Connected_Device_Max;i++)
	{
		if(!(memcmp(&Connected_Device_List[i].bt_addr,&pDev->bd_addr,6)))
		{
			stConnected_Device = &Connected_Device_List[i];
			break;
		}
		else
		{
			if(index == -1)
			{
				if(Connected_Device_List[i].profile_state == 0)
				{
					index = i;
				}
			}
		}
	}

	if(i==Connected_Device_Max)
	{
		if(index != -1)
		{
			stConnected_Device = &Connected_Device_List[index];
			memcpy(&stConnected_Device->bt_addr,&pDev->bd_addr,6);
		}
		else
		{
			bt_ao_ntc_printf("connect device record full!!!");
			return FAIL;
		}
	}


	stConnected_Device->profile_state |= 1 << Profile_Name;

	switch(Profile_Name)
	{
		case AVDTP:
		{

		}
		break;

		case AVRCP:
		{

		}
		break;

		case SPP:
		{
			stConnected_Device->spp_port = pDev->spp_port;
		}
		break;
	}

	return SUCCESS;
}


int Profile_Disconnected_handle(stBtDevInfo *pDev,Profile_Name_e Profile_Name)
{
	Connected_Device *stConnected_Device = NULL;
	int i;

	Connected_Device *Connected_Device_List = Connected_Device_List_Get();

	for(i=0;i<Connected_Device_Max;i++)
	{
		if(!(memcmp(&Connected_Device_List[i].bt_addr,&pDev->bd_addr,6)))
		{
			stConnected_Device = &Connected_Device_List[i];
			break;
		}
	}

	if(stConnected_Device == NULL)
	{
		bt_ao_ntc_printf("disconnect find record wrong !");
		return FAIL;
	}

	if(stConnected_Device->profile_state == 0)
	{
		memset(stConnected_Device,0,sizeof(Connected_Device));
	}
	else
	{
		stConnected_Device->profile_state &= ~(1 << Profile_Name);
		switch(Profile_Name)
		{
			case AVDTP:
			{

			}
			break;

			case AVRCP:
			{

			}
			break;

			case SPP:
			{
				stConnected_Device->spp_port = 0;
			}
			break;
		}
	}

	return SUCCESS;
}




/******************************************************************************************/
/**
 * \fn		BtAoEvt *_AoMsgPackage(BTAOEVTSIG_e sig,void *pData,int DataLen)
 *
 * \brief		ao msg packetage
 *
 * \param	sig:ao signal   pData:load data    DataLen:load data len
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
BtAoEvt *_AoMsgPackage(BTAOEVTSIG_e sig,void *pData,int DataLen)
{
	BtAoEvt *pMsg = NULL;

	pMsg = Q_NEW(BtAoEvt,BT_AO_SERVICE_MODULE_SIG);
	pMsg->type = sig;
	pMsg->len = DataLen;

	if(pData != NULL && DataLen > 0)
	{
		pMsg->pdata = BtMalloc(DataLen);

		if(pMsg->pdata)
		{
			memcpy(pMsg->pdata,pData,DataLen);
		}
		else
		{
			bt_ao_error_printf("BtMalloc fail!!");
		}
	}
	else
	{
		pMsg->pdata = pData;
	}

	return pMsg;
}


/******************************************************************************************/
/**
 * \fn		int _BtAoConfigSet(BT_AO_t * const me)
 *
 * \brief		bt config
 *
 * \param	me: bt ao struct
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int _BtAoConfigSet(BT_AO_t * const me)
{
	CFG_BT_T *pBtConf = GET_SDK_CFG(CFG_BT_T);
	CFGPRIVATE_BT_T* pPrivateBtConf = (CFGPRIVATE_BT_T*)GetBTSDKPrivateConfig();
	BtAudioInitInfo  BtAudioInfoInit;
	int len = 0;
	me->IsAutoRun = pBtConf->BtAutoWork;
	me->BtCurInfo.PlatformRole = pBtConf->PlatformRole;
	me->BtCurInfo.BtSurportAutoRun = pBtConf->BtAutoWork;
	me->BtCurInfo.IsSurportSwitch = pBtConf->IsSurportTwsSwitch;
	me->BtCurInfo.DevConnectCounter = 0;
	me->BtCurInfo.IsBtSourceVolumeSyncReg = FALSE;
	me->BtCurInfo.IsBtSinkVolumeSyncReg = FALSE;
	me->BtCurInfo.CancelAutoConnectFalg = FALSE;
	me->BtCurInfo.verdoridSource = pPrivateBtConf->VerdorIdSource;
	me->BtCurInfo.BtDeviceId_Pid = pPrivateBtConf->pid;
	me->BtCurInfo.BtDeviceId_Vid = pPrivateBtConf->vid;
	me->BtCurInfo.BtDeviceId_Version = pPrivateBtConf->version;
	if(pPrivateBtConf->avrcpCtProviderName == NULL){
		memset(me->BtCurInfo.avrcpCtProviderName,0,sizeof(me->BtCurInfo.avrcpCtProviderName));
	}else{
		len = strlen(pPrivateBtConf->avrcpCtProviderName);
		if(len >= 24)
		{
			len = 24;
		}
		memcpy(me->BtCurInfo.avrcpCtProviderName,pPrivateBtConf->avrcpCtProviderName,len);
		me->BtCurInfo.avrcpCtServiceName[len] = '\0';
	}

	if(pPrivateBtConf->avrcpCtServiceName == NULL){
		memset(me->BtCurInfo.avrcpCtServiceName,0,sizeof(me->BtCurInfo.avrcpCtServiceName));
	}else{
		len = strlen(pPrivateBtConf->avrcpCtServiceName);
		if(len >= 24)
		{
			len = 24;
		}
		memcpy(me->BtCurInfo.avrcpCtServiceName,pPrivateBtConf->avrcpCtServiceName,len);
		me->BtCurInfo.avrcpCtServiceName[len] = '\0';
	}


	memcpy(me->BtCurInfo.license,pPrivateBtConf->license,sizeof(pPrivateBtConf->license));


	if(pBtConf->PlatformRole == BT_MAIN)
	{
		CFG_BTMAIN_T *pBtSdkCfg = &(pBtConf->BtSdkCfg.BtMainCfg);
		me->BtCurInfo.pBTDesName = NULL;
		me->BtCurInfo.BtSurportHintPlay = pBtSdkCfg->HintWorkState;
		me->BtCurInfo.BtSurportStandBy = OFF;
		me->BtCurInfo.BtSurportTransMode = pBtSdkCfg->TransMode;
		me->BtCurInfo.BtSurportTws = pBtSdkCfg->TwsWork;
		me->BtCurInfo.IsConnectedSetDelay = pBtSdkCfg->IsConnectedSetDelay;
		me->BtCurInfo.IsIgnoreSrcSwitch = pBtSdkCfg->IsIgnoreSwitchSrc;
		me->BtCurInfo.IsIgnoreTimeout = pBtSdkCfg->IsIgnoreDevTimeout;
		me->BtCurInfo.SwitchFlag = FALSE;
		me->BtCurInfo.AutoLatency = pBtSdkCfg->AutoLatency;
		me->BtCurInfo.RetryTimes = pBtSdkCfg->AutoRetryTimes;
		me->BtCurInfo.BtSurportAutoConnet = pBtSdkCfg->AutoWorkState;
		me->BtCurInfo.AutoRole = pBtSdkCfg->Autorole;
		me->AutoTimerWork =  pBtSdkCfg->AutoWorkState;
		memcpy(me->BtCurInfo.LocalAddr.address,pBtSdkCfg->address,ADDR_LENGTH);
		memcpy(me->BtCurInfo.LocalName,pBtSdkCfg->LocalName,strlen(pBtSdkCfg->LocalName));

		if(pBtSdkCfg->TwsWork)
		{
			me->BtCurInfo.BtTwsMode = pBtSdkCfg->SinkMode;

			if(pBtSdkCfg->SinkMode == HEADPHONE)
			{
				me->BtCurInfo.pBTDesNameList = pBtSdkCfg->pHeadphoneListName;
			}
			else if(pBtSdkCfg->SinkMode == WSS)
			{
				me->BtCurInfo.pBTDesNameList = pBtSdkCfg->pSubListName;
			}

			me->BtCurInfo.DevMaxConnect = BT_SINK_DEV_NUM + BT_SOURCE_DEV_NUM;
		}
		else
		{
			me->BtCurInfo.BtTwsMode = SINK_NONE;
			me->BtCurInfo.DevMaxConnect = BT_SOURCE_DEV_NUM + BT_SPP_DEV_NUM;
		}
	}
	else if(pBtConf->PlatformRole == BT_SUB)
	{
		CFG_BTSUB_T *pBtSdkCfg = &(pBtConf->BtSdkCfg.BtSubCfg);


		me->BtCurInfo.pBTDesName = pBtSdkCfg->DesName;
		me->BtCurInfo.BtSurportHintPlay = pBtSdkCfg->HintWorkState;
		me->BtCurInfo.BtSurportTransMode = pBtSdkCfg->TransMode;
		me->BtCurInfo.IsIgnoreSrcSwitch = FALSE;
		me->BtCurInfo.IsIgnoreTimeout = FALSE;
		me->BtCurInfo.SwitchFlag = FALSE;
		me->BtCurInfo.AutoLatency = pBtSdkCfg->AutoLatency;
		me->BtCurInfo.RetryTimes = pBtSdkCfg->AutoRetryTimes;
		me->BtCurInfo.BtSurportAutoConnet = pBtSdkCfg->AutoWorkState;
		me->AutoTimerWork =  pBtSdkCfg->AutoWorkState;
		me->BtCurInfo.AutoRole = pBtSdkCfg->Autorole;
		me->BtCurInfo.IsBtAacCodecEn = FALSE;

		memcpy(me->BtCurInfo.LocalAddr.address,pBtSdkCfg->address,ADDR_LENGTH);
		memcpy(me->BtCurInfo.LocalName,pBtSdkCfg->LocalName,strlen(pBtSdkCfg->LocalName));


		me->BtCurInfo.BtSurportStandBy = pBtSdkCfg->StanbyMode;
		me->BtCurInfo.BtSurportTws = OFF;
		me->BtCurInfo.BtTwsMode = SINK_NONE;
		me->BtCurInfo.IsConnectedSetDelay = FALSE;
		me->BtCurInfo.DevMaxConnect = 1;
	}
	else
	{
		return FAIL;
	}

	BtAudioInfoInit.PlatformRole = me->BtCurInfo.PlatformRole;
	BtAudioInfoInit.SinkMode = me->BtCurInfo.BtTwsMode;
	BtAudioInfoInit.TwsWorkEn = me->BtCurInfo.BtSurportTws;
	BtAudioInfoInit.IsConnectedSetDelay = me->BtCurInfo.IsConnectedSetDelay;;
	BtAudioInit(&pstBtAudio,&BtAudioInfoInit);
	BtActionGet(&me->pBtAction);
	BT_ACTION(me->pBtAction,BtConfigInit,&me->BtCurInfo);
	return SUCCESS;
}

/******************************************************************************************/
/**
 * \fn		int _SendMsgTransModeOn(void)
 *
 * \brief		notify  ao trans mode on
 *
 * \param	void
 *
 *
 * \return	    void
 *
 ******************************************************************************************/

static int _SendMsgTransModeOn(void)
{
	QActive * const me = BtAoGet();
	BtCBKEvt *e = BtMsgPackage(BT_TRANSMIT_ON_SIG,NULL,0);
	QACTIVE_POST(me, (QEvt *)e, me);
	return SUCCESS;
}
/******************************************************************************************/
/**
 * \fn		void BtAutoCheckTimerEnable(int isTrue)
 *
 * \brief		reflecting  if platform have phone dev connected ok in Bt Mode
 *
 * \param		QActive * const me:(Input) the BT_AO_t handler here,\n
 *              		int state:(input)  :OFF/ON  timer set
 *
 * \return	    void
 *
 ******************************************************************************************/
void BtAutoCheckTimerEnable(int isTrue)
{
	BT_AO_t* const me = (BT_AO_t*)BtAoGet();

	if(me->BtCurInfo.PlatformRole == BT_SUB && me->BtCurInfo.IsSurportSwitch)
	{
		return;
	}

	if(isTrue && me->AutoTimerWork == ON)
	{
		me->BtCurInfo.BtSurportAutoConnet = ON;
		QTimeEvt_rearm(&me->timeEvt, TICKS_PER_SEC);
	}
	else
	{
		me->BtCurInfo.BtSurportAutoConnet = OFF;
	}

}
/******************************************************************************************/
/**
 * \fn		void _BtPowerOffStandBy(QActive * const me, void *pParam)
 *
 * \brief		bt power off cbk
 *
 * \param	IsTrue: allow ----TRUE   disallow------FALSE
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
#if 0
static void _BtPowerOffStandBy(QActive * const me, void *pParam)
{
	(void)me;
	(void)pParam;
	BT_AO_t *pBtAO = _AoGet();
	BtAoEvt *pMsg = _AoMsgPackage(BT_START_POWER_OFF_SIG,NULL,0);

	QACTIVE_POST(&pBtAO->super, (QEvt *)pMsg, &pBtAO->super);
	pBtAO->PowerOffFlag = TRUE;
}
#endif
/******************************************************************************************/
/**
 * \fn		int _BtInStandByRes(BT_AO_t * const me)
 *
 * \brief		bt auto standby
 *
 * \param	me:BT_AO_t
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int _BtInStandByRes(BT_AO_t * const me)
{
	static int times1S = 0;

	if(me->StandByResFlag == TRUE)
	{

		if(times1S >= 2)
		{
			QEvt *e = Q_NEW(QEvt,BT_STANDBY_SIG);

			if(e != NULL)
			{
				QF_PUBLISH((QEvt *)e, &me->super);
				return SUCCESS;
			}
			else
			{
				return FAIL;
			}
		}

		times1S++;
		return SUCCESS;
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		int _BtAoAudioSetSpeakAndWaterDelay(SrcType_e src)
 *
 * \brief		bt water level and speakout delay set
 *
 * \param	src : app source
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int _BtAoAudioSetSpeakAndWaterDelay(BtDelaySyncInfo *pDelaySyncInfo)
{
	BtMainAudioCtrlAction *pAction = NULL;
	BT_AO_t *pBtAO = _AoGet();

	if(pBtAO->BtCurInfo.PlatformRole == BT_MAIN)
	{
		pAction = &pstBtAudio->BtAudioAction.MainAudio;
	}

	if(pAction && pAction->BtAudioSetSpeakAndWaterDelay)
	{
		return pAction->BtAudioSetSpeakAndWaterDelay(pDelaySyncInfo);
	}

	return FAIL;
}

/******************************************************************************************/
/**
 * \fn		int _BtAoAudioSubSendSyncReady(void)
 *
 * \brief		olny use sub to send sync ready ok:8107 main start to encode play
 *
 * \param	void
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int _BtAoAudioSubSendSyncReady(int IsTure)
{
	BtSubAudioCtrlAction *pAction = NULL;
	BT_AO_t *pBtAO = _AoGet();

	if(pBtAO->BtCurInfo.PlatformRole == BT_SUB)
	{
		pAction = &pstBtAudio->BtAudioAction.SubAudio;
	}

	if(pAction&&pAction->BtDelaySendEncodePlayCmd)
	{
		return pAction->BtDelaySendEncodePlayCmd(IsTure);
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		int _BtAoAudioSetEncodePlaySync(void)
 *
 * \brief		bt set encode play sync :just olny to send sub sync(play/pause)
 *
 * \param	IsTrue:ture=== auto encode play   false=== cancel auto encode play
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
 int _BtAoAudioSetEncodePlaySync(int IsAutoEnc)
{
	BtMainAudioCtrlAction *pAction = NULL;
	BT_AO_t *pBtAO = _AoGet();

	if(pBtAO->BtCurInfo.PlatformRole == BT_MAIN)
	{
		pAction = &pstBtAudio->BtAudioAction.MainAudio;
	}

	if(pAction && pAction->BtAudioEncodePlaySync)
	{
		return pAction->BtAudioEncodePlaySync(IsAutoEnc);
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		int _BtAoAudioDelayEncodePlay(int IsTure)
 *
 * \brief		bt set encode play sync :just olny to send sub sync(play/pause)
 *
 * \param	IsTrue:ture=== play   false=== pause
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int _BtAoAudioDelayEncodePlay(int IsTure)
{
	BtMainAudioCtrlAction *pAction = NULL;
	BT_AO_t *pBtAO = _AoGet();

	if(pBtAO->BtCurInfo.PlatformRole == BT_MAIN)
	{
		pAction = &pstBtAudio->BtAudioAction.MainAudio;
	}

	if(pAction && pAction->BtAudioDelayEncodePlay)
	{
		return pAction->BtAudioDelayEncodePlay(IsTure);
	}

	return FAIL;
}

/******************************************************************************************/
/**
 * \fn		int _BtAoAudioCodecInit(void)
 *
 * \brief		bt audio codec init
 *
 * \param	void
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int _BtAoAudioCodecInit(void)
{
	BtAudioMgrApi *pAction = pstBtAudio;

	if(pAction && pAction->BtAudioCodecInit)
	{
		return pAction->BtAudioCodecInit();
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		int BtAoAudioAllow(int IsTrue)
 *
 * \brief		if bt allow aidio in
 *
 * \param	IsTrue: allow ----TRUE   disallow------FALSE
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int BtAoAudioAllow(int IsTrue)
{
	BtAudioMgrApi *pAction = pstBtAudio;

	if(pAction && pAction->BtAudioAllow)
	{
		return pAction->BtAudioAllow(IsTrue);
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		int _BtAoAudioHandle(QEvt const * const e)
 *
 * \brief		bt ao audio handle
 *
 * \param	e:QEvt
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int _BtAoAudioHandle(QEvt const * const e)
{
	BtAudioMgrApi *pAction = pstBtAudio;
	BT_AO_t *me = _AoGet();

	if(pAction && pAction->BtAudioHandle)
	{
		return pAction->BtAudioHandle(&me->super,e,me->BtCurInfo.BtTwsMode,me->CurMode);
	}

	return FAIL;
}

/******************************************************************************************/
/**
 * \fn		static int BtBaseMsgHandle(BtCBKEvt *msg,QState *pStatus,BT_AO_t * const me)
 *
 * \brief		bt base msg handle
 *
 * \param
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int BtBaseMsgHandle(BtCBKEvt *msg,QState *pStatus,BT_AO_t * const me)
{
	(void)me;
	int EnableResult = FAIL;
	static int IsRetryEnBt = TRUE;

	switch (msg->type)
	{
	case BT_INIT_SIG:
	{
		bt_ao_ntc_printf("BT_INIT_SIG !");

		if((me->ApUpgrade.UpGradeFlag == TRUE) || (me->BtCurInfo.BtState != BT_UNREADY))
		{
			*pStatus = Q_HANDLED();
			break;
		}

		BT_ACTION(me->pBtAction,BtInit);
		me->BtCurInfo.BtState = BT_INIT;

		if((me->BtCurInfo.BtSurportAutoRun == ON))
		{
			BtCBKEvt *pMsg = BtMsgPackage(BT_ENABLE_SIG,NULL,0);

			if(pMsg != NULL)
			{
				QACTIVE_POST(BtAoGet(), (QEvt *)pMsg, BtAoGet());
			}

			*pStatus = Q_HANDLED();
		}
		else
		{
			*pStatus = Q_HANDLED();
		}

		break;
	}
	case BT_DELINIT_SIG:
	{
		bt_ao_ntc_printf("BT_DELINIT_SIG !");
		BT_ACTION(me->pBtAction,BtDeInit);
		me->BtCurInfo.BtState = BT_UNREADY;

		if(me->ApUpgrade.UpGradeFlag == TRUE)
		{
			if(me->ApUpgrade.pFunc != NULL)
			{
				me->ApUpgrade.pFunc();
			}
		}
		else if(me->PowerOffFlag == TRUE)
		{
			PowerService_FinishPowerOff(BtAoGet());
			bt_ao_ntc_printf("\n ====== FinishPowerOff!! ======\n");
			me->PowerOffFlag = FALSE;
		}
		else
		{
			if(IsRetryEnBt == TRUE)
			{
				static int RetryTimes = 0;
				RetryTimes++;

				if(RetryTimes<3)
				{
					BtCBKEvt *pMsg = BtMsgPackage(BT_INIT_SIG,NULL,0);

					if(pMsg != NULL)
					{
						bt_ao_ntc_printf("\n ===== fail try %d BT_INIT_SIG !!! ==== \n",RetryTimes);
						QACTIVE_POST(&me->super, (QEvt *)pMsg, &me->super);
					}
				}
			}
		}

		*pStatus = Q_HANDLED();
		break;
	}
	case BT_ENABLE_SIG:
	{
	   bt_ao_ntc_printf("BT_ENABLE_SIG !");

	   if(me->BtCurInfo.BtState != BT_INIT)
	   {
	       *pStatus = Q_HANDLED();
	       break;
	   }

	   IsRetryEnBt = TRUE;

	   Init_Parm st_Init_Parm;
	   memset(&st_Init_Parm,0,sizeof(Init_Parm));
	   memcpy(st_Init_Parm.LocalAddr.address,me->BtCurInfo.LocalAddr.address,ADDR_LENGTH);
	   memcpy(st_Init_Parm.LocalName,me->BtCurInfo.LocalName,strlen(me->BtCurInfo.LocalName));
		///set eir Device ID
   	   st_Init_Parm.verdoridSource = me->BtCurInfo.verdoridSource;
   	   st_Init_Parm.BtDeviceId_Vid = me->BtCurInfo.BtDeviceId_Vid;
   	   st_Init_Parm.BtDeviceId_Pid = me->BtCurInfo.BtDeviceId_Pid;
   	   st_Init_Parm.BtDeviceId_Version = me->BtCurInfo.BtDeviceId_Version;
	   memcpy(st_Init_Parm.avrcpCtProviderName,me->BtCurInfo.avrcpCtProviderName,strlen(me->BtCurInfo.avrcpCtProviderName));
	   memcpy(st_Init_Parm.avrcpCtServiceName,me->BtCurInfo.avrcpCtServiceName,strlen(me->BtCurInfo.avrcpCtServiceName));
	   memcpy(st_Init_Parm.license,me->BtCurInfo.license,sizeof(me->BtCurInfo.license));

	   BT_ACTION(me->pBtAction,BtEnable,&EnableResult,st_Init_Parm);

	   if(EnableResult == SUCCESS)
	   {
	   		me->BtCurInfo.BtState = BT_ENABLEING;
		   *pStatus = Q_TRAN(&BT_AO_State_Intend);
	   }
	   else
	   {
		   *pStatus = Q_HANDLED();
	   }

	   break;
	}
	case BT_DISABLE_SIG:
	{
	   bt_ao_ntc_printf("BT_DISABLE_SIG !");
	   BT_ACTION(me->pBtAction,BtDisable,&EnableResult);
	   IsRetryEnBt = FALSE;

	   if(EnableResult == SUCCESS)
	   {
	   		me->BtCurInfo.BtState = BT_DISABLEING;
	   		BtAutoCheckTimerEnable(FALSE);
	   }

	   *pStatus = Q_HANDLED();
	   break;
	}
	case BT_POWER_DBM_SET_SIG:
	{
		bt_ao_printf("BT_POWER_DBM_SET_SIG !");
		DBM_LEVEL_e val = *(DBM_LEVEL_e *)(msg->pdata);
		BT_ACTION(me->pBtAction,BtRfSetPower,val);
		*pStatus = Q_HANDLED();
		break;
	}
	case BT_DUTTEST_ON_SIG:
	{
	   bt_ao_printf("BT_DUTTEST_ON_SIG !");
	   BT_ACTION(me->pBtAction,BtUdtTest);
	   *pStatus = Q_HANDLED();
	   break;
	}
	case BT_AUTOCONNECT_ENABLE_SIG:
	{
	   bt_ao_printf("BT_AUTOCONNECT_ENABLE_SIG !");
	   BT_ACTION(me->pBtAction,BtAutoConSet,ON);

		if(me->BtCurInfo.BtSurportAutoConnet != ON)
		{
			BtAutoCheckTimerEnable(TRUE);
		}

	   *pStatus = Q_HANDLED();
	   break;
	}
	case BT_AUTOCONNECT_DISABLE_SIG:
	{
	   bt_ao_printf("BT_AUTOCONNECT_DISABLE_SIG !");
	   BT_ACTION(me->pBtAction,BtAutoConSet,OFF);

		if(me->BtCurInfo.BtSurportAutoConnet != OFF)
		{
			BtAutoCheckTimerEnable(FALSE);
		}

	   *pStatus = Q_HANDLED();
	   break;
	}
	case BT_SET_SCAN_MODE_SIG:
	{
	   bt_ao_printf("BT_SET_SCAN_MODE_SIG !");
	   SCAN_MODE_e mode = *(SCAN_MODE_e *)(msg->pdata);
	    BT_ACTION(me->pBtAction,BtSetScanMode,mode);
	   *pStatus = Q_HANDLED();
	   break;
	}
	case BT_SWITCH_MODE_SIG:
	{
	   bt_ao_ntc_printf("BT_SWITCH_MODE_SIG !");
	   SINK_MODE_e SinkMode = *(SINK_MODE_e *)(msg->pdata);

	   if(me->BtCurInfo.IsSurportSwitch)
	   {
		   if(me->BtCurInfo.BtSurportTws == ON && SinkMode != me->BtCurInfo.BtTwsMode)
		   {
		   		BT_ACTION(me->pBtAction,BtSinkSwitchMode,SinkMode);
		   }
	   }

	   *pStatus = Q_HANDLED();
	   break;
	}
	case BT_HINT_ON_SIG:
	{
	   bt_ao_printf("BT_HINT_ON_SIG !");
	   me->BtCurInfo.BtSurportHintPlay = ON;
	   *pStatus = Q_HANDLED();
	   break;
	}
	case BT_HINT_OFF_SIG:
	{
	   bt_ao_printf("BT_HINT_OFF_SIG !");
	   me->BtCurInfo.BtSurportHintPlay = OFF;
	   *pStatus = Q_HANDLED();
	   break;
	}
	case BT_STANDBY_ON_SIG:
	{
	   me->BtCurInfo.BtSurportStandBy = ON;
	   bt_ao_printf("BT_STANDBY_ON_SIG !");
	   BT_ACTION(me->pBtAction,BtSetStandbyWork,ON);
	   *pStatus = Q_HANDLED();
	   break;
	}
	case BT_STANDBY_OFF_SIG:
	{
	   me->BtCurInfo.BtSurportStandBy = OFF;
	   bt_ao_printf("BT_STANDBY_OFF_SIG !");
	   BT_ACTION(me->pBtAction,BtSetStandbyWork,OFF);
	   *pStatus = Q_HANDLED();
	   break;
	}
	case BT_MODE_ON:
	{
		bt_ao_ntc_printf("BT_MODE_ON,state:%d !",me->BtCurInfo.IsBtMode);

		if(me->BtCurInfo.IsBtMode == TRUE)
		{
			*pStatus = Q_HANDLED();
			break;
		}
		else
		{
			me->BtCurInfo.IsBtMode = TRUE;

			bt_ao_printf("bt_state:%d \n",me->BtCurInfo.BtState);

			if(me->BtCurInfo.BtState == BT_READY)
			{
				BtAoAudioAllow(TRUE);

				if(me->BtCurInfo.IsIgnoreSrcSwitch)
				{
					BT_ACTION(me->pBtAction,BtPlayAtionDo);
				}

				BT_ACTION(me->pBtAction,BtModeOnAction);
				*pStatus = Q_HANDLED();
			}
			else
			{
				*pStatus = Q_HANDLED();
			}
		}
		break;
	}
	case BT_MODE_OFF:
	{
		bt_ao_ntc_printf("BT_MODE_OFF,state:%d !",me->BtCurInfo.IsBtMode);

		if(me->BtCurInfo.IsBtMode == FALSE)
		{
			*pStatus = Q_HANDLED();
		}
		else
		{
		    BT_PROFILE_STATE_e dState = DISCONNECT_OK;

			me->BtCurInfo.IsBtMode = FALSE;

			if(me->BtCurInfo.BtState == BT_READY)
			{
			    BT_ACTION(me->pBtAction,GetConnectState,PHONE_ADDR,&dState);

			    if(dState == DISCONNECT_OK)
				    BtAoAudioAllow(FALSE);

				if(me->BtCurInfo.IsIgnoreSrcSwitch)
				{
					BT_ACTION(me->pBtAction,BtPauseAtionDo);
				}

				BT_ACTION(me->pBtAction,BtModeOffAction);
			}

			*pStatus = Q_HANDLED();
		}

		break;
	}
	case BT_AP_UPGRADE_INIT:
	{
		me->ApUpgrade.UpGradeFlag = TRUE;
		me->ApUpgrade.pFunc = *((UpGradeCbk*)(msg->pdata));

		BtCBKEvt *pMsg = BtMsgPackage(BT_DISABLE_SIG,NULL,0);
		QACTIVE_POST(&me->super, (QEvt *)pMsg, &me->super);
		*pStatus = Q_HANDLED();
		break;
	}
	case BT_SET_SUB_WATERLEVEL_SIG:
	{
		BtDelaySyncInfo *pDelaySyncInfo = (BtDelaySyncInfo *)(msg->pdata);
		bt_ao_ntc_printf("BT_SET_SUB_WATERLEVEL_SIG:%d \n",(pDelaySyncInfo->src));
		me->CurMode = (pDelaySyncInfo->src);

		if(me->BtCurInfo.PlatformRole == BT_MAIN)
		{
			if(me->BtCurInfo.BtSurportTws == ON)
			{
				_BtAoAudioSetSpeakAndWaterDelay(pDelaySyncInfo);
			}
		}

		*pStatus = Q_HANDLED();
		break;
	}
	case BT_SETAPPCURRENTSOURCE:
	{
		SrcType_e* pSrc = (SrcType_e*)(msg->pdata);
		bt_ao_ntc_printf("BT_SETAPPCURRENTSOURCE:%d \n",*pSrc);
		me->CurMode = *pSrc;
		*pStatus = Q_HANDLED();
		break;
	}
	case BT_AUDIO_CODEC_RESET_SIG:
	{
		_BtAoAudioCodecInit();

		if(me->BtCurInfo.PlatformRole == BT_SUB)
		{
			_BtAoAudioSubSendSyncReady(TRUE);
		}

		*pStatus = Q_HANDLED();
	}
	break;
	default:
	{
	   bt_ao_error_printf("\n BT_BASE_SIG no exist such sig!");
	    *pStatus = Q_HANDLED();
	   break;
	}
	}

	if(msg->pdata != NULL)
	{
		BtFree(msg->pdata);
		msg->pdata = NULL;
	}
	return 0;
}

/******************************************************************************************/
/**
 * \fn		static int _BtUserActionCommHandle(BtCBKEvt *msg)
 *
 * \brief		bt user commmect action handle
 *
 * \param	msg:BtCBKEvt
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int _BtUserActionCommHandle(BtCBKEvt *msg)
{
	BT_AO_t *me = _AoGet();

	switch (msg->type)
	{
		case BT_SET_PINCODE_SIG:
		{
			bt_ao_ntc_printf("BT_SET_PINCODE_SIG !");
			BT_ACTION(me->pBtAction,BtSetPinCode,(char *)msg->pdata);
			break;
		}
		case BT_REPLY_PINCODE_SIG:
		{
			bt_ao_ntc_printf("BT_REPLY_PINCODE_SIG !");
			BtReplyPincodeInfo *pPinCode = (BtReplyPincodeInfo *)msg->pdata;
			BT_ACTION(me->pBtAction,BtReplyPinCode,&pPinCode->addr,pPinCode->pincode);
			break;
		}
		case BT_SET_BT_NAME_SIG:
		{
			bt_ao_ntc_printf("BT_SET_BT_NAME_SIG !");
			char *pName = (char *)msg->pdata;
			BT_ACTION(me->pBtAction,SetLocalName,pName);
			break;
		}
		case BT_SET_BT_LMP_NAME_SIG:
		{
			bt_ao_ntc_printf("BT_SET_BT_LMP_NAME_SIG[%s] !",(char *)msg->pdata);
			char *pName = (char *)msg->pdata;
			BT_ACTION(me->pBtAction,SetLocalLmpName,pName);
			break;
		}
		case BT_SET_BT_DEVICE_CLASS_SIG:
		{
			bt_ao_ntc_printf("BT_SET_BT_DEVICE_CLASS_SIG !");
			unsigned int *deviceClass = (unsigned int *)msg->pdata;
			BT_ACTION(me->pBtAction,SetLocalDeviceClass,deviceClass);
			break;
		}
		case BT_SDP_DID_REQUEST_SIG:
		{
			bt_ao_ntc_printf("BT_SDP_DID_REQUEST_SIG !");
			BtAddr_t *pBtAddr = (BtAddr_t *)msg->pdata;
			BT_ACTION(me->pBtAction,GetRemoveSdpDeviceID,pBtAddr);
			break;
		}
		case BT_SET_BT_ADDR_SIG:
		{
			bt_ao_ntc_printf("BT_SET_BT_ADDR_SIG !");
			BtAddr_t *pBtAddr = (BtAddr_t *)msg->pdata;
			BT_ACTION(me->pBtAction,SetLocalAddr,pBtAddr);
			break;
		}
		case BT_SEARCH_SIG:
		{
			bt_ao_ntc_printf("BT_SEARCHREQ !");
			me->BtUserAction = DO_SEARCH;
			BT_ACTION(me->pBtAction,BtStartSearch);
			break;
		}
		case BT_UNSEARCH_SIG:
		{
			bt_ao_printf("BT_UNSEARCH_SIG !");
			me->BtUserAction = DO_UNSEARCH;
			BT_ACTION(me->pBtAction,BtStopSearch);
			break;
		}
		case BT_PAIR_SIG:
		{
			bt_ao_printf("BT_PAIR_SIG !");
			BtAddr_t addr=*(BtAddr_t *)(msg->pdata);
			me->BtUserAction = DO_PAIR;
			BT_ACTION(me->pBtAction,BtPairStart,&addr);
			break;
		}
		case BT_UNPAIR_SIG:
		{
			bt_ao_printf("%s BT_UNPAIR_SIG  \n",__FUNCTION__);
			BtAddr_t addr=*(BtAddr_t *)(msg->pdata);
			me->BtUserAction = DO_UNPAIR;
			BT_ACTION(me->pBtAction,BtUnpair,&addr);
			break;
		}
		case BT_ADDBONDDEV_SIG:
		{
			bt_ao_printf("%s BT_ADDBONDDEV_SIG  \n",__FUNCTION__);
			BTPairDev *stPairDev = (BTPairDev *)(msg->pdata);
			BT_ACTION(me->pBtAction,AddBondDev,stPairDev);
			break;
		}
		case BT_CLEARBONDDEV_SIG:
		{
			bt_ao_printf("%s BT_CLEARBONDDEV_SIG  \n",__FUNCTION__);
			me->BtUserAction = DO_UNPAIR;
			BT_ACTION(me->pBtAction,ClearBondDev);
			break;
		}
		case BT_GET_REMOTE_DEVICE_RSSI:
		{
			bt_ao_printf("BT_GET_REMOTE_DEVICE_RSSI !");
			BtAddr_t addr=*(BtAddr_t *)(msg->pdata);
			BT_ACTION(me->pBtAction,BtGetRemoteDevRssi,&addr);
			break;
		}
		case BT_A2DP_DISCONNECT_SIG:
		{
			bt_ao_printf("BT_A2DP_DISCONNECT_SIG !");
			BtAddr_t addr=*(BtAddr_t *)(msg->pdata);
			BT_ACTION(me->pBtAction,Disonnect,&addr);
			me->BtUserAction = DO_DISCONNECT;
			break;
		}
		case BT_A2DP_CONNECT_SIG:
		{
			bt_ao_printf("BT_A2DP_CONNECT_SIG !");
			BtAddr_t addr=*(BtAddr_t *)(msg->pdata);
			BT_ACTION(me->pBtAction,Connect,&addr);
			me->BtUserAction = DO_CONNECT;
			break;
		}
		case BT_AVRCP_MUSIC_VOLUME_SYNC_SAMSUNG:
		{
			bt_ao_printf("BT_AVRCP_MUSIC_VOLUME_SYNC_SAMSUNG !");
			Btavrcp_volume_samsung_type* pstVolSyncSamsung = (Btavrcp_volume_samsung_type*)msg->pdata;
			BT_ACTION(me->pBtAction,BtAvrcpVolSyncSamsung,&(pstVolSyncSamsung->addr),
							pstVolSyncSamsung->volume,pstVolSyncSamsung->mute,pstVolSyncSamsung->vol_mute_set);
			break;
		}
		case BT_SPP_CONNECT_SIG:
		{
			bt_ao_printf("BT_SPP_CONNECT_SIG !");
			BtAddr_t addr=*(BtAddr_t *)(msg->pdata);
			BT_ACTION(me->pBtAction,BtSppConnect,&addr);
			break;
		}
		case BT_SPP_CONNECT_SAMSUNG_SIG:
		{
			bt_ao_printf("BT_SPP_CONNECT_SAMSUNG_SIG !");
			stBtSppConnect_Samsung* pstBtSppData = (stBtSppConnect_Samsung*)msg->pdata;
			BT_ACTION(me->pBtAction,BtSppConnectSamsung,&(pstBtSppData->BtAddr),pstBtSppData->pin);
			break;
		}
		case BT_SPP_DISCONNECT_SIG:
		{
			bt_ao_printf("BT_SPP_DISCONNECT_SIG !");
			BtAddr_t addr=*(BtAddr_t *)(msg->pdata);
			BT_ACTION(me->pBtAction,BtSppDisconnect,&addr);
			break;
		}
		case BT_SPP_DATA_OUT_SIG:
		{
			bt_ao_printf("BT_SPP_DATA_OUT_SIG !");
			//BtAddr_t addr=*(BtAddr_t *)(msg->pdata);
			BT_ACTION(me->pBtAction,BtSppDataOut,(void*)msg->pdata);
			break;
		}
		case BT_DENY_CONNECT_SET_SIG:
		{
			BtDenySetInfo *pDenyInfo = (BtDenySetInfo *)(msg->pdata);
			BT_ACTION(me->pBtAction,DenyConnectSet,&pDenyInfo->addr,pDenyInfo->state);
			bt_ao_ntc_printf("BT_DENY_CONNECT_SET_SIG:0x%x,0x%x,state:%d !",pDenyInfo->addr.address[0],pDenyInfo->addr.address[1],pDenyInfo->state);
			break;

		}
		default:
		{
			bt_ao_error_printf("USER ACTION no exit such sig!");
			break;
		}

	}

	if(msg->pdata)
	{
		if(msg->type == BT_SPP_DATA_OUT_SIG)
		{
			stBtSppData *pSppData = (stBtSppData *)msg->pdata;
			if(pSppData->pBtData != NULL){
				BtFree(pSppData->pBtData);
				pSppData->pBtData = NULL;
			}
		}
		BtFree(msg->pdata);
		msg->pdata = NULL;
	}
	return 0;
}

/******************************************************************************************/
/**
 * \fn		int BtSppCbkHandle(BtCBKEvt *msg)
 *
 * \brief		bt spp cbk handle
 *
 * \param	IsTrue: allow ----TRUE   disallow------FALSE
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int BtSppCbkHandle(BtCBKEvt *msg)
{
	stBtDevInfo *pDev = NULL;
	BT_AO_t *me = _AoGet();

	if(msg->pdata != NULL)
	{
		pDev = (stBtDevInfo *)msg->pdata;
	}

	switch (msg->type)
	{
		case BT_SPP_CONNECT_FAIL_SIG:
		{
			bt_ao_printf("%s:BT_SPP_CONNECT_FAIL_SIG \n",__FUNCTION__);
			break;
		}
		case BT_SPP_CONNECT_SUCCESS_SIG:
		{
			bt_ao_ntc_printf("BT_SPP_CONNECT_SUCCESS_SIG!");

			if(me->BtCurInfo.PlatformRole == BT_SUB && me->BtCurInfo.BtSurportHintPlay == OFF)
			{
				_BtAoAudioSubSendSyncReady(TRUE);
			}

			Profile_Connected_handle(pDev,SPP);
			//spp_port = pDev->spp_port;


			BT_ACTION(me->pBtAction,SppConnectokAction,pDev);
			break;
		}
		case BT_SPP_DISCONNECT_SUCCESS_SIG:
		{
			bt_ao_ntc_printf("BT_SPP_DISCONNECT_SUCCESS_SIG !");

			/*Connected_Device *stConnected_Device = NULL;
			int i;
			for(i=0;i<Connected_Device_Max;i++)
			{
				if(!(memcmp(&me->Connected_Device_List[i].bt_addr,&pDev->bd_addr,6)))
				{
					stConnected_Device = &me->Connected_Device_List[i];
					break;
				}
			}

			if(stConnected_Device == NULL)
			{
				bt_ao_ntc_printf("disconnect record wrong !");
				break;
			}

			stConnected_Device->profile_state &= ~(1 << SPP);
			stConnected_Device->spp_port = 0;
			if(stConnected_Device->profile_state == 0)
			{
				memset(stConnected_Device,0,sizeof(Connected_Device));
			}*/
			Profile_Disconnected_handle(pDev,SPP);

			BT_ACTION(me->pBtAction,SppDisconnectokAction,pDev);

			if(me->PowerOffFlag == TRUE)
			{
				int DevConnectNum = me->BtCurInfo.DevConnectCounter;

				if(DevConnectNum <= 0)
				{
					BtAoEvt *pMsg = _AoMsgPackage(BT_START_POWER_OFF_SIG,NULL,0);

					if(pMsg != NULL)
					{
						QACTIVE_POST(BtAoGet(), (QEvt *)pMsg, BtAoGet());
					}
				}
			}
			break;
		}
		case BT_SPP_DISCONNECT_FAIL_SIG:
		{
			bt_ao_ntc_printf("BT_SPP_DISCONNECT_FAIL_SIG !");
			break;
		}
		case BT_SPP_DATA_IN_SIG:
		{
			#if 0
			stBtSppData *pSppData = (stBtSppData *)msg->pdata;
			if(msg->pdata == NULL)
			{
				bt_ao_error_printf("BT_SPP_DATA_IN_SIG:msg->pdata=NULL,please check!");
				break;
			}
			if(pSppData->pBtData == NULL)
			{
				bt_ao_error_printf("BT_SPP_DATA_IN_SIG:pSppData->pBtData=NULL,please check!");
				break;
			}
			LOGT("BT Addr[H]:");
			BLOCKPRINTF((char*)&(pSppData->BtAddr),6);
			LOGT("BT Spp port:%d \n",pSppData->port);
			LOGT("BT Spp Data len:%d \n",pSppData->len);
			LOGT("BT Spp Data[H]:");
			BLOCKPRINTF((char*)(pSppData->pBtData),pSppData->len);
			#endif
			//BT_ACTION(me->pBtAction,SppDataInAction,pSppData);
			break;
		}
		default:
		{
			bt_ao_error_printf(" spp no exit such sig!");
			break;
		}
	}

	return 0;
}


static int BtSdpCbkHandle(BtCBKEvt *msg)
{
	//stBtDevInfo *pDev = NULL;
	//BT_AO_t *me = _AoGet();


	switch (msg->type)
	{
		case BT_SDP_DID_DATA_IN_SIG:
		{
			#if 0
			stBtSdpDidData *pSdpData = (stBtSdpDidData *)msg->pdata;
			#endif
			//BT_ACTION(me->pBtAction,SppDataInAction,pSppData);
			break;
		}
		default:
		{
			bt_ao_error_printf(" sdp no exit such sig!");
			break;
		}
	}

	return 0;
}

/******************************************************************************************/
/**
 * \fn		void _DevConnectTimeOutHandle(BtCBKEvt *msg)
 *
 * \brief		dev connect tomeout handle
 *
 * \param	msg:BtCBKEvt
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static void _DevConnectTimeOutHandle(BtCBKEvt *msg)
{
	BtAddr_t *pAddress = NULL;
	BT_AO_t *me = _AoGet();

	if(msg->pdata != NULL)
	{
		pAddress = (BtAddr_t *)msg->pdata;
		BT_ACTION(me->pBtAction,BtConnectTimeOutHandle,pAddress);
	}

}
/******************************************************************************************/
/**
 * \fn		int BtA2dpCbkHandle(BtCBKEvt *msg)
 *
 * \brief		bt a2dp cbk handle
 *
 * \param
 *              		msg:BtCBKEvt
 *
 * \return	    void
 *
 ******************************************************************************************/
extern int UserSetting_SaveBtAutoConDev(BtAddr_t auto_addr);
extern int PhoneGetAddr(BtAddr_t *pAddr);

static int BtA2dpCbkHandle(BtCBKEvt *msg)
{
	stBtDevInfo *pDev = NULL;
	BT_AO_t *me = _AoGet();

	if(msg->pdata != NULL)
	{
		pDev = (stBtDevInfo *)msg->pdata;
	}

	switch (msg->type)
	{
		case BT_A2DP_CONNECT_ING_SIG:
		{
			bt_ao_ntc_printf("BT_CONNECT_IND_SIG !");
			me->BtCurInfo.BaseState = connecting;
			BT_ACTION(me->pBtAction,ConnectingAction,pDev);
			break;
		}
		case BT_A2DP_CONNECT_FAIL_SIG:
		{
			bt_ao_ntc_printf("BT_CONNECT_FAIL_SIG!");
			me->BtCurInfo.BaseState = idle;
			BT_ACTION(me->pBtAction,ConnectFailAction,pDev);
			break;
		}
		case BT_A2DP_CONNECT_SUCCESS_SIG:
		{
			bt_ao_ntc_printf("BT_CONNECT_SUCCESS_SIG !");

			BtResetAudioCodec();
			BtAllowAudioDataIn(ON);

			Profile_Connected_handle(pDev,AVDTP);

			if(me->BtCurInfo.PlatformRole == BT_MAIN)
			{
				if(me->BtCurInfo.BtSurportTws == ON)
				{
					if(pDev->DevRole == HEADPHONE_ADDR)
					{
						_BtAoAudioSetEncodePlaySync(TRUE);
					}
				}
			}
			else
			{
				me->StandByResFlag = FALSE;
			}

			me->BtCurInfo.BaseState = connected;
			BT_ACTION(me->pBtAction,ConnectOkAction,pDev);

			/*if(me->BtCurInfo.DevConnectCounter >= me->BtCurInfo.DevMaxConnect)
			{
				if(me->BtCurInfo.PlatformRole == BT_MAIN)
				{
					BtAutoCheckTimerEnable(FALSE);
				}
			}*/ //zehai.liu todo

			if(me->BtCurInfo.BtSurportAutoConnet == ON)
			{
				BtAutoCheckTimerEnable(FALSE);

				BtAddr_t bt_auto_addr = {{0}};
	            PhoneGetAddr(&bt_auto_addr);
				UserSetting_SaveBtAutoConDev(bt_auto_addr);

	            LOGD("UserSetting_SaveBtAutoConDev [0x%02x][0x%02x][0x%02x][0x%02x][0x%02x][0x%02x]......\n",
					bt_auto_addr.address[0],bt_auto_addr.address[1],
					bt_auto_addr.address[2],bt_auto_addr.address[3],
					bt_auto_addr.address[4],bt_auto_addr.address[5]);
			}

			break;
		}
		case BT_A2DP_DISCONNECT_ING_SIG:
		{
			bt_ao_ntc_printf("BT_DISCONNECT_IND_SIG !");
			me->BtCurInfo.BaseState = idle;

			BT_ACTION(me->pBtAction,DisconnectingAction,pDev);
			break;
		}
		case BT_A2DP_DISCONNECT_FAIL_SIG:
		{
			bt_ao_ntc_printf("BT_DISCONNECT_FAIL_SIG!");
			BT_ACTION(me->pBtAction,DisconnectokAction,pDev);
			break;
		}
		case BT_A2DP_DISCONNECT_SUCCESS_SIG:
		{
			bt_ao_ntc_printf("BT_DISCONNECT_SUCCESS_SIG !");

			/*Connected_Device *stConnected_Device = NULL;
			int i;
			for(i=0;i<Connected_Device_Max;i++)
			{
				if(!(memcmp(&me->Connected_Device_List[i].bt_addr,&pDev->bd_addr,6)))
				{
					stConnected_Device = &me->Connected_Device_List[i];
					break;
				}
			}

			if(stConnected_Device == NULL)
			{
				bt_ao_ntc_printf("disconnect record wrong !");
				break;
			}

			stConnected_Device->profile_state &= ~(1 << AVDTP);
			if(stConnected_Device->profile_state == 0)
			{
				memset(stConnected_Device,0,sizeof(Connected_Device));
			}*/
			Profile_Disconnected_handle(pDev,AVDTP);

			BT_ACTION(me->pBtAction,DisconnectokAction,pDev);
			me->BtCurInfo.BaseState = idle;

			/*if(me->BtCurInfo.DevConnectCounter < me->BtCurInfo.DevMaxConnect)
			{
				BtAutoCheckTimerEnable(TRUE);
			}*/ //zehai.liu todo

			BtAutoCheckTimerEnable(TRUE);

			if(me->BtCurInfo.PlatformRole == BT_MAIN)
			{
				if(me->BtCurInfo.BtSurportTws == ON)
				{
					if(pDev->DevRole == HEADPHONE_ADDR || pDev->DevRole == WSS_ADDR)
					{
						_BtAoAudioSetEncodePlaySync(FALSE);
					}
				}

                if(!me->BtCurInfo.IsBtMode)
                {
                    BtAoAudioAllow(FALSE);
                }
			}

			if((me->PowerOffFlag == TRUE))
			{
				int DevConnectNum = me->BtCurInfo.DevConnectCounter;

				if(DevConnectNum <= 0)
				{
					BtAoEvt *pMsg = _AoMsgPackage(BT_START_POWER_OFF_SIG,NULL,0);

					if(pMsg != NULL)
					{
						QACTIVE_POST(BtAoGet(), (QEvt *)pMsg, BtAoGet());
					}
				}
			}

			break;
		}
		default:
		{
			bt_ao_error_printf("a2dp cbk no exist such sig !");
			break;
		}

	}

	return 0;
}

/******************************************************************************************/
/**
 * \fn		int BtRemoteCtrlCbkHandle(BtCBKEvt *msg)
 *
 * \brief		bt remote control cbk handle
 *
 * \param	msg:BtCBKEvt
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int BtRemoteCtrlCbkHandle(BtCBKEvt *msg)
{
	stBtDevInfo *pDev = NULL;
	BT_AO_t *me = _AoGet();

	if(msg->pdata != NULL)
	{
		pDev = (stBtDevInfo *)msg->pdata;
	}

	switch (msg->type)
	{
		case BT_AVRCP_CONNECT_SUCCESS_SIG:
		{
			bt_ao_ntc_printf("bt avrcp connect!");
			Profile_Connected_handle(pDev,AVRCP);
		}
		break;
		case BT_AVRCP_DISCONNECT_SUCCESS_SIG:
		{
			bt_ao_ntc_printf("bt avrcp disconnect!");
			Profile_Disconnected_handle(pDev,AVRCP);
		}
		break;
		case BT_VOLUME_SET_REQ_SIG:
		{
			bt_ao_ntc_printf("bt volume set!");

			break;
		}
		case BT_VOLUME_REG_REQ_SIG:
		{
			bt_ao_ntc_printf("bt volume reg!");
			BT_ACTION(me->pBtAction,VolumeSyncReg,(BtVolumeInfo *)msg->pdata);
			break;
		}
		case BT_SAMSUNG_TV_FLAG_SIG:
		{
			bt_ao_ntc_printf("bt samsung tv flag!");
			break;
		}
		case BT_NEXT_PLAY_REQ_SIG:
		{
			bt_ao_ntc_printf("bt next paly!");

			break;
		}
		case BT_PREV_PLAY_REQ_SIG:
		{
			bt_ao_ntc_printf("bt prev play!");

			break;
		}
		case BT_PAUSE_REQ_SIG:
		{
			bt_ao_ntc_printf("bt prev play!");

			break;
		}
		case BT_PLAY_REQ_SIG:
		{
			bt_ao_ntc_printf("bt prev play!");

			break;
		}
		case BT_VOLUMEUP_REQ_SIG:
		{
			break;
		}
		case BT_VOLUMEDOWN_REQ_SIG:
		{
			break;
		}

		default:
		{
			bt_ao_error_printf("avrcp no exist such cbk sig !");
			break;
		}

	}

	return 0;
}

/******************************************************************************************/
/**
 * \fn		int BtBaseMsgCbkHandle(BtCBKEvt *msg)
 *
 * \brief		bt base msg cbk handle
 *
 * \param	msg:BtCBKEvt
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int BtBaseMsgCbkHandle(BtCBKEvt *msg)
{
	static BTSearchDevInfo BtSearchInfo;
	BTPairDev PairInfo;
	BT_AO_t *me = _AoGet();

	switch (msg->type)
	{
		case BT_READY_SIG:
		{
			BtAoEvt *pMsg = _AoMsgPackage(BT_ENABLE_FINISH_SIG,NULL,0);

			me->BtCurInfo.BtState = BT_READY;
			BtAutoCheckTimerEnable(TRUE);

			if((me->BtCurInfo.PlatformRole == BT_SUB)&&(me->BtCurInfo.BtSurportStandBy == ON))
			{
				int nums = 0;
				int role = 0;
				BT_ACTION(me->pBtAction,GetBondNum,&nums);

				if(nums > 0)
				{
					BtAoEvt *pMsg = _AoMsgPackage(BT_AUTO_CONNECT_START_SIG,&role,sizeof(int));
					QACTIVE_POST(&me->super, (QEvt *)pMsg, &me->super);
				}
			}

			if(me->BtCurInfo.IsBtMode == TRUE)
			{
				BtAoAudioAllow(TRUE);
			}
			else
			{
				BtAoAudioAllow(FALSE);
			}

			BT_ACTION(me->pBtAction,BtReadyOkDo,me->BtCurInfo.IsBtMode);
			QACTIVE_POST(&me->super, (QEvt *)pMsg, &me->super);
			break;
		}
		case BT_UNREADY_SIG:
		{
			BtAoEvt *pMsg = _AoMsgPackage(BT_DISABLE_FINISH_SIG,NULL,0);

			me->BtCurInfo.BtState = BT_INIT;
			QACTIVE_POST(&me->super, (QEvt *)pMsg, &me->super);
			break;
		}
		case BT_SEARCH_RESULT_SIG:
		{
			bt_ao_printf("searching...!");
			memset(&BtSearchInfo,0,sizeof(BTSearchDevInfo));

			if(msg->pdata!=NULL)
			{
				memcpy(&BtSearchInfo,msg->pdata,sizeof(BTSearchDevInfo));
			}

			if(!(!me->BtCurInfo.IsSurportSwitch && me->BtCurInfo.BtTwsMode == WSS))
			{
				BT_ACTION(me->pBtAction,SearchResultDo,me->BtCurInfo.pBTDesNameList,&BtSearchInfo);
			}

			break;
		}
		case BT_SEARCH_FINISH_SIG:
		{
			bt_ao_ntc_printf("search finish!");

			if(!(!me->BtCurInfo.IsSurportSwitch && me->BtCurInfo.BtTwsMode == WSS))
			{
				BT_ACTION(me->pBtAction,SearchFinishDo);
			}

			break;
		}

		case BT_PAIRING_IND_SIG:
		{
			bt_ao_printf("pairing !");
			memset(&PairInfo,0,sizeof(BTPairDev));

			if(msg->pdata!=NULL)
			{
				memcpy(&PairInfo,(msg->pdata),sizeof(BTPairDev));
			}

			BT_ACTION(me->pBtAction,PairingDo,&PairInfo);
			break;
		}
		case BT_PAIRED_OK_SIG:
		{
			memset(&PairInfo,0,sizeof(BTPairDev));
			memcpy(&PairInfo,msg->pdata,sizeof(PairInfo));
			bt_ao_ntc_printf("pair ok,pair name: %s !",PairInfo.name);

			PairInfo.result = SUCCESS;
			#ifndef USER_BT_DUT
			//BT_ACTION(me->pBtAction,PairFinishDo,&PairInfo);
			#else
			//BT_ACTION(me->pBtAction,PairFinishDo,&PairInfo);
			#endif
			break;
		}

		case BT_UNPAIRED_OK_SIG:
		{
			memset(&PairInfo,0,sizeof(BTPairDev));
			memcpy(&PairInfo,msg->pdata,sizeof(PairInfo));
			bt_ao_printf("pair name: %s!",PairInfo.name);

			if(me->BtUserAction == DO_UNPAIR)
			{
				me->BtUserAction = DO_NONE;

			}
			else
			{
				//pair fail
				PairInfo.result = FAIL;
				BT_ACTION(me->pBtAction,PairFinishDo,&PairInfo);
			}

			break;
		}
		case BT_CLEARBOND_OK_SIG:
		{
			me->BtUserAction = DO_NONE;
			bt_ao_printf("clear all bonded device success!");
			BT_ACTION(me->pBtAction,ClearAutoInfo);
			break;
		}
		case BT_DEV_CONNECT_TIMEOUT_SIG:
		{
			bt_ao_warn_printf("connect timeout!");
			_DevConnectTimeOutHandle(msg);
			break;
		}
		case BT_TWS_SWITCH_FINISH_SIG:
		{
			CFG_BT_T *pBtConf = GET_SDK_CFG(CFG_BT_T);

			if(me->BtCurInfo.BtSurportTws == ON)
			{
				me->BtCurInfo.BtTwsMode = *(SINK_MODE_e *)msg->pdata;
				BT_ACTION(me->pBtAction,BtTwsSwitchFinish,me->BtCurInfo.BtTwsMode);

				if(pBtConf->PlatformRole == BT_MAIN)
				{
					CFG_BTMAIN_T *pBtSdkCfg = &(pBtConf->BtSdkCfg.BtMainCfg);

					if(me->BtCurInfo.BtTwsMode == WSS)
					{
						me->BtCurInfo.pBTDesNameList = pBtSdkCfg->pSubListName;
					}
					else if(me->BtCurInfo.BtTwsMode == HEADPHONE)
					{
						me->BtCurInfo.pBTDesNameList = pBtSdkCfg->pHeadphoneListName;
					}
				}

				bt_ao_ntc_printf("tws switch finish:%d !",me->BtCurInfo.BtTwsMode);
			}

			break;
		}
		case BT_ENCODE_PLAY_REQ_SIG:
		{
			if(me->BtCurInfo.BtSurportTws == ON)
			{
				_BtAoAudioSetEncodePlaySync(TRUE);
			}

			break;
		}
		case BT_SOURCE_STATE_UPDATE_SIG:
		{
			BtDevPlayState *pDevPlayer = (BtDevPlayState *)(msg->pdata);
			bt_ao_printf("bt player state %x!",pDevPlayer->state);
			break;
		}
		case BT_ACL_CONNECT_SIG:
		{
			bt_ao_printf("bt acl connect req!");
			BTConnectReqInfo *pResultInfo = (BTConnectReqInfo *)msg->pdata;
			BT_ACTION(me->pBtAction,ConnectReqResult,pResultInfo);
			break;
		}
		case BT_REMOTE_DEVICE_NAME_SIG:
		{
			bt_ao_printf("bt get name!");
			break;
		}
		default:
		{
			bt_ao_error_printf("base cbk no exist such cbk sig!");
			break;
		}
	}

	return 0;
}

/******************************************************************************************/
/**
 * \fn		int _BtCBKMsgHandle(BtCBKEvt *msg)
 *
 * \brief		bt cbk handle
 *
 * \param	msg:BtCBKEvt
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int _BtCBKMsgHandle(BtCBKEvt *msg)
{
	BT_AO_t *me = _AoGet();

	switch (msg->type)
	{
		case BT_BASE_CBK_SIG...BT_BASE_CBK_MAX_SIG:
		{
			BtBaseMsgCbkHandle(msg);
			break;
		}
		case BT_A2DP_CBK_SIG...BT_A2DP_CBK_MAX_SIG:
		{
			BtA2dpCbkHandle(msg);
			break;
		}
		case BT_SPP_CBK_SIG...BT_SPP_CBK_MAX_SIG:
		{
			BtSppCbkHandle(msg);
			break;
		}
		case BT_AVRCP_REMOTE_REQ_SIG...BT_AVRCP_REMOTE_REQ_MAX_SIG:
		{
			BtRemoteCtrlCbkHandle(msg);
			break;
		}
		case BT_SDP_CBK_SIG...BT_SDP_CBK_MAX_SIG:
		{
			BtSdpCbkHandle(msg);
			break;
		}
		default:
		{
			bt_ao_error_printf("no exist such cbk sig!");
			break;
		}
	}

	USER_CBK(me->pUserCBK,UserCbkFunc,msg->pdata,msg->type);

	if(msg->pdata != NULL)
	{
		if(msg->type == BT_SPP_DATA_IN_SIG)
		{
			stBtSppData *pSppData = (stBtSppData *)msg->pdata;
			if(pSppData->pBtData != NULL){
				BtFree(pSppData->pBtData);
				pSppData->pBtData = NULL;
			}
		}
		BtFree(msg->pdata);
		msg->pdata = NULL;
	}

	return 0;
}
/******************************************************************************************/
/**
 * \fn		void _BtIrMsgHandle(BtCBKEvt *pMsg)
 *
 * \brief		bt ir action
 *
 * \param	pMsg:BtCBKEvt
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static void _BtIrMsgHandle(BtCBKEvt *pMsg)
{
	BT_AO_t *me = _AoGet();

	switch(pMsg->type)
	{
	case BT_PLAY_SIG:
	{
		BT_ACTION(me->pBtAction,BtPlayAtionDo);
		break;
	}
	case BT_PAUSE_SIG:
	{
		BT_ACTION(me->pBtAction,BtPauseAtionDo);
		break;
	}
	case BT_NEXT_SIG:
	{
		BT_ACTION(me->pBtAction,BtNextAtionDo);
		break;
	}
	case BT_PREV_SIG:
	{
		BT_ACTION(me->pBtAction,BtPrevAtionDo);
		break;
	}
	case BT_WSS_PLAY_SIG:
	{
		BT_ACTION(me->pBtAction,SendWssPlay);
		break;
	}
	case BT_WSS_PAUSE_SIG:
	{
		BT_ACTION(me->pBtAction,SendWssPause);
		break;
	}
	case BT_VOLUME_CHANGE_SIG:
	{
		if((me->BtCurInfo.IsBtSinkVolumeSyncReg == TRUE && me->BtCurInfo.BtSurportTws == ON ) \
								|| me->BtCurInfo.IsBtSourceVolumeSyncReg == TRUE)
		{
			BT_ACTION(me->pBtAction,VolumeSyncSet,(BtVolumeSet *)pMsg->pdata);
		}

		break;
	}
	default:
	{
		bt_ao_error_printf("no exist ir sig!");
		break;
	}

	}

}

/******************************************************************************************/
/**
 * \fn		int BtAoServiceModuleMsgHandle(BtAoEvt *msg,QState *pStatus,BT_AO_t * const me)
 *
 * \brief		bt ao module msg handle
 *
 * \param	msg:BtAoEvt     pStatus:QState     me:BT_AO_t
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int BtAoServiceModuleMsgHandle(BtAoEvt *msg,QState *pStatus,BT_AO_t * const me)
{

	switch (msg->type)
	{
	case BT_DISABLE_FINISH_SIG:
	{
		//if(me->ApUpgrade.UpGradeFlag == TRUE)
		{
			BtCBKEvt *pMsg = BtMsgPackage(BT_DELINIT_SIG,NULL,0);

			if(pMsg != NULL)
			{
				QACTIVE_POST(&me->super, (QEvt *)pMsg, &me->super);
			}
		}

		*pStatus = Q_TRAN(&BT_AO_State_Idle);
		break;
	}
	case BT_ENABLE_FINISH_SIG:
	{
		if(me->BtCurInfo.BtSurportTransMode == ON)
		{
			//BtTransActionInit(&me->stSppDataService.pTransModeAction);
			_SendMsgTransModeOn();
		}

		if((me->PowerOffFlag == TRUE))
		{
			BtAoEvt *pMsg = _AoMsgPackage(BT_START_POWER_OFF_SIG,NULL,0);

			if(pMsg != NULL)
			{
				QACTIVE_POST(BtAoGet(), (QEvt *)pMsg, BtAoGet());
			}

		}

		*pStatus = Q_HANDLED();
		break;
	}
	case BT_STANDBY_RES_SIG:
	{
		me->StandByResFlag = TRUE;
		*pStatus = Q_HANDLED();
		break;
	}
	case BT_START_POWER_OFF_SIG:
	{
		if(me->BtCurInfo.BtState == BT_READY)
		{
			if(me->BtCurInfo.DevConnectCounter > 0)
			{
				BT_ACTION(me->pBtAction,BtPowerOffResAction);
			}
			else
			{
				BtCBKEvt *pMsg = BtMsgPackage(BT_DISABLE_SIG,NULL,0);

				if(pMsg != NULL)
				{
					QACTIVE_POST(BtAoGet(), (QEvt *)pMsg, BtAoGet());
				}
			}
		}

		*pStatus = Q_HANDLED();
		break;
	}
	case BT_AUTO_CONNECT_START_SIG:
	{
		int role = *((int *)msg->pdata);

		if(me->BtCurInfo.BtSurportStandBy == ON)
		{
			me->StandByResFlag = TRUE;
		}

		BT_ACTION(me->pBtAction,AutoConnect,role);
		*pStatus = Q_HANDLED();
		break;
	}
	default:
	{
		*pStatus = Q_HANDLED();
		break;
	}

	}

	if(msg->pdata != NULL)
	{
		BtFree(msg->pdata);
		msg->pdata = NULL;
	}

	return 0;
}
/******************************************************************************************/
/**
 * \fn		void BtTransmitData(BtAddr_t *pBtAddr,Spp_Send_Data_If *pBtData)
 *
 * \brief		bt trans mitter data send
 *
 * \param	pBtAddr:addr 		pBtData:data struct
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
/*static void BtTransmitData(BtAddr_t *pBtAddr,Spp_Send_Data_If *pBtData)
{
	SendData2TransLayer(*pBtAddr,(char *)pBtData);
	bt_ao_printf(" data: %s ;",(char *)(pBtData-sizeof(AppSendData_Type_Flag)));
}*/

/******************************************************************************************/
/**
 * \fn		int TransCmdParse(Reciverdata_Type_Flag *pRecTypeFlag)
 *
 * \brief		trans mitter cmd parse
 *
 * \param	pRecTypeFlag:Reciverdata_Type_Flag
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int TransCmdParse(Reciverdata_Type_Flag *pRecTypeFlag)
{
	switch(pRecTypeFlag->value)
	{
		case TYPE_UPGRADE_REQ:
			bt_ao_printf("TYPE_UPGRADE_REQ!");
			break;
		case TYPE_UPGRADE_READY:
			bt_ao_printf("TYPE_UPGRADE_READY!");
			break;
		case TYPE_UPGRADE_DATA_FINISH:
		bt_ao_printf("TYPE_UPGRADE_DATA_FINISH!");
			break;
		default :
			bt_ao_printf("Trans default!");
			break;
	}

	return 0;
}
/******************************************************************************************/
/**
 * \fn		int TransDataParse(Reciverdata_Type_Flag *pRecTypeFlag,char *buf)
 *
 * \brief		bt trans mittter data parse
 *
 * \param	pRecTypeFlag:Reciverdata_Type_Flag      buf:data buf
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int TransDataParse(Reciverdata_Type_Flag *pRecTypeFlag,char *buf)
{
	(void)buf;
	switch(pRecTypeFlag->value)
	{
		case TYPE_UPGRADE_DATA_IN:
			bt_ao_printf("%s: TYPE_UPGRADE_DATA_IN",__FUNCTION__);
			bt_ao_printf("\n %s \n",(buf-sizeof(Reciverdata_Type_Flag)));
		break;
		case TYPE_REMOTE_APP_DATA:
			bt_ao_printf("%s: TYPE_REMOTE_APP_DATA",__FUNCTION__);
		break;
		default:
			bt_ao_printf("%s: default",__FUNCTION__);
		break;
	}

	return 0;
}
/******************************************************************************************/
/**
 * \fn		int TransReciveDataParse(char *buf)
 *
 * \brief		trans mitter rev data parse
 *
 * \param	IsTrue: allow ----TRUE   disallow------FALSE
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int TransReciveDataParse(char *buf)
{
	Spp_receiver_Data_If *pReciveData = (Spp_receiver_Data_If *)buf;

	if(pReciveData->Type_Flag.func == PAYLOAD_CMD)
		TransCmdParse(&(pReciveData->Type_Flag));
	else if(pReciveData->Type_Flag.func == PAYLOAD_DATA)
		TransDataParse(&(pReciveData->Type_Flag),buf);
	return 0;
}
/******************************************************************************************/
/**
 * \fn		void BtTransDataMsgHandle(BtCBKEvt *msg)
 *
 * \brief		trans mittter data msg handle
 *
 * \param	msg:BtCBKEvt
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static void BtTransDataMsgHandle(BtCBKEvt *msg)
{
	//Sunplus//Spp_Send_Data_If *pSendData = NULL;
	stBtData *pSppData = NULL;

	if(msg->pdata != NULL)
	{
		pSppData = (stBtData *)msg->pdata;

		if(pSppData->pBtData != NULL)
		{
			//Sunplus//pSendData = (Spp_Send_Data_If *)pSppData->pBtData;
		}
	}

	switch (msg->type)
	{
	case BT_SEND_DATA_SIG:
	{
		/*if(pSendData != NULL)
		{
			bt_ao_printf("%s:BT_SEND_DATA_SIG \n",__FUNCTION__);
			BtTransmitData(&pSppData->BtAddr,pSendData);
		}*/
		bt_ao_printf("%s:BT_SEND_DATA_SIG \n",__FUNCTION__);

		break;
	}
	default:
	break;
	}

	if(msg->pdata != NULL)
	{
		/*if(pSendData != NULL)
		{
			if(pSendData->pdatabuf)
			{
				BtFree(pSendData->pdatabuf);
			}
		}*/

		BtFree(msg->pdata);
	}

}

/******************************************************************************************/
/**
 * \fn		void BtTransLayerCbkMsgHandle(BtCBKEvt *msg)
 *
 * \brief		trans mitter  layer cbk handle
 *
 * \param	msg:BtCBKEvt
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static void BtTransLayerCbkMsgHandle(BtCBKEvt *msg)
{

	//Sunplus//Spp_receiver_Data_If *pRecvData = NULL;
	stBtData *pSppData = NULL;
	char *pRecvBuf = NULL;

	switch (msg->type)
	{
	case BT_TRANS_DATA_FINISH_SIG:
	{
		//cbk to user the data send finish and can send next
		bt_ao_printf("%s:BT_TRANS_DATA_FINISH_SIG\n",__FUNCTION__);
		break;
	}
	case BT_RECV_TRANS_DATA_SIG:
	{
		/*pRecvData = (Spp_receiver_Data_If *)msg->pdata;

		if(pRecvData == NULL)
		{
			break;
		}

		TransReciveDataParse((char *)pRecvData);

		if(pRecvData->pdatabuf)
		{
			BtFree(pRecvData->pdatabuf);
		}*/
		break;
	}
	case BT_RECV_DATA_SIG:
	{
		//Sunplus//stBtSppData *pSppData = NULL;
		if(msg->pdata != NULL)
		{
			//Sunplus//pSppData = (stBtSppData *)msg->pdata;
			pSppData = (stBtData *)msg->pdata;

			if(pSppData->pBtData == NULL)
			{
				break;
			}

			pRecvBuf = (char *)pSppData->pBtData;
		}
		//translayer analys
		//Sunplus//TransportLayerReceiverDataAnalys(pSppData->BtAddr,pRecvBuf,pSppData->len);

		if(pRecvBuf)
		{
			BtFree(pRecvBuf);
		}
		break;
	}
	default:
	break;
	}

	if(msg->pdata != NULL)
	{
		BtFree(msg->pdata);
	}

}
/******************************************************************************************/
/**
 * \fn		QActive *BtAoGet(void)
 *
 * \brief		get bt ao
 *
 * \param	void
 *
 *
 * \return	      QActive *
 *
 ******************************************************************************************/
QActive *BtAoGet(void)
{
	return &gstBtAO.super;
}
/******************************************************************************************/
/**
 * \fn		BtCBKEvt *BtMsgPackage(BTEVTSIG_e sig,void *pData,int DataLen)
 *
 * \brief		BT MSG packetage
 *
 * \param	sig:BTEVTSIG_e      BTEVTSIG_e:laod data		DataLen:data len
 *
 *
 * \return	    BtCBKEvt*
 *
 ******************************************************************************************/
BtCBKEvt *BtMsgPackage(BTEVTSIG_e sig,void *pData,int DataLen)
{
	void *pMsgData = NULL;
	BtCBKEvt *pMsg = NULL;

	pMsgData = pData;
	switch(sig)
	{
	case BT_BASE_TYPE_SIG...BT_BASE_TYPE_MAX__SIG:
		pMsg = Q_NEW(BtCBKEvt,BT_BASE_SIG);
	break;
	case BT_COMM_TYPE_SIG...BT_COMM_TYPE_MAX_SIG:
		pMsg = Q_NEW(BtCBKEvt,BT_COMM_SIG);
	break;
	case BT_TRANSMIT_TYPE_SIG...BT_TRANSMIT_TYPE_MAX_SIG:
		pMsg = Q_NEW(BtCBKEvt,BT_TRANSMIT_SIG);
	break;
	default:
		bt_ao_printf("package err!! bt ao no exsit such signal!\n");
		return NULL;
		break;
	}

	pMsg->type = sig;

	if(pMsgData != NULL && DataLen > 0)
	{
		pMsg->pdata = BtMalloc((DataLen+1));

		if(pMsg->pdata)
		{
			memset(pMsg->pdata,0,(DataLen+1));
			memcpy(pMsg->pdata,pData,DataLen);
		}
		else
		{
			bt_ao_error_printf("BtMalloc FAIL!!");
		}
	}
	else
	{
		pMsg->pdata = pMsgData;
	}

	return pMsg;
}

/******************************************************************************************/
/**
 * \fn		int AoIsBtMode(void)
 *
 * \brief		if bt mode on
 *
 * \param	void
 *
 *
 * \return	      TRUE 		FALSE
 *
 ******************************************************************************************/
int AoIsBtMode(void)
{
	BT_AO_t *me = _AoGet();
	int IsTrue = me->BtCurInfo.IsBtMode;
	return IsTrue;
}

/******************************************************************************************/
/**
 * \fn		int AoGetTransModeState(void)
 *
 * \brief		get trans mode state
 *
 * \param	void
 *
 *
 * \return	   on / off
 *
 ******************************************************************************************/
int AoGetTransModeState(void)
{
	BT_AO_t *me = _AoGet();
	int state = me->BtCurInfo.BtSurportTransMode;
	return state;
}

/******************************************************************************************/
/**
 * \fn		int AoGetTwsState()
 *
 * \brief		get tws state
 *
 * \param	void
 *
 *
 * \return	    state: on /off
 *
 ******************************************************************************************/
int AoGetTwsState()
{
	BT_AO_t *me = _AoGet();
	int state = me->BtCurInfo.BtSurportTws;
	return state;
}
/******************************************************************************************/
/**
 * \fn		int AoBtGetState(BT_STATE_e *pState)
 *
 * \brief		get bt state
 *
 * \param	pState:BT_STATE_e
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int AoBtGetState(BT_STATE_e *pState)
{
	BT_AO_t *me = _AoGet();

	if(pState != NULL)
	{
		*pState = me->BtCurInfo.BtState;
		return SUCCESS;
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		int AoBtIsPlay(void)
 *
 * \brief		if bt is playing
 *
 * \param	void
 *
 *
 * \return	     TRUE        FALSE
 *
 ******************************************************************************************/
int AoBtIsPlay(void)
{
	int IsTrue = FALSE;
	BT_AO_t *me = _AoGet();

	BT_ACTION(me->pBtAction,IsBtPlaying,&IsTrue);
	return IsTrue;
}
/******************************************************************************************/
/**
 * \fn		int AoBtIsSearching(void)
 *
 * \brief		bt if is searching now
 *
 * \param	void
 *
 *
 * \return	   TRUE        FALSE
 *
 ******************************************************************************************/
int AoBtIsSearching(void)
{
	int IsTrue = FALSE;
	BT_AO_t *me = _AoGet();

	BT_ACTION(me->pBtAction,BtIsSearching,&IsTrue);
	return IsTrue;
}
/******************************************************************************************/
/**
 * \fn		int AoBtGetAddrByDevRole(BtAddr_t *pBtAddr,DEVROLE_DEX_e dex)
 *
 * \brief		get bt addr by role
 *
 * \param	pBtAddr:BtAddr_t     dex:DEVROLE_DEX_e
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int AoBtGetAddrByDevRole(BtAddr_t *pBtAddr,DEVROLE_DEX_e dex)
{
	BT_AO_t *me = _AoGet();

	if(pBtAddr != NULL)
	{
		BT_ACTION(me->pBtAction,GetBtAddrByDevRole,pBtAddr,dex);
		return SUCCESS;
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		int AoBtGetConnectStateByDevRole(DEVROLE_DEX_e dex,BT_PROFILE_STATE_e *pState)
 *
 * \brief		get connect state by role
 *
 * \param	pState:BT_PROFILE_STATE_e    dex:DEVROLE_DEX_e
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int AoBtGetConnectStateByDevRole(DEVROLE_DEX_e dex,BT_PROFILE_STATE_e *pState)
{
	BT_AO_t *me = _AoGet();

	if(pState != NULL)
	{
		BT_ACTION(me->pBtAction,GetConnectState,dex,pState);
		return SUCCESS;
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		int AoBtGetProfileState(BtAddr_t *pBtAddr,BT_PROFILE_e profile,BT_PROFILE_STATE_e *pstate)
 *
 * \brief		bt get profile connect state
 *
 * \param	pBtAddr:addr   profile:BT_PROFILE_e     pstate:BT_PROFILE_STATE_e
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
	int AoBtGetProfileState(BtAddr_t *pBtAddr,BT_PROFILE_e profile,BT_PROFILE_STATE_e *pstate)
	{
		//BT_AO_t *me = _AoGet();
		(void)pBtAddr;
		//if(memcmp(pBtAddr,&stProfile_State.bt_addr,6)==0)
		/*{
			if(profile == A2DP_SINK || profile == A2DP_SOURCE)
			{
				if(stProfile_State.profile & 1<<AVDTP)
				{
					*pstate = CONNECT_OK;
				}
				else
				{
					*pstate = DISCONNECT_OK;
				}
			}
		}*/

		LOGD("%s,profile:%d,state:%d\n",__FUNCTION__,profile,*pstate);

		return SUCCESS;

		/*if(pBtAddr != NULL && pstate != NULL)
		{
			BT_ACTION(me->pBtAction,GetProfileState,pBtAddr,profile,pstate);
			return SUCCESS;
		}

		return FAIL;*/
	}




/******************************************************************************************/
/**
 * \fn		int AoBtGetRemoteDevInfo(BtAddr_t *pBtAddr,stBtDevInfo *pBtDev)
 *
 * \brief		get dev info by addr
 *
 * \param	pBtAddr:addr 				pBtDev:stBtDevInfo
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int AoBtGetRemoteDevInfo(BtAddr_t *pBtAddr,stBtDevInfo *pBtDev)
{
	BT_AO_t *me = _AoGet();

	if(pBtAddr != NULL && pBtDev != NULL)
	{
		BT_ACTION(me->pBtAction,GetRemoteDevInfo,pBtAddr,pBtDev);
		return SUCCESS;
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		int AoBtGetRemoteDevByIdex(int dex,stBtDevInfo *pBtDev)
 *
 * \brief		get bt dev info by dex
 *
 * \param	dex:dex 		pBtDev:	stBtDevInfo
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int AoBtGetRemoteDevByIdex(int dex,stBtDevInfo *pBtDev)
{
	BT_AO_t *me = _AoGet();

	if(pBtDev != NULL)
	{
		BT_ACTION(me->pBtAction,GetRemoteDevByIdex,dex,pBtDev);
		return SUCCESS;
	}

	return FAIL;
}

/******************************************************************************************/
/**
 * \fn		int AoBtLocalNameGet(char *pName)
 *
 * \brief		get local name
 *
 * \param	pName:bt name
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int AoBtLocalNameGet(char *pName)
{
	BT_AO_t *me = _AoGet();

	if(pName != NULL)
	{
		BT_ACTION(me->pBtAction,GetLocalName,pName);
		return SUCCESS;
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		int AoBtGetBondDevNum(int *pBondedNum)
 *
 * \brief		get bonded dev nums
 *
 * \param	pBondedNum: bonded dev nums
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int AoBtGetBondDevNum(int *pBondedNum)
{
	BT_AO_t *me = _AoGet();

	if(pBondedNum != NULL)
	{
		BT_ACTION(me->pBtAction,GetBondNum,pBondedNum);
		return SUCCESS;
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		int AoBtGetBondDevByIdex(int sdBondedNum,stBtDevInfo *pBtDev)
 *
 * \brief		get bonded dev info
 *
 * \param	sdBondedNum: bonded dev Idex   pBtDev: Dev Info
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int AoBtGetBondDevByIdex(int sdBondedNum,stBtDevInfo *pBtDev)
{
	BT_AO_t *me = _AoGet();

	if(pBtDev != NULL)
	{
		BT_ACTION(me->pBtAction,GetBondByIdex,sdBondedNum,pBtDev);
		return SUCCESS;
	}

	return FAIL;
}

/******************************************************************************************/
/**
 * \fn		int AoBtGetRemoteDevNum(int *pDevNum)
 *
 * \brief		get remote dev nums
 *
 * \param	pDevNum: dev nums
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int AoBtGetRemoteDevNum(int *pDevNum)
{
	BT_AO_t *me = _AoGet();

	if(pDevNum != NULL)
	{
		BT_ACTION(me->pBtAction,GetRemoteDevNum,pDevNum);
		return SUCCESS;
	}

	return FAIL;
}

/******************************************************************************************/
/**
 * \fn		int AoBtLocalAddrGet(BtAddr_t*pBtAddr)
 *
 * \brief		get local addr
 *
 * \param	pBtAddr: BtAddr_t
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int AoBtLocalAddrGet(BtAddr_t*pBtAddr)
{
	BT_AO_t *me = _AoGet();

	if(pBtAddr != NULL)
	{
		BT_ACTION(me->pBtAction,GetLocalAddr,pBtAddr);
		return SUCCESS;
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		int AoBtGetLocalInfo(BtLocalInfo *pdev)
 *
 * \brief		get local info
 *
 * \param	Ipdev:BtLocalInfo
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int AoBtGetLocalInfo(BtLocalInfo *pdev)
{
	BT_AO_t *me = _AoGet();

	if(pdev != NULL)
	{
		BT_ACTION(me->pBtAction,GetLocalInfo,pdev);
		return SUCCESS;
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		void AoBtScanModeSet(SCAN_MODE_e mode)
 *
 * \brief		get  scan mode
 *
 * \param	mode:SCAN_MODE_e
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
void AoBtScanModeSet(SCAN_MODE_e mode)
{
	BT_AO_t *me = _AoGet();
	BT_ACTION(me->pBtAction,BtSetScanMode,mode);
}
/******************************************************************************************/
/**
 * \fn		int AoBtScanModeGet(SCAN_MODE_e *pMode)
 *
 * \brief		get scan mode
 *
 * \param	pMode:SCAN_MODE_e
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int AoBtScanModeGet(SCAN_MODE_e *pMode)
{
	BT_AO_t *me = _AoGet();

	if(pMode != NULL)
	{
		BT_ACTION(me->pBtAction,BtGetScanMode,pMode);
		return SUCCESS;
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		int AoBtCbkRegister(AO_USER_CBK_FUNC* pMsgSendFunc)
 *
 * \brief		ao cbk register
 *
 * \param	pMsgSendFunc :cbk function
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int AoBtCbkRegister(AO_USER_CBK_FUNC* pMsgSendFunc)
{
	if(pMsgSendFunc == NULL)
	{
		return FAIL;
	}

	BT_AO_t * me = _AoGet();
	me->pUserCBK = pMsgSendFunc;
	return SUCCESS;
}

/******************************************************************************************/
/**
 * \fn		int AoBtCbkDelRegister(AO_USER_CBK_FUNC* pMsgSendFunc)
 *
 * \brief		delect ao cbk register
 *
 * \param	pMsgSendFunc :cbk function
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int AoBtCbkDelRegister(AO_USER_CBK_FUNC* pMsgSendFunc)
{
	BT_AO_t * me = _AoGet();

	if(pMsgSendFunc != me->pUserCBK)
	{
		return FAIL;
	}

	me->pUserCBK = NULL;
	return SUCCESS;
}

/******************************************************************************************/
/**
 * \fn		void TransmitModeCtr(BtTransActive * const me,QEvt const * const e,QState *pdStatus)
 *
 * \brief		trans mitter control handle
 * \param
 *              	me:BtTransActive  		e:QEvt          pdStatus:QState
 *
 * \return	    void
 *
 ******************************************************************************************/
void TransmitModeCtr(BtTransActive * const me,QEvt const * const e,QState *pdStatus)
{
	(void)me;
	BtCBKEvt *msg = NULL;
	msg = (BtCBKEvt *)e;

	switch (msg->type)
	{
	case BT_TRANSMIT_ON_SIG:
	{
		bt_ao_printf("%s:BT_TRANSMIT_ON_SIG \n",__FUNCTION__);
		BT_ACTION(me->pTransModeAction,TransModeOn);
		*pdStatus = Q_TRAN(&Transmit_Active);
		break;
	}
	case BT_TRANSMIT_OFF_SIG:
	{
		BT_ACTION(me->pTransModeAction,TransModeOff);
		*pdStatus = Q_TRAN(&Transmit_Idle);
		break;
	}
	default:
	{
		*pdStatus = Q_HANDLED();
		break;
	}
  	}

	if(msg->pdata != NULL)
	{
		BtFree(msg->pdata);
	}
}

/******************************************************************************************/
/**
 * \fn		void TransmitMsgHandle(BtTransActive * const me,QEvt const * const e,QState *pdStatus)
 *
 * \brief		trans mitter handle
 *
 * \param	me:BtTransActive  		e:QEvt          pdStatus:QState
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
void TransmitMsgHandle(BtTransActive * const me,QEvt const * const e,QState *pdStatus)
{
	(void)me;
	BtCBKEvt *msg = NULL;
	msg = (BtCBKEvt *)e;
	BT_AO_t *pBtAoInfo = _AoGet();

	switch (msg->type)
	{
	case BT_TRANSMIT_ON_SIG:
	{
		BT_ACTION(me->pTransModeAction,TransModeOn);
		*pdStatus = Q_TRAN(&Transmit_Active);
		break;
	}
	case BT_TRANSMIT_OFF_SIG:
	{
		BT_ACTION(me->pTransModeAction,TransModeOff);
		*pdStatus = Q_TRAN(&Transmit_Idle);
		break;
	}
	case BT_UPGRADE_SUB_SIG:
	{
		bt_ao_printf("BT_UPGRADE_SUB_SIG\n");
		*pdStatus = Q_HANDLED();
		break;
	}
	case BT_SEND_DATA_SIG:
	{
		BtTransDataMsgHandle(msg);
		*pdStatus = Q_HANDLED();
		break;
	}
	case BT_SEND_CUSTOM_DATA_SIG:
	{
		bt_ao_ntc_printf("BT_CUSTOM_DATA_IN_SIG\n");

		stBtData *pMsgData = (stBtData *)msg->pdata;

		BT_ACTION(pBtAoInfo->pBtAction,SendUserCmd,&pMsgData->BtAddr,pMsgData->len,(UINT8 *)pMsgData->pBtData);

		if(pMsgData->pBtData)
		{
			BtFree(pMsgData->pBtData);
		}

		*pdStatus = Q_HANDLED();
		break;
	}
	default:
	{
		*pdStatus = Q_HANDLED();
		break;
	}
  	}

	if(msg->pdata != NULL)
	{
		BtFree(msg->pdata);
	}
}
/******************************************************************************************/
/**
 * \fn		void TransmitCbkHandle(BtTransActive * const me,QEvt const * const e,QState *pdStatus)
 *
 * \brief		trans mitter handle
 *
 * \param	me:BtTransActive  		e:QEvt          pdStatus:QState
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
void TransmitCbkHandle(BtTransActive * const me,QEvt const * const e,QState *pdStatus)
{
	(void)me;
	BtCBKEvt *msg = NULL;
	msg = (BtCBKEvt *)e;
	BT_AO_t *pBtAoInfo = _AoGet();

	switch (msg->type)
	{
		case BT_RECV_TRANS_DATA_SIG:
		case BT_TRANS_DATA_FINISH_SIG:
		case BT_RECV_DATA_SIG:
		{
			BtTransLayerCbkMsgHandle(msg);
			*pdStatus = Q_HANDLED();
			break;
		}
		case BT_CUSTOM_DATA_IN_SIG:
		{
			bt_ao_ntc_printf("BT_CUSTOM_DATA_IN_SIG\n");
			stBtData *pMsgData = (stBtData *)msg->pdata;

			USER_CBK(pBtAoInfo->pUserCBK,UserCbkFunc,msg->pdata,msg->type);

			if(pMsgData->pBtData)
			{
				BtFree(pMsgData->pBtData);
			}

			*pdStatus = Q_HANDLED();
			break;
		}
		default:
		{
			*pdStatus = Q_HANDLED();
			break;
		}
  	}

	if(msg->pdata != NULL)
	{
		BtFree(msg->pdata);
	}
}

/******************************************************************************************/
/**
 * \fn		QState Transmit_Active(BtTransActive * const me, QEvt const * const e)
 *
 * \brief		trans mitter active state
 *
 * \param	me:BtTransActive  				e:QEvt
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
QState Transmit_Active(BtTransActive * const me, QEvt const * const e)
{
    (void)e; /*!< avoid compiler warning about unused parameter */
	(void)me;
	QState dStatus;

	LOGI("Upgrade_Idle: Upgrade_Idle \n");
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGI("Transmit_Active: Q_ENTRY_SIG \n");
			dStatus = Q_HANDLED();
			break;

		}case Q_EXIT_SIG:
		{
			LOGI("Transmit_Active: Q_EXIT_SIG \n");
			dStatus = Q_HANDLED();
			break;

		}
		case BT_TRANSMIT_SIG:
		{
			TransmitMsgHandle(me,e,&dStatus);
			break;
		}
		case BT_TRANSMIT_CBK_SIG:
		{
			TransmitCbkHandle(me,e,&dStatus);
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
 * \fn		QState Transmit_Idle(BtTransActive * const me, QEvt const * const e)
 *
 * \brief		trans mitter idle state
 *
 * \param	me:BtTransActive  				e:QEvt
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
QState Transmit_Idle(BtTransActive * const me, QEvt const * const e)
{
    (void)e; /*!< avoid compiler warning about unused parameter */
	(void)me;
	QState dStatus;

	bt_ao_printf("Transmit_Idle: Transmit_Idle \n");

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGI("%s: Q_ENTRY_SIG \n",__FUNCTION__);
			dStatus = Q_HANDLED();
			break;

		}case Q_EXIT_SIG:
		{
			LOGI("%s: Q_EXIT_SIG \n",__FUNCTION__);
			dStatus = Q_HANDLED();
			break;

		}
		case Q_INIT_SIG:
		{
			LOGI("%s: Q_INIT_SIG \n",__FUNCTION__);
			dStatus = Q_HANDLED();
			break;

		}
		case BT_TRANSMIT_SIG:
		{
			LOGI("%s: BT_TRANSMIT_SIG \n",__FUNCTION__);
			TransmitModeCtr(me,e,&dStatus);
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
QState Transmit_Initial(BtTransActive * const me, QEvt const * const e)
{
	(void)me;
	(void)e;

	QState dStatus;
	dStatus = Q_TRAN(&Transmit_Idle);
	return dStatus;
}

/******************************************************************************************/
/**
 * \fn		void UpgradeService_Ctor(UPGRADESERVICE * const me)
 *
 * \brief		UPGRADESERVICE constructor
 *
 * \param	 	me : (Input) the UPGRADESERVICE hierarchical state machine
 *
 * \return		none.
 *
 * \note		do HSM constructor here.
 *
 ******************************************************************************************/
void TransmitService_Ctor(BtTransActive *const me)
{
	QHsm_ctor(&me->super, Q_STATE_CAST(& Transmit_Initial));
}


/******************************************************************************************/
/**
 * \fn		QState BT_AO_State_Idle(BT_AO_t * const me, QEvt const * const e)
 *
 * \brief		HSM function: BT ao is ilde
 *
 * \param  	me : (Input) the BT ao handler here
 * \param  	e : (Input) the event BT ao get
 *
 * \return		the value of state machine function return.
 *
 * \sa		qep.h
 *
 ******************************************************************************************/
QState BT_AO_State_Idle(BT_AO_t * const me, QEvt const * const e)
{
	(void)me;
    QState dStatus;

	switch (e->sig)
	{
		case Q_INIT_SIG:
		{
			dStatus = Q_HANDLED();
			break;
		}
		case BT_BASE_SIG:
		{
			BtCBKEvt *msg = (BtCBKEvt *)e;
			BtBaseMsgHandle(msg,&dStatus,me);
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
 * \fn		QState BT_AO_State_Idle(BT_AO_t * const me, QEvt const * const e)
 *
 * \brief		HSM function: bt ao is ilde
 *
 * \param  	me : (Input) the bt ao handler here
 * \param  	e : (Input) the event bt ao get
 *
 * \return		the value of state machine function return.
 *
 * \sa		qep.h
 *
 ******************************************************************************************/
QState BT_AO_State_Intend(BT_AO_t * const me, QEvt const * const e)
{
	QState dStatus = 0;
	(void)me; /*!< avoid compiler warning about unused parameter */

	switch (e->sig)
	{
		case BT_BASE_SIG:
		{
			BtBaseMsgHandle((BtCBKEvt *)e,&dStatus,me);
			break;
		}
		case BT_COMM_SIG:
		case BT_SERVICE_CBK_SIG:
		{
			dStatus = Q_SUPER(&BT_AO_State_Active);
			break;
		}
		case BT_TRANSMIT_SIG:
		case BT_TRANSMIT_CBK_SIG:
		{
			bt_ao_printf("%s BT_TRANSMIT_SIG  \n",__FUNCTION__);
			QMSM_DISPATCH(&me->stSppDataService.super, e);
			dStatus = Q_HANDLED();
			break;
		}
		case BT_TIME_CHECK_SIG:
		{
			int result = FAIL;
			int role = -1;

			BT_ACTION(me->pBtAction,BtAutoConCheck,&result,&role);
			_BtInStandByRes(me);

			if(me->BtCurInfo.BtSurportAutoConnet == ON)
			{
				BtAutoCheckTimerEnable(TRUE);
			}

			if(result == SUCCESS)
			{
				BtAoEvt *pMsg = _AoMsgPackage(BT_AUTO_CONNECT_START_SIG,&role,sizeof(int));
				QACTIVE_POST(&me->super, (QEvt *)pMsg, &me->super);
			}

			dStatus = Q_HANDLED();
			break;
		}
		case BT_TIME_DELAY_SIG:
		{
			if(me->BtCurInfo.PlatformRole == BT_SUB)
			{
				_BtAoAudioSubSendSyncReady(FALSE);
			}
			else
			{
				_BtAoAudioDelayEncodePlay(FALSE);
			}

			dStatus = Q_HANDLED();
			break;
		}
		case BT_AO_AUDIO_SIG:
		{
			bt_ao_printf("\n BT_AO_AUDIO_SIG !");
			_BtAoAudioHandle(e);
			dStatus = Q_HANDLED();
			break;
		}
		case BT_AO_SERVICE_MODULE_SIG:
		{
			BtAoServiceModuleMsgHandle((BtAoEvt *)e,&dStatus,me);
			break;
		}
		case ODSP_MESSAGE_SIG:
		case ODSP_RESET_START_SIG:
		case ODSP_RESET_END_SIG:
		{
			bt_ao_printf("\n ODSP_MESSAGE_SIG !");
			_BtAoAudioHandle(e);
			dStatus = Q_HANDLED();
			break;
		}
        default:
		{
			bt_ao_printf("%s  sig:%d,QHsm_top in \n",__FUNCTION__,e->sig);
            dStatus = Q_SUPER(&QHsm_top);
            break;
        }
    }
	return dStatus;
}


/******************************************************************************************/
/**
 * \fn		int BtCbkSigHandle(BtCBKEvt *pMsg)
 *
 * \brief		bt signal cbk handle
 *
 * \param	pMsg
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int BtCbkSigHandle(BtCBKEvt *pMsg)
{
	BtCBKEvt *msg = pMsg;

	if(msg == NULL)
	{
		return FAIL;
	}

	switch (msg->type)
	{
		case BT_SRV_CBK_SIG...BT_SRV_CBK_MAX_SIG:
		{
			_BtCBKMsgHandle(msg);
			break;
		}
		default:
		{
			break;
		}
	}

	if(msg->pdata != NULL)
	{
		BtFree(msg->pdata);
		msg->pdata = NULL;
	}

	return SUCCESS;
}

/******************************************************************************************/
/**
 * \fn		int BtCommonSigHandle(BtCBKEvt *pMsg)
 *
 * \brief		bt commentsignal handle
 *
 * \param	pMsg
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int BtCommonSigHandle(BtCBKEvt *pMsg)
{
	BtCBKEvt *msg = pMsg;

	if(msg == NULL)
	{
		return FAIL;
	}

	switch (msg->type)
	{
		case BT_USER_ACTION_SIG...BT_USER_ACTION_MAX_SIG:
		{
			bt_ao_printf("msg->type:%d \n",msg->type);
			_BtUserActionCommHandle(msg);
			break;
		}
		case BT_IR_SIG...BT_IR_MAX_SIG:
		{
			_BtIrMsgHandle(msg);
			break;
		}
		default:
		{
			bt_ao_printf("default msg->type:%d \n",msg->type);
			break;
		}
	}

	if(msg->pdata != NULL)
	{
		BtFree(msg->pdata);
		msg->pdata = NULL;
	}

	return SUCCESS;
}
/******************************************************************************************/
/**
 * \fn		QState BT_AO_State_Active(BT_AO_t * const me, QEvt const * const e)
 *
 * \brief		HSM function: BT ao is active
 *
 * \param  	me : (Input) the BT ao handler here
 * \param  	e : (Input) the event BT ao get
 *
 * \return		the value of state machine function return.
 *
 * \sa		qep.h
 *
 ******************************************************************************************/
QState BT_AO_State_Active(BT_AO_t * const me, QEvt const * const e)
{
	(void)me;
    QState dStatus = 0;
	BtCBKEvt *msg = (BtCBKEvt *)e;

	switch(e->sig)
	{
	case BT_SERVICE_CBK_SIG:
	{
		BtCbkSigHandle(msg);
		dStatus = Q_HANDLED();
		break;
	}
	case BT_COMM_SIG:
	{
		BtCommonSigHandle(msg);
		dStatus = Q_HANDLED();
		break;
	}
	default:
	{
		dStatus = Q_HANDLED();
		break;
	}
	}
    return dStatus;
}


/******************************************************************************************/
/**
 * \fn		QState BT_AO_Initial(BT_AO_t * const me, QEvt const * const e)
 *
 * \brief		HSM function: BT ao Initial func, when AO start, it will be this state fisrt
 *
 * \param  	me : (Input) the BT ao handler here
 * \param  	e : (Input) the event BTao get
 *
 * \return		the value of state machine function return.
 *
 * \sa		qep.h
 *
 ******************************************************************************************/
QState BT_AO_Initial(BT_AO_t * const me, QEvt const * const e)
{
	(void)me; /*!< avoid compiler warning about unused parameter */
    (void)e; /*!< avoid compiler warning about unused parameter */

	/*!< do event subsribe here, and init private variable */
	me->PowerOffFlag = FALSE;
	me->pUserCBK = NULL;
	me->BtUserAction = DO_NONE;
	me->CurMode = MD_MAX;
	me->ApUpgrade.pFunc = NULL;
	me->ApUpgrade.UpGradeFlag = FALSE;
	me->pBtAction = NULL;
	me->stSppDataService.pTransModeAction = NULL;
	me->StandByResFlag = FALSE;
	QEQueue_init(&me->stBtAoQueue, me->pstBtAoQueueSto, Q_DIM(me->pstBtAoQueueSto));
	QTimeEvt_ctorX(&me->timeEvt, &me->super, BT_TIME_CHECK_SIG, 0U);
	QMSM_INIT(&me->stSppDataService.super, (QEvt *)0);
	QActive_subscribe(&me->super, ODSP_MESSAGE_SIG);
	QActive_subscribe(&me->super, ODSP_RESET_START_SIG);
	QActive_subscribe(&me->super, ODSP_RESET_END_SIG);
	//PowerService_RegPowerOffLSR(&me->super, _BtPowerOffStandBy);

	_BtAoConfigSet(me);

	memset(me->Connected_Device_List,0,sizeof(me->Connected_Device_List));
    return Q_TRAN(&BT_AO_State_Idle);
}

/******************************************************************************************/
/**
 * \fn		void BT_AO_Ctor(void)
 *
 * \brief		BT ao constructor
 *
 * \param  	none.
 *
 * \return		none.
 *
 * \note		do active object constructor here.
 *
 ******************************************************************************************/
void BT_AO_Ctor(void)
{
    BT_AO_t * const me = &gstBtAO;
    QActive_ctor(&me->super, Q_STATE_CAST(&BT_AO_Initial));
	TransmitService_Ctor(&me->stSppDataService);
}

/******************************************************************************************/
/**
 * \fn		void BT_AO_Start(void)
 *
 * \brief		create a task to run BT routine.
 *
 * \param  	none
 *
 * \return		none
 *
 * \note		you should use AO_INIT() to regist this functon, so that the system will run this func if QP is ready
 *
 ******************************************************************************************/
void BT_AO_Start(void)
{
	static QEvt const *pstBtAOQueue[BT_AO_Queue_Cnt];

	BT_AO_Ctor(); /*!< instantiate the SysDaemon active objects */
	QACTIVE_START(&gstBtAO.super,           /*!< AO to start */
			  TSK_BTAO_PRIO, 					/*!< QP priority of the AO */
			  pstBtAOQueue,      /*!< event queue storage */
			  Q_DIM(pstBtAOQueue), /*!< queue length [events] */
			  TSK_BTAO_NAME,           /*!< ao name */
			  TSK_BTAO_STACK_DEPTH,                    /*!< size of the stack [bytes] */
			 (QEvt *)0);             /*!< initialization event */
}

#ifndef BUILT4ISP2
AO_INIT(BT_AO_Start);
#endif
#endif //__BT_AO_C__

