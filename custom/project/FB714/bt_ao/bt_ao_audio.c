#ifndef __BT_AO_AUDIO_C__
#define __BT_AO_AUDIO_C__
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
#include "log_utils.h"
#include "std_interfaces.h"
#include "bt_ao_audio.h"
#include "bt_mw_core.h"
//#include "bt_heap_manager.h"
#include "bt_ao_service_api.h"
#include "bt_ao.h"
#include "bt_audio_a2dp.h"


/*---------------------------------------------------------------------------*
 *    MACRO DECLARATIONS
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 *                            GLOBAL VARIABLES                               *
 *---------------------------------------------------------------------------*/
typedef enum
{
	AUDIO_DECODE,
 	AUDIO_ENCODE,
} BtAudioPlayRole_t;


typedef enum AudioState
{
	DECODE_PAUSE,
	DECODE_PLAY,
}AudioState_e;


typedef enum AudioEncState
{
	WSS_PAUSE,
	WSS_PLAY,
}AudioEncState_e;


typedef enum BtAudioSig
{
	BT_AUDIO_CODEC_SIG,//for hintplay use when bintpaly src return back to bt src
	BT_AUDIO_SYNC_SIG,
	BT_AUDIO_ALLOW_SIG,
	BT_AUDIO_MAX_SIG,
}BtAudioSig_e;


typedef enum BtAudioOpcode
{
	BT_AUDIO_CODEC_INIT,
	BT_AUDIO_SPEAKOUT_AND_WATER_DELAY_SET,//speakout  delay && waterlevel set
	BT_AUDIO_ENCODE_PLAY_SYNC,
	BT_AUDIO_SUB_SEND_SYNC_READY,
	BT_AUDIO_ALLOW_IN,
	BT_AUDIO_DISALLOW_IN,
}BtAudioOpcode_e;


typedef struct BtAudioEven
{
	QEvent super;
	BtAudioSig_e  	eType;
	BtAudioOpcode_e eOpcode;
	void			*pData;
}stBtAudioEven;



typedef struct stAudioState
{
	AudioState_e	 BtDecState;
	AudioEncState_e  BtWssEncState;
}BtAudioState;


typedef struct WaterDelaySync
{
	BtDelaySyncInfo DelaySyncInfo;
	BtAddr_t SinkAddr;
}stWaterDelaySync;


typedef struct SpeakDelay
{
	SrcType_e SourceMode;
}stSpeakDelay;


typedef struct EncAudioRole
{
	ENABLE_STATE_e EncWorkState;
	SINK_MODE_e    EncRole;
}stEncAudioRoleMgr;


typedef struct BtAudioInfo
{
	int	PlatformRole;
	int	IsConnectedSetDelay;
	BtAudioState      BtDspAudioState;
	stEncAudioRoleMgr EncAudioRoleInfo;
	QTimeEvt DelayTimeEvt;
}stBtAudioInfo;


stBtAudioInfo   BtAudioInfoMgr =
{
	.PlatformRole = BT_MAIN,
	.BtDspAudioState = {DECODE_PAUSE,WSS_PAUSE},
	.EncAudioRoleInfo = {OFF,SINK_NONE},
};

//static BtAudioState BtDspAudioState = {DECODE_PAUSE,WSS_PAUSE};
//static stEncAudioRoleMgr EncAudioRoleInfo = {OFF,SINK_NONE};
//QTimeEvt DelayTimeEvt;

static int dHintSoundState = 0;
/*---------------------------------------------------------------------------*
 *                            EXTERNAL REFERENCES                            *
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 *                            FUNCTION DECLARATIONS                          *
 *---------------------------------------------------------------------------*/
int BtAudioHandle(QActive *me, QEvt const * const e,SINK_MODE_e role,SrcType_e src);
int BtAudioCodecInit(void);
int BtAudioAllow(int IsTrue);
int BtAudioSetWaterDelay(BtDelaySyncInfo *pDelaySyncInfo);
int BtAudioEncodePlaySync(int IsTure);
int BtDelayEncodePlay(int IsTure);
int BtDelaySendEncodePlayCmd(int IsTure);


BtAudioMgrApi BtAudioMain =
{
	BtAudioHandle,
	BtAudioCodecInit,
	BtAudioAllow,

	.BtAudioAction.MainAudio =
	{
		NULL,//Sunplus//BtAudioSetWaterDelay,
		NULL,//Sunplus//BtAudioEncodePlaySync,
		NULL,//Sunplus//BtDelayEncodePlay,
	},
};


BtAudioMgrApi BtAudioSub =
{
	BtAudioHandle,
	BtAudioCodecInit,
	BtAudioAllow,

	.BtAudioAction.SubAudio=
	{
		NULL,//Sunplus//BtDelaySendEncodePlayCmd,
	},
};
/******************************************************************************************/
/**
 * \fn		int _BtDspStatePrintf(eODSP_MSG eMsg,UINT32 param)
 *
 * \brief		bt ao audio printf dsp cbk state
 * \param	eMsg:eODSP_MSG       param:eODSP_STATUS
 *
 *
 * \return	    	void
 *
 ******************************************************************************************/
static int _BtDspStatePrintf(eODSP_MSG eMsg,UINT32 param)
{
	(void)eMsg;
	(void)param;

	bt_ao_printf("receive ODSP_MESSAGE_SIG: %s param=%s\n"
				, eMsg ==ODSP_MSG_STATE_CHANGE? "STATE_CHANGE ":
				eMsg ==ODSP_ENC_MSG_STATE_CHANGE? "ENC_STATE_CHANGE ": "GHOST"
				, param == ODSP_STATUS_INIT 	? "ODSP_STATUS_INIT    ":
				param == ODSP_STATUS_IDLE	  ? "ODSP_STATUS_IDLE	 ":
				param == ODSP_STATUS_PREPARE  ? "ODSP_STATUS_PREPARE ":
				param == ODSP_STATUS_PREPARE0 ? "ODSP_STATUS_PREPARE0":
				param == ODSP_STATUS_PLAY	  ? "ODSP_STATUS_PLAY	 ":
				param == ODSP_STATUS_PAUSE	  ? "ODSP_STATUS_PAUSE	 ":
				param == ODSP_STATUS_FINAL	  ? "ODSP_STATUS_FINAL	 ":
				param == ODSP_ENC_STATUS_INIT	 ? "ODSP_ENC_STATUS_INIT   ":
				param == ODSP_ENC_STATUS_IDLE	 ? "ODSP_ENC_STATUS_IDLE  ":
				param == ODSP_ENC_STATUS_PREPARE  ? "ODSP_ENC_STATUS_PREPARE ":
				param == ODSP_ENC_STATUS_ENCODE ? "ODSP_ENC_STATUS_ENCODE":
				param == ODSP_ENC_STATUS_PAUSE ? "ODSP_ENC_STATUS_PAUSE": "GHOST");

	return 0;
}
/******************************************************************************************/
/**
 * \fn		stBtAudioInfo* _BtGetAudioInfo(void)
 *
 * \brief		bt ao audio get audio state
 *
 * \param	pState:BtAudioState       role:BtAudioPlayRole_t
 *
 *
 * \return	    	void
 *
 ******************************************************************************************/
static stBtAudioInfo* _BtGetAudioInfo(void)
{
	return &BtAudioInfoMgr;
}
/******************************************************************************************/
/**
 * \fn		int _BtGetAudioState(int *pState,BtAudioPlayRole_t role)
 *
 * \brief		bt ao audio get audio state
 *
 * \param	pState:BtAudioState       role:BtAudioPlayRole_t
 *
 *
 * \return	    	void
 *
 ******************************************************************************************/
static int _BtGetAudioState(int *pState,BtAudioPlayRole_t role)
{
	stBtAudioInfo* pBtAudioInfo = _BtGetAudioInfo();

	if(pBtAudioInfo)
	{
		if(role == AUDIO_ENCODE)
		{
			if(pState != NULL)
			{
				*pState = pBtAudioInfo->BtDspAudioState.BtWssEncState;
				return SUCCESS;
			}

			return FAIL;
		}
		else
		{
			*pState = pBtAudioInfo->BtDspAudioState.BtDecState;
			return SUCCESS;
		}
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		 int _BtSetAudioState(int state,BtAudioPlayRole_t role)
 *
 * \brief		bt ao audio set audio state
 *
 * \param	state:BtAudioState       role:BtAudioPlayRole_t
 *
 *
 * \return	    	tEncAudioRoleMgr *
 *
 ******************************************************************************************/
static int _BtSetAudioState(int state,BtAudioPlayRole_t role)
{
	stBtAudioInfo* pBtAudioInfo = _BtGetAudioInfo();

	if(pBtAudioInfo)
	{
		if(role == AUDIO_ENCODE)
		{
			pBtAudioInfo->BtDspAudioState.BtWssEncState = state;
			return FAIL;
		}
		else
		{
			pBtAudioInfo->BtDspAudioState.BtDecState = state;
			return SUCCESS;
		}
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		stEncAudioRoleMgr *_BtAudioGetEncRoleInfo(void)
 *
 * \brief		bt ao audio get audio encode info
 *
 * \param	void
 *
 *
 * \return	    	tEncAudioRoleMgr *
 *
 ******************************************************************************************/
static stEncAudioRoleMgr *_BtAudioGetEncRoleInfo(void)
{
	stBtAudioInfo* pBtAudioInfo = _BtGetAudioInfo();

	if(pBtAudioInfo)
	{
		return &pBtAudioInfo->EncAudioRoleInfo;
	}

	return NULL;
}
/******************************************************************************************/
/**
 * \fn		int _BtSetAudioRole(SINK_MODE_e role)
 *
 * \brief		bt ao audio set audio role
 *
 * \param	role:tws mode
 *
 *
 * \return	      void
 *
 ******************************************************************************************/
static int _BtSetAudioRole(SINK_MODE_e role)
{
	stBtAudioInfo* pBtAudioInfo = _BtGetAudioInfo();

	if(pBtAudioInfo)
	{
		pBtAudioInfo->EncAudioRoleInfo.EncRole = role;
		return SUCCESS;
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		stBtAudioEven *BtAudioMsgPackage(BtAudioSig_e sig,BtAudioOpcode_e Opcode,void *pData,int DataLen)
 *
 * \brief		bt ao audio msg package
 *
 * \param	sig:BtAudioSig_e    Opcode:BtAudioOpcode_e   pData:data  DataLen: data length
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static stBtAudioEven *BtAudioMsgPackage(BtAudioSig_e sig,BtAudioOpcode_e Opcode,void *pData,int DataLen)
{
	stBtAudioEven *pMsg = NULL;

	pMsg = Q_NEW(stBtAudioEven,BT_AO_AUDIO_SIG);
	pMsg->eType = sig;
	pMsg->eOpcode = Opcode;

	if(pData != NULL && DataLen > 0)
	{
		pMsg->pData = BtMalloc(DataLen);

		if(pMsg->pData)
		{
			memcpy(pMsg->pData,pData,DataLen);
		}
		else
		{
			bt_ao_error_printf("BtMalloc fail!!");
		}
	}
	else
	{
		pMsg->pData = pData;
	}

	return pMsg;
}
/******************************************************************************************/
/**
 * \fn		int _BtSendSinkPause(void)
 *
 * \brief		bt ao audio send sink dev do pause
 *
 * \param	void
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
/*static int _BtSendSinkPause(void)
{
	BtAddr_t Addr = {{0}};
	stEncAudioRoleMgr *pEncRoleInfo = _BtAudioGetEncRoleInfo();

	if(pEncRoleInfo->EncWorkState == ON)
	{
		if(pEncRoleInfo->EncRole == WSS)
		{
			BtServiceGetWssAddr(&Addr);
		}
		else if(pEncRoleInfo->EncRole == HEADPHONE)
		{
			BtServiceGetAutoHeadPhoneAddr(&Addr);
		}
		else
		{
			bt_ao_error_printf("no such role!\n");
			return FAIL;
		}

		_BtSetAudioState(WSS_PAUSE,AUDIO_ENCODE);
		BtServiceSendAudioSync(FALSE,&Addr);

		return SUCCESS;
	}

	return FAIL;
}*/

/******************************************************************************************/
/**
 * \fn		int _BtSendSinkPlay(void)
 *
 * \brief		bt ao audio send sink dev do play
 *
 * \param	void
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int _BtSendSinkPlay(void)
{
	BtAddr_t Addr = {{0}};
	stEncAudioRoleMgr *pEncRoleInfo = _BtAudioGetEncRoleInfo();

	(void)Addr;

	if(pEncRoleInfo->EncWorkState == ON)
	{
		if(pEncRoleInfo->EncRole == WSS)
		{
			//BtServiceGetWssAddr(&Addr);
		}
		else if(pEncRoleInfo->EncRole == HEADPHONE)
		{
			//BtServiceGetAutoHeadPhoneAddr(&Addr);
		}
		else
		{
			bt_ao_error_printf("no such role!\n");
			return FAIL;
		}

		_BtSetAudioState(WSS_PLAY,AUDIO_ENCODE);
		//BtServiceSendAudioSync(TRUE,&Addr);

		return SUCCESS;
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		int BtDspDecStateHandle(QActive *me,eODSP_MSG eMsg,UINT32 param)
 *
 * \brief		bt ao audio dsp decode state cbk handle
 *
 * \param	me:QActive       eMsg:eODSP_MSG    param:eODSP_STATUS
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int BtDspDecStateHandle(QActive *me,eODSP_MSG eMsg,UINT32 param)
{
	(void)eMsg;
	(void)me;
	int DecState = DECODE_PAUSE;

	_BtGetAudioState(&DecState,AUDIO_DECODE);

	switch(param)
	{
	case ODSP_STATUS_IDLE:
		bt_ao_printf(" ODSP_STATUS_IDLE \n");
	case ODSP_STATUS_SEEK:
	{
		bt_ao_printf(" ODSP_STATUS_SEEK \n");

		if(DecState != DECODE_PAUSE)
		{
			_BtSetAudioState(DECODE_PAUSE,AUDIO_DECODE);
		}

		break;
	}
	case ODSP_STATUS_PREPARE:
	{
		_BtSetAudioState(DECODE_PAUSE,AUDIO_DECODE);
		break;
	}
	case ODSP_STATUS_PLAY:
	{
		bt_ao_printf("ODSP_STATUS_PLAY \n");

		if(DecState != DECODE_PLAY)
		{
			_BtSetAudioState(DECODE_PLAY,AUDIO_DECODE);
		}

		break;
	}
	case ODSP_STATUS_PAUSE:
	{
		bt_ao_printf("ODSP_STATUS_PAUSE \n");

		if(DecState != DECODE_PAUSE)
		{
			_BtSetAudioState(DECODE_PAUSE,AUDIO_DECODE);
		}

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
 * \fn		int BtDspEncStateHandle(QActive *me,eODSP_MSG eMsg,UINT32 param)
 *
 * \brief		bt ao audio dsp encode state cbk handle
 *
 * \param	me:QActive       eMsg:eODSP_MSG    param:eODSP_STATUS
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int BtDspEncStateHandle(QActive *me,eODSP_MSG eMsg,UINT32 param,int src)
{
	(void)eMsg;
	(void)me;
	int state = WSS_PAUSE;
	int DecState = DECODE_PAUSE;
	static int PauseSrc = MD_MAX;
	static int PlaySrc = MD_MAX;

	_BtGetAudioState(&state,AUDIO_ENCODE);
	_BtGetAudioState(&DecState,AUDIO_DECODE);

	switch(param)
	{
	case ODSP_ENC_STATUS_PREPARE:
	{
		//_BtSetAudioState(WSS_PAUSE,AUDIO_ENCODE);
		break;
	}
	case ODSP_ENC_STATUS_INIT:
		bt_ao_printf("\n ODSP_ENC_STATUS_INIT \n");
		break;
	case ODSP_ENC_STATUS_IDLE:
	{
		bt_ao_printf("\n ODSP_ENC_STATUS_IDLE \n");

		if(src == MD_SPDIFIN2 || src == MD_SPDIFIN3)
		{
			if(PauseSrc == src)
			{
				if(state != WSS_PAUSE)
				{
					//Sunplus//_BtSendSinkPause();
				}
			}
			else
			{
				PauseSrc = src;
			}

		}
		else
		{
			if(state != WSS_PAUSE)
			{
				//Sunplus//_BtSendSinkPause();
			}
		}

		break;
	}
	case ODSP_ENC_STATUS_ENCODE:
	{
		bt_ao_printf("\n ODSP_ENC_STATUS_ENCODE \n");

		if(src == MD_SPDIFIN2 || src == MD_SPDIFIN3)
		{
			if(PlaySrc == src)
			{
				if(state != WSS_PLAY)
				{
					//Sunplus//_BtSendSinkPlay();
				}
			}
			else
			{
				PlaySrc = src;
			}
		}
		else
		{
			if(state != WSS_PLAY)
			{
				//Sunplus//_BtSendSinkPlay();
			}
		}

		break;
	}
	case ODSP_ENC_STATUS_PAUSE:
	{
		bt_ao_printf("\n ODSP_ENC_STATUS_PAUSE \n");

		if(state != WSS_PAUSE)
		{
			//Sunplus//_BtSendSinkPause();
		}

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
 * \fn		int _BtAudioStateCbkHandle(QActive *me,eODSP_MSG eMsg,UINT32 param)
 *
 * \brief		bt ao audio dsp state cbk handle
 *
 * \param	me:QActive       eMsg:eODSP_MSG    param:eODSP_STATUS
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int _BtAudioStateCbkHandle(QActive *me,eODSP_MSG eMsg,UINT32 param,int src)
{
	static eODSP_STATUS_HINT state = ODSP_STATUS_HINT_OFF;
	stBtAudioInfo* pBtAudioInfo = _BtGetAudioInfo();

	_BtDspStatePrintf(eMsg,param);

	switch(eMsg)
	{
		case ODSP_MSG_PREPARE_END:
		{
			bt_ao_printf("ODSP_MSG_PREPARE_END!");
			break;
		}
		case ODSP_MSG_STATE_CHANGE:
		{
			BtDspDecStateHandle(me,eMsg,param);
			break;
		}
		case ODSP_ENC_MSG_STATE_CHANGE:
		{
			BtDspEncStateHandle(me,eMsg,param,src);
			break;
		}
		case ODSP_MSG_OVER:
			bt_ao_printf(" ODSP_MSG_OVER!");
		case ODSP_MSG_ERR:
			bt_ao_printf("ODSP_MSG_ERR!");
		{
			break;
		}
		case ODSP_ENC_MSG_ERR:
			bt_ao_printf("ODSP_ENC_MSG_ERR!");
	    case ODSP_ENC_MSG_OVER:
		{
			bt_ao_printf("bt ao ODSP_ENC_MSG_OVER!");
			break;
	    }
		case ODSP_ENC_MSG_PREPARE_END:
		{
			bt_ao_printf(" ODSP_MSG_PREPARE_END!");
			break;
		}
		case ODSP_MSG_HINT:
		{
			if(ODSP_STATUS_HINT_ON == param)
			{
				if(pBtAudioInfo->PlatformRole == BT_MAIN && src == MD_BT)
				{
					dHintSoundState = 1;
				}
				else
				{
					if(state != param)
					{
						state = param;
						//BtServiceAllowAudioIn(OFF);
					}
				}
			}
			else if(ODSP_STATUS_HINT_OFF == param)
			{

				if(pBtAudioInfo->PlatformRole == BT_MAIN && src == MD_BT)
				{
					dHintSoundState = 0;
				}
				else
				{
					if(state != param)
					{
						state = param;
						//BtServiceAudioCodecInit();
						//BtServiceAllowAudioIn(ON);
						//BtServiceSubSendSyncReady();
					}
				}
			}

			break;
		}

        case ODSP_HINTSOUND_END_SIG:
        {
            bt_ao_ntc_printf("#### soundhint end sig in bt~~~\n");

            if(pBtAudioInfo->PlatformRole == BT_MAIN)
            {
                //Sunplus//BtServiceAllowAudioDataIn(ON);
            }
            else
            {
                if(state != param)
                {
                    state = param;
                }
            }
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
 * \fn		int _BtAudioSetDelayWater(SrcType_e eMeida ,BtAddr_t *pSinkAddr)
 *
 * \brief		bt ao audio set sub waterlevel (bytes) and speak out delay (ms)
 *
 * \param	eMeida: SrcType_e  pSinkAddr:bt addr(8107sub)
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int _BtAudioSetDelayWater(BtDelaySyncInfo *pDelaySyncInfo ,BtAddr_t *pSinkAddr)
{
	stBtAudioInfo* pBtAudioInfo = _BtGetAudioInfo();
	static SrcType_e dCurSrc = MD_MAX;
	BtAddr_t BtAddr = {{0}};
	UINT16	delay_time =0;
	UINT8 	PackageSize = 144;
	UINT32  sub_water_level = 0;

	if(pDelaySyncInfo == NULL)
	{
		return FAIL;
	}

	delay_time = pDelaySyncInfo->DelayTime;
	sub_water_level = pDelaySyncInfo->WaterLevelPacketNums*PackageSize;

	//bt_ao_ntc_printf("==== src  lats:%d,cur: %d  ==== \n",dCurSrc, pDelaySyncInfo->src);

	if(pDelaySyncInfo->src == MD_NONE)
	{
		if(pBtAudioInfo->IsConnectedSetDelay)
		{
			//bt_ao_ntc_printf("==== delay_time:%d ==== \n",delay_time);
			AudDspService_Set_SpeakerOutDelay(NULL,delay_time);
			dCurSrc = pDelaySyncInfo->src;
		}
	}
	else
	{
		if(dCurSrc != pDelaySyncInfo->src && !pBtAudioInfo->IsConnectedSetDelay)
		{
			//bt_ao_ntc_printf("==== delay_time:%d ==== \n",delay_time);
			AudDspService_Set_SpeakerOutDelay(NULL,delay_time);
			dCurSrc = pDelaySyncInfo->src;
		}
	}

	if(memcmp(&BtAddr,pSinkAddr,sizeof(BtAddr_t)) /*&& BtServiceIsA2dpConnected(pSinkAddr)*/)
	{
		//bt_ao_ntc_printf("===== WssSetWater:%d ====\n",sub_water_level);
		//BtServiceSetWaterLevel(pSinkAddr,sub_water_level);

		if(pBtAudioInfo->IsConnectedSetDelay)
		{
			if(dCurSrc != pDelaySyncInfo->src)
			{
				//bt_ao_ntc_printf("==== delay_time:%d ==== \n",delay_time);
				AudDspService_Set_SpeakerOutDelay(NULL,delay_time);
				dCurSrc = pDelaySyncInfo->src;
			}
		}

		return SUCCESS;
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		int _BtAudioCtrlHandle(QActive *me,stBtAudioEven *pMsg)
 *
 * \brief		bt ao audio control handle
 *
 * \param	me: QActive   pMsg: stBtAudioEven
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int _BtAudioCtrlHandle(QActive *me,stBtAudioEven *pMsg)
{
	(void)pMsg;
	(void)me;

	switch(pMsg->eType)
	{
		case BT_AUDIO_CODEC_SIG:
		{
			if(pMsg->eOpcode == BT_AUDIO_CODEC_INIT)
			{
				//BtServiceAudioCodecInit();
				BTAudio_A2DP_codecInit();
			}
		}
		break;
		case BT_AUDIO_SYNC_SIG:
		{
			if(pMsg->eOpcode == BT_AUDIO_SPEAKOUT_AND_WATER_DELAY_SET)
			{
				stWaterDelaySync *pSysncData = (stWaterDelaySync *)(pMsg->pData);
				stEncAudioRoleMgr *pEncRoleInfo = _BtAudioGetEncRoleInfo();

				if(pEncRoleInfo->EncWorkState != ON || pEncRoleInfo->EncRole != WSS)
				{
					return FAIL;
				}

				_BtAudioSetDelayWater(&pSysncData->DelaySyncInfo,&pSysncData->SinkAddr);
			}
			else if(pMsg->eOpcode == BT_AUDIO_ENCODE_PLAY_SYNC)
			{
				int isAutoEnc = *((int *)pMsg->pData);
				int DecState = DECODE_PAUSE;

				_BtGetAudioState(&DecState,AUDIO_DECODE);

				if(isAutoEnc && DecState == DECODE_PLAY)
				{
					_BtSendSinkPlay();
				}

				//BtAudioIsEncodeAutoSync(isAutoEnc);
				BTAudio_A2DP_Encode_setSync(isAutoEnc);
			}
			else if(pMsg->eOpcode == BT_AUDIO_SUB_SEND_SYNC_READY)
			{
				//BtServiceSubSendSyncReady();
			}

			break;
		}
		case BT_AUDIO_ALLOW_SIG:
		{
			if(pMsg->eOpcode == BT_AUDIO_ALLOW_IN)
			{
				//BtServiceAllowAudioDataIn(ON);
				A2DP_AV_IN_setAllowAudioInFlag(ON);
			}
			else if(pMsg->eOpcode == BT_AUDIO_DISALLOW_IN)
			{
				//BtServiceAllowAudioDataIn(OFF);
				A2DP_AV_IN_setAllowAudioInFlag(OFF);
			}
			else
			{
				bt_ao_error_printf("why go here?please cheack the sig or opcode!\n");
			}

			break;
		}
		default:
		{
			break;
		}

	}

	return SUCCESS;
}
/******************************************************************************************/
/**
 * \fn		int _BtAoAudioCtrl(BtAudioSig_e SigType,BtAudioOpcode_e Opcode,void *pData,int DataLen)
 *
 * \brief		bt ao audio control
 *
 * \param	SigType:signal    Opcode: Opcode   pData:data   DataLen:data len
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
static int _BtAudioCtrl(BtAudioSig_e SigType,BtAudioOpcode_e Opcode,void *pData,int DataLen)
{
	QActive * const me = BtAoGet();
	stBtAudioEven *pMsg = BtAudioMsgPackage(SigType,Opcode,pData,DataLen);
	QACTIVE_POST(me, (QEvt *)pMsg, me);
	return SUCCESS;
}
/******************************************************************************************/
/**
 * \fn		int BtAudioHandle(QActive *me, QEvt const * const e,SINK_MODE_e role)
 *
 * \brief		bt ao audio cbk handle
 *
 * \param	SigType:signal    me: QActive   e:qp sig  role:tws mode
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int BtAudioHandle(QActive *me, QEvt const * const e,SINK_MODE_e role,SrcType_e src)
{

	_BtSetAudioRole(role);

	switch (e->sig)
	{
	case ODSP_MESSAGE_SIG:
	case ODSP_HINTSOUND_END_SIG:
	{
		UINT32 param=((OdspMessageEvt *)e)->dParam;
		eODSP_MSG eMsg=((OdspMessageEvt *)e)->eMsg;

		_BtAudioStateCbkHandle(me,eMsg,param,(int)src);
		break;
	}
	case BT_AO_AUDIO_SIG:
	{
		bt_ao_printf("\n BT_AO_AUDIO_SIG !");
		stBtAudioEven *pMsg = (stBtAudioEven *)e;
		_BtAudioCtrlHandle(me,pMsg);

		if(pMsg->pData)
		{
			BtFree(pMsg->pData);
		}

		break;
	}
	case ODSP_RESET_START_SIG:
	{
		if(src == MD_BT && !dHintSoundState)
		{
			//Sunplus//BtServiceAllowAudioIn(OFF,ON);
		}

		break;
	}
	case ODSP_RESET_END_SIG:
	{
		if(src == MD_BT && !dHintSoundState)
		{
			//Sunplus//BtServiceAllowAudioIn(ON,ON);
		}

		break;
	}
	default:
	{
		bt_ao_error_printf("no such sig!!\n");
		return FAIL;
		break;
	}
	}

	return SUCCESS;
}
/******************************************************************************************/
/**
 * \fn		int BtAudioSetWaterDelay(BtDelaySyncInfo *pDelaySyncInfo)
 *
 * \brief		bt water level and speakout delay set
 *
 * \param	src : app source
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int BtAudioSetWaterDelay(BtDelaySyncInfo *pDelaySyncInfo)
{
	stWaterDelaySync SyncData;

	if(pDelaySyncInfo)
	{
		SyncData.DelaySyncInfo = *pDelaySyncInfo;
		//BtServiceGetWssAddr(&SyncData.SinkAddr);
		return _BtAudioCtrl(BT_AUDIO_SYNC_SIG,BT_AUDIO_SPEAKOUT_AND_WATER_DELAY_SET,&SyncData,sizeof(stWaterDelaySync));
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		int BtAudioSetEncodePlaySync(void)
 *
 * \brief		bt set encode play sync :just olny to send sub sync(play/pause)
 *
 * \param	IsTrue:ture=== play   false=== pause
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int BtAudioEncodePlaySync(int IsAutoEnc)
{
	return _BtAudioCtrl(BT_AUDIO_SYNC_SIG,BT_AUDIO_ENCODE_PLAY_SYNC,&IsAutoEnc,sizeof(int));
}

//BTAudio_A2DP_Result_e BTAudio_A2DP_codecInit(void);

/******************************************************************************************/
/**
 * \fn		int BtAudioCodecInit(void)
 *
 * \brief		bt audio codec init
 *
 * \param	void
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int BtAudioCodecInit(void)
{
	return _BtAudioCtrl(BT_AUDIO_CODEC_SIG,BT_AUDIO_CODEC_INIT,NULL,0);
	//return BTAudio_A2DP_codecInit();//_BtAudioCtrl(BT_AUDIO_CODEC_SIG,BT_AUDIO_CODEC_INIT,NULL,0);
}
/******************************************************************************************/
/**
 * \fn		int BtAudioAllow(int IsTrue)
 *
 * \brief		if bt allow aidio in
 *
 * \param	IsTrue: allow ----TRUE   disallow------FALSE
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int BtAudioAllow(int IsTrue)
{
	if(IsTrue)
	{
		_BtAudioCtrl(BT_AUDIO_ALLOW_SIG,BT_AUDIO_ALLOW_IN,NULL,0);
		//BtServiceAllowAudioDataIn(ON);
		//A2DP_AV_IN_setAllowAudioInFlag(ON);
	}
	else
	{
		_BtAudioCtrl(BT_AUDIO_ALLOW_SIG,BT_AUDIO_DISALLOW_IN,NULL,0);
		//BtServiceAllowAudioDataIn(OFF);
		//A2DP_AV_IN_setAllowAudioInFlag(OFF);
	}

	return SUCCESS;
}
/******************************************************************************************/
/**
 * \fn		int _BtDelaySendEncodePlayCmd(int IsTure)
 *
 * \brief		olny use 8107sub ,delay to send cmd to 8107main which can play encode
 *
 * \param	IsTure:TRUE , FALSE
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int BtDelaySendEncodePlayCmd(int IsTure)
{
	stBtAudioInfo* pBtAudioInfo = _BtGetAudioInfo();
	QTimeEvt *pTimer = &pBtAudioInfo->DelayTimeEvt;

	if(IsTure)
	{
		QTimeEvt_rearm(pTimer, TICKS_PER_500MS);
	}
	else
	{
		_BtAudioCtrl(BT_AUDIO_SYNC_SIG,BT_AUDIO_SUB_SEND_SYNC_READY,NULL,0);
	}

	return SUCCESS;
}
/******************************************************************************************/
/**
 * \fn		int BtDelayEncodePlay(int IsTure)
 *
 * \brief		8107main delay do encode play
 *
 * \param	IsTure:TRUE , FALSE
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int BtDelayEncodePlay(int IsTure)//now just ntc 8107sub do sync
{
	stBtAudioInfo* pBtAudioInfo = _BtGetAudioInfo();

	QTimeEvt *pTimer = &pBtAudioInfo->DelayTimeEvt;

	if(IsTure)
	{
		QTimeEvt_rearm(pTimer, TICKS_PER_200MS*2);
	}
	else
	{
		_BtSendSinkPlay();
	}

	return SUCCESS;
}
/******************************************************************************************/
/**
 * \fn		int BtAudioInit(void **pBtAudioAction)
 *
 * \brief		bt ao audio init
 *
 * \param	pBtAudioAction : audio action
 *
 *
 * \return	    void
 *
 ******************************************************************************************/
int BtAudioInit(BtAudioMgrApi **pBtAudioAction, BtAudioInitInfo *pAudioInfoInit)
{
	QActive *me = BtAoGet();
	stBtAudioInfo* pBtAudioInfo = _BtGetAudioInfo();
	QTimeEvt *pTimer = &pBtAudioInfo->DelayTimeEvt;
	stEncAudioRoleMgr *pEncAudioInfo = &pBtAudioInfo->EncAudioRoleInfo;

	pBtAudioInfo->PlatformRole = pAudioInfoInit->PlatformRole;
	pBtAudioInfo->IsConnectedSetDelay = pAudioInfoInit->IsConnectedSetDelay;

	QTimeEvt_ctorX(pTimer, me, BT_TIME_DELAY_SIG, 0U);

	if(pBtAudioInfo->PlatformRole == BT_MAIN)
	{
		if(pAudioInfoInit->TwsWorkEn == ON)
		{
			pEncAudioInfo->EncWorkState = pAudioInfoInit->TwsWorkEn;
			pEncAudioInfo->EncRole = pAudioInfoInit->SinkMode;
		}
		else
		{
			pEncAudioInfo->EncWorkState = OFF;
			pEncAudioInfo->EncRole = SINK_NONE;
		}

		if(pBtAudioAction)
		{
			*pBtAudioAction = &BtAudioMain;
		}

		return SUCCESS;
	}
	else if(pBtAudioInfo->PlatformRole == BT_SUB)
	{
		pEncAudioInfo->EncWorkState = OFF;
		pEncAudioInfo->EncRole = SINK_NONE;

		if(pBtAudioAction)
		{
			*pBtAudioAction = &BtAudioSub;
		}

		return SUCCESS;
	}

	return FAIL;
}


#endif//__BT_AO_AUDIO_C__

