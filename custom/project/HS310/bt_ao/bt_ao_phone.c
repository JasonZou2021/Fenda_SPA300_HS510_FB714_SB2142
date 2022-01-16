#ifndef __BT_AO_PHONE_C__
#define __BT_AO_PHONE_C__
#include <stdio.h>
#include "types.h"
#include "initcall.h"
#include "std_interfaces.h"
#define LOG_EXTRA_STR  "[AO]"
#define LOG_MODULE_ID  ID_BT
#include "log_utils.h"
#include "bt_mw_core.h"
#include "bt_ao_auto.h"
#include "bt_ao_phone.h"



typedef enum DEV_INFO_DEX
{
	PHONE_INFO,
	DEV_INFO_MAX = BT_SOURCE_DEV_NUM,
}DEV_INFO_DEX_e;


typedef struct BtSouceMgr
{
	//stBtDevAction 	*pSourceAction;
	stBtDevInfo    	RemoteDev[BT_SOURCE_DEV_NUM];
	BT_IR_CMD_e     AudioState;
	char 			IsRegVolumeSync;
}stBtSouceMgr;

//source dev :phone
//static void _PhoneConnect(BtAddr_t *pAddr);
//static void _PhoneConnectOkAction(stBtDevInfo *pDev ,char IsTrue);
//static void _PhoneDisconnect(BtAddr_t *pAddr);
//static void _PhoneDisconnectOkAction(stBtDevInfo *pDev ,char BtModeState);
//static void _PhoneSaveDevInfo(stBtDevInfo *pDev);
//static void _PhoneVolumeSyncReg(BtVolumeInfo *pVolumeInfo);
//static void _PhoneVolumeSyncSet(BtVolumeSet *pVolumeInfo);

//static int _PhonePlay(void);
//static int _PhonePause(void);
//static int _PhoneGetAddr(BtAddr_t *pAddr);
//static int _IsPhoneAddr(BtAddr_t *pAddr,int *pState);
//static int _PhoneConnectState(BT_PROFILE_STATE_e *pState);
//static int _PhoneIsPlaying(int *pIsTrue);


/*static stBtDevAction PhoneAction =
{
	_PhoneConnect,
	_PhoneDisconnect,
	_PhoneConnectOkAction,
	_PhoneDisconnectOkAction,
	_PhoneConnectState,
	_PhoneGetAddr,
	_PhonePlay,
	_PhonePause,
	_PhoneIsPlaying,
	_IsPhoneAddr,
	_PhoneVolumeSyncReg,
	_PhoneVolumeSyncSet,
};*/

static stBtSouceMgr SourceMgrInfo;


static stBtSouceMgr * _GetBtSourceCurInfo(void)
{
	return &SourceMgrInfo;
}


/*static void _PhoneDisconnect(BtAddr_t *pAddr)
{
	BtServiceA2dpDisconnect(pAddr,PHONE_ADDR);
}*/


/*static void _PhoneConnect(BtAddr_t *pAddr)
{
	BtServiceA2dpConnect(pAddr,PHONE_ADDR);
}*/


/*static void _PhoneConnectOkAction(stBtDevInfo *pDev ,char IsTrue)
{
	if(IsTrue == TRUE)
	{
		int role = ROLE_PHONE;

		BtServiceSetAutoAddr(&pDev->bd_addr);
		BtAutoClear(role);
		BtAutoClearCurRetryTimes(role);
	}
	else
	{
		_PhoneDisconnect(&pDev->bd_addr);
	}

	_PhoneSaveDevInfo(pDev);
}*/



/*static void _PhoneDisconnectOkAction(stBtDevInfo *pDev,char BtModeState)
{
	int state = FALSE;
	stBtSouceMgr *pBtCurInfo = _GetBtSourceCurInfo();

	if(BtModeState == TRUE)
	{
		int role = ROLE_PHONE;

		if(BtIsAutoConnected(role))
		{
			if(BtGetAutoTimeOutFlag(role) != ON)
			{
				if(_IsPhoneAddr(&pDev->bd_addr,&state))
				{
					BtSetAutoState(OFF,role);
				}
			}
		}
		else
		{
			if(_IsPhoneAddr(&pDev->bd_addr,&state))
			{
				BtSetAutoState(ON,role);
			}
		}
	}

	if(_IsPhoneAddr(&pDev->bd_addr,&state))
	{
		_PhoneSaveDevInfo(pDev);
		pBtCurInfo->IsRegVolumeSync = FALSE;
	}
}*/


/*static int _PhonePlay(void)
{
	BtAddr_t Addr = {{0}};
	int IsTrue = FALSE;

	_PhoneGetAddr(&Addr);
	_PhoneIsPlaying(&IsTrue);

	if(IsTrue)
	{
		return SUCCESS;
	}

	if(BtServiceIsA2dpConnected(&Addr))
	{
		BtServiceAvrcpCtl(&Addr,AV_PLAY);
	}

	return SUCCESS;
}*/


/*static int _PhonePause(void)
{
	BtAddr_t Addr = {{0}};
	int IsTrue = FALSE;

	_PhoneGetAddr(&Addr);
	_PhoneIsPlaying(&IsTrue);

	if(!IsTrue)
	{
		return SUCCESS;
	}

	if(BtServiceIsA2dpConnected(&Addr))
	{
		BtServiceAvrcpCtl(&Addr,AV_PAUSE);
	}

	return SUCCESS;
}*/


/*static int _IsPhoneAddr(BtAddr_t *pAddr,int *pState)
{
	stBtSouceMgr *pBtCurInfo = _GetBtSourceCurInfo();

	if(memcmp(&(pBtCurInfo->RemoteDev[PHONE_INFO].bd_addr),pAddr,sizeof(BtAddr_t))==0)
	{
		*pState = TRUE;
		return TRUE;
	}

	*pState = FALSE;
	return FALSE;
}*/


/*static int _PhoneConnectState(BT_PROFILE_STATE_e *pState)
{
	stBtSouceMgr *pBtCurInfo = _GetBtSourceCurInfo();

	if(pState != NULL)
	{
		*pState = BT_A2DP_PROFILE(pBtCurInfo->RemoteDev[PHONE_INFO].BtConnState);
		bt_ao_printf("\n State:%d \n",*pState);
		return SUCCESS;
	}

	return FAIL;
}*/



/*static void _PhoneSaveDevInfo(stBtDevInfo *pDev)
{
	stBtSouceMgr *pBtCurInfo = _GetBtSourceCurInfo();
	BtAddr_t *pBtAddr = &(pBtCurInfo->RemoteDev[PHONE_INFO].bd_addr);
	(void)pBtAddr;

	memcpy(&(pBtCurInfo->RemoteDev[PHONE_INFO]),pDev,sizeof(stBtDevInfo));

	bt_ao_printf("\n _PhoneSaveDevInfo:%x,%x,%x,%x,%x,%x\n",pBtAddr->address[0],pBtAddr->address[1], \
		pBtAddr->address[2],pBtAddr->address[3],pBtAddr->address[4],pBtAddr->address[5]);
}*/


/*static void _PhoneVolumeSyncReg(BtVolumeInfo *pVolumeInfo)
{
	(void)pVolumeInfo;
	stBtSouceMgr *pBtCurInfo = _GetBtSourceCurInfo();
	pBtCurInfo->IsRegVolumeSync = TRUE;
}*/


/*static void _PhoneVolumeSyncSet(BtVolumeSet *pVolumeInfo)
{
	(void)pVolumeInfo;
	stBtSouceMgr *pBtCurInfo = _GetBtSourceCurInfo();

	if(pBtCurInfo->IsRegVolumeSync == TRUE)
	{
		if(memcmp(&(pBtCurInfo->RemoteDev[PHONE_INFO].bd_addr),&pVolumeInfo->addr,sizeof(BtAddr_t))==0)
		{
			BtServiceAvrcpNotifyVolumeChange(&pVolumeInfo->addr,(UINT8)pVolumeInfo->volume,PHONE_ADDR);
		}
	}
}*/


/*static int _PhoneGetAddr(BtAddr_t *pAddr)
{
	stBtSouceMgr *pBtCurInfo = _GetBtSourceCurInfo();
	BtAddr_t TempAddr = {{0}};

	if(pAddr != NULL)
	{
		if(memcmp(&TempAddr,&pBtCurInfo->RemoteDev[PHONE_INFO].bd_addr,sizeof(BtAddr_t)) == 0)
		{
			BtServiceGetAutoAddr(pAddr);
		}
		else
		{
			memcpy(pAddr,&pBtCurInfo->RemoteDev[PHONE_INFO].bd_addr,sizeof(BtAddr_t));
		}
		return SUCCESS;
	}

	return FAIL;
}*/


/*static int _PhoneIsPlaying(int *pIsTrue)
{
	BT_IR_CMD_e state;
	BtAddr_t BtAddr = {{0}};

	_PhoneGetAddr(&BtAddr);
	BtServiceGetAudioState(&BtAddr,&state);

	if(AV_PLAY == state)
	{
		*pIsTrue = TRUE;
		return TRUE;
	}

	*pIsTrue = FALSE;
	return FALSE;
}*/

void PhoneSaveDevInfo(stBtDevInfo *pDev)
{
	stBtSouceMgr *pBtCurInfo = _GetBtSourceCurInfo();
	BtAddr_t *pBtAddr = &(pBtCurInfo->RemoteDev[PHONE_INFO].bd_addr);
	(void)pBtAddr;

	memcpy(&(pBtCurInfo->RemoteDev[PHONE_INFO]),pDev,sizeof(stBtDevInfo));

	bt_ao_ntc_printf("\n _PhoneSaveDevInfo:%x,%x,%x,%x,%x,%x\n",pBtAddr->address[0],pBtAddr->address[1], \
		pBtAddr->address[2],pBtAddr->address[3],pBtAddr->address[4],pBtAddr->address[5]);
	bt_ao_ntc_printf("BtConnState:%d",pBtCurInfo->RemoteDev[PHONE_INFO].BtConnState);
}


int IsPhoneAddr(BtAddr_t *pAddr,int *pState)
{
	stBtSouceMgr *pBtCurInfo = _GetBtSourceCurInfo();

	if(memcmp(&(pBtCurInfo->RemoteDev[PHONE_INFO].bd_addr),pAddr,sizeof(BtAddr_t))==0)
	{
		*pState = TRUE;
		return TRUE;
	}

	*pState = FALSE;
	return FALSE;
}


int PhoneGetAddr(BtAddr_t *pAddr)
{
	stBtSouceMgr *pBtCurInfo = _GetBtSourceCurInfo();
	BtAddr_t TempAddr = {{0}};

	if(pAddr != NULL)
	{
		if(memcmp(&TempAddr,&pBtCurInfo->RemoteDev[PHONE_INFO].bd_addr,sizeof(BtAddr_t)) == 0)
		{
			//BtServiceGetAutoAddr(pAddr);
		}
		else
		{
			memcpy(pAddr,&pBtCurInfo->RemoteDev[PHONE_INFO].bd_addr,sizeof(BtAddr_t));
		}
		return SUCCESS;
	}

	return FAIL;
}

extern int BtCheckAllDeviceProfileState(Profile_Name_e profile,BT_PROFILE_STATE_e *pstate);
void PhoneDisconnect(BtAddr_t *pAddr)
{
	/*(void)pAddr;
	BT_PROFILE_STATE_e AVDTP_state = DISCONNECT_OK;;
	BtCheckAllDeviceProfileState(AVDTP,&AVDTP_state);
	LOGD("AVDTP_state:%d\n",AVDTP_state);
	if(AVDTP_state == CONNECT_OK)
		SDK_Music_IF_Disconnect(pAddr); */ //zehai.liu todo

	(void)pAddr;
	stBtSouceMgr *pBtCurInfo = _GetBtSourceCurInfo();
	BtAddr_t *pBtAddr = &(pBtCurInfo->RemoteDev[PHONE_INFO].bd_addr);
	BT_PROFILE_STATE_e AVDTP_state = DISCONNECT_OK;;
	BtCheckAllDeviceProfileState(AVDTP,&AVDTP_state);
	LOGD("AVDTP_state:%d\n",AVDTP_state);
	if(AVDTP_state == CONNECT_OK)
		SDK_Music_IF_Disconnect(pBtAddr);
}


void PhoneConnect(BtAddr_t *pAddr)
{
	SDK_Music_IF_Connect(pAddr);
}

/*
static void _PhoneConnect(BtAddr_t *pAddr)
{
    INT8 i4_ret;
    BtAddr_t TV_addr;

    i4_ret = BtServiceA2dpConnect(pAddr, PHONE_ADDR);
    //  1 The current BT state is not in disconnected state
    //  0 Success ¡ä???¦Ì¡Á2?3¨¦1|
    // -1 Fail    ¡ä???¦Ì¡Á2?¨º¡ì¡ã¨¹
    // -2 This MAC address has not been paired

    bt_ao_printf("\n _PhoneConnect:%d \n", i4_ret);

    if(i4_ret > 0)
    {
        if(((SourceManage_GetSourceState() == FUNCTION_BT)||(SourceManage_GetSourceState() == FUNCTION_BTSTANDBY))
            &&(1 == a_Get_ExistOfTvMacAdress())
            &&(ON_TV_CONNECT == a_Get_TvConnectMode()))
        {
            c_timer_reset_and_work(mTimerBtStartTVConnectA2DP);
        }
    }
    else if(i4_ret == 0)
    {
        c_timer_stop(mTimerBtStartTVConnectA2DP);
    }
    else if(i4_ret == -1)
    {
        if(((SourceManage_GetSourceState() == FUNCTION_BT)||(SourceManage_GetSourceState() == FUNCTION_BTSTANDBY))
            &&(1 == a_Get_ExistOfTvMacAdress())
            &&(ON_TV_CONNECT == a_Get_TvConnectMode()))
        {
            //c_timer_reset_time(mTimerBtStartTVConnectA2DP, 400);
            c_timer_reset_and_work(mTimerBtStartTVConnectA2DP);
        }
    }
    else if(i4_ret == -2)
    {
        if(((SourceManage_GetSourceState() == FUNCTION_BT)||(SourceManage_GetSourceState() == FUNCTION_BTSTANDBY))
            &&(1 == a_Get_ExistOfTvMacAdress())
            &&(ON_TV_CONNECT == a_Get_TvConnectMode()))
		{
			//BtAddr_t TV_addr;

            BT_Pairing_TV_flag = 1;

            TV_addr = Spdif_Get_TV_MAC_Address();

            BTDevClassChange(1, 0);
            BTLocalNameChange(0);

            BtPairSart(&TV_addr);
        }
    }
}

*/

int PhonePlay(void)
{
	//BtAddr_t Addr = {{0}};
	/*int IsTrue = FALSE;
	BT_PROFILE_STATE_e state = DISCONNECT_OK;

	PhoneIsPlaying(&IsTrue);

	if(IsTrue)
	{
		return SUCCESS;
	}*/ //zehai.liu todo

	BT_PROFILE_STATE_e state = DISCONNECT_OK;

	PhoneConnectStateGet(&state);

	if(state == CONNECT_OK)
	{
		LOGD("SDK_Music_IF_Play!!\n");
		BtAddr_t Addr;
		PhoneGetAddr(&Addr);
		SDK_Music_IF_Play(&Addr);
	}

	return SUCCESS;
}


int PhonePause(void)
{
	//BtAddr_t Addr = {{0}};
	/*int IsTrue = FALSE;
	BT_PROFILE_STATE_e state = DISCONNECT_OK;

	PhoneIsPlaying(&IsTrue);

	if(!IsTrue)
	{
		return SUCCESS;
	}*/ //zehai.liu todo

	BT_PROFILE_STATE_e state = DISCONNECT_OK;

	PhoneConnectStateGet(&state);

	if(state == CONNECT_OK)
	{
		LOGD("SDK_Music_IF_Pause!!\n");
		BtAddr_t Addr;
		PhoneGetAddr(&Addr);
		SDK_Music_IF_Pause(&Addr);
	}

	return SUCCESS;
}


int PhonePrev(void)
{
	//BtAddr_t Addr = {{0}};
	BT_PROFILE_STATE_e state = DISCONNECT_OK;

	PhoneConnectStateGet(&state);

	if(state == CONNECT_OK)
	{
		LOGD("SDK_Music_IF_Prev!!\n");
		BtAddr_t Addr;
		PhoneGetAddr(&Addr);
		SDK_Music_IF_Prev(&Addr);
	}

	return SUCCESS;
}


int PhoneNext(void)
{
	//BtAddr_t Addr = {{0}};
	BT_PROFILE_STATE_e state = DISCONNECT_OK;

	PhoneConnectStateGet(&state);

	if(state == CONNECT_OK)
	{
		LOGD("SDK_Music_IF_Next!!\n");
		BtAddr_t Addr;
		PhoneGetAddr(&Addr);
		SDK_Music_IF_Next(&Addr);
	}

	return SUCCESS;
}



int PhoneConnectStateGet(BT_PROFILE_STATE_e *pState)
{
	stBtSouceMgr *pBtCurInfo = _GetBtSourceCurInfo();

	if(pState != NULL)
	{
		*pState = BT_A2DP_PROFILE(pBtCurInfo->RemoteDev[PHONE_INFO].BtConnState);
		bt_ao_printf("\n State:%d \n",*pState);
		return SUCCESS;
	}

	return FAIL;
}


int PhoneAudioStateGet(BT_IR_CMD_e *pState)
{
	stBtSouceMgr *pBtCurInfo = _GetBtSourceCurInfo();
	*pState = pBtCurInfo->AudioState;

	return SUCCESS;
}


int PhoneAudioPlayStateUpdate(BT_IR_CMD_e state)
{
	stBtSouceMgr *pBtCurInfo = _GetBtSourceCurInfo();
	pBtCurInfo->AudioState = state;

	return SUCCESS;
}


void PhonePairFailAction(BTPairDev *pDev)
{
	stBtSouceMgr *pBtCurInfo = _GetBtSourceCurInfo();
	BtAddr_t *pBtAddr = &pDev->bd_addr;

	//_PhoneSaveDevInfo(pDev);
	bt_ao_ntc_printf("\n _PhoneSaveDevInfo:%x,%x,%x,%x,%x,%x\n",pBtAddr->address[0],pBtAddr->address[1], \
			pBtAddr->address[2],pBtAddr->address[3],pBtAddr->address[4],pBtAddr->address[5]);

	pBtCurInfo->RemoteDev[PHONE_INFO].bd_addr = pDev->bd_addr;
	memcpy(pBtCurInfo->RemoteDev[PHONE_INFO].name,pDev->name,pDev->name_len);
	pBtCurInfo->RemoteDev[PHONE_INFO].name_len = pDev->name_len;
	memcpy(pBtCurInfo->RemoteDev[PHONE_INFO].linkKey,pDev->linkKey,sizeof(pDev->linkKey));
	pBtCurInfo->RemoteDev[PHONE_INFO].cod = pDev->cod;
	pBtCurInfo->RemoteDev[PHONE_INFO].DevRole = PHONE_ADDR;
	pBtCurInfo->RemoteDev[PHONE_INFO].BtPairState = 0;
	pBtCurInfo->RemoteDev[PHONE_INFO].BtConnState = 0;
	pBtCurInfo->RemoteDev[PHONE_INFO].result = SUCCESS;

}



void PhonePairOkAction(BTPairDev *pDev)
{
	stBtSouceMgr *pBtCurInfo = _GetBtSourceCurInfo();
	BtAddr_t *pBtAddr = &pDev->bd_addr;

	bt_ao_ntc_printf("\n pair ok:%x,%x,%x,%x,%x,%x\n",pBtAddr->address[0],pBtAddr->address[1], \
			pBtAddr->address[2],pBtAddr->address[3],pBtAddr->address[4],pBtAddr->address[5]);

	pBtCurInfo->RemoteDev[PHONE_INFO].bd_addr = pDev->bd_addr;
	memcpy(pBtCurInfo->RemoteDev[PHONE_INFO].name,pDev->name,pDev->name_len);
	pBtCurInfo->RemoteDev[PHONE_INFO].name_len = pDev->name_len;
	memcpy(pBtCurInfo->RemoteDev[PHONE_INFO].linkKey,pDev->linkKey,sizeof(pDev->linkKey));
	pBtCurInfo->RemoteDev[PHONE_INFO].cod = pDev->cod;
	pBtCurInfo->RemoteDev[PHONE_INFO].DevRole = PHONE_ADDR;
	pBtCurInfo->RemoteDev[PHONE_INFO].BtPairState = 1;
	pBtCurInfo->RemoteDev[PHONE_INFO].BtConnState = 0;
	pBtCurInfo->RemoteDev[PHONE_INFO].result = SUCCESS;

}


void PhoneConnectOkAction(stBtDevInfo *pDev)
{
	//int role = ROLE_PHONE;
	stBtSouceMgr *pBtCurInfo = _GetBtSourceCurInfo();
	BtAddr_t *pBtAddr = &(pBtCurInfo->RemoteDev[PHONE_INFO].bd_addr);

	//BtAutoClear(role);
	//BtAutoClearCurRetryTimes(role);
	//bt_ao_ntc_printf("connect ok:%d",pDev->BtConnState);

	bt_ao_ntc_printf("\n %x,%x,%x,%x,%x,%x\n",pDev->bd_addr.address[0],pDev->bd_addr.address[1], \
				pDev->bd_addr.address[2],pDev->bd_addr.address[3],pDev->bd_addr.address[4],pDev->bd_addr.address[5]);

	bt_ao_ntc_printf("\n %x,%x,%x,%x,%x,%x\n",pBtAddr->address[0],pBtAddr->address[1], \
					pBtAddr->address[2],pBtAddr->address[3],pBtAddr->address[4],pBtAddr->address[5]);

	//if(memcmp(pBtAddr->address,&pDev->bd_addr,sizeof(BtAddr_t))==0)
	{
		bt_ao_ntc_printf("connect ok:%d \n",pDev->BtConnState);
		pBtCurInfo->RemoteDev[PHONE_INFO].BtConnState = pDev->BtConnState;
	}

}


void PhoneDisconnectOkAction(stBtDevInfo *pDev)
{
	int state = FALSE;
	stBtSouceMgr *pBtCurInfo = _GetBtSourceCurInfo();

	/*int role = ROLE_PHONE;


	bt_ao_ntc_printf("\n %x,%x,%x,%x,%x,%x\n",pDev->bd_addr.address[0],pDev->bd_addr.address[1], \
				pDev->bd_addr.address[2],pDev->bd_addr.address[3],pDev->bd_addr.address[4],pDev->bd_addr.address[5]);

	if(BtIsAutoConnected(role))
	{
		if(BtGetAutoTimeOutFlag(role) != ON)
		{
			if(IsPhoneAddr(&pDev->bd_addr,&state))
			{
				BtSetAutoState(OFF,role);
			}
		}
	}
	else
	{
		if(IsPhoneAddr(&pDev->bd_addr,&state))
		{
			BtSetAutoState(ON,role);
		}
	}*/

	if(IsPhoneAddr(&pDev->bd_addr,&state))
	{
		bt_ao_ntc_printf("disconnect ok:%d \n",pDev->BtConnState);
		pBtCurInfo->RemoteDev[PHONE_INFO].BtConnState = pDev->BtConnState;
		pBtCurInfo->IsRegVolumeSync = FALSE;
	}
}

void PhoneVolumeSyncReg(BtVolumeInfo *pVolumeInfo)
{
	(void)pVolumeInfo;
	stBtSouceMgr *pBtCurInfo = _GetBtSourceCurInfo();
	pBtCurInfo->IsRegVolumeSync = TRUE;
}


void PhoneVolumeSyncSet(BtVolumeSet *pVolumeInfo)
{
	(void)pVolumeInfo;
	stBtSouceMgr *pBtCurInfo = _GetBtSourceCurInfo();

	if(pBtCurInfo->IsRegVolumeSync == TRUE)
	{
		if(memcmp(&(pBtCurInfo->RemoteDev[PHONE_INFO].bd_addr),&pVolumeInfo->addr,sizeof(BtAddr_t))==0)
		{
			//BtServiceAvrcpNotifyVolumeChange(&pVolumeInfo->addr,(UINT8)pVolumeInfo->volume,PHONE_ADDR);
		}
	}
}


int PhoneIsPlaying(int *pIsTrue)
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


//int BtSourceInit(stBtDevAction **pAction)
int BtSourceInit(void)
{
	int i = 0;
	stBtSouceMgr *pSourceMgrInfo = &SourceMgrInfo;
	//*pAction = &PhoneAction;
	//pSourceMgrInfo->pSourceAction = &PhoneAction;

	memset(&pSourceMgrInfo->RemoteDev,0,sizeof(stBtDevInfo)*DEV_INFO_MAX);

	for(i=0;i<DEV_INFO_MAX;i++)
	{
		pSourceMgrInfo->RemoteDev[i].BtConnState = (DISCONNECT_OK<<BT_A2DP_PROFILE_BIT)|(DISCONNECT_OK<<BT_SPP_PROFILE_BIT);
		pSourceMgrInfo->RemoteDev[i].BtPairState = PAIR_NONE;
		pSourceMgrInfo->RemoteDev[i].DevRole = UNKOWN_ROLE_ADDR;
	}

	return SUCCESS;
}

#endif

