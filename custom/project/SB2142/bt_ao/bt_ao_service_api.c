#ifndef __BT_AO_SERVICE_C__
#define __BT_AO_SERVICE_C__
#include "bt_ao.h"
#include "bt_ao_service_api.h"
//#include "auddrv.h"
#include "AudDspService.h"
#include "bt_common.h"
//#include "bt_heap_manager.h"
#include "Usersetting.h"
#define LOG_MODULE_ID  ID_BT
#include "log_utils.h"

#define DO_CBK(P_CB, P_CBACK, ...)\
    if (P_CB && P_CB->P_CBACK) {            \
        P_CB->P_CBACK(__VA_ARGS__);         \
    }                                       \
    else {                                  \
        bt_ao_ntc_printf("USER Callback is NULL");  \
    }


static int _CheckBtIsReady(void);
static int _BtAoServiceUserCbkAction(void *pMsg,int SigType);

static BtUserCbkHandle *pCBK_FUNC = NULL;

AO_USER_CBK_FUNC AoUserCbk =
{
	_BtAoServiceUserCbkAction,
};


int BtAoSrvUserCbkRegister(BtUserCbkHandle* pMsgSendFunc)
{
	if(pMsgSendFunc == NULL)
	{
		return FAIL;
	}

	pCBK_FUNC = pMsgSendFunc;
	return AoBtCbkRegister(&AoUserCbk);
}


int BtAoSrvUserCbkDelRegister(BtUserCbkHandle* pMsgSendFunc)
{
	if(pCBK_FUNC == pMsgSendFunc)
	{
		pCBK_FUNC = NULL;
		return AoBtCbkDelRegister(&AoUserCbk);
	}

	return FAIL;
}


QActive *GetBtAo(void)
{
	return BtAoGet();
}


int BtSearchStart(void)
{
	QActive * const me = BtAoGet();
	BtCBKEvt *msg = NULL;

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	if(AoBtIsSearching())
	{
		return FAIL;
	}

	msg = BtMsgPackage(BT_SEARCH_SIG,NULL,0);

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}



int BtSearchStop(void)
{
	QActive * const me = BtAoGet();
	BtCBKEvt *msg = NULL;

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	if(!AoBtIsSearching())
	{
		return FAIL;
	}

	msg = BtMsgPackage(BT_UNSEARCH_SIG,NULL,0);

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}



int BtReplyPincode(BtAddr_t *pBtAddr,char *pPinCode)
{
	QActive * const me = BtAoGet();
	BtCBKEvt *msg = NULL;
	BtReplyPincodeInfo PincodeInfo;

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	PincodeInfo.addr = *pBtAddr;
	memcpy(PincodeInfo.pincode,pPinCode,sizeof(PincodeInfo.pincode));
	msg = BtMsgPackage(BT_REPLY_PINCODE_SIG,&PincodeInfo,sizeof(BtReplyPincodeInfo));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}


int BtSetPincode(char *pPinCode)
{
	QActive * const me = BtAoGet();
	BtCBKEvt *msg = NULL;
	char Pincode[4] = {0};

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	memcpy(Pincode,pPinCode,sizeof(Pincode));
	msg = BtMsgPackage(BT_SET_PINCODE_SIG,Pincode,sizeof(Pincode));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}


int BtPairSart(BtAddr_t *pBtAddr)
{
	(void)pBtAddr;
	QActive * const me = BtAoGet();
	BtCBKEvt *msg = NULL;

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}


	msg = BtMsgPackage(BT_PAIR_SIG,pBtAddr,sizeof(BtAddr_t));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}



int BtUnpair(BtAddr_t *pBtAddr)
{
	(void)pBtAddr;
	QActive * const me = BtAoGet();
	BtCBKEvt *msg = NULL;

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	msg = BtMsgPackage(BT_UNPAIR_SIG,pBtAddr,sizeof(BtAddr_t));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}



int BtA2dpConnect(BtAddr_t *pBtAddr)
{
	(void)pBtAddr;
	QActive * const me = BtAoGet();
	BtCBKEvt *msg = NULL;

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	msg = BtMsgPackage(BT_A2DP_CONNECT_SIG,pBtAddr,sizeof(BtAddr_t));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}

int BtA2dpDisconnect(BtAddr_t *pBtAddr)
{
	(void)pBtAddr;
	QActive * const me = BtAoGet();
	BtCBKEvt *msg = NULL;

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	msg = BtMsgPackage(BT_A2DP_DISCONNECT_SIG,pBtAddr,sizeof(BtAddr_t));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}



int BtSppConnect(BtAddr_t *pBtAddr)
{
	(void)pBtAddr;
	QActive * const me = BtAoGet();
	BtCBKEvt *msg = NULL;

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	msg = BtMsgPackage(BT_SPP_CONNECT_SIG,pBtAddr,sizeof(BtAddr_t));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}

int SPP_IF_Connect_Samsung(BtAddr_t btAddr,unsigned char pin)
{
	QActive * const me = BtAoGet();
	BtCBKEvt *msg = NULL;
	stBtSppConnect_Samsung MsgData;

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}
	memset(&MsgData,0x00,sizeof(stBtSppConnect_Samsung));
	MsgData.BtAddr = btAddr;
	MsgData.pin = pin;
	msg = BtMsgPackage(BT_SPP_CONNECT_SAMSUNG_SIG,&MsgData,sizeof(stBtSppConnect_Samsung));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}





int BtSppDisconnect(BtAddr_t *pBtAddr)
{
	QActive * const me = BtAoGet();
	BtCBKEvt *msg = NULL;

	if((!_CheckBtIsReady()) || (pBtAddr == NULL))
	{
		return FAIL;
	}

	msg = BtMsgPackage(BT_SPP_DISCONNECT_SIG,pBtAddr,sizeof(BtAddr_t));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}

int BtCustomDataSend(BtAddr_t BtAddr,UINT8 *pData, int len)
{
	BtCBKEvt *pMsg = NULL;
	QActive * const me = BtAoGet();
	stBtSppData MsgData;
	stBtSppData *pMsgData = NULL;

	if((!_CheckBtIsReady()))
	{
		return FAIL;
	}

	memset(&MsgData,0x00,sizeof(stBtSppData));

	pMsg = BtMsgPackage(BT_SPP_DATA_OUT_SIG,&MsgData,sizeof(stBtSppData));

	pMsgData = (stBtSppData *)pMsg->pdata;

	if(pMsgData)
	{
		pMsgData->len = len;
		pMsgData->pBtData = BtMalloc(len);

		if(pMsgData->pBtData)
		{
			memcpy(pMsgData->pBtData,pData,len);
			pMsgData->BtAddr = BtAddr;
			QACTIVE_POST(me, (QEvt *)pMsg, me);
			return SUCCESS;
		}
		else
		{
			BtFree(pMsgData);
		}
	}

	return FAIL;
}



int BtRejectConnectSet(BtAddr_t *pBtAddr,ENABLE_STATE_e state)
{
	QActive * const me = BtAoGet();
	BtCBKEvt *msg = NULL;
	BtDenySetInfo DenyInfo;

	if((!_CheckBtIsReady()) || (pBtAddr == NULL))
	{
		return FAIL;
	}

	DenyInfo.addr = *pBtAddr;
	DenyInfo.state = state;

	msg = BtMsgPackage(BT_DENY_CONNECT_SET_SIG,&DenyInfo,sizeof(BtDenySetInfo));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}


int BtStateGet(BT_STATE_e *pstate)
{
	if(pstate == NULL)
	{
		return FAIL;
	}

	AoBtGetState(pstate);
	return SUCCESS;
}



//Sunplus//int BtInit(void)
int BtInitStart(void)
{
	BT_STATE_e BtState = BT_READY;
	QActive * const me = BtAoGet();

	AoBtGetState(&BtState);

	if(BtState==BT_UNREADY)
	{
		BtCBKEvt *msg = NULL;
		msg = BtMsgPackage(BT_INIT_SIG,NULL,0);

		if(msg != NULL)
		{
			QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
			return SUCCESS;
		}
	}

	return FAIL;
}



int BtOn(void)
{
	BT_STATE_e BtState = BT_READY;
	QActive * const me = BtAoGet();

	AoBtGetState(&BtState);

	if(BtState==BT_UNREADY ||BtState==BT_INIT)
	{
		if(BtState==BT_UNREADY)
		{
			BtCBKEvt *msg = NULL;
			msg = BtMsgPackage(BT_INIT_SIG,NULL,0);

			if(msg != NULL)
			{
				QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
			}
		}

		BtCBKEvt *msg1 = NULL;
		msg1 = BtMsgPackage(BT_ENABLE_SIG,NULL,0);

		if(msg1 != NULL)
		{
			QACTIVE_POST(me, (QEvt *)msg1, (void*)NULL);
		}
	}
	else
	{
		return FAIL;
	}

	return SUCCESS;
}


int BtOff(void)
{
	BT_STATE_e BtState = BT_UNREADY;
	QActive * const me = BtAoGet();

	AoBtGetState(&BtState);

	if(BtState == BT_READY || BtState==BT_INIT)
	{
		if(BtState == BT_READY)
		{
			BtCBKEvt *msg = NULL;
			msg = BtMsgPackage(BT_DISABLE_SIG,NULL,0);

			if(msg != NULL)
			{
				QACTIVE_POST(me,(QEvt *)msg, (void*)0);
			}
			else
			{
				return FAIL;
			}
		}

		BtCBKEvt *msg = NULL;
		msg = BtMsgPackage(BT_DELINIT_SIG,NULL,0);

		if(msg != NULL)
		{
			QACTIVE_POST(me,(QEvt *)msg, (void*)0);
		}
		else
		{
			return FAIL;
		}
	}

	return SUCCESS;
}




int BtUdtTestOn(void)
{
	BT_STATE_e BtState = BT_UNREADY;
	QActive * const me = BtAoGet();

	AoBtGetState(&BtState);

	if(_CheckBtIsReady())
	{
		BtCBKEvt *msg = NULL;
		msg = BtMsgPackage(BT_DUTTEST_ON_SIG,NULL,0);

		if(msg != NULL)
		{
			QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
			return SUCCESS;
		}
	}

	return FAIL;
}




int BtRfPowerSet(DBM_LEVEL_e DbmValue)
{
	(void)DbmValue;
	QActive * const me = BtAoGet();
	BtCBKEvt *msg = NULL;

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	msg = BtMsgPackage(BT_POWER_DBM_SET_SIG,&DbmValue,sizeof(DBM_LEVEL_e));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}



int BtPlay(void)
{
	BtCBKEvt *msg = NULL;
	QActive * const me = BtAoGet();

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	msg = BtMsgPackage(BT_PLAY_SIG,NULL,0);

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}



int BtPause(void)
{
	BtCBKEvt *msg = NULL;
	QActive * const me = BtAoGet();

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	msg = BtMsgPackage(BT_PAUSE_SIG,NULL,0);

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}



int BtNext(void)
{
	BtCBKEvt *msg = NULL;
	QActive * const me = BtAoGet();

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	msg = BtMsgPackage(BT_NEXT_SIG,NULL,0);

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}



int BtPrev(void)
{
	BtCBKEvt *msg = NULL;
	QActive * const me = BtAoGet();

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	msg = BtMsgPackage(BT_PREV_SIG,NULL,0);

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}



int BtWssPlay(void)
{
	BtCBKEvt *msg = NULL;
	BtAddr_t addr = {{0}};
	BT_PROFILE_STATE_e state = DISCONNECT_OK;
	QActive * const me = BtAoGet();

	if(_CheckBtIsReady())
	{
		if(AoGetTwsState() != ON)
		{
			return FAIL;
		}
	}
	else
	{
		return FAIL;
	}

	AoBtGetAddrByDevRole(&addr,WSS_ADDR);
	AoBtGetProfileState(&addr,AVDTP,&state);

	if(state != CONNECT_OK)
	{
		return FAIL;
	}

	msg = BtMsgPackage(BT_WSS_PLAY_SIG,NULL,0);

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}


int BtWssPause(void)
{
	BtCBKEvt *msg = NULL;
	BtAddr_t addr = {{0}};
	BT_PROFILE_STATE_e state = DISCONNECT_OK;
	QActive * const me = BtAoGet();

	if(_CheckBtIsReady())
	{
		if(AoGetTwsState() != ON)
		{
			return FAIL;
		}
	}
	else
	{
		return FAIL;
	}

	AoBtGetAddrByDevRole(&addr,WSS_ADDR);
	AoBtGetProfileState(&addr,AVDTP,&state);
	if(state != CONNECT_OK)
	{
		return FAIL;
	}

	msg = BtMsgPackage(BT_WSS_PAUSE_SIG,NULL,0);

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}



int BtSetVolumeSync(int volume, DEVROLE_DEX_e role)
{
	BtCBKEvt *msg = NULL;
	BtVolumeSet VolumeSetInfo ={{{0}},0};
	BT_PROFILE_STATE_e state = DISCONNECT_OK;
	QActive * const me = BtAoGet();

	if(_CheckBtIsReady())
	{
		if(AoGetTwsState() != ON)
		{
			return FAIL;
		}
	}
	else
	{
		return FAIL;
	}

	VolumeSetInfo.volume = volume;
	AoBtGetAddrByDevRole(&VolumeSetInfo.addr,role);

	if(role == PHONE_ADDR)
	{
		AoBtGetProfileState(&VolumeSetInfo.addr,AVDTP,&state);
	}
	else if(role == HEADPHONE_ADDR)
	{
		AoBtGetProfileState(&VolumeSetInfo.addr,AVDTP,&state);
	}

	if(state != CONNECT_OK)
	{
		return FAIL;
	}

	msg = BtMsgPackage(BT_VOLUME_CHANGE_SIG,&VolumeSetInfo,sizeof(BtVolumeSet));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}



int BtAutoConnect(AUTOCONN_DEX_e role)
{
	BtCBKEvt *msg = NULL;
	QActive * const me = BtAoGet();

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	msg = BtMsgPackage(BT_AUTOCONNECT_ENABLE_SIG,&role,sizeof(DEVROLE_DEX_e));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}

int BtCancelAutoConnect(AUTOCONN_DEX_e role)
{
	QActive * const me = BtAoGet();
	BtCBKEvt *msg = NULL;

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	msg = BtMsgPackage(BT_AUTOCONNECT_DISABLE_SIG,&role,sizeof(DEVROLE_DEX_e));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}



int BtModeOn(void)
{
	QActive * const me = BtAoGet();

	BtCBKEvt *msg = NULL;
	msg = BtMsgPackage(BT_MODE_ON,NULL,0);

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}

int BtModeOff(void)
{
	QActive * const me = BtAoGet();
	BtCBKEvt *msg = NULL;

	msg = BtMsgPackage(BT_MODE_OFF,NULL,0);

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}


int BtTransModeSet(ENABLE_STATE_e state)
{
	BtCBKEvt *msg = NULL;
	QActive * const me = BtAoGet();
	ENABLE_STATE_e CurState = OFF;

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	CurState = (ENABLE_STATE_e)AoGetTransModeState();

	if(CurState == state)
	{
		return FAIL;
	}

	if(state == ON)
	{
		msg = BtMsgPackage(BT_TRANSMIT_ON_SIG,NULL,0);
	}
	else
	{
		msg = BtMsgPackage(BT_TRANSMIT_OFF_SIG,NULL,0);
	}

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}


int BtLocalNameGet(char *pName)
{
	if(pName == NULL)
	{
		return	FAIL;
	}

	AoBtLocalNameGet(pName);
	return	SUCCESS;
}


int BtLocalNameSet(char *pName,int len)
{
	BtCBKEvt *msg = NULL;
	QActive * const me = BtAoGet();

	if(pName == NULL || !_CheckBtIsReady())
	{
		return FAIL;
	}


	msg = BtMsgPackage(BT_SET_BT_NAME_SIG,pName,len);

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}

int BASE_IF_LmpNameResChange(char *pName)
{
	BtCBKEvt *msg = NULL;
	QActive * const me = BtAoGet();

	if(pName == NULL || !_CheckBtIsReady())
	{
		return FAIL;
	}

	msg = BtMsgPackage(BT_SET_BT_LMP_NAME_SIG,pName,strlen(pName));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}

int BASE_IF_WriteDevClass(unsigned int devclass)
{
	BtCBKEvt *msg = NULL;
	QActive * const me = BtAoGet();

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}


	msg = BtMsgPackage(BT_SET_BT_DEVICE_CLASS_SIG,&devclass,sizeof(unsigned int));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}

int BASE_IF_SDP_ServiceSearch_DID(BtAddr_t address)
{
	BtCBKEvt *msg = NULL;
	QActive * const me = BtAoGet();

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}


	msg = BtMsgPackage(BT_SDP_DID_REQUEST_SIG,&address,sizeof(BtAddr_t));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}




int BtLocalAddrGet(BtAddr_t *pAddr)
{
	if(pAddr == NULL)
	{
		return FAIL;
	}

	AoBtLocalAddrGet(pAddr);
	return SUCCESS;
}


int BtLocalAddrSet(BtAddr_t *pAddr)
{
	BtCBKEvt *msg = NULL;
	QActive * const me = BtAoGet();

	if(pAddr == NULL || !_CheckBtIsReady())
	{
		return FAIL;
	}


	msg = BtMsgPackage(BT_SET_BT_ADDR_SIG,pAddr,sizeof(BtAddr_t));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}



int BtGetLocalInfo(BtLocalInfo *devinfo)
{
	(void)devinfo;

	if(devinfo!=NULL)
	{
		AoBtGetLocalInfo(devinfo);
		return SUCCESS;
	}

	return FAIL;
}


//maybe changed by msg post to user
int BtIsPlay(void)
{
	return AoBtIsPlay();
}



int BtSinkModeSwitch(SINK_MODE_e mode)
{
	QActive * const me = BtAoGet();
	BtCBKEvt *msg = NULL;

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	msg = BtMsgPackage(BT_SWITCH_MODE_SIG,&mode,sizeof(SINK_MODE_e));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}


int BtScanModeSet(SCAN_MODE_e state)
{
	BtCBKEvt *msg = NULL;
	QActive * const me = BtAoGet();

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	msg = BtMsgPackage(BT_SET_SCAN_MODE_SIG,&state,sizeof(SCAN_MODE_e));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}


int BtScanModeGet(SCAN_MODE_e *pstate)
{
	if(!_CheckBtIsReady() || pstate == NULL)
	{
		return FAIL;
	}

	return AoBtScanModeGet(pstate);
}


/*****************************************************************************
Function Name: Music_IF_Volume_Sync_Samsung
Purposes	 :  set absolute volume
Description  :
Arguments	 :
	[In] volume: volume value
	[In] ismute: mute state
	[In] vol_mute: 0x00-set mute; 0x01-set volume
Return Value :
See also :
		None
*******************************************************************************/
int Music_IF_Volume_Sync_Samsung(BtAddr_t *pAddr,UINT_8 Volume,UINT_8 ismute,UINT_8 vol_mute)
{
	BtCBKEvt *msg = NULL;
	Btavrcp_volume_samsung_type sMsg;
	//bt_printf("%s [%d][%d]!\n",__FUNCTION__,Volume,ismute);
	QActive * const me = BtAoGet();

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}
	sMsg.addr = *pAddr;
	sMsg.volume = Volume;
	sMsg.mute = ismute;
	sMsg.vol_mute_set = vol_mute;

	msg = BtMsgPackage(BT_AVRCP_MUSIC_VOLUME_SYNC_SAMSUNG,&sMsg, sizeof(Btavrcp_volume_samsung_type));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;

}


int BtSetWssDelayLevel(BtDelaySyncInfo DelaySyncInfo)
{
	BtCBKEvt *msg = NULL;
	QActive * const me = BtAoGet();

	msg = BtMsgPackage(BT_SET_SUB_WATERLEVEL_SIG,&DelaySyncInfo,sizeof(BtDelaySyncInfo));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}

int BtSetAppCurrentSource(SrcType_e src)
{
	BtCBKEvt *msg = NULL;
	QActive * const me = BtAoGet();

	msg = BtMsgPackage(BT_SETAPPCURRENTSOURCE,&src,sizeof(SrcType_e));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}




int BtResetAudioCodec(void)
{
	BtCBKEvt *msg = NULL;
	QActive * const me = BtAoGet();

	msg = BtMsgPackage(BT_AUDIO_CODEC_RESET_SIG,NULL,0);

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}


int BtGetProfileState(BtAddr_t *pBtAddr,BT_PROFILE_e profile,BT_PROFILE_STATE_e *pstate)
{
	if(!_CheckBtIsReady())
	{
		return FAIL;
	}


	AoBtGetProfileState(pBtAddr,profile,pstate);
	return SUCCESS;
}


int BtCheckAllDeviceProfileState(Profile_Name_e profile,BT_PROFILE_STATE_e *pstate)
{
	if(!_CheckBtIsReady())
	{
		return FAIL;
	}


	Connected_Device *pConnected_Device_List = Connected_Device_List_Get();

	if(pConnected_Device_List == NULL)
	{
		return FAIL;
	}

	int i;
	for(i=0;i<Connected_Device_Max;i++)
	{
		if(pConnected_Device_List[i].profile_state & 1<<profile)
		{
			*pstate = CONNECT_OK;
			break;
		}
	}

	if(i == Connected_Device_Max)
	{
		*pstate = DISCONNECT_OK;
	}

	return SUCCESS;
}


int BtGetAddrByDevRole(BtAddr_t *pBtAddr,DEVROLE_DEX_e dex)
{
	if((!_CheckBtIsReady()) || (pBtAddr==NULL))
	{
		return FAIL;
	}

	AoBtGetAddrByDevRole(pBtAddr,dex);
	return SUCCESS;
}


int BtAllowAudioDataIn(int dOnOff)
{
	return BtAoAudioAllow(dOnOff);
}


int BtGetConnectStateByDevRole(DEVROLE_DEX_e dex,BT_PROFILE_STATE_e *pState)
{
	if((!_CheckBtIsReady()) || (pState==NULL))
	{
		return FAIL;
	}

	AoBtGetConnectStateByDevRole(dex,pState);
	return SUCCESS;
}


int BtGetBondDevNums(int *pBondedNum)
{
	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	if(pBondedNum==NULL)
	{
		return FAIL;
	}

	int res = AoBtGetBondDevNum(pBondedNum);
	return res;
}


int BtGetBondDevInfo(int sdBondedNum,stBtDevInfo *pBtDev)
{
	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	if(pBtDev==NULL)
	{
		return FAIL;
	}

	int res = AoBtGetBondDevByIdex(sdBondedNum,pBtDev);
	return res;
}



int BtGetRemoteDevInfo(BtAddr_t *pBtAddr,stBtDevInfo* pDevInfo)
{
	LOGD("[%s] call in\n",__FUNCTION__);
	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	if(pBtAddr==NULL || pDevInfo==NULL)
	{
		return FAIL;
	}

	int i;
	BTPairDev Saved_PairInfo[BT_DEVICE_NUM];

	UserSetting_ReadAllPairDev(Saved_PairInfo);

	for(i=0;i<BT_DEVICE_NUM;i++)
	{
		if(memcmp(pBtAddr,&Saved_PairInfo[i].bd_addr,sizeof(BtAddr_t))==0)
		{
			LOGD("find dst addr...i:%d...\n",i);
			break;
		}
	}

	if(i==BT_DEVICE_NUM)
	{
		LOGD("cant not find dst addr...\n");
		return FAIL;
	}

	memcpy(&pDevInfo->bd_addr,&Saved_PairInfo[i].bd_addr,ADDR_LENGTH);
	memcpy(pDevInfo->name,Saved_PairInfo[i].name,Saved_PairInfo[i].name_len);
	pDevInfo->name_len = Saved_PairInfo[i].name_len;
	//memcpy(pDevInfo,&Saved_PairInfo[i],sizeof(stBtDevInfo));

	LOGD("Device Info:\nAddress:");
	BLOCKPRINTF(&pDevInfo->bd_addr,ADDR_LENGTH);
	LOGD("Name:%s\n",pDevInfo->name);
	LOGD("Name_Len:%s\n",pDevInfo->name_len);

	return SUCCESS;
	/*int res = AoBtGetRemoteDevInfo(pBtAddr,pDevInfo);
	return res;*/
}

int BtShowAllRemoteDevInfo(void)
{
	stBtDevInfo BtDev;
	int DevNum = 0;
	int res = FAIL;
	int i = 0,j = 0;

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	res = AoBtGetRemoteDevNum(&DevNum);

	for(i=1;i<=DevNum;i++)
	{
		AoBtGetRemoteDevByIdex(i,&BtDev);
		for(j=0;j<6;j++)
			bt_ao_ntc_printf("AllRemoteDevInfo[%d]:addr:%x,\t",j,BtDev.bd_addr.address[j]);

		bt_ao_ntc_printf("name:%s, role:%d, constate:%d, pairstate:%d \n",BtDev.name,BtDev.DevRole,BtDev.BtConnState,BtDev.BtPairState);
	}

	return res;
}



static int BtSendData(BtAddr_t *pBtAddr,char *pFlag,char *pBuf, UINT32 len)
{
	BtCBKEvt *pMsg = NULL;
	stBtData *pSendData = NULL;
	stBtData SendData ;
	Spp_Send_Data_If data;
	QActive * const me = BtAoGet();

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	SendData.BtAddr = *pBtAddr;
	SendData.len = sizeof(Spp_Send_Data_If);
	SendData.pBtData = BtZoreMalloc(SendData.len);

	if(SendData.pBtData == NULL)
	{
		return FAIL;
	}

	pMsg = BtMsgPackage(BT_SEND_DATA_SIG,&SendData,sizeof(SendData));//may be will change
	pSendData = (stBtData *)pMsg->pdata;

	memcpy(&data.Type_Flag,pFlag,sizeof(AppSendData_Type_Flag));
	data.payload_len = len;
	data.pdatabuf = BtZoreMalloc(len);

	if(data.pdatabuf == NULL)
	{
		BtFree(SendData.pBtData);
		return FAIL;
	}

	memcpy(data.pdatabuf,pBuf,sizeof(len));
	memcpy(pSendData->pBtData,&data,sizeof(Spp_Send_Data_If));

	if(pMsg != NULL)
	{
		bt_ao_printf("%s:QF_PUBLISH\n",__FUNCTION__);
		QACTIVE_POST(me, (QEvt *)pMsg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}



int BtPlatformApUpgradeInit(int (*pBtApUpgradeCbk)(void))
{
	QActive * const me = BtAoGet();
	BtCBKEvt *msg = NULL;
	BT_STATE_e state = BT_UNREADY;

	BtStateGet(&state);

	if(pBtApUpgradeCbk == NULL)
	{
		bt_ao_warn_printf("FunCbk == NULL \n");
		return FAIL;
	}

	msg = BtMsgPackage(BT_AP_UPGRADE_INIT,&pBtApUpgradeCbk,sizeof(pBtApUpgradeCbk));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, NULL);
		return SUCCESS;
	}

	return FAIL;
}



int BtAPSendUpgradeData(void * pData, UINT32 dLength, UINT8 isAllData)
{
	int res = FAIL;
	AppSendData_Type_Flag wTypeFlag;
	BtAddr_t BtAddr = {{0}};
	BT_PROFILE_STATE_e state = DISCONNECT_OK;

	if((!_CheckBtIsReady()) || (AoGetTransModeState() != ON))
	{
			return FAIL;
	}

	wTypeFlag.func = PAYLOAD_DATA;
	wTypeFlag.all_data_send = isAllData;
	wTypeFlag.Error_check = 1;
	wTypeFlag.value = TYPE_UPGRADE_DATA_IN;

	AoBtGetAddrByDevRole(&BtAddr,WSS_ADDR);
	AoBtGetProfileState(&BtAddr,SPP,&state);

	if(state != CONNECT_OK)
	{
		return FAIL;
	}
	bt_ao_printf("%s:BtSendData\n",__FUNCTION__);

	res = BtSendData(&BtAddr,(char *)&wTypeFlag, pData, dLength);

	if(res == SUCCESS)
	{
		return SUCCESS;
	}

	return FAIL;
}



int BtAPSendData(TransFuncType bCmd, void * pData, UINT32 dLength, UINT8 isAllData)
{
	int res = FAIL;
	AppSendData_Type_Flag wTypeFlag;
	BtAddr_t BtAddr = {{0}};
	BT_PROFILE_STATE_e state = DISCONNECT_OK;

	if((!_CheckBtIsReady()) || (AoGetTransModeState() != ON))
	{
		return FAIL;
	}

	wTypeFlag.func = PAYLOAD_DATA;
	wTypeFlag.all_data_send = isAllData;
	wTypeFlag.Error_check = 1;
	wTypeFlag.value = bCmd;

	AoBtGetAddrByDevRole(&BtAddr,REMOTEAPP_ADDR);
	AoBtGetProfileState(&BtAddr,AVDTP,&state);

	if(state != CONNECT_OK)
	{
		return FAIL;
	}

	res = BtSendData(&BtAddr,(char *)&wTypeFlag, pData, dLength);

	if(res == SUCCESS)
	{
		return SUCCESS;
	}

	return FAIL;
}



int BTAPSendCmd(TransCmdType bCmd)
{
	int res = FAIL;
	AppSendData_Type_Flag wTypeFlag;
	BtAddr_t BtAddr = {{0}};
	BT_PROFILE_STATE_e state = DISCONNECT_OK;

	if((!_CheckBtIsReady()) || (AoGetTransModeState() != ON))
	{
		return FAIL;
	}

	wTypeFlag.func = PAYLOAD_CMD;
	wTypeFlag.all_data_send = 1;
	wTypeFlag.Error_check = 0;
	wTypeFlag.value = bCmd;

	AoBtGetAddrByDevRole(&BtAddr,REMOTEAPP_ADDR);
	AoBtGetProfileState(&BtAddr,AVDTP,&state);

	if(state != CONNECT_OK)
	{
		return FAIL;
	}

	res = BtSendData(&BtAddr,(char *)&wTypeFlag,NULL,0);

	if(res == SUCCESS)
	{
		return SUCCESS;
	}

	return FAIL;
}


//when usb check sub bin should call this function
int BtUpGradeSub(void)
{
	BtCBKEvt *pMsg = NULL;
	QActive * const me = BtAoGet();

	if((!_CheckBtIsReady()) || (AoGetTransModeState() != ON))
	{
		return FAIL;
	}

	pMsg = BtMsgPackage(BT_UPGRADE_SUB_SIG,NULL,0);

	if(pMsg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)pMsg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}



int BtSendCustomData(BtAddr_t BtAddr,UINT8 *pData, int len)
{
	BtCBKEvt *pMsg = NULL;
	QActive * const me = BtAoGet();
	stBtData MsgData;
	stBtData *pMsgData = NULL;

	if((!_CheckBtIsReady()) || (AoGetTransModeState() != ON))
	{
		return FAIL;
	}

	if(pData)
	{
		int DataLen = strlen(pData);

		if(DataLen<len)
			len = DataLen;
	}
	else
	{
		return FAIL;
	}

	memset(&MsgData,0x00,sizeof(stBtData));

	pMsg = BtMsgPackage(BT_SEND_CUSTOM_DATA_SIG,&MsgData,sizeof(stBtData));

	pMsgData = (stBtData *)pMsg->pdata;

	if(pMsgData)
	{
		pMsgData->len = len;
		pMsgData->pBtData = BtMalloc(len);

		if(pMsgData->pBtData)
		{
			memcpy(pMsgData->pBtData,pData,len);
			pMsgData->BtAddr = BtAddr;
			QACTIVE_POST(me, (QEvt *)pMsg, me);
			return SUCCESS;
		}
		else
		{
			BtFree(pMsgData);
		}
	}

	return FAIL;
}


int BtSubStandby(ENABLE_STATE_e state)
{
	BtCBKEvt *pMsg = NULL;
	QActive * const me = BtAoGet();

	if(state == ON)
	{
		pMsg = BtMsgPackage(BT_STANDBY_ON_SIG,NULL,0);
	}
	else
	{
		pMsg = BtMsgPackage(BT_STANDBY_OFF_SIG,NULL,0);
	}

	if(pMsg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)pMsg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}




static int _CheckBtIsReady(void)
{
	BT_STATE_e BtState = BT_UNREADY;

	AoBtGetState(&BtState);

	if(BtState == BT_READY)
	{
		return TRUE;
	}

	return FALSE;
}


int BtAddBondDev(BTPairDev *stPairDev)
{
	QActive * const me = BtAoGet();
	BtCBKEvt *msg = NULL;

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}
    //(void)stPairDev;
	msg = BtMsgPackage(BT_ADDBONDDEV_SIG,stPairDev,sizeof(BTPairDev));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;

}




int BtClearAllBondDev(void)
{

	LOGD("%s call in\n",__FUNCTION__);

	UserSetting_ClearAllPairDev();
	UserSetting_ClearBtAutoConDev();

	QActive * const me = BtAoGet();
	BtCBKEvt *msg = NULL;

	if(!_CheckBtIsReady())
	{
		return FAIL;
	}

	msg = BtMsgPackage(BT_CLEARBONDDEV_SIG,NULL,0);

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}

int BtGetRemoteDeviceRssi(BtAddr_t *pBtAddr)
{
	QActive * const me = BtAoGet();
	BtCBKEvt *msg = NULL;

	if((!_CheckBtIsReady()) || (pBtAddr == NULL))
	{
		return FAIL;
	}

	msg = BtMsgPackage(BT_GET_REMOTE_DEVICE_RSSI,pBtAddr,sizeof(BtAddr_t));

	if(msg != NULL)
	{
		QACTIVE_POST(me, (QEvt *)msg, (void*)NULL);
		return SUCCESS;
	}

	return FAIL;
}

static int _BtAoServiceUserCbkAction(void *pMsg,int SigType)
{
	int MsgType = -1;

	switch(SigType)
	{
	case BT_READY_SIG:
	{
		MsgType = BT_CBK_READY_SIG;
		break;
	}
	case BT_UNREADY_SIG:
	{
		MsgType = BT_CBK_UNREADY_SIG;
		break;
	}
	case BT_TWS_SWITCH_FINISH_SIG:
	{
		MsgType = BT_CBK_TWS_SWITCH_OK_SIG;
		break;
	}
	case BT_SEARCH_RESULT_SIG:
	{
		MsgType = BT_CBK_SEARCH_RESULT_SIG;
		break;
	}
	case BT_SEARCH_FINISH_SIG:
	{
		MsgType = BT_CBK_SEARCH_FINISH_SIG;
		break;
	}
	case BT_ACL_CONNECT_SIG:
	{
		MsgType = BT_CBK_ACL_CONNECT_SIG;
		break;
	}
	case BT_PAIRING_IND_SIG:
	{
		MsgType = BT_CBK_PAIRING_IND_SIG;
		break;
	}
	case BT_PAIRED_OK_SIG:
	{
		MsgType = BT_CBK_PAIRED_OK_SIG;
		break;
	}
	case BT_UNPAIRED_OK_SIG:
	{
		MsgType = BT_CBK_UNPAIRED_OK_SIG;
		break;
	}
	case BT_A2DP_CONNECT_ING_SIG:
	{
		MsgType = BT_CBK_A2DP_CONNECT_ING_SIG;
		break;
	}
	case BT_A2DP_CONNECT_FAIL_SIG:
	{
		MsgType = BT_CBK_A2DP_CONNECT_FAIL_SIG;
		break;
	}
	case BT_A2DP_CONNECT_SUCCESS_SIG:
	{
		MsgType = BT_CBK_A2DP_CONNECT_SUCCESS_SIG;
		break;
	}
	case BT_A2DP_DISCONNECT_ING_SIG:
	{
		break;
	}
	case BT_A2DP_DISCONNECT_FAIL_SIG:
	{
		MsgType = BT_CBK_A2DP_DISCONNECT_FAIL_SIG;
		break;
	}
	case BT_A2DP_DISCONNECT_SUCCESS_SIG:
	{
		MsgType = BT_CBK_A2DP_DISCONNECT_SUCCESS_SIG;
		break;
	}
	case BT_SPP_CONNECT_FAIL_SIG:
	{
		MsgType = BT_CBK_SPP_CONNECT_FAIL_SIG;
		break;
	}
	case BT_SPP_CONNECT_SUCCESS_SIG:
	{
		MsgType = BT_CBK_SPP_CONNECT_SUCCESS_SIG;
		break;
	}
	case BT_SPP_DISCONNECT_SUCCESS_SIG:
	{
		MsgType = BT_CBK_SPP_DISCONNECT_SUCCESS_SIG;
		break;
	}
	case BT_SPP_DISCONNECT_FAIL_SIG:
	{
		MsgType = BT_CBK_SPP_DISCONNECT_FAIL_SIG;
		break;
	}
	case BT_SPP_DATA_IN_SIG:
	{
		MsgType = BT_CBK_SPP_DATA_IN_SIG;
		break;
	}
	case BT_DEV_CONNECT_TIMEOUT_SIG:
	{
		MsgType = BT_CBK_DEV_CONNECT_TIMEOUT_SIG;
		break;
	}
	case BT_AVRCP_CONNECT_SUCCESS_SIG:
	{
		MsgType = BT_CBK_AVRCP_CONNECTOK_SIG;
		break;
	}
	case BT_AVRCP_DISCONNECT_SUCCESS_SIG:
	{
		MsgType = BT_CBK_AVRCP_DISCONNECTOK_SIG;
		break;
	}
	case BT_VOLUME_SET_REQ_SIG:
	{
		MsgType = BT_CBK_VOLUME_SET_REQ_SIG;
		break;
	}
	case BT_VOLUME_REG_REQ_SIG:
	{
		MsgType = BT_CBK_VOLUME_REG_SIG;
		break;
	}
	case BT_SAMSUNG_TV_FLAG_SIG:
	{
		MsgType = BT_CBK_SAMSUNG_TV_FLAG_SIG;
		break;
	}
	case BT_NEXT_PLAY_REQ_SIG:
	{
		MsgType = BT_CBK_NEXT_PLAY_REQ_SIG;
		break;
	}
	case BT_PREV_PLAY_REQ_SIG:
	{
		MsgType = BT_CBK_PREV_PLAY_REQ_SIG;
		break;
	}
	case BT_PLAY_REQ_SIG:
	{
		MsgType = BT_CBK_PLAY_REQ_SIG;
		break;
	}
	case BT_PAUSE_REQ_SIG:
	{
		MsgType = BT_CBK_PAUSE_REQ_SIG;
		break;
	}
	case BT_POWER_REQ_SIG:
	{
		MsgType = BT_CBK_POWER_REQ_SIG;
		break;
	}
	case BT_VOLUMEUP_REQ_SIG:
	{
		MsgType = BT_CBK_VOLUMEUP_REQ_SIG;
		break;
	}
	case BT_VOLUMEDOWN_REQ_SIG:
	{
		MsgType = BT_CBK_VOLUMEDOWN_REQ_SIG;
		break;
	}
	case BT_MUTE_REQ_SIG:
	{
		MsgType = BT_CBK_MUTE_REQ_SIG;
		break;
	}
	case BT_SOURCE_STATE_UPDATE_SIG:
	{
		MsgType = BT_CBK_SOURCE_STATE_UPDATE_SIG;
		break;
	}
	case BT_CUSTOM_DATA_IN_SIG:
	{
		MsgType = BT_CBK_CUSTOM_DATA_IN_SIG;
		break;
	}
	case BT_SDP_DID_DATA_IN_SIG:
	{
		MsgType = BT_CBK_SDP_DID_DATA_IN_SIG;
		break;
	}
	case BT_REMOTE_DEVICE_NAME_SIG:
	{
		MsgType = BT_CBK_REMOTE_DEVICE_NAME_SIG;
		break;
	}
	case BT_REMOTE_DEVICE_RSSI_SIG:
	{
		MsgType = BT_CBK_REMOTE_DEVICE_RSSI_SIG;
		break;
	}
	default:
	{
		MsgType = -1;
		break;
	}
	}

	if(MsgType != -1)
	{
		DO_CBK(pCBK_FUNC,CbkFunction,pMsg,MsgType);
		return SUCCESS;
	}

	return FAIL;
}



#endif //__BT_AO_SERVICE_C__

