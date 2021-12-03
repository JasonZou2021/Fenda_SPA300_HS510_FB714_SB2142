#ifndef __BT_AO_TWS_C__
#define __BT_AO_TWS_C__
/******************************************************************************/
/**
*
* \file	bt_ao_tws.c
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
#include "types.h"
#include "log_utils.h"
#include "std_interfaces.h"
#include "bt_ao.h"
#include "bt_mw_core.h"
#include "bt_ao_auto.h"
#include "bt_ao_tws.h"

/*---------------------------------------------------------------------------*
 *    MACRO DECLARATIONS
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 *    GLOBAL VARIABLES
 *---------------------------------------------------------------------------*/
typedef enum eTwsRole
{
	WSS_DEV = 0,
	HEADPHONE_DEV,
	TWS_MAX_DEV,
}TwsRole_e;


typedef struct BtTwsMgr
{
	SINK_MODE_e		TwsRole;
	char       		IsTwsSurportSwitch;
	stBtTwsAction 	*pTwsSinkAction;
	stBtDevInfo    	RemoteDev[TWS_MAX_DEV];
	char 			IsRegVolumeSync;//just for headphone
}stBtTwsMgr;

static stBtTwsMgr TwsMgrInfo;
static TwsCbkPost *pTwsPost = NULL;
/*---------------------------------------------------------------------------*
 *    EXTERNAL REFERENCE
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 *    FUNTION DECLARATIONS
 *---------------------------------------------------------------------------*/
static void _TwsInit(SINK_MODE_e role,stBtTwsAction **pAction,char IsSurportSwitch);
static void _TwsRoleSwitch(SINK_MODE_e role);
static void _TwsActionGetSinkAction(stBtTwsAction **pAction);


//sink for sub dev
static void _WssActionInit(void);
static void _WssBtReadyAction(void);
static void _WssSearchResultAction(const char **pBTDesName,BTSearchDevInfo *pBtSearchInfo);
static void _WssSearchFinishAction(void);
static void _WssPairingAction(BTPairDev *pPairInfo);
static void _WssPairFinishAction(BTPairDev *pPairInfo);
static void _WssConnect(BtAddr_t *pAddr);
static void _WssConnectOkAction(stBtDevInfo *pDev);
static void _WssDisconnect(BtAddr_t *pAddr);
static void _WssDisconnectOkAction(stBtDevInfo *pDev);
static void _WssSaveDevInfo(stBtDevInfo *pDev);
static int _WssSppInfoUpdate(stBtDevInfo *pDev);
static int _WssSppConnect(BtAddr_t *pAddr);
static int _WssSppConnectOkAction(stBtDevInfo *pDev);
static int _WssSppDisconnect(BtAddr_t *pAddr);
static int _WssSppDisconnectOkAction(stBtDevInfo *pDev);
static int _WssGetConnectState(BT_PROFILE_STATE_e *pState);
static int _WssGetSppConnectState(BT_PROFILE_STATE_e *pState);
static int _WssPause(void);
static int _WssPlay(void);
static int _WssGetAddr(BtAddr_t *pAddr);
static int _IsWssAddr(BtAddr_t *pAddr,int *pState);
static int _WssIsFirstConnected(void);


//sink dev :headphone
static void _HeadPhoneActionInit(void);
static void _HeadPhoneBtReadyAction(void);
static void _HeadPhoneSearchResultAction(const char **pBTDesName,BTSearchDevInfo *pBtSearchInfo);
static void _HeadPhoneSearchFinishAction(void);
static void _HeadPhonePairingAction(BTPairDev *pPairInfo);
static void _HeadPhonePairFinishAction(BTPairDev *pPairInfo);
static void _HeadPhoneConnect(BtAddr_t *pAddr);
static void _HeadPhoneConnectOkAction(stBtDevInfo *pDev);
static void _HeadPhoneDisconnect(BtAddr_t *pAddr);
static void _HeadPhoneDisconnectOkAction(stBtDevInfo *pDev);
static void _HeadPhoneSaveDevInfo(stBtDevInfo *pDev);
static void _HeadPhoneVolumeSyncReg(BtVolumeInfo *pVolumeInfo);
static void _HeadPhoneVolumeSyncSet(BtVolumeSet *pVolumeInfo);
static int _HeadPhoneConnectState(BT_PROFILE_STATE_e *pState);
static int _HeadPhoneGetAddr(BtAddr_t *pAddr);
static int _HeadPhonePlay(void);
static int _HeadPhonePause(void);
static int _IsHeadPhoneAddr(BtAddr_t *pAddr,int *pState);


//main for sub action
static stBtTwsAction WssAction =
{
	_WssBtReadyAction,
	_WssSearchResultAction,
	_WssSearchFinishAction,
	_WssPairingAction,
	_WssPairFinishAction,
	_WssConnect,
	_WssDisconnect,
	_WssConnectOkAction,
	_WssDisconnectOkAction,
	_WssSppConnect,
	_WssSppDisconnect,
	_WssSppConnectOkAction,
	_WssSppDisconnectOkAction,
	_WssGetConnectState,
	_WssGetSppConnectState,
	_WssGetAddr,
	_WssPlay,
	_WssPause,
	_IsWssAddr,
	_WssIsFirstConnected,
	NULL,
	NULL,
	NULL,
};


static stBtTwsAction HeadPhoneAction =
{
	_HeadPhoneBtReadyAction,
	_HeadPhoneSearchResultAction,
	_HeadPhoneSearchFinishAction,
	_HeadPhonePairingAction,
	_HeadPhonePairFinishAction,
	_HeadPhoneConnect,
	_HeadPhoneDisconnect,
	_HeadPhoneConnectOkAction,
	_HeadPhoneDisconnectOkAction,
	NULL,
	NULL,
	NULL,
	NULL,
	_HeadPhoneConnectState,
	NULL,
	_HeadPhoneGetAddr,
	_HeadPhonePlay,
	_HeadPhonePause,
	_IsHeadPhoneAddr,
	NULL,
	NULL,
	_HeadPhoneVolumeSyncReg,
	_HeadPhoneVolumeSyncSet,
};


stBtTwsCtrlApi   BtTwsCtrlApi  =
{
	_TwsInit,
	_TwsRoleSwitch,
	_TwsActionGetSinkAction,
};


//Tws for main
static stBtTwsMgr * _GetBtTwsCurInfo(void)
{
	return &TwsMgrInfo;
}



static void _TwsInit(SINK_MODE_e role,stBtTwsAction **pAction,char IsSurportSwitch)
{
	stBtTwsMgr *pTwsMgr = _GetBtTwsCurInfo();
	int i = 0;

	pTwsMgr->TwsRole = role;
	pTwsMgr->IsTwsSurportSwitch = IsSurportSwitch;

	if(role == WSS)
	{
		pTwsMgr->pTwsSinkAction = &WssAction;
	}
	else if(role == HEADPHONE)
	{
		pTwsMgr->pTwsSinkAction = &HeadPhoneAction;
	}
	else
	{
		pTwsMgr->pTwsSinkAction = NULL;
	}

	for(i=0;i<TWS_MAX_DEV;i++)
	{
		memset(&pTwsMgr->RemoteDev,0,sizeof(stBtDevInfo));
	}

	if(pAction != NULL)
	{
		*pAction = pTwsMgr->pTwsSinkAction;
	}
}



static void _TwsRoleSwitch(SINK_MODE_e role)
{
	stBtTwsMgr *pWssAction = _GetBtTwsCurInfo();

	if(role == WSS)
	{
		_WssActionInit();
	}
	else if(role == HEADPHONE)
	{
		_HeadPhoneActionInit();
	}
	else
	{
		pWssAction->TwsRole = SINK_NONE;
		pWssAction->pTwsSinkAction = NULL;
		//BtServiceWssSetMode(SINK_NONE);
	}

	if(pTwsPost != NULL && pTwsPost->BtTwsSwichFinishSend != NULL)
	{
		pTwsPost->BtTwsSwichFinishSend(role);
	}
}


static void _TwsActionGetSinkAction(stBtTwsAction **pAction)
{
	stBtTwsMgr *pWssAction = _GetBtTwsCurInfo();
	*pAction = pWssAction->pTwsSinkAction;
}


//wss api
static void _WssActionInit(void)
{
	stBtTwsMgr *pWssAction = _GetBtTwsCurInfo();

	pWssAction->TwsRole = WSS;
	pWssAction->pTwsSinkAction = &WssAction;
	_WssBtReadyAction();
}




static void _WssBtReadyAction(void)
{
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();
	BtAddr_t HeadPhoneAddr = {{0}};
	int res =FAIL;

	BtAutoClear(ROLE_WSS);
	res = _HeadPhoneGetAddr(&HeadPhoneAddr);

	if(res == SUCCESS)
	{
		pBtCurInfo->RemoteDev[HEADPHONE_DEV].bd_addr = HeadPhoneAddr;
		//BtServiceSetDenyConnect(&HeadPhoneAddr,ON);
	}

	//BtServiceWssSetMode(WSS);

	BtAddr_t WssAddr = {{0}};
	//res = BtServiceGetWssAddr(&WssAddr);

	if(res == SUCCESS)//(strlen(WssAddr.address) > 0)//(BtServiceSubIsFirstConnect())
	{
		bt_ao_printf("Bt Sub IsFirstConnect !");

		//BtServiceSetDenyConnect(&WssAddr,OFF);
		pBtCurInfo->RemoteDev[WSS_DEV].bd_addr = WssAddr;
		pBtCurInfo->RemoteDev[WSS_DEV].BtPairState = PAIRED;

		if(pBtCurInfo->IsTwsSurportSwitch)
		{
			BtAutoSet(ON,ROLE_WSS,&WssAddr);
		}
	}
	else
	{
		if(pBtCurInfo->IsTwsSurportSwitch)
		{
			//BtServiceStartSearch();
		}
	}
}


static void _WssSearchResultAction(const char **pBTDesName,BTSearchDevInfo *pBtSearchInfo)
{
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();
	stBtDevInfo *pWssDev = &pBtCurInfo->RemoteDev[WSS_DEV];
	BT_PROFILE_STATE_e state = DISCONNECT_OK;
	int i = 0;

	_WssGetConnectState(&state);

	if(state != DISCONNECT_OK /*|| !BtServiceIsSearching()*/)
	{
		return;
	}

	for(i=0;i<BT_SUB_NUN;i++)
	{
		if((pBTDesName!=NULL)&&(memcmp(pBTDesName[i],pBtSearchInfo->name,strlen(pBTDesName[i]))==0))
		{
			/*if(BtServiceIsSearching())
			{
				BtServiceStopSearch();
			}*/

			memset(pWssDev,0,sizeof(stBtDevInfo));
			pWssDev->bd_addr = pBtSearchInfo->BtAddr;
			memcpy(pWssDev->name,pBtSearchInfo->name,pBtSearchInfo->NameLen);
			pWssDev->name_len = pBtSearchInfo->NameLen;
			return;
		}
	}

}



static void _WssSearchFinishAction(void)
{
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();
	stBtDevInfo *pWssDev = &pBtCurInfo->RemoteDev[WSS_DEV];
	BT_PROFILE_STATE_e state = DISCONNECT_OK;
	BtAddr_t bTempAddr = {{0x0}};

	if(memcmp(pWssDev->bd_addr.address,bTempAddr.address,sizeof(BtAddr_t)) != 0)
	{
		if(pWssDev->BtPairState == UNPAIR)
		{
			//connect target
			//BtServicePairStart(&(pWssDev->bd_addr));
		}
		else if(pWssDev->BtPairState == PAIRED && pWssDev->BtConnState == DISCONNECT_OK)
		{
			//BtServiceA2dpConnect(&(pWssDev->bd_addr),WSS_ADDR);
		}

		return;
	}

	_WssGetConnectState(&state);

	if(state == DISCONNECT_OK)
	{
		bt_ao_printf("\n StartSearch again! \n");
		//BtServiceStartSearch();
	}

}





static void _WssPairingAction(BTPairDev *pPairInfo)
{
	(void)pPairInfo;
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();
	pBtCurInfo->RemoteDev[WSS_DEV].BtPairState = PAIRING;
}


static void _WssPairFinishAction(BTPairDev *pPairInfo)
{
	(void)pPairInfo;
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();
	pBtCurInfo->RemoteDev[WSS_DEV].BtPairState = PAIRED;
}


#if 0
static int _WssSetConnectState(BT_PROFILE_STATE_e state)
{
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();

	stBtDevInfo *pDev = &pBtCurInfo->RemoteDev[WSS_DEV];
	pDev->BtConnState = state << BT_A2DP_PROFILE_BIT;
	return SUCCESS;
}
#endif


static int _WssGetConnectState(BT_PROFILE_STATE_e *pState)
{
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();

	if(pState != NULL)
	{
		*pState = BT_A2DP_PROFILE(pBtCurInfo->RemoteDev[WSS_DEV].BtConnState);
		return SUCCESS;
	}

	return FAIL;
}



static int _WssGetSppConnectState(BT_PROFILE_STATE_e *pState)
{
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();

	if(pState != NULL)
	{
		*pState = BT_SPP_PROFILE(pBtCurInfo->RemoteDev[WSS_DEV].BtConnState);
		return SUCCESS;
	}

	return FAIL;
}


static int _WssIsFirstConnected(void)
{
	//return BtServiceSubIsFirstConnect();
	return 0;
}



static void _WssConnectOkAction(stBtDevInfo *pDev)
{
	int role = ROLE_WSS;

	if(pDev == NULL || pDev->DevRole != WSS_ADDR)
	{
		return;
	}

	//BtServiceSubFirstConnectedSet();
	_WssSaveDevInfo(pDev);
	BtAutoClear(role);
	BtAutoClearCurRetryTimes(role);
}



static void _WssDisconnectOkAction(stBtDevInfo *pDev)
{
	int state = FALSE;
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();

	if(pDev->DevRole != WSS_ADDR)
	{
		return;
	}

	if(_IsWssAddr(&pDev->bd_addr,&state))
	{
		if(pBtCurInfo->IsTwsSurportSwitch)
		{
			BtAutoSet(ON,ROLE_WSS,&pDev->bd_addr);
		}

		_WssSaveDevInfo(pDev);
	}
}



static void _WssDisconnect(BtAddr_t *pAddr)
{
	(void)pAddr;
	//BtServiceA2dpDisconnect(pAddr,WSS_ADDR);
}



static int _IsWssAddr(BtAddr_t *pAddr,int *pState)
{
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();

	if(memcmp(&(pBtCurInfo->RemoteDev[WSS_DEV].bd_addr),pAddr,sizeof(BtAddr_t))==0)
	{
		*pState = TRUE;
		return TRUE;
	}

	*pState = FALSE;
	return FALSE;
}


static void _WssConnect(BtAddr_t *pAddr)
{
	(void)pAddr;
	//BtServiceA2dpConnect(pAddr,WSS_ADDR);
}



static int _WssSppInfoUpdate(stBtDevInfo *pDev)
{
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();
	stBtDevInfo *pDevInfo = &pBtCurInfo->RemoteDev[WSS_DEV];

	if(pDev != NULL)
	{
		pDevInfo->BtConnState &= (~(pDevInfo->BtConnState & BIT_X2Y(BT_SPP_PROFILE_BIT,BT_SPP_PROFILE_BIT+1)));
		pDevInfo->BtConnState |= (BT_SPP_PROFILE(pDev->BtConnState)<<BT_SPP_PROFILE_BIT);
		return SUCCESS;
	}

	return FAIL;
}


static void _WssSaveDevInfo(stBtDevInfo *pDev)
{
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();
	stBtDevInfo *pDevInfo = &pBtCurInfo->RemoteDev[WSS_DEV];

	pDevInfo->bd_addr = pDev->bd_addr;
	memcpy(pDevInfo->name,pDev->name,pDev->name_len);
	pDevInfo->name_len = pDev->name_len;
	pDevInfo->DevRole = pDev->DevRole;
	pDevInfo->BtConnState &= (~(pDevInfo->BtConnState & BIT_X2Y(BT_A2DP_PROFILE_BIT,BT_A2DP_PROFILE_BIT+1)));
	pDevInfo->BtConnState |= BT_A2DP_PROFILE(pDev->BtConnState)<<BT_A2DP_PROFILE_BIT;
}


static int _WssGetAddr(BtAddr_t *pAddr)
{
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();
	BtAddr_t Addr = {{0}};

	if(pAddr != NULL)
	{
		if(memcmp(&pBtCurInfo->RemoteDev[WSS_DEV].bd_addr,&Addr,sizeof(BtAddr_t)) == 0)
		{
			//int res = BtServiceGetWssAddr(pAddr);
			int res = 0;

			if(res == SUCCESS)
			{
				pBtCurInfo->RemoteDev[WSS_DEV].bd_addr = *pAddr;
			}
			else
			{
				return FAIL;
			}
		}
		else
		{
			memcpy(pAddr,&pBtCurInfo->RemoteDev[WSS_DEV].bd_addr,sizeof(BtAddr_t));
		}

		return SUCCESS;
	}

	return FAIL;
}


static int _WssPlay(void)
{
	BtAddr_t Addr = {{0}};
	_WssGetAddr(&Addr);

	/*if(BtServiceIsA2dpConnected(&Addr))
	{
		//if(BT_SPP_PROFILE(BtCurInfo.RemoteDev[WSS_INFO].BtConnState) != CONNECT_OK)
		{
			//BtServiceSourceSendStart(&Addr);
			BtServiceSendAudioSync(1,&Addr);
		}
	}*/

	return SUCCESS;
}


static int _WssPause(void)
{
	BtAddr_t Addr = {{0}};
	_WssGetAddr(&Addr);

	/*if(BtServiceIsA2dpConnected(&Addr))
	{
		//if(BT_SPP_PROFILE(BtCurInfo.RemoteDev[WSS_INFO].BtConnState) != CONNECT_OK)
		{
			//BtServiceSourceSendPause(&Addr);
			BtServiceSendAudioSync(0,&Addr);
		}
	}*/

	return SUCCESS;
}



static int _WssSppConnect(BtAddr_t *pAddr)
{
	(void)pAddr;
	BtAddr_t addr = {{0}};
	_WssGetAddr(&addr);

	if(pAddr == NULL)
	{
		return FAIL;
	}

	if(memcmp(&addr,pAddr,sizeof(BtAddr_t)) == 0)
	{
		//BtServiceSppConnect(&addr);
		return SUCCESS;
	}

	return FAIL;
}



static int _WssSppDisconnect(BtAddr_t *pAddr)
{
	(void)pAddr;

	BtAddr_t addr = {{0}};
	_WssGetAddr(&addr);

	if(pAddr == NULL)
	{
		return FAIL;
	}

	if(memcmp(&addr,pAddr,sizeof(BtAddr_t)) == 0)
	{
		//BtServiceSppDisconnect(&addr);
		return SUCCESS;
	}

	return FAIL;
}


static int _WssSppConnectOkAction(stBtDevInfo *pDev)
{
	if(pDev != NULL && pDev->DevRole == WSS_ADDR)
	{
		_WssSppInfoUpdate(pDev);
		return SUCCESS;
	}

	return FAIL;
}



static int _WssSppDisconnectOkAction(stBtDevInfo *pDev)
{
	if(pDev != NULL && pDev->DevRole == WSS_ADDR)
	{
		_WssSppInfoUpdate(pDev);
		return SUCCESS;
	}

	return FAIL;
}





//headphone
static void _HeadPhoneActionInit(void)
{
	stBtTwsMgr *pWssAction = _GetBtTwsCurInfo();

	pWssAction->TwsRole = HEADPHONE;
	pWssAction->pTwsSinkAction = &HeadPhoneAction;
	_HeadPhoneBtReadyAction();
}


static void _HeadPhoneVolumeSyncReg(BtVolumeInfo *pVolumeInfo)
{
	(void)pVolumeInfo;
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();
	pBtCurInfo->IsRegVolumeSync = TRUE;
}



static void _HeadPhoneVolumeSyncSet(BtVolumeSet *pVolumeInfo)
{
	(void)pVolumeInfo;
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();

	if(pBtCurInfo->IsRegVolumeSync == TRUE)
	{
		if(memcmp(&(pBtCurInfo->RemoteDev[HEADPHONE_DEV].bd_addr),&pVolumeInfo->addr,sizeof(BtAddr_t))==0)
		{
			//BtServiceAvrcpNotifyVolumeChange(&pVolumeInfo->addr,(UINT8)pVolumeInfo->volume,HEADPHONE_ADDR);
		}
	}
}


static void _HeadPhoneBtReadyAction(void)
{
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();
	BtAddr_t HeadPhoneAddr = {{0}};
	int res =FAIL;

	BtAutoClear(ROLE_HEADPHONE);

	/*if(BtServiceSubIsFirstConnect())
	{
		BtAddr_t WssAddr = {{0}};
		BtServiceGetWssAddr(&WssAddr);
		BtServiceSetDenyConnect(&WssAddr,ON);
		pBtCurInfo->RemoteDev[WSS_DEV].bd_addr = WssAddr;
	}*/

	//BtServiceWssSetMode(HEADPHONE);
	res = _HeadPhoneGetAddr(&HeadPhoneAddr);

	bt_ao_error_printf("\n res:%d,HeadPhoneAddr:%x,%x \n",res,HeadPhoneAddr.address[0],HeadPhoneAddr.address[1]);

	if(res == SUCCESS)
	{
		//BtServiceSetDenyConnect(&HeadPhoneAddr,OFF);
		pBtCurInfo->RemoteDev[HEADPHONE_DEV].bd_addr = HeadPhoneAddr;
		BtAutoSet(ON,ROLE_HEADPHONE,&HeadPhoneAddr);
	}
	else
	{
		//BtServiceStartSearch();
	}
}


static void _HeadPhonePairingAction(BTPairDev *pPairInfo)
{
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();

	if(memcmp(&(pBtCurInfo->RemoteDev[HEADPHONE_DEV].bd_addr),&pPairInfo->bd_addr,sizeof(BtAddr_t))== 0)
	{
		pBtCurInfo->RemoteDev[HEADPHONE_DEV].BtPairState = PAIRING;
	}
}



static int _HeadPhoneConnectState(BT_PROFILE_STATE_e *pState)
{
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();

	if(pState != NULL)
	{
		*pState = BT_A2DP_PROFILE(pBtCurInfo->RemoteDev[HEADPHONE_DEV].BtConnState);
		return SUCCESS;
	}

	return FAIL;
}


static void _HeadPhoneConnectOkAction(stBtDevInfo *pDev)
{
	int role = ROLE_HEADPHONE;

	if(pDev == NULL || pDev->DevRole != HEADPHONE_ADDR)
	{
		return;
	}

	//BtServiceSourceSendStart(&pDev->bd_addr);
	BtAutoClear(role);
	BtAutoClearCurRetryTimes(role);
	_HeadPhoneSaveDevInfo(pDev);
}


static void _HeadPhoneDisconnectOkAction(stBtDevInfo *pDev)
{
	int state = FALSE;
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();

	if(pDev == NULL || pDev->DevRole != HEADPHONE_ADDR)
	{
		return;
	}

	if(_IsHeadPhoneAddr(&pDev->bd_addr,&state))
	{
		_HeadPhoneSaveDevInfo(pDev);
		pBtCurInfo->IsRegVolumeSync = FALSE;
	}
}


static void _HeadPhoneDisconnect(BtAddr_t *pAddr)
{
	(void)pAddr;
	//BtServiceA2dpDisconnect(pAddr,HEADPHONE_ADDR);
}



static int _IsHeadPhoneAddr(BtAddr_t *pAddr,int *pState)
{
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();

	if(memcmp(&(pBtCurInfo->RemoteDev[HEADPHONE_DEV].bd_addr),pAddr,sizeof(BtAddr_t))==0)
	{
		*pState = TRUE;
		return TRUE;
	}

	*pState = FALSE;
	return FALSE;
}



static void _HeadPhoneConnect(BtAddr_t *pAddr)
{
	(void)pAddr;
	//BtServiceA2dpConnect(pAddr,HEADPHONE_ADDR);
}



static void _HeadPhoneSaveDevInfo(stBtDevInfo *pDev)
{
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();
	memcpy(&(pBtCurInfo->RemoteDev[HEADPHONE_DEV]),pDev,sizeof(stBtDevInfo));
}


static void _HeadPhoneSearchResultAction(const char **pBTDesName,BTSearchDevInfo *pBtSearchInfo)
{
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();
	stBtDevInfo *pHeadPhoneDev = &pBtCurInfo->RemoteDev[HEADPHONE_DEV];
	BT_PROFILE_STATE_e state = DISCONNECT_OK;
	int i = 0;

	_HeadPhoneConnectState(&state);

	if(state != DISCONNECT_OK /*|| !BtServiceIsSearching()*/)
	{
		return;
	}

	for(i=0;i<BT_HEADPHONE_NUN;i++)
	{
		if((pBTDesName!=NULL)&&(memcmp(pBTDesName[i],pBtSearchInfo->name,strlen(pBTDesName[i]))==0))
		{
			/*if(BtServiceIsSearching())
			{
				BtServiceStopSearch();
			}*/

			memset(pHeadPhoneDev,0,sizeof(stBtDevInfo));
			pHeadPhoneDev->bd_addr = pBtSearchInfo->BtAddr;
			memcpy(pHeadPhoneDev->name,pBtSearchInfo->name,pBtSearchInfo->NameLen);
			pHeadPhoneDev->name_len = pBtSearchInfo->NameLen;
			return;
		}
	}

}



static void _HeadPhoneSearchFinishAction(void)
{
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();
	stBtDevInfo *pHeadPhoneDev = &pBtCurInfo->RemoteDev[HEADPHONE_DEV];
	BT_PROFILE_STATE_e state = DISCONNECT_OK;
	BtAddr_t HeaadPhoneTempAddr = {{0}};

	if(memcmp(pHeadPhoneDev->bd_addr.address,HeaadPhoneTempAddr.address,sizeof(BtAddr_t)) != 0)
	{
		if(pHeadPhoneDev->BtPairState == UNPAIR)
		{
			//connect target
			//BtServicePairStart(&(pHeadPhoneDev->bd_addr));
		}
		else if(pHeadPhoneDev->BtPairState == PAIRED && pHeadPhoneDev->BtConnState == DISCONNECT_OK)
		{
			//BtServiceA2dpConnect(&(pHeadPhoneDev->bd_addr),HEADPHONE_ADDR);
		}

		return;
	}

	_HeadPhoneConnectState(&state);

	if(state == DISCONNECT_OK)
	{
		bt_ao_printf("\n StartSearch again! \n");
		//BtServiceStartSearch();
	}

}



static int _HeadPhoneGetAddr(BtAddr_t *pAddr)
{
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();
	BtAddr_t HeaadPhoneAddr = {{0}};

	if(pAddr != NULL)
	{
		if(memcmp(&pBtCurInfo->RemoteDev[HEADPHONE_DEV].bd_addr,&HeaadPhoneAddr,sizeof(BtAddr_t)) == 0)
		{
			//int res = BtServiceGetAutoHeadPhoneAddr(pAddr);
			int res = 0;

			if(res ==SUCCESS)
			{
				pBtCurInfo->RemoteDev[HEADPHONE_DEV].bd_addr = *pAddr;
			}
			else
			{
				return FAIL;
			}
		}
		else
		{
			memcpy(pAddr,&pBtCurInfo->RemoteDev[HEADPHONE_DEV].bd_addr,sizeof(BtAddr_t));
		}

		return SUCCESS;
	}

	return FAIL;
}



static void _HeadPhonePairFinishAction(BTPairDev *pPairInfo)
{
	BtAddr_t HeaadPhoneAddr = {{0}};
	stBtTwsMgr *pBtCurInfo = _GetBtTwsCurInfo();

	_HeadPhoneGetAddr(&HeaadPhoneAddr);
	if(memcmp(&HeaadPhoneAddr,&pPairInfo->bd_addr,sizeof(BtAddr_t))== 0)
	{
		pBtCurInfo->RemoteDev[HEADPHONE_DEV].BtPairState = PAIRED;
		//BtServiceA2dpConnect(&pPairInfo->bd_addr,HEADPHONE_ADDR);
	}
}



static int _HeadPhonePlay(void)
{
	BtAddr_t Addr = {{0}};
	_HeadPhoneGetAddr(&Addr);

	/*if(BtServiceIsA2dpConnected(&Addr))
	{
		BtServiceSendAudioSync(1,&Addr);
	}*/

	return SUCCESS;
}



static int _HeadPhonePause(void)
{
	BtAddr_t Addr = {{0}};
	_HeadPhoneGetAddr(&Addr);

	/*if(BtServiceIsA2dpConnected(&Addr))
	{
		BtServiceSendAudioSync(0,&Addr);
	}*/

	return SUCCESS;
}


stBtTwsCtrlApi *BtGetTwsAction(void)
{
	return &BtTwsCtrlApi;
}



int BtTwsRegisterCbk(TwsCbkPost *pTwsCbk)
{
	if(pTwsCbk)
	{
		pTwsPost = pTwsCbk;
		return SUCCESS;
	}

	return FAIL;
}
#endif//__BT_AO_TWS_C__

