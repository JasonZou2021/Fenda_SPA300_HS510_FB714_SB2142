#ifndef __BT_AO_SUB_ACTION_C__
#define __BT_AO_SUB_ACTION_C__
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
#include <stdio.h>
#include "types.h"
#include "initcall.h"
#include "std_interfaces.h"
#include "log_utils.h"
#include "bt_mw_core.h"
#include "bt_ao_action.h"
#include "bt_ao_auto.h"

/*---------------------------------------------------------------------------*
 *    MACRO DECLARATIONS
 *---------------------------------------------------------------------------*/
#define SUB_MAX_DEV		1

/*---------------------------------------------------------------------------*
 *    GLOBAL VARIABLES
 *---------------------------------------------------------------------------*/
static stBtDevInfo    	SubRemoteDev[SUB_MAX_DEV];
static stBTInfo *pBtSdkCurInfo = NULL;
static char SubDesBtName[NAME_LEN];

/*---------------------------------------------------------------------------*
 *    EXTERNAL REFERENCE
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 *    FUNTION DECLARATIONS
 *---------------------------------------------------------------------------*/
static void _SubUpdateDevInfo(stBtDevInfo *pDev);//a2dp
static void _SubSppUpdateDevInfo(stBtDevInfo *pDev);
static void _SubBtDisconnect(BtAddr_t *pAddr);
static int _SubConnectState(BT_PROFILE_STATE_e *pState);
static int _SubGetMainAddr(BtAddr_t *pAddr);

/******************************************************************************************/
/**
 * \fn		stBTInfo * _GetBtCurInfo(void)
 *
 * \brief		get the bt current info
 *
 * \param
 *              		none
 *
 * \return	    stBTInfo: bt info struct
 *
 ******************************************************************************************/
static stBTInfo * _GetBtCurInfo(void)
{
	return pBtSdkCurInfo;
}
/******************************************************************************************/
/**
 * \fn		void _SubGetConnectStateByDevRole(DEVROLE_DEX_e dex,BT_PROFILE_STATE_e *pState)
 *
 * \brief		get the dev connect state by role
 *
 * \param
 *              		dex:role    pState:connect state
 *
 * \return	    stBTInfo: bt info struct
 *
 ******************************************************************************************/
static void _SubGetConnectStateByDevRole(DEVROLE_DEX_e dex,BT_PROFILE_STATE_e *pState)
{
	if(pState != NULL)
	{
		return ;
	}

	switch(dex)
	{
		case PHONE_ADDR:
		{
			_SubConnectState(pState);
			break;
		}
		default:
		{
			break;
		}
	}
}


/******************************************************************************************/
/**
 * \fn		void _SubGetAddrByDevRole(BtAddr_t *pBtAddr,DEVROLE_DEX_e dex)
 *
 * \brief		get dev addr by dev role
 *
 * \param
 *              		pBtAddr:addr   dex:role
 *
 * \return	    void
 ******************************************************************************************/
static void _SubGetAddrByDevRole(BtAddr_t *pBtAddr,DEVROLE_DEX_e dex)
{
	if(pBtAddr != NULL)
	{
		switch(dex)
		{
			case PHONE_ADDR:
			{
				_SubGetMainAddr(pBtAddr);
				break;
			}
			default:
			{
				break;
			}
		}
	}
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
static void _SubBtPowerOffRes(void)
{
	stBTInfo *pCurInfo = _GetBtCurInfo();
	char BtModeState = pCurInfo->IsBtMode;

	if(BtModeState == TRUE)
	{
		BT_PROFILE_STATE_e state = DISCONNECT_OK;
		_SubConnectState(&state);

		if(state == CONNECT_OK)
		{
			BtAddr_t BtAddr = {{0}};
			_SubGetMainAddr(&BtAddr);
			_SubBtDisconnect(&BtAddr);
		}
	}


}



/******************************************************************************************/
/**
 * \fn		void _SubConfigInfoInit(void *pBtSubConf)
 *
 * \brief		config bt
 *
 * \param
 *              		pBtSubConf: bt config info
 *
 * \return	    void
 *
 ******************************************************************************************/
static void _SubConfigInfoInit(void *pBtSubConf)
{
	stBTInfo *pBtConf = (stBTInfo *)pBtSubConf;
	BtAutoConConfig AutoCnfInfo;
	stBtConfig BtCnfSet;

	pBtSdkCurInfo = pBtConf;

	if(pBtSubConf == NULL)
	{
		return ;
	}

	AutoCnfInfo.AutoLatency = pBtConf->AutoLatency;
	AutoCnfInfo.RetryTimes = pBtConf->RetryTimes;
	AutoCnfInfo.WorkState = pBtConf->BtSurportAutoConnet;
	AutoCnfInfo.role = pBtConf->AutoRole;
	AutoCnfInfo.CurAutoState = OFF;

	BtAutoInit(&AutoCnfInfo);
	memset(SubDesBtName,0,sizeof(SubDesBtName));
	memcpy(SubDesBtName,pBtConf->pBTDesName,NAME_LEN);
	memset(SubRemoteDev,0,sizeof(SubRemoteDev));

	BtCnfSet.DevMaxConnect = BT_SOURCE_DEV_NUM;
	BtCnfSet.IsBtSimpleModeEn = pBtConf->IsBtSimpleModeEn;
	BtCnfSet.IsBtAacCodecEn = pBtConf->IsBtAacCodecEn;
	BtCnfSet.SystemRole = pBtConf->PlatformRole;

	(void)BtCnfSet;
	//BtServiceConfigSet(&BtCnfSet);
}
/******************************************************************************************/
/**
 * \fn		int _SubBtIsPlaying(int *pIsTrue)
 *
 * \brief		get the state of if bt is playing
 *
 * \param
 *              		pIsTrue:  true  false
 *
 * \return	   0:SUCCESS     -1:FAIL
 *
 ******************************************************************************************/
static int _SubBtIsPlaying(int *pIsTrue)
{
	BT_IR_CMD_e state;
	BtAddr_t BtAddr = {{0}};

	_SubGetMainAddr(&BtAddr);
	(void)state;
	(void)pIsTrue;

	return 0;
	//BtServiceGetAudioState(&BtAddr,&state);


	/*if(AV_PLAY == state)
	{
		*pIsTrue = TRUE;
		return TRUE;
	}

	*pIsTrue = FALSE;
	return FALSE;*/
}
/******************************************************************************************/
/**
 * \fn		void _SubBtConnectFailAction(stBtDevInfo *pDev)
 *
 * \brief		bt action when connnect fail
 *
 * \param
 *              		pDev:bt dev info
 *
 * \return	    void
 *
 ******************************************************************************************/
static void _SubBtConnectFailAction(stBtDevInfo *pDev)
{
	if(pDev->DevRole == PHONE_ADDR)
	{
		BtSetCurAutoState(ON);
		BtAutoSet(ON,ROLE_PHONE,&pDev->bd_addr);
		BtSetAutoTime(4,ROLE_PHONE);
	}
}
/******************************************************************************************/
/**
 * \fn		int _SubGetMainAddr(BtAddr_t *pAddr)
 *
 * \brief		get the 8107main addr(source addr)
 *
 * \param
 *              		pAddr:addr
 *
 * \return	      0:SUCCESS     -1:FAIL
 *
 ******************************************************************************************/
static int _SubGetMainAddr(BtAddr_t *pAddr)
{
	stBtDevInfo *pBtDevInfo = &SubRemoteDev[0];//&SubRemoteDev[SUB_MAX_DEV]; //change abel 20180706

	if(pAddr != NULL)
	{
		memcpy(pAddr,&pBtDevInfo[0].bd_addr,sizeof(BtAddr_t));
		return SUCCESS;
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		int _SubConnectState(BT_PROFILE_STATE_e *pState)
 *
 * \brief		get connenct state(a2dp)
 *
 * \param
 *              		pState:DISCONNECT_OK....
 *
 * \return	    0:SUCCESS     -1:FAIL
 *
 ******************************************************************************************/
static int _SubConnectState(BT_PROFILE_STATE_e *pState)
{
	stBtDevInfo *pBtDevInfo = &SubRemoteDev[0];//&SubRemoteDev[SUB_MAX_DEV]; //change abel 20180706

	if(pState != NULL)
	{
		*pState = BT_A2DP_PROFILE(pBtDevInfo[0].BtConnState);
		return SUCCESS;
	}

	return FAIL;
}
/******************************************************************************************/
/**
 * \fn		void _SubBtAutoConnectCheck(char *pResult)
 *
 * \brief		bt check if auto connnect
 *
 * \param
 *              		pResult: 0--success   -1---fail
 *
 * \return	    void
 *
 ******************************************************************************************/
static void _SubBtAutoConnectCheck(int *pResult ,int *pRole)
{
	stBTInfo *pCurInfo = _GetBtCurInfo();
	BT_STATE_e state = pCurInfo->BtState;
	int *pConnectTime = BtGetAutoCounterTime(ROLE_PHONE);

	if(BtGetAutoConnectWorkState() == ON)
	{
		*pRole = ROLE_PHONE;

		do
		{
			if(pCurInfo->IsBtMode != TRUE)
			{
				//bt_ao_printf("no bt mode\n");
				*pConnectTime = 0;
				*pResult = FAIL;
				return;
			}

			if(state != BT_READY)
			{
				//bt_ao_printf("BT no READY\n");
				*pConnectTime = 0;
				*pResult = FAIL;
				return;
			}

			if(pCurInfo->BaseState != idle)
			{
				bt_ao_printf("basestate: no idel\n");
				*pConnectTime = 0;
				*pResult = FAIL;
				return;
			}

			/*if(BtServiceIsSearching())
			{
				//bt_ao_printf("is searching !\n");
				*pConnectTime = 0;
				*pResult = FAIL;
				return;
			}*/

			if(BtAutoConnectCheck(*pRole) == SUCCESS)
			{
				*pResult = SUCCESS;
				*pConnectTime = 0;
				return;
			}
		}while(0);

	}

}
/******************************************************************************************/
/**
 * \fn		void _SubBtAutoConnect(int AutoRole)
 *
 * \brief		check if do connect
 *
 * \param
 *              		none
 *
 * \return	    pResult:0---success   -1----fail
 *
 ******************************************************************************************/
void _SubBtAutoConnect(int AutoRole)
{
	BtAutoCon(AutoRole);
	return;
}
/******************************************************************************************/
/**
 * \fn		int _SubConnectTimeOutHandle(BtAddr_t *pAddress)
 *
 * \brief		bt  action  when bt timeout
 *
 * \param
 *              		pAddress: addr
 *
 * \return	    void
 *
 ******************************************************************************************/
static int _SubConnectTimeOutHandle(BtAddr_t *pAddress)
{
	if(pAddress != NULL)
	{
		BtSetCurAutoState(ON);
		BtSetAutoTimeOutFlag(ON,ROLE_PHONE);
		BtAutoSet(ON,ROLE_PHONE,pAddress);
		return SUCCESS;
	}

	return FAIL;

}
/******************************************************************************************/
/**
 * \fn		void _SubSppUpdateDevInfo(stBtDevInfo *pDev)
 *
 * \brief		update spp dev info
 *
 * \param
 *              		none
 *
 * \return	    pDev: bt info struct
 *
 ******************************************************************************************/
static void _SubSppUpdateDevInfo(stBtDevInfo *pDev)
{
	stBtDevInfo *pBtDevInfo = &SubRemoteDev[0];//&SubRemoteDev[SUB_MAX_DEV]; //change abel 20180706
	pBtDevInfo[0].BtConnState |= pDev->BtConnState;
}
/******************************************************************************************/
/**
 * \fn		 _SubUpdateDevInfo(stBtDevInfo *pDev)//a2dp
 *
 * \brief		update dev info
 *
 * \param
 *              		none
 *
 * \return	   void
 *
 ******************************************************************************************/
static void _SubUpdateDevInfo(stBtDevInfo *pDev)//a2dp
{
	stBtDevInfo *pBtDevInfo = &SubRemoteDev[0];//&SubRemoteDev[SUB_MAX_DEV]; //change abel 20180706
	memcpy(&(pBtDevInfo[0]),pDev,sizeof(stBtDevInfo));
}
/******************************************************************************************/
/**
 * \fn		int _SubBtModeOnAction(void)
 *
 * \brief		entry bt source action
 *
 * \param
 *              		none
 *
 * \return	    0:success     -1:fail
 *
 ******************************************************************************************/
static int _SubBtModeOnAction(void)
{
	int sdBondedNum = 0;
	int eRet = SUCCESS;
	stBtDevInfo dev;
	stBTInfo *pBtCurInfo = _GetBtCurInfo();
	BT_STATE_e BtState = pBtCurInfo->BtState;

	memset(&dev,0,sizeof(stBtDevInfo));

	if(BtState != BT_READY)
	{
		return FAIL;
	}

	//eRet = BtServiceBondNum(&sdBondedNum);
	bt_ao_ntc_printf("bond_num = %d\n",sdBondedNum);

	if(eRet == SUCCESS)
	{
		if(pBtCurInfo->IsSurportSwitch)
		{
			#if 0	//delete for samsung
			if(sdBondedNum == 0)
			{

				BtServiceSetScanMode(BOTH_DISCOVERY_CONNECTABLE);
			}
			else
			{
				BtServiceSetScanMode(ONLY_CONNECTABLE);
			}
			#endif
		}
		else
		{
			#if 0	//delete for samsung
			BtServiceSetScanMode(ONLY_CONNECTABLE);
			#endif
			if(sdBondedNum > 0)
			{
				//BtServiceGetBondByIdex(1,&dev);
				stBtDevInfo *pBtDevInfo = &SubRemoteDev[0];
				memcpy(&(pBtDevInfo[0]),&dev,sizeof(stBtDevInfo));

			#if 0
				int i = 0;
				for(i=0;i<6;i++)
					bt_ao_printf("dev.bd_addr = %x,",dev.bd_addr.address[i]);
				bt_ao_printf("\n\n");
			#endif
				if(pBtCurInfo->BtSurportStandBy == ON)
				{
					//BtServiceA2dpConnect(&dev.bd_addr,PHONE_ADDR);
				}
				else
				{
					if(BtGetAutoConnectWorkState() == ON)
					{
						//BtServiceA2dpConnect(&dev.bd_addr,PHONE_ADDR);
						//BtSetCurAutoState(ON);
						//BtAutoSet(ON,ROLE_PHONE,&dev.bd_addr);
					}
				}
			}
			else
			{
				//BtServiceStartSearch();
			}
		}
	}
	else
	{
		bt_ao_error_printf("get Bonded Devices Num error! eRet=%d \n",eRet);
	}

	return SUCCESS;
}
/******************************************************************************************/
/**
 * \fn		int _SubBtModeOffAction(void)
 *
 * \brief		exit bt source action
 *
 * \param
 *              		none
 *
 * \return	    0:success     -1:fail
 *
 ******************************************************************************************/
static int _SubBtModeOffAction(void)
{
	BtAddr_t BtAddr ={{0}};
	BT_PROFILE_STATE_e state = DISCONNECT_OK;
	#if 0	//delete for samsung
	BtServiceSetScanMode(SCAN_NONE);
	#endif
	_SubGetMainAddr(&BtAddr);
	_SubConnectState(&state);

	if(state == CONNECT_OK)
	{
		if(BtGetAutoConnectWorkState() == ON)
		{
			BtSetCurAutoState(ON);
			BtAutoSet(ON,ROLE_PHONE,&BtAddr);
		}

		_SubBtDisconnect(&BtAddr);
	}

	return SUCCESS;
}
/******************************************************************************************/
/**
 * \fn		void _SubBtConnetingAction(stBtDevInfo *pDev)
 *
 * \brief		bt connecting action:update info
 *
 * \param
 *              		pDev: bt dev info
 *
 * \return	    void
 *
 ******************************************************************************************/
static void _SubBtConnetingAction(stBtDevInfo *pDev)
{
	_SubUpdateDevInfo(pDev);
}
/******************************************************************************************/
/**
 * \fn		void _SubBtConnetOkAction(stBtDevInfo *pDev)
 *
 * \brief		bt connect ok action : info update
 *
 * \param
 *              		pDev: bt dev info
 *
 * \return	    void
 *
 ******************************************************************************************/
static void _SubBtConnetOkAction(stBtDevInfo *pDev)
{
	stBTInfo *pCurInfo = _GetBtCurInfo();
	pCurInfo->DevConnectCounter++;

	if(pCurInfo->IsBtMode == ON)
	{
		int role = ROLE_PHONE;

		BtSetCurAutoState(OFF);
		BtAutoSet(OFF,role,&pDev->bd_addr);
		pCurInfo->CancelAutoConnectFalg = FALSE;

		if(!BtIsAutoConnected(role))
		{
			BtSetIsAutoConnected(TRUE,role);
		}

		if(BtGetAutoTimeOutFlag(role) == ON)
		{
			BtSetAutoTimeOutFlag(OFF,role);
		}

		if(BtIsAutoConnectFail(role) == TRUE)
		{
			BtSetIsAutoConnectFail(FALSE,role);
		}
		#if 0	//delete for samsung
		if(pCurInfo->DevMaxConnect <= pCurInfo->DevConnectCounter)
		{
			BtServiceSetScanMode(SCAN_NONE);
		}
		#endif
	}
	else
	{
		//BtServiceA2dpDisconnect(&pDev->bd_addr,PHONE_ADDR);
	}

	_SubUpdateDevInfo(pDev);
}
/******************************************************************************************/
/**
 * \fn		void _SubBtDisconnetingAction(stBtDevInfo *pDev)
 *
 * \brief		Bt Disconneting action
 *
 * \param
 *              		pDev: bt dev info
 *
 * \return	   void
 *
 ******************************************************************************************/
static void _SubBtDisconnetingAction(stBtDevInfo *pDev)
{
	stBTInfo *pCurInfo = _GetBtCurInfo();

	if(!pCurInfo->CancelAutoConnectFalg)
	{
		_SubUpdateDevInfo(pDev);
	}
}
/******************************************************************************************/
/**
 * \fn		void _SubBtDisconnetOkAction(stBtDevInfo *pDev)
 *
 * \brief		a2dp disconenct ok action
 *
 * \param
 *              		pDev:bt dev info
 *
 * \return	    void
 *
 ******************************************************************************************/
static void _SubBtDisconnetOkAction(stBtDevInfo *pDev)
{
	stBTInfo *pCurInfo = _GetBtCurInfo();

	if(!pCurInfo->CancelAutoConnectFalg)
	{
		_SubUpdateDevInfo(pDev);
	}

	if(pCurInfo->DevConnectCounter > 0)
	{
		BT_PROFILE_STATE_e state = DISCONNECT_OK;
		BtDoGetProfileState(&pDev->bd_addr,SPP_PROFILE,&state);

		if(state != CONNECT_OK)
		{
			pCurInfo->DevConnectCounter--;
		}
	}

	if(pCurInfo->BtSurportStandBy != ON)
	{
		if(BtGetAutoConnectWorkState() == ON)
		{
			if(!pCurInfo->CancelAutoConnectFalg)
			{
				BtSetCurAutoState(ON);
				BtAutoSet(ON,ROLE_PHONE,&pDev->bd_addr);
			}
		}
	}
}
/******************************************************************************************/
/**
 * \fn		void _SubSppConnectOkAction(stBtDevInfo *pDev)
 *
 * \brief		spp connect finish action
 *
 * \param
 *              		pDev: bt dev info
 *
 * \return	    void
 *
 ******************************************************************************************/
static void _SubSppConnectOkAction(stBtDevInfo *pDev)
{
	_SubSppUpdateDevInfo(pDev);
}
/******************************************************************************************/
/**
 * \fn		void _SubSppDisconnectOkAction(stBtDevInfo *pDev)
 *
 * \brief		spp disconnect finish action
 *
 * \param
 *              		pDev: bt dev info
 *
 * \return	    void
 *
 ******************************************************************************************/
static void _SubSppDisconnectOkAction(stBtDevInfo *pDev)
{
	stBTInfo *pCurInfo = _GetBtCurInfo();

	if(pCurInfo->DevConnectCounter > 0)
	{
		pCurInfo->DevConnectCounter--;
	}

	_SubSppUpdateDevInfo(pDev);
}
/******************************************************************************************/
/**
 * \fn		void _SubBtDisconnect(BtAddr_t *pAddr)
 *
 * \brief		do disconnect
 *
 * \param
 *              		pAddr: bt addr
 *
 * \return	    void
 *
 ******************************************************************************************/
static void _SubBtDisconnect(BtAddr_t *pAddr)
{
	BtAddr_t  addr = *pAddr;
	(void)addr;
	//BtServiceA2dpDisconnect(&addr,PHONE_ADDR);
}
/******************************************************************************************/
/**
 * \fn		void _SubBtConnect(BtAddr_t *pAddr)
 *
 * \brief		do connect
 *
 * \param
 *              		pAddr: bt addr
 *
 * \return	    void
 *
 ******************************************************************************************/
static void _SubBtConnect(BtAddr_t *pAddr)
{
	BtAddr_t  addr = *pAddr;
	(void)addr;
	//BtServiceA2dpConnect(&addr,PHONE_ADDR);
}
/******************************************************************************************/
/**
 * \fn		void _SubSearchResultAction(const char **pBTDesName,BTSearchDevInfo *pBtSearchInfo)
 *
 * \brief		match the search result
 *
 * \param
 *              		pBTDesName:8107 name    pBtSearchInfo:search dev info
 *
 * \return	    void
 *
 ******************************************************************************************/
static void _SubSearchResultAction(const char **pBTDesName,BTSearchDevInfo *pBtSearchInfo)
{
	(void)pBTDesName;
	(void)pBtSearchInfo;
	stBtDevInfo *pRemoteDev = &SubRemoteDev[0];

	if((0 == memcmp(pBtSearchInfo->name,SubDesBtName,strlen(SubDesBtName))))
	{
		/*if(BtServiceIsSearching())
		{
			BtServiceStopSearch();
		}*/

		memset(pRemoteDev,0,sizeof(stBtDevInfo));
		pRemoteDev->bd_addr = pBtSearchInfo->BtAddr;
		memcpy(pRemoteDev->name,pBtSearchInfo->name,pBtSearchInfo->NameLen);
		pRemoteDev->name_len = pBtSearchInfo->NameLen;
	}

}
/******************************************************************************************/
/**
 * \fn		void _SubSearchFinishAction(void)
 *
 * \brief		search finish action
 *
 * \param
 *              		none
 *
 * \return	    void
 *
 ******************************************************************************************/
static void _SubSearchFinishAction(void)
{
	//int sdRemoteDevicesNum = 0;
	//int i = 0;
	//int eRet = SUCCESS;
	stBtDevInfo RemoteDevice = SubRemoteDev[0];
	BT_PROFILE_STATE_e state = DISCONNECT_OK;
	stBTInfo *pCurInfo = _GetBtCurInfo();
	//eRet = BtServiceGetRemoteDevNum(&sdRemoteDevicesNum);

	//if(eRet == SUCCESS)
	{
		//for(i = 1; i <= sdRemoteDevicesNum; i++)
		{
			//memset(&RemoteDevice, 0, sizeof(stBtDevInfo));
			//BtServiceGetRemoteDevByIdex(i, &RemoteDevice);
			if(0 == (memcmp(SubDesBtName,RemoteDevice.name,strlen(SubDesBtName))))
			{
				if((RemoteDevice.BtPairState == UNPAIR) || RemoteDevice.BtPairState == PAIR_NONE)
				{
					RemoteDevice.BtPairState = PAIRING;
					pCurInfo->CancelAutoConnectFalg = FALSE;
					//BtServicePairStart(&RemoteDevice.bd_addr);
				}

				return;
			}
		}
	}

	_SubConnectState(&state);

	if(state != CONNECT_OK)
	{
		//BtServiceStartSearch();
	}

}


static void _SubAclResultAction(BTConnectReqInfo *pResultInfo)
{
	stBTInfo *pCurInfo = _GetBtCurInfo();
	stBtDevInfo *pRemoteDevice = &SubRemoteDev[0];

	if(pResultInfo->result == FAIL)
	{
		pCurInfo->BaseState = idle;
		pRemoteDevice->BtPairState = PAIR_NONE;
	}
	else
	{
		pCurInfo->BaseState = pairing;
		pRemoteDevice->BtPairState = PAIRING;
	}
}
/******************************************************************************************/
/**
 * \fn		void _SubUnpair(BtAddr_t *pBtAddr)
 *
 * \brief		Unpair action
 *
 * \param
 *              		none
 *
 * \return	    void
 *
 ******************************************************************************************/
static void _SubUnpair(BtAddr_t *pBtAddr)
{
	stBTInfo *pCurInfo = _GetBtCurInfo();
	stBtDevInfo *pRemoteDevice = &SubRemoteDev[0];
	pCurInfo->CancelAutoConnectFalg = TRUE;
	BtAutoClear(ROLE_PHONE);
	memset(pRemoteDevice,0,sizeof(stBtDevInfo));
	pRemoteDevice->BtPairState = UNPAIR;
	BtDoUnpair(pBtAddr);
}


static void _SubPairingAction(BTPairDev *pPairInfo)
{
	(void)pPairInfo;
	stBTInfo *pCurInfo = _GetBtCurInfo();
	stBtDevInfo *pRemoteDevice = &SubRemoteDev[0];
	pRemoteDevice->BtPairState = PAIRING;
	pCurInfo->BaseState = pairing;
}
/******************************************************************************************/
/**
 * \fn		void _SubPairFinishAction(BTPairDev *pPairInfo)
 *
 * \brief		remote dev pair finish action
 *
 * \param
 *              		pPairInfo: pair info struct
 *
 * \return	    void
 *
 ******************************************************************************************/
static void _SubPairFinishAction(BTPairDev *pPairInfo)
{
	stBTInfo *pCurInfo = _GetBtCurInfo();
	stBtDevInfo *pRemoteDevice = &SubRemoteDev[0];

	if(!pCurInfo->IsSurportSwitch)
	{
		if(pPairInfo->result == SUCCESS)
		{
			pRemoteDevice->BtPairState = PAIRED;
			pCurInfo->BaseState = paired;

			//BtServiceA2dpConnect(&pPairInfo->bd_addr,PHONE_ADDR);
		}
		else
		{
			pCurInfo->BaseState = idle;

			if(!pCurInfo->CancelAutoConnectFalg)
			{
				pRemoteDevice->BtPairState = PAIRING;
				pCurInfo->BaseState = pairing;
				//BtServicePairStart(&pPairInfo->bd_addr);
			}

		#if 0
			//maybe have some buger here
			BtSetIsAutoConnectFail(TRUE,ROLE_PHONE);
			BtSetCurAutoState(ON);
			BtAutoSet(ON,ROLE_PHONE,&pPairInfo->bd_addr);
			BtSetAutoTime(6,ROLE_PHONE);
		#endif
		}
	}
}
/******************************************************************************************/
/**
 * \fn		void _SubBtReadyAction(int IsBtModeOn)
 *
 * \brief		bt ready
 *
 * \param
 *              		IsBtModeOn: if bt in  mode
 *
 * \return	    void
 *
 ******************************************************************************************/
static void _SubBtReadyAction(int IsBtModeOn)
{
	stBTInfo *pCurInfo = _GetBtCurInfo();
	BtAddr_t BtAddr = pCurInfo->LocalAddr;
	BtAddr_t BtAddrTemp = {{0}};

	//BtServiceGetLocalAddr(&BtAddrTemp);

	/*if((!pCurInfo->IsBtNameRamdom))
	{
		//BtServiceSetLocalName(pCurInfo->LocalName);
	}
	else
	{
		char NewName[NAME_LEN+20];
		memset(NewName,0,sizeof(NewName));
		int val = sprintf(NewName, "%s_%02x%02x%02x", pCurInfo->LocalName,
			BtAddrTemp.address[0],BtAddrTemp.address[1],BtAddrTemp.address[2]);
		//BtServiceSetLocalName(NewName);
		memcpy(pCurInfo->LocalName,NewName,val);
	}*/

	if(1)//(!pCurInfo->IsBtAddrRamdom)
	{
		//BtServiceSetLocalAddr(&BtAddr);
	}
	else
	{
		pCurInfo->LocalAddr.address[0] = BtAddrTemp.address[0];
		pCurInfo->LocalAddr.address[1] = BtAddrTemp.address[1];
		pCurInfo->LocalAddr.address[2] = BtAddrTemp.address[2];
		BtAddr = pCurInfo->LocalAddr;
		//BtServiceSetLocalAddr(&BtAddr);
	}

	bt_ao_ntc_printf("USER_NAME :%s",pCurInfo->LocalName);
	bt_ao_ntc_printf("USER_ADDR :%x,%x,%x \n",BtAddr.address[0],BtAddr.address[1],BtAddr.address[2]);

	if(IsBtModeOn != TRUE)
	{
		#if 0	//delete for samsung
		BtServiceSetScanMode(SCAN_NONE);
		#endif
	}
	else
	{
		_SubBtModeOnAction();
	}

}

/******************************************************************************************/
/**
 * \fn		void _SubBtDoPlayAction(void)
 *
 * \brief		ctrl bt remote dev do play
 *
 * \param
 *              		none
 *
 * \return	   void
 *
 ******************************************************************************************/
static void _SubBtDoPlayAction(void)
{
	BtAddr_t BtAddr = {{0}};
	BT_PROFILE_STATE_e state = DISCONNECT_OK;

	_SubConnectState(&state);
	if(state == CONNECT_OK)
	{
		_SubGetMainAddr(&BtAddr);
		//BtServiceAvrcpCtl(&BtAddr,AV_PLAY);
	}
}
/******************************************************************************************/
/**
 * \fn		void _SubBtDoPauseAction(void)
 *
 * \brief		ctrl bt remote dev do pause
 *
 * \param
 *              		none
 *
 * \return	    void
 *
 ******************************************************************************************/
static void _SubBtDoPauseAction(void)
{
	BtAddr_t BtAddr = {{0}};
	BT_PROFILE_STATE_e state = DISCONNECT_OK;

	_SubConnectState(&state);
	if(state == CONNECT_OK)
	{
		_SubGetMainAddr(&BtAddr);
		//BtServiceAvrcpCtl(&BtAddr,AV_PAUSE);
	}
}
/******************************************************************************************/
/**
 * \fn		void _SubBtDoPrevAction(void)
 *
 * \brief		ctrl bt remote dev do prev play
 *
 * \param
 *              		none
 *
 * \return	    void
 *
 ******************************************************************************************/
static void _SubBtDoPrevAction(void)
{
	BtAddr_t BtAddr = {{0}};
	BT_PROFILE_STATE_e state = DISCONNECT_OK;

	_SubConnectState(&state);
	if(state == CONNECT_OK)
	{
		_SubGetMainAddr(&BtAddr);
		//BtServiceAvrcpCtl(&BtAddr,AV_PREV);
	}

}
/******************************************************************************************/
/**
 * \fn		void _SubBtDoNextAction(void)
 *
 * \brief		ctrl bt remote dev do next play
 *
 * \param
 *              		none
 *
 * \return	    void
 *
 ******************************************************************************************/
static void _SubBtDoNextAction(void)
{

	BtAddr_t BtAddr = {{0}};
	BT_PROFILE_STATE_e state = DISCONNECT_OK;

	_SubConnectState(&state);
	if(state == CONNECT_OK)
	{
		_SubGetMainAddr(&BtAddr);
		//BtServiceAvrcpCtl(&BtAddr,AV_NEXT);
	}
}

/******************************************************************************************/
/**
 * \fn		void _SubBtClearAutoInfo(void)
 *
 * \brief		clear auto info
 *
 *
 * \return	    ret
 *
 ******************************************************************************************/
static int _SubBtClearAutoInfo(void)
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


static stBTAction BtSub =
{
	_SubConfigInfoInit,
	_SubBtModeOnAction,
	_SubBtModeOffAction,
	BtDoInit,
	BtDoDeInit,
	BtDoEnable,
	BtDoDisable,
	_SubBtReadyAction,
	IsBtDoSearching,
	BtDoStartSearch,
	BtDoStopSearch,
	_SubSearchResultAction,
	_SubSearchFinishAction,
	_SubAclResultAction,
	BtDoPairStart,
	_SubUnpair,
	_SubPairingAction,
	_SubPairFinishAction,
	_SubBtConnect,
	BtDenyConnect,
	_SubBtConnetingAction,
	_SubBtConnetOkAction,
	_SubBtConnectFailAction,
	_SubBtDisconnect,
	_SubBtDisconnetingAction,
	_SubBtDisconnetOkAction,
	NULL,
	BtDoSppConnect,
	NULL,
	_SubSppConnectOkAction,
	BtDoSppDisconnect,
	_SubSppDisconnectOkAction,
	NULL,
	BtDoUdtTest,
	BtDoRfPowerSet,
	_SubBtAutoConnectCheck,
	_SubBtAutoConnect,
	BtSetAutoConnectWorkState,
	BtDoScanModeSet,
	BtDoScanModeGet,
	NULL,
	NULL,
	NULL,
	_SubBtPowerOffRes,
	_SubBtDoPlayAction,
	_SubBtDoPauseAction,
	_SubBtDoNextAction,
	_SubBtDoPrevAction,
	_SubBtIsPlaying,
	NULL,
	NULL,
	_SubConnectTimeOutHandle,
	BtSetPinCode,
	BtReplyPinCode,
	_SubGetAddrByDevRole,
	_SubGetConnectStateByDevRole,
	BtDoGetProfileState,
	BtDoGetRemoteDevInfo,
	BtDoGetRemoteDevByIdex,
	BtDoLocalNameGet,
	BtDoLocalNameSet,
	NULL,
	NULL,
	NULL,
	BtDoLocalAddrGet,
	BtDoLocalAddrSet,
	BtDoGetLocalInfo,
	BtDoGetBondDevNum,
	BtDoGetBondByIdex,
	BtDoGetRemoteDevNum,
	NULL,
	NULL,
	BtDoSendUserCmd,
	BtDoAddBondDev,
	BtDoClearBondDev,
	_SubBtClearAutoInfo,
	BtDoGetRemoteDevRssi,
};

/******************************************************************************************/
/**
 * \fn		void BtActionGet(stBTAction **pBtAction)
 *
 * \brief		get the bt action api
 *
 * \param
 *              		pBtAction: bt action api struct
 *
 * \return	    void
 *
 ******************************************************************************************/
void BtActionGet(stBTAction **pBtAction)
{
	*pBtAction = &BtSub;
}
/******************************************************************************************/
/**
 * \fn		void BtActionDel(stBTAction **pBtAction)
 *
 * \brief		del pBtAction api
 *
 * \param
 *                pBtAction: bt action api struct
 *
 * \return	    void
 *
 ******************************************************************************************/
void BtActionDel(stBTAction **pBtAction)
{
	if(*pBtAction != NULL)
	{
		*pBtAction = NULL;
	}
}


#endif

