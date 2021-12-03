#ifndef __BT_AO_AUTO_C__
#define __BT_AO_AUTO_C__
#include "initcall.h"
#define LOG_EXTRA_STR  "[AO]"
#define LOG_MODULE_ID  ID_BT
#include "log_utils.h"
#include "std_interfaces.h"
#include "types.h"
#include "bt_ao_auto.h"
#include "bt_mw_core.h"
#include "bt_ao_phone.h"


static stAutoConMgr AutoMgr[ROLE_MAX];
static BtAutoConConfig BtAutoConfigInfo = {OFF,OFF,AUTO_NONE,15,3};


//AutoMgr api
static BtAutoConConfig *_BtGetAutoConfigInfo(void)
{
	return &BtAutoConfigInfo;
}



static stAutoConMgr *_GetBtAutoInfo(AutoRole_e role)
{
	return &AutoMgr[role];
}


static int _BtSetAutoRole(AutoRole_e role)
{
	stAutoConMgr *pAutoInfo = _GetBtAutoInfo(role);
	pAutoInfo->role = role;
	return SUCCESS;
}



static int _BtSetAutoAddress(BtAddr_t *pAddress,AutoRole_e role)
{
	stAutoConMgr *pAutoInfo = _GetBtAutoInfo(role);

	if(pAddress != NULL)
	{
		pAutoInfo->address = *pAddress;
		return SUCCESS;
	}

	return FAIL;
}




void BtAutoInit(BtAutoConConfig *pAutoCnfInfo)
{
	int i = 0;
	BtAutoConConfig *pAutoConfigInfo = _BtGetAutoConfigInfo();
	stAutoConMgr AutoInfo = {ROLE_NONE,{{0}},OFF,OFF,20,0,FALSE,FALSE,0xff,0};

	for(i=0;i<ROLE_MAX;i++)
	{
		AutoMgr[i] = AutoInfo;
	}

	memcpy(pAutoConfigInfo,pAutoCnfInfo,sizeof(BtAutoConConfig));

	if(pAutoCnfInfo->WorkState == ON)
	{
		if(pAutoCnfInfo->role == AUTO_PHONE_TWS)
		{
			BtSetAutoTime(pAutoCnfInfo->AutoLatency,ROLE_PHONE);
			BtSetAutoTime(pAutoCnfInfo->AutoLatency,ROLE_HEADPHONE);
			BtSetAutoRetryTimes(pAutoCnfInfo->RetryTimes,ROLE_PHONE);
			BtSetAutoTime(pAutoCnfInfo->AutoLatency,ROLE_WSS);
			BtSetAutoRetryTimes(pAutoCnfInfo->RetryTimes,ROLE_WSS);
		}
		else if(pAutoCnfInfo->role == AUTO_PHONE)
		{
			BtSetAutoTime(pAutoCnfInfo->AutoLatency,ROLE_PHONE);
			BtSetAutoRetryTimes(pAutoCnfInfo->RetryTimes,ROLE_PHONE);
		}
		else if(pAutoCnfInfo->role == AUTO_TWS)
		{
			BtSetAutoTime(pAutoCnfInfo->AutoLatency,ROLE_HEADPHONE);
			BtSetAutoRetryTimes(pAutoCnfInfo->RetryTimes,ROLE_HEADPHONE);
			BtSetAutoTime(pAutoCnfInfo->AutoLatency,ROLE_WSS);
			BtSetAutoRetryTimes(pAutoCnfInfo->RetryTimes,ROLE_WSS);
		}
	}
}



void BtSetIsAutoConnected(int IsConnected,AutoRole_e role)
{
	stAutoConMgr *pAutoInfo = _GetBtAutoInfo(role);
	pAutoInfo->IsAutoConnected = IsConnected;

	if(IsConnected)
	{
		pAutoInfo->CounterRetryTimes = 0;
	}
}



int BtIsAutoConnected(AutoRole_e role)
{
	stAutoConMgr *pAutoInfo = _GetBtAutoInfo(role);
	return pAutoInfo->IsAutoConnected;
}


void BtSetIsAutoConnectFail(int IsConnected,AutoRole_e role)
{
	stAutoConMgr *pAutoInfo = _GetBtAutoInfo(role);
	pAutoInfo->IsConnectFail = IsConnected;
}


int BtIsAutoConnectFail(AutoRole_e role)
{
	stAutoConMgr *pAutoInfo = _GetBtAutoInfo(role);
	return pAutoInfo->IsConnectFail;
}



ENABLE_STATE_e BtGetAutoState(AutoRole_e role)
{
	stAutoConMgr *pAutoInfo = _GetBtAutoInfo(role);
	return pAutoInfo->state;
}



int BtSetAutoState(ENABLE_STATE_e state,AutoRole_e role)
{
	stAutoConMgr *pAutoInfo = _GetBtAutoInfo(role);
	pAutoInfo->state = state;
	pAutoInfo->TimeCounter = 0;
	return SUCCESS;
}


int BtSetAutoTimeOutFlag(ENABLE_STATE_e state,AutoRole_e role)
{
	stAutoConMgr *pAutoInfo = _GetBtAutoInfo(role);
	pAutoInfo->TimeOutFlag = state;
	return 0;
}



ENABLE_STATE_e BtGetAutoTimeOutFlag(AutoRole_e role)
{
	stAutoConMgr *pAutoInfo = _GetBtAutoInfo(role);
	return pAutoInfo->TimeOutFlag;
}



int BtGetAutoTime(AutoRole_e role)
{
	stAutoConMgr *pAutoInfo = _GetBtAutoInfo(role);
	return pAutoInfo->Latency;
}



int BtSetAutoTime(int time1s,AutoRole_e role)
{
	stAutoConMgr *pAutoInfo = _GetBtAutoInfo(role);
	pAutoInfo->Latency = time1s;
	return SUCCESS;
}


int *BtGetAutoCounterTime(AutoRole_e role)
{
	stAutoConMgr *pAutoInfo = _GetBtAutoInfo(role);
	return &pAutoInfo->TimeCounter;
}



#if 1
static int _BtGetAutoRetryTimes(AutoRole_e role)
{
	stAutoConMgr *pAutoInfo = _GetBtAutoInfo(role);
	return pAutoInfo->RetryTimes;
}
#endif


int BtSetAutoRetryTimes(int times,AutoRole_e role)
{
	stAutoConMgr *pAutoInfo = _GetBtAutoInfo(role);
	pAutoInfo->RetryTimes= times;
	return SUCCESS;
}


extern int UserSetting_ReadBtAutoConDev(BtAddr_t * auto_addr);

int BtGetAutoAddress(BtAddr_t *pAddress,AutoRole_e role)
{
	//stAutoConMgr *pAutoInfo = _GetBtAutoInfo(role);

	(void) role;
	BtAddr_t Addr = {{0}};
	UserSetting_ReadBtAutoConDev(&Addr);

	if(pAddress != NULL)
	{
		//*pAddress = pAutoInfo->address;
		*pAddress = Addr;
		return SUCCESS;
	}

	return FAIL;
}



void BtAutoClear(int role)
{
	int *pTimeCounter = BtGetAutoCounterTime(role);
	BtAutoConConfig *pAutoConfigInfo = _BtGetAutoConfigInfo();

	*pTimeCounter = 0;
	BtSetAutoState(OFF,role);
	BtSetAutoTime(pAutoConfigInfo->AutoLatency,role);
	_BtSetAutoRole(role);

	if(!BtIsAutoConnected(role))
	{
		BtSetIsAutoConnected(TRUE,role);
	}

	if(BtGetAutoTimeOutFlag(role) == ON)
	{
		BtSetAutoTimeOutFlag(OFF,role);
	}

	if(BtIsAutoConnectFail(role))
	{
		BtSetIsAutoConnectFail(FALSE,role);
	}

}



int BtAutoSet(ENABLE_STATE_e state,AutoRole_e role,BtAddr_t *pAddress)
{
	int *pTimeCounter = BtGetAutoCounterTime(role);
	*pTimeCounter = 0;

	if(pAddress != NULL)
	{
		_BtSetAutoRole(role);
		BtSetAutoState(state,role);
		_BtSetAutoAddress(pAddress,role);
		return SUCCESS;
	}

	return FAIL;
}


int BtAutoClearCurRetryTimes(AutoRole_e role)
{
	stAutoConMgr *pAutoInfo = _GetBtAutoInfo(role);
	pAutoInfo->CounterRetryTimes = 0;
	return 0;
}

ENABLE_STATE_e BtGetAutoConnectWorkState(void)
{
	BtAutoConConfig *pAutoCnfInfo = _BtGetAutoConfigInfo();
	return pAutoCnfInfo->WorkState;
}


void BtSetCurAutoState(ENABLE_STATE_e state)
{
	BtAutoConConfig *pAutoCnfInfo = _BtGetAutoConfigInfo();
	pAutoCnfInfo->CurAutoState = state;
}


ENABLE_STATE_e BtGetCurAutoState(void)
{
	BtAutoConConfig *pAutoCnfInfo = _BtGetAutoConfigInfo();
	return pAutoCnfInfo->CurAutoState;
}


AUTOCONN_DEX_e BtGetSurportAutoRole(void)
{
	BtAutoConConfig *pAutoCnfInfo = _BtGetAutoConfigInfo();
	return pAutoCnfInfo->role;
}



void BtSetAutoConnectWorkState(ENABLE_STATE_e state)
{
	BtAutoConConfig *pAutoCnfInfo = _BtGetAutoConfigInfo();
	pAutoCnfInfo->WorkState = state;

	switch(pAutoCnfInfo->role)
	{
	case AUTO_PHONE_TWS:
	{
		BtSetAutoTime(pAutoCnfInfo->AutoLatency,ROLE_PHONE);
		BtSetAutoTime(pAutoCnfInfo->AutoLatency,ROLE_HEADPHONE);
		BtSetAutoTime(pAutoCnfInfo->AutoLatency,ROLE_WSS);
		break;
	}
	case AUTO_TWS:
	{
		BtSetAutoTime(pAutoCnfInfo->AutoLatency,ROLE_HEADPHONE);
		BtSetAutoTime(pAutoCnfInfo->AutoLatency,ROLE_WSS);
		break;
	}
	case AUTO_PHONE:
	{
		BtSetAutoTime(pAutoCnfInfo->AutoLatency,ROLE_PHONE);
		break;
	}
	case AUTO_NONE:
	{
		break;
	}
	}
}

extern int FirstAutoConFlag;

int BtAutoConnectCheck(AutoRole_e AutoRole)
{
	stAutoConMgr *pDevAutoInfo = _GetBtAutoInfo(AutoRole);
	int *pConnectTime = BtGetAutoCounterTime(AutoRole);
	AUTOCONN_DEX_e AutoMode = BtGetSurportAutoRole();

	if(AutoMode == AUTO_NONE)
	{
		return  FAIL;
	}
	else if(AutoMode == AUTO_TWS)
	{
		if(AutoRole == ROLE_PHONE)
		{
			return  FAIL;
		}
	}
	else if(AutoMode == AUTO_PHONE)
	{
		if(AutoRole == ROLE_WSS || AutoRole == ROLE_HEADPHONE)
		{
			return  FAIL;
		}
	}

	do
	{
		if(BtGetAutoState(AutoRole) != ON)
		{
		 	bt_ao_printf("role:0x%x,auto != ON\n",AutoRole);
			*pConnectTime = 0;
			return  FAIL;
		}

		if(pDevAutoInfo->CounterRetryTimes>=_BtGetAutoRetryTimes(AutoRole) &&  _BtGetAutoRetryTimes(AutoRole)!=0xff)
		{
			bt_ao_ntc_printf("retry times %d \n",pDevAutoInfo->CounterRetryTimes);
			*pConnectTime = 0;
			return  FAIL;
		}

		*pConnectTime = *pConnectTime + 1;

		/*if((*pConnectTime) < BtGetAutoTime(AutoRole))
		{
			bt_ao_printf("ConnectTime = %d\n",*pConnectTime);
			return  FAIL;
		}*/

		if (FirstAutoConFlag == FALSE)
		{
			if((*pConnectTime) < BtGetAutoTime(AutoRole))
		    {
				bt_ao_printf("ConnectTime = %d\n",*pConnectTime);
				bt_ao_ntc_printf("[%s][%d].\n", __FUNCTION__, __LINE__);
			    return  FAIL;
		    }
		}

        FirstAutoConFlag = FALSE;
		BtSetIsAutoConnected(FALSE,AutoRole);
		*pConnectTime = 0;
		pDevAutoInfo->CounterRetryTimes++;
	}while(0);

	return	SUCCESS;
}



int BtAutoCon(AutoRole_e AutoRole)
{
	BtAddr_t Addr = {{0}};
	BtAutoConConfig *pAutoCnfInfo = _BtGetAutoConfigInfo();

	BtGetAutoAddress(&Addr,AutoRole);

	if(BtIsAutoConnectFail(AutoRole))
	{
		//BtServiceUnpair(&Addr);
		//BtServicePairStart(&Addr);
		BtSetAutoTime(pAutoCnfInfo->AutoLatency,AutoRole);
	}
	else
	{
		/*if(AutoRole == ROLE_PHONE)
		{
			bt_ao_printf("PHONE_ADDR;%x,%x",Addr.address[0],Addr.address[1]);
			BtServiceA2dpConnect(&Addr,PHONE_ADDR);
		}
		else if(AutoRole == ROLE_HEADPHONE)
		{
			BtServiceA2dpConnect(&Addr,HEADPHONE_ADDR);
		}
		else if(AutoRole == ROLE_WSS)
		{
			BtServiceA2dpConnect(&Addr,WSS_ADDR);
		}*/
		if(AutoRole == ROLE_PHONE)
		{
			//bt_ao_printf("PHONE_ADDR;%x,%x",Addr.address[0],Addr.address[1]);
			LOGD("[0x%02x][0x%02x][0x%02x][0x%02x][0x%02x][0x%02x]......\n",
			      Addr.address[0],Addr.address[1],
			      Addr.address[2],Addr.address[3],
			      Addr.address[4],Addr.address[5]);

			SDK_Music_IF_Connect(&Addr);
		}
	}

	return SUCCESS;
}


#endif //__BT_AO_AUTO_C__
