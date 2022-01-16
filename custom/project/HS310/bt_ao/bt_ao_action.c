#ifndef __BT_AO_ACTION_C__
#define __BT_AO_ACTION_C__

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
#include <stdio.h>
#include "types.h"
#include "initcall.h"
#include "std_interfaces.h"
#define LOG_EXTRA_STR  "[AO]"
#define LOG_MODULE_ID  ID_BT
#include "log_utils.h"
#include "bt_mw_core.h"
#include "bt_ao_action.h"
#include "bt_ao_auto.h"
#include "bt_ao_tws.h"
#include "bt_ao_remote.h"
#include "bt_ao_phone.h"


#define TWS_ACTION(P_CB, P_CBACK, ...)\
    if (P_CB && P_CB->P_CBACK) {            \
        P_CB->P_CBACK(__VA_ARGS__);         \
    }                                       \
    else {                                  \
        bt_ao_ntc_printf("tws_action null!");  \
    }


#define REMOTEAPP_ACTION(P_CB, P_CBACK, ...)\
    if (P_CB && P_CB->P_CBACK) {            \
        P_CB->P_CBACK(__VA_ARGS__);         \
    }                                       \
    else {                                  \
        bt_ao_ntc_printf("removeapp_action null!");  \
    }


#define SOURCE_ACTION(P_CB, P_CBACK, ...)\
    if (P_CB && P_CB->P_CBACK) {            \
        P_CB->P_CBACK(__VA_ARGS__);         \
    }                                       \
    else {                                  \
        bt_ao_ntc_printf("source_action null!");  \
    }


typedef struct stBtDevActionMgr
{
	stBtDevAction *pSourceAction;
	stBtTwsAction *pSinkAction;
}BtDevActionMgr;

UINT32 dRandTimes = 0;


//Sunplus//static stBtTwsCtrlApi *pTwsAction = NULL;
static BtDevActionMgr BtActionMgr = {NULL,NULL};
static stBTInfo *pBtSdkCurInfo = NULL;
static SINK_MODE_e TwsSinkMode = SINK_NONE;
extern int BtRegTwsCBkFunc(void);

static stBTInfo * _GetBtCurInfo(void);
static void _MainBtDisconnect(BtAddr_t *pAddr);
static void _MainBtConnetOkAction(stBtDevInfo *pDev);
static void _MainBtDisconnetOkAction(stBtDevInfo *pDev);
static int _MainBtGetRemoteAppAction(stBtRemoteAppAction **pRemoteAppAction);
static int _MainBtConnectTimeOutHandle(BtAddr_t *pAddress);



//================for main=============//
//for main api
static stBTInfo * _GetBtCurInfo(void)
{
	return pBtSdkCurInfo;
}


static BtDevActionMgr * _GetBtMgrAction(void)
{
	return &BtActionMgr;
}



static int _MainBtIsPlaying(int *pIsTrue)
{
		BT_IR_CMD_e state = AV_PLAY;
		(void)state;

		PhoneAudioStateGet(&state);

		if(AV_PLAY == state)
		{
			*pIsTrue = TRUE;
			return TRUE;
		}

		*pIsTrue = FALSE;
		return FALSE;
}

/*{
	BT_IR_CMD_e state;
	BtAddr_t BtAddr = {{0}};
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	SOURCE_ACTION(pBtAction->pSourceAction,GetAddr,&BtAddr);
	BtServiceGetAudioState(&BtAddr,&state);

	if(AV_PLAY == state)
	{
		*pIsTrue = TRUE;
		return TRUE;
	}

	*pIsTrue = FALSE;
	return FALSE;
}*/


static void _MainGetAddrByDevRole(BtAddr_t *pBtAddr,DEVROLE_DEX_e dex)
{
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	if(pBtAddr != NULL)
	{
		switch(dex)
		{
			case WSS_ADDR:
			case HEADPHONE_ADDR:
			{
				TWS_ACTION(pBtAction->pSinkAction,GetAddr,pBtAddr);
			}
			break;
			case PHONE_ADDR:
			{
				SOURCE_ACTION(pBtAction->pSourceAction,GetAddr,pBtAddr);
			}
			break;
			case REMOTEAPP_ADDR:
			{
				stBtRemoteAppAction *pRemoteAction = NULL;
				_MainBtGetRemoteAppAction(&pRemoteAction);
				REMOTEAPP_ACTION((pRemoteAction),GetAddr,pBtAddr);
			}
			break;
			default:
			{
				break;
			}
		}
	}
}



static void _MainGetConnectStateByDevRole(DEVROLE_DEX_e dex,BT_PROFILE_STATE_e *pState)
{
		switch(dex)
		{
			case PHONE_ADDR:
			{
				PhoneConnectStateGet(pState);
				bt_ao_ntc_printf("PhoneConnectStateGet:%d",*pState);
				break;
			}
			case REMOTEAPP_ADDR:
			{
				stBtRemoteAppAction *pRemoteAction = BtGetRemoteAppAction();;
				REMOTEAPP_ACTION((pRemoteAction),GetConnectState,pState);
				break;
			}
			default:
			{
				break;
			}
		}

}

/*{
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	switch(dex)
	{
		case WSS_ADDR:
		case HEADPHONE_ADDR:
		{
			TWS_ACTION(pBtAction->pSinkAction,GetConnectState,pState);
			break;
		}
		case PHONE_ADDR:
		{
			SOURCE_ACTION(pBtAction->pSourceAction,GetConnectState,pState);
			break;
		}
		case REMOTEAPP_ADDR:
		{
			stBtRemoteAppAction *pRemoteAction = NULL;
			_MainBtGetRemoteAppAction(&pRemoteAction);
			REMOTEAPP_ACTION((pRemoteAction),GetConnectState,pState);
			break;
		}
		default:
		{
			break;
		}
	}

}*/


static void _MainPairingAction(BTPairDev *pPairInfo)
{
		(void)pPairInfo;
		stBTInfo *pCurInfo = _GetBtCurInfo();

		pCurInfo->BtPairState = PAIRING;
		pCurInfo->BaseState = pairing;
}

/*{
	stBTInfo *pCurInfo = _GetBtCurInfo();
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	pCurInfo->BtPairState = PAIRING;
	pCurInfo->BaseState = pairing;

	if(pCurInfo->BtSurportTws == ON)
	{
		TWS_ACTION((pBtAction->pSinkAction),Pairing,pPairInfo);
	}
}*/


static int _MainBtConnectTimeOutHandle(BtAddr_t *pAddress)
{
		stBTInfo *pBtCurInfo = _GetBtCurInfo();

		if(pBtCurInfo->IsIgnoreTimeout)
		{
			//bt_ao_printf("IgnoreTimeout !");
			return SUCCESS;
		}

		if(pAddress != NULL)
		{
			int state = FALSE;
			IsPhoneAddr(pAddress,&state);

			if(state)
			{
				BtSetCurAutoState(ON);
				BtSetAutoTimeOutFlag(ON,ROLE_PHONE);
				BtAutoSet(ON,ROLE_PHONE,pAddress);
				return SUCCESS;
			}
		}

		return FAIL;
}

/*{
	stBTInfo *pBtCurInfo = _GetBtCurInfo();
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	if(pBtCurInfo->IsIgnoreTimeout)
	{
		//bt_ao_printf("IgnoreTimeout !");
		return SUCCESS;
	}

	if(pAddress != NULL)
	{
		int state = FALSE;
		SOURCE_ACTION(pBtAction->pSourceAction,IsEqualAddr,pAddress,&state);

		if(state)
		{
			BtSetCurAutoState(ON);
			BtSetAutoTimeOutFlag(ON,ROLE_PHONE);
			BtAutoSet(ON,ROLE_PHONE,pAddress);
			return SUCCESS;
		}
		else
		{
			if(pBtCurInfo->BtSurportTws==ON && pBtCurInfo->BtTwsMode==HEADPHONE)
			{
				int state = FALSE;

				TWS_ACTION((pBtAction->pSinkAction),IsEqualAddr,pAddress,&state);

				if(state)
				{
					BtSetCurAutoState(ON);
					BtSetAutoTimeOutFlag(ON,ROLE_HEADPHONE);
					BtAutoSet(ON,ROLE_HEADPHONE,pAddress);
					return SUCCESS;
				}
			}
		}
	}

	return FAIL;
}*/



/*static int _MainBtTwsInit(SINK_MODE_e role, char IsSurportSwitch)
{
	pTwsAction = BtGetTwsAction();

	if(pTwsAction != NULL && pTwsAction->BtTwsInit != NULL)
	{
		BtRegTwsCBkFunc();
		pTwsAction->BtTwsInit(role,&(BtActionMgr.pSinkAction),IsSurportSwitch);
		return SUCCESS;
	}

	return FAIL;
}*/


//bt dev for remote app
static int _MainBtGetRemoteAppAction(stBtRemoteAppAction **pRemoteAppAction)
{
	*pRemoteAppAction = BtGetRemoteAppAction();

	if(pRemoteAppAction == NULL)
	{
		return FAIL;
	}

	return SUCCESS;
}


static void _MainBtAutoConnectCheck(int *pResult,int *pRole)
{
		stBTInfo *pBtCurInfo = _GetBtCurInfo();

		if(BtGetAutoConnectWorkState() == ON)//just for config
		{
			if(BtGetCurAutoState() == OFF)
			{
				*pResult = FAIL;
				return;
			}

			if(pBtCurInfo->BtState != BT_READY)
			{
				//bt_ao_printf("bt no ready !\n");
				*pResult = FAIL;
				return;
			}

			if(!(pBtCurInfo->BaseState==idle || pBtCurInfo->BaseState==connected))
			{
				//bt_ao_printf("basestate: no idel\n");
				int *pConnectTime = BtGetAutoCounterTime(ROLE_PHONE);
				*pConnectTime = 0;
				*pResult = FAIL;
				return;
			}

			*pRole = ROLE_PHONE;

			if(pBtCurInfo->IsBtMode != TRUE)
			{
				//bt_ao_printf("no bt mode\n");
				int *pConnectTime = BtGetAutoCounterTime(ROLE_PHONE);
				*pConnectTime = 0;
				*pResult = FAIL;
				return;
			}
			else
			{
				if(BtAutoConnectCheck(*pRole) == SUCCESS)
				{
					int *pConnectTime = BtGetAutoCounterTime(*pRole);
					*pConnectTime = 0;
					*pResult = SUCCESS;
					return;
				}

			}
		}

		*pResult = FAIL;
		return;
	}

/*{
	stBTInfo *pBtCurInfo = _GetBtCurInfo();
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	if(BtGetAutoConnectWorkState() == ON)//just for config
	{
		if(BtGetCurAutoState() == OFF)
		{
			*pResult = FAIL;
			return;
		}

		if(pBtCurInfo->BtState != BT_READY)
		{
			//bt_ao_printf("bt no ready !\n");
			*pResult = FAIL;
			return;
		}

		if(!(pBtCurInfo->BaseState==idle || pBtCurInfo->BaseState==connected))
		{
		 	//bt_ao_printf("basestate: no idel\n");
		 	int *pConnectTime = BtGetAutoCounterTime(ROLE_PHONE);
			*pConnectTime = 0;
			*pResult = FAIL;
			return;
		}

#if 0
		if(pBtCurInfo->BtPairState == UNPAIR)
		{
			int *pConnectTime = BtGetAutoCounterTime(ROLE_PHONE);
			*pConnectTime = 0;
			*pResult = FAIL;
			return;
		}
#endif
		if(BtServiceIsSearching())
		{
			//bt_ao_printf("is searching !\n");
			*pResult = FAIL;
			return;
		}

		if((pBtCurInfo->BtSurportTws == ON) && (pBtCurInfo->IsSurportSwitch == TRUE))
		{

			if(pBtCurInfo->BtTwsMode == HEADPHONE)
			{
				*pRole = ROLE_HEADPHONE;


			}
			else if(pBtCurInfo->BtTwsMode == WSS)
			{
				*pRole = ROLE_WSS;
			}

			if(BtAutoConnectCheck(*pRole) == SUCCESS)
			{
				*pResult = SUCCESS;
				return;
			}

			*pRole = ROLE_PHONE;

			if(pBtCurInfo->IsBtMode != TRUE || pBtCurInfo->BtTwsMode != WSS)
			{
				//bt_ao_printf("no bt mode\n");
				int *pConnectTime = BtGetAutoCounterTime(ROLE_PHONE);
				*pConnectTime = 0;
				*pResult = FAIL;
				return;
			}
			else
			{

				if(!(BtIsAutoConnectFail(ROLE_PHONE))&& !(BtGetAutoTimeOutFlag(ROLE_PHONE)))
				{
					BT_PROFILE_STATE_e state = DISCONNECT_OK;
					TWS_ACTION(pBtAction->pSinkAction,GetConnectState,&state);

					if(state != CONNECT_OK)
					{
						//bt_ao_printf("sub no connect!\n");
						int *pConnectTime = BtGetAutoCounterTime(ROLE_PHONE);
						*pConnectTime = 0;
						*pResult = FAIL;
						return;
					}
				}

				if(BtAutoConnectCheck(*pRole) == SUCCESS)
				{
					int *pConnectTime = BtGetAutoCounterTime(*pRole);
					*pConnectTime = 0;
					*pResult = SUCCESS;
					return;
				}
			}

		}
		else
		{
			*pRole = ROLE_PHONE;

			if(pBtCurInfo->IsBtMode != TRUE)
			{
				//bt_ao_printf("no bt mode\n");
				int *pConnectTime = BtGetAutoCounterTime(ROLE_PHONE);
				*pConnectTime = 0;
				*pResult = FAIL;
				return;
			}
			else
			{
				if(BtAutoConnectCheck(*pRole) == SUCCESS)
				{
					int *pConnectTime = BtGetAutoCounterTime(*pRole);
					*pConnectTime = 0;
					*pResult = SUCCESS;
					return;
				}

			}
		}
	}

	*pResult = FAIL;
	return;
}*/
/******************************************************************************************/
/**
 * \fn		void _MainBtAutoConnect(int AutoRole)
 *
 * \brief		do some action(disconnect phone and inform sub doing power off,when they are connecting),when platform power off
 *
 * \param
 *              		none
 *
 * \return	    void
 *
 ******************************************************************************************/
void _MainBtAutoConnect(int AutoRole)
{
	BtAutoCon(AutoRole);
	return;
}
/******************************************************************************************/
/**
 * \fn		void _MainBtPowerOffRes(void)
 *
 * \brief		do some action(disconnect phone and inform sub doing power off,when they are connecting),when platform power off
 *
 * \param
 *              		none
 *
 * \return	    void
 *
 ******************************************************************************************/
static void _MainBtPowerOffRes(void)
{
		BtAddr_t BtAddr = {{0}};
		stBTInfo *pBtCurInfo = _GetBtCurInfo();
		BT_PROFILE_STATE_e state = DISCONNECT_OK;

		if(pBtCurInfo->IsBtMode == TRUE || pBtCurInfo->IsIgnoreSrcSwitch)
		{
			PhoneConnectStateGet(&state);

			if(state == CONNECT_OK)
			{
				PhoneDisconnect(&BtAddr);
			}
		}

}

/*{
	BtAddr_t BtAddr = {{0}};
	stBTInfo *pBtCurInfo = _GetBtCurInfo();
	BT_PROFILE_STATE_e state = DISCONNECT_OK;
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	if(pBtCurInfo->IsBtMode == TRUE || pBtCurInfo->IsIgnoreSrcSwitch)
	{
		SOURCE_ACTION(pBtAction->pSourceAction,GetAddr,&BtAddr);
		BtServiceGetProfileState(&BtAddr,A2DP_SINK,&state);

		if(state == CONNECT_OK)
		{
			SOURCE_ACTION(pBtAction->pSourceAction,Disconnect,&BtAddr);
		}
	}

	if(pBtCurInfo->BtSurportTws == ON)
	{
		TWS_ACTION((pBtAction->pSinkAction),GetAddr,&BtAddr);
		BtServiceGetProfileState(&BtAddr,A2DP_SOURCE,&state);

		if(state == CONNECT_OK)
		{
			TWS_ACTION((pBtAction->pSinkAction),Disconnect,&BtAddr);
		}
	}

}*/






static void _MainConfigInfoInit(void *pBtMainConf)
{

		stBTInfo *pBtConf = (stBTInfo *)pBtMainConf;
		BtAutoConConfig AutoCnfInfo;
		stBtConfig BtCnfSet;

		pBtSdkCurInfo = pBtConf;

		if(pBtMainConf == NULL)
		{
			return ;
		}

		AutoCnfInfo.AutoLatency = pBtConf->AutoLatency;
		AutoCnfInfo.RetryTimes = pBtConf->RetryTimes;
		AutoCnfInfo.WorkState = pBtConf->BtSurportAutoConnet;
		AutoCnfInfo.role = pBtConf->AutoRole;
		AutoCnfInfo.CurAutoState = OFF;

		BtAutoInit(&AutoCnfInfo);
		BtSourceInit();

		TwsSinkMode = SINK_NONE;
		BtCnfSet.DevMaxConnect = BT_SINK_DEV_NUM;
}

/*{

	stBTInfo *pBtConf = (stBTInfo *)pBtMainConf;
	BtAutoConConfig AutoCnfInfo;
	stBtConfig BtCnfSet;

	pBtSdkCurInfo = pBtConf;

	if(pBtMainConf == NULL)
	{
		return ;
	}

	AutoCnfInfo.AutoLatency = pBtConf->AutoLatency;
	AutoCnfInfo.RetryTimes = pBtConf->RetryTimes;
	AutoCnfInfo.WorkState = pBtConf->BtSurportAutoConnet;
	AutoCnfInfo.role = pBtConf->AutoRole;
	AutoCnfInfo.CurAutoState = OFF;

	BtAutoInit(&AutoCnfInfo);

	BtCnfSet.IsBtSimpleModeEn = pBtConf->IsBtSimpleModeEn;
	BtCnfSet.SystemRole = pBtConf->PlatformRole;
	BtCnfSet.BitPoolValue = pBtConf->BitPoolValue;
	BtCnfSet.IsBtAacCodecEn = pBtConf->IsBtAacCodecEn;

	BtSourceInit(&BtActionMgr.pSourceAction);

	if(pBtConf->BtSurportTws == ON)
	{
		_MainBtTwsInit(pBtConf->BtTwsMode,pBtConf->IsSurportSwitch);
		TwsSinkMode = pBtConf->BtTwsMode;
		BtCnfSet.DevMaxConnect = BT_SINK_DEV_NUM + BT_SOURCE_DEV_NUM;
		BtServiceConfigSet(&BtCnfSet);
	}
	else
	{
		TwsSinkMode = SINK_NONE;
		BtCnfSet.DevMaxConnect = BT_SINK_DEV_NUM;
		BtServiceConfigSet(&BtCnfSet);
	}

}*/


static int _MainSwitchSinkMode(SINK_MODE_e SinkMode)
{
   (void)SinkMode;
   return 0;
}
/*{
	stBTInfo *pBtCurInfo = _GetBtCurInfo();
	BtDevActionMgr *pBtAction = _GetBtMgrAction();
	BtAddr_t addr = {{0}};
	int res = FAIL;
	stBtDevInfo BtDev;

	if(pBtCurInfo->BtSurportTws != ON || SinkMode == pBtCurInfo->BtTwsMode)
	{
		return FAIL;
	}

	BtServiceStopSearch();
	BtServiceSetScanMode(SCAN_NONE);
	TWS_ACTION(pBtAction->pSinkAction,GetAddr,&addr);
	memset(&BtDev,0,sizeof(stBtDevInfo));
	res = BtServiceGetRemoteDevInfo(&addr,&BtDev);

	if(BtDev.BtConnState == CONNECT_OK && res != FAIL)
	{
		pBtCurInfo->SwitchFlag = TRUE;
		TWS_ACTION(pBtAction->pSinkAction,Disconnect,&addr);
	}
	else
	{
		if((res == FAIL) ||((res != FAIL) && (pBtCurInfo->SwitchFlag != TRUE && BtDev.BtConnState == DISCONNECT_OK)))
		{
			stBtTwsCtrlApi *pAction = pTwsAction;
			pBtCurInfo->SwitchFlag = FALSE;

			if(SinkMode == HEADPHONE)
			{
				TWS_ACTION(pAction,BtTwsSwitchActionInit,HEADPHONE);
			}
			else if(SinkMode == WSS)
			{
				TWS_ACTION(pAction,BtTwsSwitchActionInit,WSS);
			}
		}
		else if(BtDev.BtConnState != DISCONNECT_OK)
		{
			pBtCurInfo->SwitchFlag = TRUE;
		}
	}

	TwsSinkMode = SinkMode;
	return SUCCESS;
}*/



static int _MainSwitchSinkFinish(SINK_MODE_e SinkMode)
{
	   (void)SinkMode;
	   return 0;
}
/*{
	BtAddr_t BtAddr = {{0}};
	stBTInfo *pBtCurInfo = _GetBtCurInfo();
	BtDevActionMgr *pAction = _GetBtMgrAction();
	BT_PROFILE_STATE_e state = DISCONNECT_OK;

	if(pBtCurInfo->BtSurportTws == ON)
	{
		BtServiceStopSearch();

		if(pBtCurInfo->IsBtMode && SinkMode == WSS)
		{
			BtServiceSetScanMode(BOTH_DISCOVERY_CONNECTABLE);
		}
		else
		{
			BtServiceSetScanMode(ONLY_CONNECTABLE);
		}

		if(SinkMode == HEADPHONE)
		{
			SOURCE_ACTION(pAction->pSourceAction,GetConnectState,&state);

			if(state == CONNECT_OK)
			{
				SOURCE_ACTION(pAction->pSourceAction,GetAddr,&BtAddr);
				SOURCE_ACTION(pAction->pSourceAction,Disconnect,&BtAddr);
			}
		}

		TWS_ACTION(pTwsAction,BtTwsGetSinkAction,&(pAction->pSinkAction));

		return SUCCESS;
	}

	return FAIL;
}*/


int IsBtModeFlag = FALSE;
int FirstAutoConFlag = FALSE;

static int _MainBtModeOnAction(void)
{
		stBTInfo *pBtCurInfo = _GetBtCurInfo();

		bt_ao_ntc_printf("\n %s ",__FUNCTION__);

		if(pBtCurInfo->BtState != BT_READY)
		{
			return FAIL;
		}

		IsBtModeFlag = TRUE;
		FirstAutoConFlag = TRUE;

		SDK_BASE_IF_Change_Discoverable(ON);
		SDK_BASE_IF_Change_Connectable(ON);

		return SUCCESS;
}

/*{
	stBTInfo *pBtCurInfo = _GetBtCurInfo();

	bt_ao_ntc_printf("\n %s ",__FUNCTION__);

	if(pBtCurInfo->BtState != BT_READY)
	{
		return FAIL;
	}

	if(pBtCurInfo->BtSurportTws == ON && pBtCurInfo->BtTwsMode == HEADPHONE)
	{
		BtServiceSetScanMode(ONLY_CONNECTABLE);
	}
	else
	{
		BtServiceSetScanMode(BOTH_DISCOVERY_CONNECTABLE);
	}

	return SUCCESS;
}*/



static int _MainBtModeOffAction(void)
{
		BtAddr_t BtAddr = {{0}};
		BT_PROFILE_STATE_e state = DISCONNECT_OK;
		stBTInfo *pBtCurInfo = _GetBtCurInfo();

		bt_ao_printf("%s \n",__FUNCTION__);

		if(!pBtCurInfo->IsIgnoreSrcSwitch)
		{
			PhoneConnectStateGet(&state);

			if(state == CONNECT_OK)
			{
				_MainGetAddrByDevRole(&BtAddr,PHONE_ADDR);

				if(BtGetAutoConnectWorkState() == ON)
				{
					if(BtGetSurportAutoRole() == AUTO_PHONE || BtGetSurportAutoRole() == AUTO_PHONE_TWS)
					{
						BtSetCurAutoState(ON);
						BtAutoSet(ON,ROLE_PHONE,&BtAddr);
					}
				}
				else
				{
					BtSetCurAutoState(OFF);
					BtAutoSet(OFF,ROLE_PHONE,&BtAddr);
				}
				_MainBtDisconnect(&BtAddr);
				BtDoSppDisconnect(&BtAddr);
			}
			else
			{
				if(BtGetAutoState(ROLE_PHONE) != ON)
				{
					BtSetAutoState(OFF,ROLE_PHONE);
				}
			}
		}

        IsBtModeFlag = FALSE;

		SDK_BASE_IF_Change_Connectable(OFF);
		SDK_BASE_IF_Change_Discoverable(OFF);

		return SUCCESS;
}

/*{
	BtAddr_t BtAddr = {{0}};
	BT_PROFILE_STATE_e state = DISCONNECT_OK;
	stBTInfo *pBtCurInfo = _GetBtCurInfo();

	bt_ao_printf("%s \n",__FUNCTION__);

	if(!pBtCurInfo->IsIgnoreSrcSwitch)
	{
		_MainGetConnectStateByDevRole(PHONE_ADDR,&state);

		if(state == CONNECT_OK)
		{
			_MainGetAddrByDevRole(&BtAddr,PHONE_ADDR);

			if(BtGetAutoConnectWorkState() == ON)
			{
				if(BtGetSurportAutoRole() == AUTO_PHONE || BtGetSurportAutoRole() == AUTO_PHONE_TWS)
				{
					BtSetCurAutoState(ON);
					BtAutoSet(ON,ROLE_PHONE,&BtAddr);
				}
			}
			else
			{
				BtSetCurAutoState(OFF);
				BtAutoSet(OFF,ROLE_PHONE,&BtAddr);
			}

			_MainBtDisconnect(&BtAddr);
		}
		else
		{
			if(BtGetAutoState(ROLE_PHONE) != ON)
			{
				BtSetAutoState(OFF,ROLE_PHONE);
			}
		}
	}

	if(pBtCurInfo->BtSurportTws == ON)
	{
		if(pBtCurInfo->BtTwsMode == WSS)
		{
			if(BtServiceSubIsFirstConnect())
			{
				bt_ao_printf("\n Sub First Connected!");
				BT_PROFILE_STATE_e SubState = DISCONNECT_OK;
				_MainGetConnectStateByDevRole(WSS_ADDR,&SubState);

				if(SubState != CONNECT_OK)
				{
					BtServiceSetScanMode(ONLY_CONNECTABLE);
				}
				else
				{
					BtServiceSetScanMode(SCAN_NONE);
				}
			}
			else
			{
				BtServiceSetScanMode(BOTH_DISCOVERY_CONNECTABLE);
			}
		}
		else
		{
			BT_PROFILE_STATE_e HeadPhoneState = DISCONNECT_OK;
			_MainGetConnectStateByDevRole(HEADPHONE_ADDR,&HeadPhoneState);

			if(HeadPhoneState != CONNECT_OK)
			{
				BtServiceSetScanMode(ONLY_CONNECTABLE);
			}
			else
			{
				BtServiceSetScanMode(SCAN_NONE);
			}
		}
	}
	else
	{
		BtServiceSetScanMode(SCAN_NONE);
	}

	return SUCCESS;
}*/



static void _MainBtConnetOkAction(stBtDevInfo *pDev)
{
		stBTInfo *pCurInfo = _GetBtCurInfo();

		pCurInfo->DevConnectCounter++;
		bt_ao_ntc_printf("\n DevConnectCounter = %d\n",pCurInfo->DevConnectCounter);

		if(pCurInfo->IsBtMode == TRUE)
		{
			int role = ROLE_PHONE;

			BtAutoClear(role);
			BtAutoClearCurRetryTimes(role);
		}
		else
		{
			PhoneDisconnect(&pDev->bd_addr);
		}

		/*if((pCurInfo->DevConnectCounter >= pCurInfo->DevMaxConnect))
		{
			SDK_BASE_IF_Change_Connectable(OFF);
			SDK_BASE_IF_Change_Discoverable(OFF);
		}*/ //zehai.liu todo

		PhoneSaveDevInfo(pDev);

		SDK_BASE_IF_Change_Connectable(OFF);
		SDK_BASE_IF_Change_Discoverable(OFF);
}

/*{
	stBTInfo *pCurInfo = _GetBtCurInfo();
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	pCurInfo->DevConnectCounter++;
	bt_ao_ntc_printf("\n DevConnectCounter = %d\n",pCurInfo->DevConnectCounter);

	if(pDev->DevRole == PHONE_ADDR)
	{
		int IsTrue = TRUE;

		if(pCurInfo->IsBtMode)
		{
			if(pCurInfo->BtTwsMode == HEADPHONE)
			{
				IsTrue = FALSE;
			}
		}
		else
		{
			IsTrue = FALSE;
		}

		SOURCE_ACTION(pBtAction->pSourceAction,ConnectOk,pDev,IsTrue);
	}
	else if(pDev->DevRole == HEADPHONE_ADDR)
	{
		TWS_ACTION((pBtAction->pSinkAction),ConnectOk,pDev);

		if((pCurInfo->SwitchFlag==TRUE && TwsSinkMode!=HEADPHONE) || pCurInfo->BtTwsMode != HEADPHONE)
		{
			TWS_ACTION((pBtAction->pSinkAction),Disconnect,&pDev->bd_addr);
		}
	}
	else if(pDev->DevRole == WSS_ADDR)
	{
		BtAddr_t BtAddr = {{0}};
		BtAddr_t BtTempAddr = {{0}};
		BT_PROFILE_STATE_e state = DISCONNECT_OK;

		SOURCE_ACTION(pBtAction->pSourceAction,GetAddr,&BtAddr);
		SOURCE_ACTION(pBtAction->pSourceAction,GetConnectState,&state);

		if(memcmp(BtAddr.address,BtTempAddr.address,sizeof(BtAddr_t))!=0 && state != CONNECT_OK)
		{
			BtSetCurAutoState(ON);
			BtAutoSet(ON,ROLE_PHONE,&BtAddr);
		}

		TWS_ACTION((pBtAction->pSinkAction),ConnectOk,pDev);

		if((pCurInfo->SwitchFlag==TRUE && TwsSinkMode!=WSS) || pCurInfo->BtTwsMode != WSS)
		{
			TWS_ACTION((pBtAction->pSinkAction),Disconnect,&pDev->bd_addr);
		}
	}

	if((pCurInfo->DevConnectCounter >= pCurInfo->DevMaxConnect)&&(pDev->DevRole != WSS_ADDR))
	{
		BtServiceSetScanMode(SCAN_NONE);
	}
	else
	{
		if(pCurInfo->DevConnectCounter < pCurInfo->DevMaxConnect)
		{
			if(pCurInfo->IsBtMode == TRUE)
			{
				if(pCurInfo->BtSurportTws == ON && pCurInfo->BtTwsMode == WSS)
				{
					BtServiceSetScanMode(BOTH_DISCOVERY_CONNECTABLE);
				}
				else
				{
					if(pDev->DevRole != PHONE_ADDR)
					{
						BtServiceSetScanMode(SCAN_NONE);
					}
				}
			}
			else
			{
				if(pCurInfo->BtSurportTws == ON)
				{
					if(pDev->DevRole != PHONE_ADDR)
					{
						BtServiceSetScanMode(SCAN_NONE);
					}
				}
			}
		}
	}
}*/



static void _MainBtConnectFailAction(stBtDevInfo *pDev)
{
	BT_PROFILE_STATE_e state = DISCONNECT_OK;
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	SOURCE_ACTION(pBtAction->pSourceAction,GetConnectState,&state);

	if(state == DISCONNECT_OK)
	{
		if(pDev->DevRole == PHONE_ADDR)
		{
		 	BtSetAutoTimeOutFlag(TRUE,ROLE_PHONE);
			BtSetCurAutoState(ON);
			BtAutoSet(ON,ROLE_PHONE,&pDev->bd_addr);
			BtSetAutoTime(4,ROLE_PHONE);
		}
	}

}


#if 0
static void _MainBtDisconnetingAction(stBtDevInfo *pDev)
{
	(void)pDev;
	return ;
}
#endif

static void _MainBtDisconnetOkAction(stBtDevInfo *pDev)
{
		BT_PROFILE_STATE_e state = DISCONNECT_OK;
		stBTInfo *pCurInfo = _GetBtCurInfo();
		int IsTrue = TRUE;

		PhoneConnectStateGet(&state);

		if(state == DISCONNECT_OK)
		{
			if(pCurInfo->DevConnectCounter > 0)
				pCurInfo->DevConnectCounter--;

			pCurInfo->IsBtSourceVolumeSyncReg = FALSE;
		}

		if(pCurInfo->IsBtMode == TRUE)
		{
			int role = ROLE_PHONE;

			if(BtIsAutoConnected(role))
			{
				if(BtGetAutoTimeOutFlag(role) != ON)
				{
					if(IsPhoneAddr(&pDev->bd_addr,&IsTrue))
					{
						BtSetAutoState(OFF,role);
					}
				}
			}
			else
			{
				if(IsPhoneAddr(&pDev->bd_addr,&IsTrue))
				{
					BtSetAutoState(ON,role);
				}
			}
		}

		bt_ao_ntc_printf("\n DIS DevCounter = %d,max:%d\n",pCurInfo->DevConnectCounter,pCurInfo->DevMaxConnect);

		if(pCurInfo->IsBtMode == TRUE)
		{
			SDK_BASE_IF_Change_Connectable(ON);
			SDK_BASE_IF_Change_Discoverable(ON);
		}
		else
		{
			SDK_BASE_IF_Change_Connectable(OFF);
			SDK_BASE_IF_Change_Discoverable(OFF);
		}

		/*if(pCurInfo->DevConnectCounter >= pCurInfo->DevMaxConnect)
		{
			SDK_BASE_IF_Change_Connectable(OFF);
			SDK_BASE_IF_Change_Discoverable(OFF);
		}
		else
		{
			if(pCurInfo->IsBtMode == TRUE)
			{
				SDK_BASE_IF_Change_Connectable(ON);
				SDK_BASE_IF_Change_Discoverable(ON);
			}
			else
			{
				SDK_BASE_IF_Change_Connectable(OFF);
				SDK_BASE_IF_Change_Discoverable(OFF);
			}
		}*/

}

/*{
	BT_PROFILE_STATE_e state = DISCONNECT_OK;
	stBTInfo *pCurInfo = _GetBtCurInfo();
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	if(pDev->DevRole == PHONE_ADDR)
	{
		SOURCE_ACTION(pBtAction->pSourceAction,GetConnectState,&state);

		if(state == CONNECT_OK)
		{
			if(pCurInfo->DevConnectCounter > 0)
				pCurInfo->DevConnectCounter--;

			pCurInfo->IsBtSourceVolumeSyncReg = FALSE;
		}

		SOURCE_ACTION(pBtAction->pSourceAction,DisconnectOk,pDev,pCurInfo->IsBtMode);

	}
	else if(pDev->DevRole == WSS_ADDR)
	{
		if(pCurInfo->DevConnectCounter > 0)
			pCurInfo->DevConnectCounter--;

		TWS_ACTION((pBtAction->pSinkAction),DisconnectOk,pDev);

		if(pCurInfo->SwitchFlag)
		{
			BT_PROFILE_STATE_e state = DISCONNECT_OK;

			TWS_ACTION(pBtAction->pSinkAction,GetSppConnectState,&state);

			if(state == DISCONNECT_OK)
			{
				pCurInfo->SwitchFlag = FALSE;
				TWS_ACTION(pTwsAction,BtTwsSwitchActionInit,TwsSinkMode);
			}
		}
	}
	else if(pDev->DevRole == HEADPHONE_ADDR)
	{
		if(pCurInfo->DevConnectCounter > 0)
			pCurInfo->DevConnectCounter--;

		pCurInfo->IsBtSinkVolumeSyncReg = FALSE;

		TWS_ACTION((pBtAction->pSinkAction),DisconnectOk,pDev);

		if(pCurInfo->SwitchFlag)
		{
			pCurInfo->SwitchFlag = FALSE;
			TWS_ACTION(pTwsAction,BtTwsSwitchActionInit,TwsSinkMode);
		}
	}
	else
	{
		bt_ao_error_printf("error role!!!\n");
	}

	bt_ao_ntc_printf("\n DIS DevCounter = %d,max:%d\n",pCurInfo->DevConnectCounter,pCurInfo->DevMaxConnect);

	if(pCurInfo->DevConnectCounter >= pCurInfo->DevMaxConnect)
	{
		BtServiceSetScanMode(SCAN_NONE);
	}
	else
	{
		if(pCurInfo->IsBtMode == TRUE)
		{
			if((pCurInfo->BtSurportTws != ON) || (pCurInfo->BtSurportTws == ON && pCurInfo->BtTwsMode == WSS))
			{
				BtServiceSetScanMode(BOTH_DISCOVERY_CONNECTABLE);
			}
			else
			{
				if(pDev->DevRole != PHONE_ADDR)
				{
					BtServiceSetScanMode(ONLY_CONNECTABLE);
				}
			}
		}
		else
		{
			if(pCurInfo->BtSurportTws == ON)
			{
				if(pDev->DevRole != PHONE_ADDR)
				{
					BtServiceSetScanMode(ONLY_CONNECTABLE);
				}
			}
			else
			{
				BtServiceSetScanMode(SCAN_NONE);
			}
		}
	}

}*/


static void _MainBtDisconnect(BtAddr_t *pAddr)
{
	/*int state = FALSE;
	stBTInfo *pCurInfo = _GetBtCurInfo();
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	if(pCurInfo->BtSurportTws == ON)
	{
		TWS_ACTION((pBtAction->pSinkAction),IsEqualAddr,pAddr,&state);

		if(state)
		{
			TWS_ACTION((pBtAction->pSinkAction),Disconnect,pAddr);
		}
		else
		{
			SOURCE_ACTION(pBtAction->pSourceAction,IsEqualAddr,pAddr,&state);

			if(state)
			{
				SOURCE_ACTION(pBtAction->pSourceAction,Disconnect,pAddr);
			}
			else
			{
				SOURCE_ACTION(pBtAction->pSourceAction,Disconnect,pAddr);//error may be check the flow
			}
		}
	}
	else
	{
		SOURCE_ACTION(pBtAction->pSourceAction,IsEqualAddr,pAddr,&state);

		if(state)
		{
			SOURCE_ACTION(pBtAction->pSourceAction,Disconnect,pAddr);
		}
		else
		{
			SOURCE_ACTION(pBtAction->pSourceAction,Disconnect,pAddr);
		}
	}*/
	PhoneDisconnect(pAddr);

}



static void _MainBtConnect(BtAddr_t *pAddr)
{
	/*int state = FALSE;
	stBTInfo *pCurInfo = _GetBtCurInfo();
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	if((pCurInfo->BtSurportTws == ON))
	{
		TWS_ACTION((pBtAction->pSinkAction),IsEqualAddr,pAddr,&state);

		if(state)
		{
			TWS_ACTION((pBtAction->pSinkAction),Connect,pAddr);
		}
		else
		{
			SOURCE_ACTION(pBtAction->pSourceAction,Connect,pAddr);
		}
	}
	else
	{
		SOURCE_ACTION(pBtAction->pSourceAction,Connect,pAddr);
	}*/
	PhoneConnect(pAddr);

}


static void _MainSearchResultAction(const char **pBTDesName,BTSearchDevInfo *pBtSearchInfo)
{
	/*stBTInfo *pCurInfo = _GetBtCurInfo();
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	if(pCurInfo->BtSurportTws == ON)
	{
		if(!(!pCurInfo->IsSurportSwitch && pCurInfo->BtTwsMode == WSS))
		{
			TWS_ACTION((pBtAction->pSinkAction),SearchResult,pBTDesName,pBtSearchInfo);
		}
	}*/
	(void)pBTDesName;
	(void)pBtSearchInfo;
}


static void _MainSearchFinishAction(void)
{
	/*stBTInfo *pCurInfo = _GetBtCurInfo();
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	BtServiceRecoverSearchingScanMode();

	if(pCurInfo->BtSurportTws == ON)
	{
		if(!(!pCurInfo->IsSurportSwitch && pCurInfo->BtTwsMode == WSS))
		{
			TWS_ACTION((pBtAction->pSinkAction),SearchFinish);
		}
	}*/
	stBTInfo *pCurInfo = _GetBtCurInfo();
	pCurInfo->BaseState = idle;
}



static void _MainAclResultAction(BTConnectReqInfo *pResultInfo)
{
	stBTInfo *pCurInfo = _GetBtCurInfo();
	//BtDevActionMgr *pBtAction = _GetBtMgrAction();

	if(pResultInfo->result == FAIL)
	{
		pCurInfo->BaseState = idle;
		pCurInfo->BtPairState = PAIR_NONE;
	}
	else
	{
		pCurInfo->BaseState = pairing;
		pCurInfo->BtPairState = PAIRING;
	}

#if 0
	if(pResultInfo->reason == 0x18)
	{
		int IsTrue = FALSE;
		SOURCE_ACTION((pBtAction->pSourceAction),IsEqualAddr,&pResultInfo->bd_addr,&IsTrue);

		if(IsTrue)
		{
			BtAutoSet(ON,ROLE_PHONE,&pResultInfo->bd_addr);
			BtSetAutoTime(4,ROLE_PHONE);
		}
	}
#endif
}



static void _MainPairFinishAction(BTPairDev *pPairInfo)
{
		stBTInfo *pCurInfo = _GetBtCurInfo();
		BT_PROFILE_STATE_e state = DISCONNECT_OK;
		int IsTrue = FALSE;

		PhoneConnectStateGet(&state);

		if(pPairInfo->result == SUCCESS)
		{
			IsPhoneAddr(&pPairInfo->bd_addr,&IsTrue);

			if(pCurInfo->BaseState == pairing && IsTrue)
			{
				SDK_Music_IF_Connect(&pPairInfo->bd_addr);
			}
			else
			{
				if(state == CONNECT_OK)
				{
					pCurInfo->BaseState = paired;
				}
				else
				{
					pCurInfo->BaseState = idle;
				}
			}

			pCurInfo->BtPairState = PAIRED;
		}
		else
		{
			//maybe have some buger here
			pCurInfo->BaseState = idle;
			pCurInfo->BtPairState = UNPAIR;

			if(state == DISCONNECT_OK)
			{
				int *pConnectTime = BtGetAutoCounterTime(ROLE_PHONE);
				*pConnectTime = 0;
			}
		}

}

/*{
	stBTInfo *pCurInfo = _GetBtCurInfo();
	BtDevActionMgr *pBtAction = _GetBtMgrAction();
	BT_PROFILE_STATE_e state = DISCONNECT_OK;
	int IsTrue = FALSE;

	SOURCE_ACTION(pBtAction->pSourceAction,GetConnectState,&state);

	if(pPairInfo->result == SUCCESS)
	{
		SOURCE_ACTION((pBtAction->pSourceAction),IsEqualAddr,&pPairInfo->bd_addr,&IsTrue);

		if(pCurInfo->BaseState == pairing && IsTrue)
		{
			//BtSetAutoTime(0,ROLE_PHONE);
			BtServiceA2dpConnect(&pPairInfo->bd_addr,PHONE_ADDR);
		}
		else
		{
			if(state == CONNECT_OK)
			{
				pCurInfo->BaseState = paired;
			}
			else
			{
				pCurInfo->BaseState = idle;
			}
		}

		pCurInfo->BtPairState = PAIRED;

		if(pCurInfo->BtSurportTws == ON)
		{
			TWS_ACTION((pBtAction->pSinkAction),PairFinish,pPairInfo);
		}
	}
	else
	{
		//maybe have some buger here
		pCurInfo->BaseState = idle;
		pCurInfo->BtPairState = UNPAIR;

		if(pCurInfo->BtSurportTws == ON)
		{
			TWS_ACTION((pBtAction->pSinkAction),IsEqualAddr,&pPairInfo->bd_addr,&IsTrue);
		}

		if(!IsTrue && state == DISCONNECT_OK)
		{
			int *pConnectTime = BtGetAutoCounterTime(ROLE_PHONE);
			*pConnectTime = 0;
		}
		else if(IsTrue)
		{
			if(pCurInfo->BtTwsMode == HEADPHONE)
			{
				BtSetIsAutoConnectFail(TRUE,ROLE_HEADPHONE);
				BtSetCurAutoState(ON);
				BtAutoSet(ON,ROLE_HEADPHONE,&pPairInfo->bd_addr);
				BtSetAutoTime(6,ROLE_HEADPHONE);
			}
		}
	}

}*/

extern int UserSetting_ReadBtAutoConDev(BtAddr_t * auto_addr);

static void _MainBtReadyAction(int IsBtModeOn)
{
		stBTInfo *pCurInfo = _GetBtCurInfo();
		(void)pCurInfo;

		SDK_BASE_IF_Get_LocalBtAddr();
		/*SDK_SDP_IF_Set_DeviceID(pCurInfo->verdoridSource, pCurInfo->BtDeviceId_Vid,
			pCurInfo->BtDeviceId_Pid, pCurInfo->BtDeviceId_Version, 0);*/
		if(BtGetAutoConnectWorkState() == ON)
		{
			BtSetCurAutoState(ON);

			if(BtGetSurportAutoRole() == AUTO_PHONE || BtGetSurportAutoRole() == AUTO_PHONE_TWS)
			{
				BtAddr_t BtAddr = {{0}};
				BtAddr_t BtTempAddr = {{0}};
				UserSetting_ReadBtAutoConDev(&BtTempAddr);
                LOGD("UserSetting_ReadBtAutoConDev [0x%02x][0x%02x][0x%02x][0x%02x][0x%02x][0x%02x]......\n",
		             BtTempAddr.address[0],BtTempAddr.address[1],
		             BtTempAddr.address[2],BtTempAddr.address[3],
		             BtTempAddr.address[4],BtTempAddr.address[5]);
				if(memcmp(BtAddr.address,BtTempAddr.address,sizeof(BtAddr_t))!=0)
				{
					BtAutoSet(ON,ROLE_PHONE,&BtAddr);
				}

				/*(void)BtTempAddr;
				(void)BtAddr;
				//if(BtServiceGetAutoAddr(&BtAddr) == SUCCESS)
				{
					bt_ao_printf("\n Get Auto_Addr:%x,%x,%x \n\n",BtAddr.address[0],BtAddr.address[1],BtAddr.address[2]);

					if(memcmp(BtAddr.address,BtTempAddr.address,sizeof(BtAddr_t))!=0)
					{
						BtAutoSet(ON,ROLE_PHONE,&BtAddr);
					}
				}*/
			}
		}

		if(IsBtModeOn != TRUE)
		{
			SDK_BASE_IF_Change_Connectable(OFF);
			SDK_BASE_IF_Change_Discoverable(OFF);
		}
		else
		{
			_MainBtModeOnAction();
		}
}

/*{
	stBTInfo *pCurInfo = _GetBtCurInfo();
	BtDevActionMgr *pBtAction = _GetBtMgrAction();
	BtAddr_t BtAddr = pCurInfo->LocalAddr;
	BtAddr_t BtAddrTemp = {{0}};
	//UINT32 dRamdonVal = 0;

	BtServiceGetLocalAddr(&BtAddrTemp);

	if((!pCurInfo->IsBtNameRamdom))
	{
		BtServiceSetLocalName(pCurInfo->LocalName);
	}
	else
	{
		char NewName[NAME_LEN+20];
		memset(NewName,0,sizeof(NewName));
		int val = sprintf(NewName, "%s_%02x%02x%02x", pCurInfo->LocalName,
			BtAddrTemp.address[0],BtAddrTemp.address[1],BtAddrTemp.address[2]);
		BtServiceSetLocalName(NewName);
		memcpy(pCurInfo->LocalName,NewName,val);
		bt_ao_ntc_printf("ramdom USER_NAME :%s",pCurInfo->LocalName);
	}

	if(!pCurInfo->IsBtAddrRamdom)
	{
		BtServiceSetLocalAddr(&BtAddr);
	}
	else
	{
		//bt_ao_ntc_printf("0x%x,0x%x,0x%x\n",BtAddrTemp.address[5],BtAddrTemp.address[4],BtAddrTemp.address[3]);
	//
	//	dRandTimes += GetSTC();
	//	//bt_ao_ntc_printf("\n dRandTimes: %d\n",dRandTimes);
	//	srand(dRandTimes);
	//	dRamdonVal = rand();
	//	BtAddrTemp.address[0] = dRamdonVal&0xff;
	//	BtAddrTemp.address[1] = (dRamdonVal>>8)&0xff;
	//	BtAddrTemp.address[2] = (dRamdonVal>>16)&0xff;

	//	if(BtAddrTemp.address[2]==0x9E && BtAddrTemp.address[1]==0x8B)
	//	{
	//		if(BtAddrTemp.address[0]<=0x3F)
	//		{
	//			BtAddrTemp.address[0] = 0x40 + ((dRamdonVal>>24) & 0xBF); //sum is between 0x40 to 0xFF
	//		}
	//	}
	//
		pCurInfo->LocalAddr.address[0] = BtAddrTemp.address[0];
		pCurInfo->LocalAddr.address[1] = BtAddrTemp.address[1];
		pCurInfo->LocalAddr.address[2] = BtAddrTemp.address[2];
		BtAddr = pCurInfo->LocalAddr;
		BtServiceSetLocalAddr(&BtAddr);
		bt_ao_ntc_printf("ramdom USER_ADDR :%x,%x,%x \n",BtAddr.address[0],BtAddr.address[1],BtAddr.address[2]);
	}

	BtServiceSetLocalSDP_DeviceID(pCurInfo->BtDeviceId_Pid,
								 pCurInfo->BtDeviceId_Vid,
								 pCurInfo->BtDeviceId_Version);

	BtServiceSetLocalEirInfo(pCurInfo->LocalName,
							 pCurInfo->verdoridSource,
							 pCurInfo->BtDeviceId_Pid,
							 pCurInfo->BtDeviceId_Vid,
							 pCurInfo->BtDeviceId_Version);
	//avrcp
	if(strlen(pCurInfo->avrcpCtProviderName)>0){
		BtServiceSetLocalAvrcpCtSrdName(pCurInfo->avrcpCtProviderName);
	}
	if(strlen(pCurInfo->avrcpCtServiceName)>0){
		BtServiceSetLocalAvrcpCtSrvName(pCurInfo->avrcpCtServiceName);
	}
	BtServiceSetLocalLmpName(pCurInfo->LocalName);
	if(BtGetAutoConnectWorkState() == ON)
	{
		BtSetCurAutoState(ON);

		if(BtGetSurportAutoRole() == AUTO_PHONE || BtGetSurportAutoRole() == AUTO_PHONE_TWS)
		{
			BtAddr_t BtAddr = {{0}};
			BtAddr_t BtTempAddr = {{0}};

			if(BtServiceGetAutoAddr(&BtAddr) == SUCCESS)
			{
				bt_ao_printf("\n Get Auto_Addr:%x,%x,%x \n\n",BtAddr.address[0],BtAddr.address[1],BtAddr.address[2]);

				if(memcmp(BtAddr.address,BtTempAddr.address,sizeof(BtAddr_t))!=0)
				{
					BtAutoSet(ON,ROLE_PHONE,&BtAddr);
				}
			}
		}
	}

	if(pCurInfo->BtSurportTws == ON)
	{
		TWS_ACTION(pBtAction->pSinkAction,ReadyOk);
	}

	if(IsBtModeOn != TRUE)
	{
		if(pCurInfo->BtSurportTws != ON)
		{
			BtServiceSetScanMode(SCAN_NONE);
		}
		else
		{
			if(pCurInfo->BtTwsMode == HEADPHONE)
			{
				BtServiceSetScanMode(ONLY_CONNECTABLE);
			}
			else
			{
				BtAddr_t WssAddr = {{0}},WssTempAddr = {{0}};

				BtServiceGetWssAddr(&WssAddr);

				if(memcmp(&WssTempAddr,&WssAddr,sizeof(BtAddr_t)) == 0)
				{
					BtServiceSetScanMode(BOTH_DISCOVERY_CONNECTABLE);
				}
				else
				{
					BtServiceSetScanMode(ONLY_CONNECTABLE);
				}
			}
		}
	}
	else
	{
		_MainBtModeOnAction();
	}

	BtServiceSetMainMode(pCurInfo->BtTwsMode);
}*/



static void _MainBtSearchAction(void)
{
		stBTInfo *pCurInfo = _GetBtCurInfo();
		pCurInfo->BaseState = inquiring;
		SDK_BASE_IF_StartSearch();
}

/*{
	stBTInfo *pCurInfo = _GetBtCurInfo();
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	if(pCurInfo->BtSurportTws == ON)
	{
		if(pCurInfo->BtTwsMode == HEADPHONE)
		{
			BT_PROFILE_STATE_e state = DISCONNECT_OK;
			BtAddr_t address = {{0}};

			TWS_ACTION(pBtAction->pSinkAction,GetConnectState,&state);
			BtAutoClear(ROLE_HEADPHONE);

			if(state == CONNECT_OK)
			{
				TWS_ACTION(pBtAction->pSinkAction,GetAddr,&address);
				bt_ao_printf("\n headphone:%x,%x ",address.address[0],address.address[1]);
				TWS_ACTION(pBtAction->pSinkAction,Disconnect,&address);
			}
		}
	}

	BtServiceStartSearch();
}*/


static void _MainSppConnectOkAction(stBtDevInfo *pDev)
{
		stBTInfo *pCurInfo = _GetBtCurInfo();
		BtDevActionMgr *pBtAction = _GetBtMgrAction();
		(void)pCurInfo;
		(void)pBtAction;

		if(pCurInfo->BtSurportTransMode == ON)
		{
			if(pDev->DevRole == REMOTEAPP_ADDR)
			{
				stBtRemoteAppAction *pRemoteAppAction = NULL;

				//_MainBtGetRemoteAppAction(&pRemoteAppAction);
				REMOTEAPP_ACTION((pRemoteAppAction),SppConnectOk,pDev);
			}
		}
}

/*{
	stBTInfo *pCurInfo = _GetBtCurInfo();
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	if(pCurInfo->BtSurportTws == ON)
	{
		if(pDev->DevRole == WSS_ADDR)
		{
			TWS_ACTION((pBtAction->pSinkAction),SppConnectOk,pDev);
		}

	}

	//if(pCurInfo->BtSurportTransMode == ON)
	{
		if(pDev->DevRole == REMOTEAPP_ADDR)
		{
			stBtRemoteAppAction *pRemoteAppAction = NULL;

			_MainBtGetRemoteAppAction(&pRemoteAppAction);
			REMOTEAPP_ACTION((pRemoteAppAction),SppConnectOk,pDev);
		}
	}

	if(pCurInfo->DevConnectCounter >= pCurInfo->DevMaxConnect)
	{
		BtServiceSetScanMode(SCAN_NONE);
	}
	else
	{
		if(pCurInfo->IsBtMode == TRUE)
		{
			BtServiceSetScanMode(BOTH_DISCOVERY_CONNECTABLE);
		}
		else
		{
			BtServiceSetScanMode(SCAN_NONE);
		}
	}
}*/


static void _MainSppDisconnectOkAction(stBtDevInfo *pDev)
{
		stBTInfo *pCurInfo = _GetBtCurInfo();
		BtDevActionMgr *pBtAction = _GetBtMgrAction();
		(void)pCurInfo;
		(void)pBtAction;

		if(pCurInfo->BtSurportTransMode == ON)
		{
			if(pDev->DevRole == REMOTEAPP_ADDR)
			{
				stBtRemoteAppAction *pRemoteAppAction = NULL;

				//_MainBtGetRemoteAppAction(&pRemoteAppAction);
				REMOTEAPP_ACTION((pRemoteAppAction),SppDisconnectOk,pDev);
			}
		}

}

/*{
	stBTInfo *pCurInfo = _GetBtCurInfo();
	BtDevActionMgr *pBtAction = _GetBtMgrAction();


	//if(pCurInfo->BtSurportTws == ON ||pCurInfo->BtSurportTransMode == ON)
	{
		if(pDev->DevRole == WSS_ADDR || pDev->DevRole == HEADPHONE_ADDR)
		{
			TWS_ACTION((pBtAction->pSinkAction),SppDisconnectOk,pDev);

			if(pCurInfo->SwitchFlag && pDev->DevRole == WSS_ADDR)
			{
				pCurInfo->SwitchFlag = FALSE;
				TWS_ACTION(pTwsAction,BtTwsSwitchActionInit,TwsSinkMode);
			}
		}
		else if(pDev->DevRole == REMOTEAPP_ADDR)
		{
			stBtRemoteAppAction *pRemoteAppAction = NULL;

			_MainBtGetRemoteAppAction(&pRemoteAppAction);
			REMOTEAPP_ACTION((pRemoteAppAction),SppDisconnectOk,pDev);
		}

	}

}*/


static void _MainBtDoPrevAction(void)
{
	/*BtAddr_t BtAddr;
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	SOURCE_ACTION(pBtAction->pSourceAction,GetAddr,&BtAddr);
	BtServiceAvrcpCtl(&BtAddr,AV_PREV);*/
	PhonePrev();
}


static void _MainBtDoNextAction(void)
{
	/*BtAddr_t BtAddr;
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	SOURCE_ACTION(pBtAction->pSourceAction,GetAddr,&BtAddr);
	BtServiceAvrcpCtl(&BtAddr,AV_NEXT);*/
	PhoneNext();
}


static void _MainBtDoPlayAction(void)
{
	/*stBTInfo *pCurInfo = _GetBtCurInfo();
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	SOURCE_ACTION(pBtAction->pSourceAction,Play);

	if(pCurInfo->BtSurportTws == ON)
	{
		//TWS_ACTION((pBtAction->pSinkAction),Play);
	}*/
	PhonePlay();
}


static void _MainBtDoPauseAction(void)
{
	/*stBTInfo *pCurInfo = _GetBtCurInfo();
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	SOURCE_ACTION(pBtAction->pSourceAction,Pause);

	if(pCurInfo->BtSurportTws == ON)
	{
		//TWS_ACTION((pBtAction->pSinkAction),Pause);
	}*/
	PhonePause();

}


static void _MainBtSendWssPlayAction(void)
{
	stBTInfo *pCurInfo = _GetBtCurInfo();
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	if(pCurInfo->BtSurportTws == ON)
	{
		TWS_ACTION((pBtAction->pSinkAction),Play);
	}
}


static void _MainBtSendWssPauseAction(void)
{
	stBTInfo *pCurInfo = _GetBtCurInfo();
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	if(pCurInfo->BtSurportTws == ON)
	{
		TWS_ACTION((pBtAction->pSinkAction),Pause);
	}
}


static int _MainBtVolumeSyncReg(BtVolumeInfo *pVolumeInfo)
{
		stBTInfo *pCurInfo = _GetBtCurInfo();

		if(pVolumeInfo == NULL)
		{
			return FAIL;
		}

		if(pVolumeInfo->role == PHONE_ADDR)
		{
			PhoneVolumeSyncReg(pVolumeInfo);
			pCurInfo->IsBtSourceVolumeSyncReg = TRUE;
			return SUCCESS;
		}
		else
		{
			bt_ao_error_printf("volume sync reg role err!!");
		}

		return FAIL;
}

/*{
	stBTInfo *pCurInfo = _GetBtCurInfo();
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	if(pVolumeInfo == NULL)
	{
		return FAIL;
	}

	if(pVolumeInfo->role == HEADPHONE_ADDR)
	{
		TWS_ACTION((pBtAction->pSinkAction),VolumeSyncReg,pVolumeInfo);
		pCurInfo->IsBtSinkVolumeSyncReg = TRUE;
		return SUCCESS;
	}
	else if(pVolumeInfo->role == PHONE_ADDR)
	{
		SOURCE_ACTION(pBtAction->pSourceAction,VolumeSyncReg,pVolumeInfo);
		pCurInfo->IsBtSourceVolumeSyncReg = TRUE;
		return SUCCESS;
	}
	else
	{
		bt_ao_error_printf("volume sync reg role err!!");
	}

	return FAIL;
}*/


static int _MainBtVolumeSyncSet(BtVolumeSet *pVolumeInfo)
{
		if(pVolumeInfo == NULL)
		{
			return FAIL;
		}

		PhoneVolumeSyncSet(pVolumeInfo);

		return FAIL;
}

/*{
	stBTInfo *pCurInfo = _GetBtCurInfo();
	BtDevActionMgr *pBtAction = _GetBtMgrAction();

	if(pVolumeInfo == NULL)
	{
		return FAIL;
	}


	if(pCurInfo->BtSurportTws == ON && pCurInfo->BtTwsMode == HEADPHONE)
	{
		TWS_ACTION((pBtAction->pSinkAction),VolumeSyncSet,pVolumeInfo);
	}

	SOURCE_ACTION(pBtAction->pSourceAction,VolumeSyncSet,pVolumeInfo);

	return FAIL;
}*/

static void _MainBtAvrcpVolSyncSamsung(BtAddr_t *BtAddr,unsigned char volume,
														unsigned char mute,unsigned char volume_mute_set)
{
	//Sunplus//BtServiceAvrcpVolSyncSamsung(BtAddr,volume,mute,volume_mute_set);
	BtAddr_t addr = {{0x0,0x0,0x0,0x0,0x0,0x0}};
	if(memcmp(BtAddr,&addr,ADDR_LENGTH)==0)
		return;
	SDK_AVRCP_VolSyncSamsung(BtAddr, volume/*0~127*/, mute, volume_mute_set);///mute/mute_set???samsung????????????,mute??
}

static int _MainBtClearAutoInfo(void)
{
	int ret;
	BtAddr_t BtAddr = {{0}};
	ret = BtAutoSet(OFF,ROLE_PHONE,&BtAddr);
	if(ret)
	{
		return FAIL;
	}
	return SUCCESS;
}





static stBTAction BtMain =
{
	_MainConfigInfoInit,
	_MainBtModeOnAction,
	_MainBtModeOffAction,
	BtDoInit,
	BtDoDeInit,
	BtDoEnable,
	BtDoDisable,
	_MainBtReadyAction,
	IsBtDoSearching,
	_MainBtSearchAction,
	BtDoStopSearch,
	_MainSearchResultAction,
	_MainSearchFinishAction,
	_MainAclResultAction,
	BtDoPairStart,
	BtDoUnpair,
	_MainPairingAction,
	_MainPairFinishAction,
	_MainBtConnect,
	BtDenyConnect,
	NULL,//_MainBtConnetingAction,
	_MainBtConnetOkAction,
	_MainBtConnectFailAction,
	_MainBtDisconnect,
	NULL,//_MainBtDisconnetingAction,
	_MainBtDisconnetOkAction,
	_MainBtAvrcpVolSyncSamsung,
	BtDoSppConnect,
	BtDoSppConnectSamsung,
	_MainSppConnectOkAction,
	BtDoSppDisconnect,
	_MainSppDisconnectOkAction,
	BtDoSppDataOut,
	BtDoUdtTest,
	BtDoRfPowerSet,
	_MainBtAutoConnectCheck,
	_MainBtAutoConnect,
	BtSetAutoConnectWorkState,
	BtDoScanModeSet,
	BtDoScanModeGet,
	_MainSwitchSinkMode,
	_MainSwitchSinkFinish,
	NULL,
	_MainBtPowerOffRes,
	_MainBtDoPlayAction,
	_MainBtDoPauseAction,
	_MainBtDoNextAction,
	_MainBtDoPrevAction,
	_MainBtIsPlaying,
	_MainBtSendWssPlayAction,
	_MainBtSendWssPauseAction,
	_MainBtConnectTimeOutHandle,
	BtSetPinCode,
	BtReplyPinCode,
	_MainGetAddrByDevRole,
	_MainGetConnectStateByDevRole,
	BtDoGetProfileState,
	BtDoGetRemoteDevInfo,
	BtDoGetRemoteDevByIdex,
	BtDoLocalNameGet,
	BtDoLocalNameSet,
	BtDoLocalLmpNameSet,
	BtDoLocalDeviceClassSet,
	BtDoRemoveSdpDeviceIDGet,
	BtDoLocalAddrGet,
	BtDoLocalAddrSet,
	BtDoGetLocalInfo,
	BtDoGetBondDevNum,
	BtDoGetBondByIdex,
	BtDoGetRemoteDevNum,
	_MainBtVolumeSyncReg,
	_MainBtVolumeSyncSet,
	BtDoSendUserCmd,
	BtDoAddBondDev,
	BtDoClearBondDev,
	_MainBtClearAutoInfo,
	BtDoGetRemoteDevRssi,
};



void BtActionGet(stBTAction **pBtAction)
{
	*pBtAction = &BtMain;
}

void BtActionDel(stBTAction **pBtAction)
{
	if(*pBtAction != NULL)
	{
		*pBtAction = NULL;
	}
}



void BtTransActionInit(stBtTransAction **pTransAction)
{
	bt_ao_printf("\n %s !!!",__FUNCTION__);
	*pTransAction = NULL;//&BtTrans;
}


void BtTransActionUnInit(stBtTransAction **pTransAction)
{
	bt_ao_printf("\n %s !!!",__FUNCTION__);

	if(*pTransAction != NULL)
	{
		*pTransAction = NULL;
	}
}

#endif //__BT_AO_ACTION_C__

