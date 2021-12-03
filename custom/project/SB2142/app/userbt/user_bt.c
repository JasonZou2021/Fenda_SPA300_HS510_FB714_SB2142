#ifndef __BT_USER_C__
#define __BT_USER_C__
#include "user_bt.h"
#include "bt_ao_service_api.h"


#define USER_CBK(P_CB, P_CBACK, ...)\
	if (P_CB && P_CB->P_CBACK) {            \
		P_CB->P_CBACK(__VA_ARGS__);         \
	}                                       \
	else {                                  \
		bt_ao_printf("USER Callback is NULL");  \
	}


typedef int (*BtReadyPost)(void);
typedef int (*BtUnReadyPost)(void);
typedef int (*BtTwsModePost)(SINK_MODE_e eMode);
typedef int (*BtSearchInfoPost)(BTSearchDevInfo *pdata);
typedef int (*BtSearchFinishPost)(void);
typedef int (*BtPairReqInfoPost)(BTPairDev *pdata);
typedef int (*BtPairFinishPost)(BTPairDev *pdata);
typedef int (*BtUnpairInfoPost)(BTPairDev *pdata);
typedef int (*BtConnectInfoPost)(stBtDevInfo *pdata);
typedef int (*BtDisconnectInfoPost)(stBtDevInfo *pdata);
typedef int (*BtConnectResultInfoPost)(stBtDevInfo *pdata);
typedef int (*BtDisconnectResultInfoPost)(stBtDevInfo *pdata);
typedef int (*BtSppUpGradePost)(stBtDevInfo *pdata);
typedef int (*BtSppConnectPost)(stBtDevInfo *pdata);
typedef int (*BtSppDisconnectPost)(stBtDevInfo *pdata);
typedef int (*BtDataPost)(Spp_receiver_Data_If *pdata);
typedef int (*BtCtrVolumePost)(BtVolumeInfo *pData);
typedef int (*BtCtrVolumeRegPost)(BtVolumeInfo *pData);
typedef int (*BtCtrPrevPost)(BtAddr_t *pAddr);
typedef int (*BtCtrNextPost)(BtAddr_t *pAddr);
typedef int (*BtCtrPlayPost)(BtAddr_t *pAddr);
typedef int (*BtCtrPausePost)(BtAddr_t *pAddr);
typedef int (*BtAclConnectReqPost)(BTConnectReqInfo *pInfo);
typedef int (*BtSourcePlayerStatePost)(BtDevPlayState *pDevPlayerInfo);


typedef struct stDevCBKIF
{
	BtReadyPost   BtReadyOkPost;
	BtUnReadyPost	BtUnreadyPost;
	BtTwsModePost    BtTwsModeChangedPost;
	BtSearchInfoPost SearchPost;
	BtSearchFinishPost SearchFinishPost;
	BtAclConnectReqPost	AclConnectReq;
	BtPairReqInfoPost PairPost;
	BtPairFinishPost PairFinish;
	BtUnpairInfoPost  UnpairPost;
	BtConnectInfoPost ConnectReqPost;
	BtDisconnectInfoPost DisconnectReqPost;
	BtConnectResultInfoPost ConnectResultPost;
	BtDisconnectResultInfoPost DisonnectResultPost;
	BtSppUpGradePost UpGradePost;
	BtSppConnectPost SppConnectPost;
	BtSppDisconnectPost SppDisconnectPost;
	BtDataPost SppDataInPost;
	BtCtrVolumePost		VolumeSetPost;
	BtCtrVolumeRegPost	VolumeRegPost;
	BtCtrPrevPost		PrevReqPost;
	BtCtrNextPost		NextReqPost;
	BtCtrPlayPost		PlayReqPost;
	BtCtrPausePost		PauseReqPost;
	BtSourcePlayerStatePost SourcePlayerStatePost;
} BtCBKFunc;


static int BtReadySend(void);
static int BtUnReadySend(void);
static int BtTwsModeInfoSend(SINK_MODE_e eMode);
static int SearchDevResultSend(BTSearchDevInfo *pdata);
static int SearchDevFinishSend(void);
static int BtAclReqSend(BTConnectReqInfo *pInfo);
static int PairFinishDevInfoSend(BTPairDev *pdata);
static int PairReqInfoSend(BTPairDev *pdata);
static int UnPairInfoSend(BTPairDev *pdata);
static int ConnectReqInfoSend(stBtDevInfo *pdata);
static int DisconnectReqInfoSend(stBtDevInfo *pdata);
static int ConnectOkInfoSend(stBtDevInfo *pdata);
static int DisconnectOkInfoSend(stBtDevInfo *pdata);
static int SppConnectOkSend(stBtDevInfo *pdata);
static int SppDisconnectOkSend(stBtDevInfo *pdata);
static int BtRecvDataSend(Spp_receiver_Data_If *pdata);
static int BtVolumeSetSend(BtVolumeInfo *pData);
static int BtVolumeRegSend(BtVolumeInfo *pData);
static int BtPrevPlaySend(BtAddr_t *pAddr);
static int BtNextPlaySend(BtAddr_t *pAddr);
static int BtPlaySend(BtAddr_t *pAddr);
static int BtPauseSend(BtAddr_t *pAddr);
static int BtSourcePlayerStateSend(BtDevPlayState *pDevPlayerInfo);
//static int BtRemoteDeviceNameSend(BTPairDev *pdata);



static QActive *pDesActive = NULL;

static BtCBKFunc gUserCbkPost =
{
	BtReadySend,
	BtUnReadySend,
	BtTwsModeInfoSend,
	SearchDevResultSend,
	SearchDevFinishSend,
	BtAclReqSend,
	PairReqInfoSend,
	PairFinishDevInfoSend,
	UnPairInfoSend,    //pair fail or unpair finish
	ConnectReqInfoSend,
	DisconnectReqInfoSend,
	ConnectOkInfoSend,
	DisconnectOkInfoSend,
	NULL,//SppUpgradeMsgSend,
	SppConnectOkSend,
	SppDisconnectOkSend,
	BtRecvDataSend,
	BtVolumeSetSend,
	BtVolumeRegSend,
	BtPrevPlaySend,
	BtNextPlaySend,
	BtPlaySend,
	BtPauseSend,
	BtSourcePlayerStateSend,
	//BtRemoteDeviceNameSend,
};



static BtCBKFunc *_GetBtAppCbkPostFunc(void)
{
	return &gUserCbkPost;
}

static QActive *GetDesActiveAo(void)
{
	return pDesActive;
}

BtAppCBKEvt *UserBtMsgPackage(BT_APP_EVTSIG_e sig, void *pData, int DataLen)
{
	BtAppCBKEvt *pMsg = NULL;

	pMsg = Q_NEW(BtAppCBKEvt, BT_APP_SIG);
	pMsg->type = sig;

	if (pData != NULL && DataLen > 0)
	{
		pMsg->pdata = BtMalloc(DataLen);

		if (pMsg->pdata)
		{
			memcpy(pMsg->pdata, pData, DataLen);
		}
		else
		{
			bt_ao_error_printf("BtMollac fail!!");
		}
	}
	else
	{
		pMsg->pdata = pData;
	}

	return pMsg;
}


static int BtReadySend(void)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	(void)sender;

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_READY_OK_SIG, NULL, 0);
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}


static int BtUnReadySend(void)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	(void)sender;

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_UNREADY_OK_SIG, NULL, 0);
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}



static int BtTwsModeInfoSend(SINK_MODE_e eMode)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	(void)sender;

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_TWS_MODE_CHANGED_SIG, &eMode, sizeof(eMode));
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}


static int PairReqInfoSend(BTPairDev *pdata)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	(void)sender;

	if (pdata == NULL)
	{
		return FAIL;
	}

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_PAIRING_ING_SIG, pdata, sizeof(BTPairDev));
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}

static int PairFinishDevInfoSend(BTPairDev *pdata)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	(void)sender;

	if (pdata == NULL)
	{
		return FAIL;
	}

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_PAIRED_OK_SIG, pdata, sizeof(BTPairDev));
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}

static int UnPairInfoSend(BTPairDev *pdata)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	(void)sender;

	if (pdata == NULL)
	{
		return FAIL;
	}

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_UNPAIRED_OK_SIG, pdata, sizeof(BTPairDev));
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}


static int SearchDevResultSend(BTSearchDevInfo *pdata)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	(void)sender;

	if (pdata == NULL)
	{
		return FAIL;
	}

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_SEARCH_RESULT_SIG, pdata, sizeof(BTSearchDevInfo));
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}

static int SearchDevFinishSend(void)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	(void)sender;

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_SEARCH_FINISH_SIG, NULL, 0);
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}


static int ConnectReqInfoSend(stBtDevInfo *pdata)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	(void)sender;

	if (pdata == NULL)
	{
		return FAIL;
	}

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_AVDP_CONNECT_ING_SIG, pdata, sizeof(stBtDevInfo));
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}

static int DisconnectReqInfoSend(stBtDevInfo *pdata)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	(void)sender;

	if (pdata == NULL)
	{
		return FAIL;
	}

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_AVDP_DISCONNECT_ING_SIG, pdata, sizeof(stBtDevInfo));
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}
static int ConnectOkInfoSend(stBtDevInfo *pdata)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	(void)sender;

	if (pdata == NULL)
	{
		return FAIL;
	}

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_AVDP_CONNECT_SUCCESS_SIG, pdata, sizeof(stBtDevInfo));
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}

static int DisconnectOkInfoSend(stBtDevInfo *pdata)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	(void)sender;

	if (pdata == NULL)
	{
		return FAIL;
	}

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_AVDP_DISCONNECT_SUCCESS_SIG, pdata, sizeof(stBtDevInfo));
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}

static int SppConnectOkSend(stBtDevInfo *pdata)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	(void)sender;

	if (pdata == NULL)
	{
		return FAIL;
	}

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_SPP_CONNECT_SUCCESS_SIG, pdata, sizeof(stBtDevInfo));
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}


static int SppDisconnectOkSend(stBtDevInfo *pdata)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	(void)sender;

	if (pdata == NULL)
	{
		return FAIL;
	}

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_SPP_DISCONNECT_SUCCESS_SIG, pdata, sizeof(stBtDevInfo));
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}



static int BtRecvDataSend(Spp_receiver_Data_If *pdata)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	Spp_receiver_Data_If *pMsgData = NULL;
	(void)sender;

	if (pdata == NULL)
	{
		return FAIL;
	}

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_RECV_DATA_SIG, pdata, sizeof(Spp_receiver_Data_If) + pdata->payload_len);
	pMsgData = (Spp_receiver_Data_If *)msg->pdata;
	pMsgData->pdatabuf = pMsgData + sizeof(Spp_receiver_Data_If);
	memcpy(pMsgData->pdatabuf, pdata->pdatabuf, pdata->payload_len);
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}


static int BtVolumeSetSend(BtVolumeInfo *pData)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	(void)sender;

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_VOLUME_SET_REQ_SIG, pData, sizeof(BtVolumeInfo));
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}


static int BtVolumeRegSend(BtVolumeInfo *pData)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	(void)sender;

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_VOLUME_REG_REQ_SIG, pData, sizeof(BtVolumeInfo));
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}


static int BtPrevPlaySend(BtAddr_t *pAddr)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	(void)sender;

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_PREV_PLAY_REQ_SIG, pAddr, sizeof(BtAddr_t));
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}


static int BtNextPlaySend(BtAddr_t *pAddr)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	(void)sender;

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_NEXT_PLAY_REQ_SIG, pAddr, sizeof(BtAddr_t));
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}


static int BtPlaySend(BtAddr_t *pAddr)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	(void)sender;

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_PLAY_REQ_SIG, pAddr, sizeof(BtAddr_t));
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}


static int BtPauseSend(BtAddr_t *pAddr)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	(void)sender;

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_PAUSE_REQ_SIG, pAddr, sizeof(BtAddr_t));
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}


static int BtAclReqSend(BTConnectReqInfo *pInfo)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	(void)sender;

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_ACL_CONNECT_SIG, pInfo, sizeof(BTConnectReqInfo));
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}


static int BtSourcePlayerStateSend(BtDevPlayState *pDevPlayerInfo)
{
	QActive *const me = GetDesActiveAo();
	QActive *const sender = GetBtAo();
	(void)sender;

	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_SOURCE_STATE_UPDATE_SIG, pDevPlayerInfo, sizeof(BtDevPlayState));
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}

/*static int BtRemoteDeviceNameSend(BTPairDev *pdata)
{
	QActive * const me = GetDesActiveAo();
	QActive * const sender = GetBtAo();
	(void)sender;

	if(pdata == NULL)
	{
		return FAIL;
	}
	BtAppCBKEvt *msg = UserBtMsgPackage(BT_APP_REMOTE_DEVICE_NAME_SIG,pdata,sizeof(BTPairDev));
	QACTIVE_POST(me, (QEvt *)msg, sender);
	return SUCCESS;
}*/




static int UserBtBaseCbkHandle(void *pMsg, BTCBK_EVTSIG_e type)
{
	BtCBKFunc *UserCBK = _GetBtAppCbkPostFunc();
	BTSearchDevInfo *pBtSearchInfo = NULL;
	BTPairDev *pPairInfo = NULL;

	switch (type)
	{
		case BT_CBK_READY_SIG:
		{
			USER_CBK(UserCBK, BtReadyOkPost);
			break;
		}
		case BT_CBK_UNREADY_SIG:
		{
			USER_CBK(UserCBK, BtUnreadyPost);
			break;
		}
		case BT_CBK_TWS_SWITCH_OK_SIG:
		{
			USER_CBK(UserCBK, BtTwsModeChangedPost, *((SINK_MODE_e *)pMsg));
			break;
		}
		case BT_CBK_SEARCH_RESULT_SIG:
		{
			pBtSearchInfo = (BTSearchDevInfo *)pMsg;
			USER_CBK(UserCBK, SearchPost, pBtSearchInfo);
			break;
		}
		case BT_CBK_SEARCH_FINISH_SIG:
		{
			USER_CBK(UserCBK, SearchFinishPost);
			break;
		}
		case BT_CBK_ACL_CONNECT_SIG:
		{
			BTConnectReqInfo *pResultInfo = (BTConnectReqInfo *)pMsg;
			USER_CBK(UserCBK, AclConnectReq, pResultInfo);
			break;
		}
		case BT_CBK_PAIRING_IND_SIG:
		{
			pPairInfo = (BTPairDev *)pMsg;
			USER_CBK(UserCBK, PairPost, pPairInfo);
			break;
		}
		case BT_CBK_PAIRED_OK_SIG:
		{
			pPairInfo = (BTPairDev *)pMsg;
			USER_CBK(UserCBK, PairFinish, pPairInfo);
			break;
		}
		case BT_CBK_UNPAIRED_OK_SIG:
		case BT_CBK_PAIRED_FAIL_SIG:
		{
			pPairInfo = (BTPairDev *)pMsg;
			USER_CBK(UserCBK, UnpairPost, pPairInfo);
			break;
		}
		default:
		{
			break;
		}
	}

	return 0;
}



static int UserBtA2dpCbkHandle(stBtDevInfo *pDev, BTCBK_EVTSIG_e type)
{
	BtCBKFunc *UserCBK = _GetBtAppCbkPostFunc();

	switch (type)
	{
		case BT_CBK_A2DP_CONNECT_ING_SIG:
		{
			USER_CBK(UserCBK, ConnectReqPost, pDev);
			break;
		}
		case BT_CBK_A2DP_CONNECT_FAIL_SIG:
		{
			break;
		}
		case BT_CBK_A2DP_CONNECT_SUCCESS_SIG:
		{
			USER_CBK(UserCBK, ConnectResultPost, pDev);
			break;
		}
		case BT_CBK_A2DP_DISCONNECT_ING_SIG:
		{
			USER_CBK(UserCBK, DisconnectReqPost, pDev);
			break;
		}
		case BT_CBK_A2DP_DISCONNECT_FAIL_SIG:
		{
			break;
		}
		case BT_CBK_A2DP_DISCONNECT_SUCCESS_SIG:
		{
			USER_CBK(UserCBK, DisonnectResultPost, pDev);
			break;
		}
		default:
		{
			break;
		}
	}

	return 0;
}



static int UserBtSppCbkHandle(stBtDevInfo *pDev, BTCBK_EVTSIG_e type)
{
	BtCBKFunc *UserCBK = _GetBtAppCbkPostFunc();

	switch (type)
	{
		case BT_CBK_SPP_CONNECT_SUCCESS_SIG:
		{
			USER_CBK(UserCBK, SppConnectPost, pDev);
			break;
		}
		case BT_CBK_SPP_CONNECT_FAIL_SIG:
		case BT_CBK_SPP_DISCONNECT_SUCCESS_SIG:
		{
			USER_CBK(UserCBK, SppDisconnectPost, pDev);
			break;
		}
		default:
		{
			break;
		}
	}

	return 0;
}



static int UserBtCtrlCbkHandle(void *pMsg, BTCBK_EVTSIG_e type)
{
	BtCBKFunc *UserCBK = _GetBtAppCbkPostFunc();
	(void)pMsg;

	switch (type)
	{
		case BT_CBK_VOLUME_SET_REQ_SIG:
		{
			USER_CBK(UserCBK, VolumeSetPost, (BtVolumeInfo *)pMsg);
			break;
		}
		case BT_CBK_VOLUME_REG_SIG:
		{
			USER_CBK(UserCBK, VolumeSetPost, (BtVolumeInfo *)pMsg);
			break;
		}
		case BT_CBK_NEXT_PLAY_REQ_SIG:
		{
			USER_CBK(UserCBK, PrevReqPost, (BtAddr_t *)pMsg);
			break;
		}
		case BT_CBK_PREV_PLAY_REQ_SIG:
		{
			USER_CBK(UserCBK, NextReqPost, (BtAddr_t *)pMsg);
			break;
		}
		case BT_CBK_PLAY_REQ_SIG:
		{
			USER_CBK(UserCBK, PlayReqPost, (BtAddr_t *)pMsg);
			break;
		}
		case BT_CBK_PAUSE_REQ_SIG:
		{
			USER_CBK(UserCBK, PauseReqPost, (BtAddr_t *)pMsg);
			break;
		}
		default:
		{
			break;
		}
	}

	return 0;
}



static int UserAppBtCbkHandle(void *pMsg, BTCBK_EVTSIG_e type)
{

	switch (type)
	{
		case BT_CBK_A2DP_SIG...BT_CBK_A2DP_MAX_SIG:
		{
			stBtDevInfo *pDev = (stBtDevInfo *)pMsg;
			UserBtA2dpCbkHandle(pDev, type);
			break;
		}
		case BT_CBK_SPP_SIG...BT_CBK_SPP_MAX_SIG:
		{
			stBtDevInfo *pDev = (stBtDevInfo *)pMsg;
			UserBtSppCbkHandle(pDev, type);
			break;
		}
		case BT_CBK_READY_SIG...BT_CBK_UNPAIRED_OK_SIG:
		{
			UserBtBaseCbkHandle(pMsg, type);
			break;
		}
		case BT_CBK_CTR_REQ_SIG...BT_CBK_CTR_REQ_MAX_SIG:
		{
			UserBtCtrlCbkHandle(pMsg, type);
			break;
		}
		case BT_CBK_SOURCE_STATE_UPDATE_SIG:
		{
			BtCBKFunc *UserCBK = _GetBtAppCbkPostFunc();
			USER_CBK(UserCBK, SourcePlayerStatePost, (BtDevPlayState *)pMsg);
			break;
		}
		default:
		{
			bt_ao_printf("%s default  \n", __FUNCTION__);
			break;
		}

	}

	return 0;
}


static BtUserCbkHandle UserAppBtCbk =
{
	UserAppBtCbkHandle,
};


int UserBtCbkRegister(QActive *pstDesActive)
{
	if (pstDesActive != NULL)
	{
		pDesActive = pstDesActive;
		return BtAoSrvUserCbkRegister(&UserAppBtCbk);
	}

	return FAIL;
}


int UserBtCbkDelRegister(QActive *pstDesActive)
{
	if (pstDesActive != NULL && pDesActive == pstDesActive)
	{
		return BtAoSrvUserCbkDelRegister(&UserAppBtCbk);
	}

	return FAIL;
}


#endif //__BT_USER_C__

