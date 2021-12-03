#ifndef __BT_AO_REMOTE_C__
#define __BT_AO_REMOTE_C__
#include "types.h"
#include "bt_mw_core.h"
//#include "bt_ao_auto.h"
#include "bt_ao_remote.h"

typedef struct BtRemoteAppMgr
{
	stBtDevInfo    	RemoteDev[REMOTE_APP_MAX_DEV];
}stBtRemoteAppMgr;


//remote app
static int _BtRemoteAppInit(void);
static int _RemoteAppSppConnect(BtAddr_t *pAddr);
static int _RemoteAppSppDisconnect(BtAddr_t *pAddr);
static int _RemoteAppSppConnectOkAction(stBtDevInfo *pDev);
static int _RemoteAppSppDisconnectOkAction(stBtDevInfo *pDev);
static int _RemoteAppGetConnectState(BT_PROFILE_STATE_e *pState);
static int _RemoteAppGetAddr(BtAddr_t *pAddr);
static int _IsRemoteAppAddr(BtAddr_t *pAddr,int *pState);
static int _RemoteAppInfoUpdate(stBtDevInfo *pDev);


stBtRemoteAppMgr BtRemoteAppInfo;

static stBtRemoteAppAction RemoteAppAction =
{
	_BtRemoteAppInit,
	_RemoteAppSppConnect,
	_RemoteAppSppDisconnect,
	_RemoteAppSppConnectOkAction,
	_RemoteAppSppDisconnectOkAction,
	_RemoteAppGetConnectState,
	_RemoteAppGetAddr,
	_IsRemoteAppAddr,
	NULL,
};



//remote
static int _BtRemoteAppInit(void)
{
	int i = 0;

	for(i=0;i<REMOTE_APP_MAX_DEV;i++)
	{
		memset(&BtRemoteAppInfo,0,sizeof(stBtRemoteAppMgr));
	}

	return SUCCESS;
}


static stBtRemoteAppMgr * _BtGetRemoteAppInfo(void)
{
	return &BtRemoteAppInfo;
}


static int _IsRemoteAppAddr(BtAddr_t *pAddr,int *pState)
{
	stBtRemoteAppMgr *pBtCurInfo = _BtGetRemoteAppInfo();

	if(memcmp(&(pBtCurInfo->RemoteDev[REMOTE_APP_DEV].bd_addr),pAddr,sizeof(BtAddr_t))==0)
	{
		*pState = TRUE;
		return TRUE;
	}

	*pState = FALSE;
	return FALSE;
}


static int _RemoteAppInfoUpdate(stBtDevInfo *pDev)
{
	if(pDev != NULL)
	{
		stBtRemoteAppMgr *pBtCurInfo = _BtGetRemoteAppInfo();
		//LOGT("bt addr:");BLOCKPRINTF(&pDev->bd_addr,6);
		memcpy(&pBtCurInfo->RemoteDev[REMOTE_APP_DEV].bd_addr,&pDev->bd_addr,sizeof(BtAddr_t));
		pBtCurInfo->RemoteDev[REMOTE_APP_DEV].BtConnState |= pDev->BtConnState;
		return SUCCESS;
	}

	return FAIL;
}


static int _RemoteAppGetAddr(BtAddr_t *pAddr)
{
	stBtRemoteAppMgr *pBtCurInfo = _BtGetRemoteAppInfo();

	if(pAddr != NULL)
	{
		memcpy(pAddr,&pBtCurInfo->RemoteDev[REMOTE_APP_DEV].bd_addr,sizeof(BtAddr_t));
		return SUCCESS;
	}

	return FAIL;
}

#if 0
static int _RemoteAppSetConnectState(BT_PROFILE_STATE_e state)
{
	stBtRemoteAppMgr *pBtCurInfo = _BtGetRemoteAppInfo();
	stBtDevInfo *pDev = &pBtCurInfo->RemoteDev[REMOTE_APP_DEV];
	pDev->BtConnState = state;
	return SUCCESS;
}
#endif

static int _RemoteAppGetConnectState(BT_PROFILE_STATE_e *pState)
{
	stBtRemoteAppMgr *pBtCurInfo = _BtGetRemoteAppInfo();
	stBtDevInfo *pDev = &pBtCurInfo->RemoteDev[REMOTE_APP_DEV];

	if(pState != NULL)
	{
		*pState = pDev->BtConnState;
		return SUCCESS;
	}

	return FAIL;
}

#define SDP_UUID_MAX_LEN		16
#define SDP_SCLASS_UUID_SERIAL 0x1101


typedef struct _BT_SPP_PARMER{
      t_bdaddr             spp_connect_addr;
	  UINT8                connect_uuid[SDP_UUID_MAX_LEN];
	  UINT8                uuid_len;
}BTSppConnect;


static int _RemoteAppSppConnect(BtAddr_t *pAddr)
{
	/*(void)pAddr;
	BtAddr_t addr = {{0}};
	_RemoteAppGetAddr(&addr);

	if(pAddr != NULL)
	{
		BtServiceSppConnect(&addr);
		return SUCCESS;
	}

	return FAIL;*/
	(void)pAddr;
	BTSppConnect stSppInfo;
	memset(&stSppInfo,0x00,sizeof(stSppInfo));

	if(pAddr != NULL)
	{
		stSppInfo.connect_uuid[0] = SDP_SCLASS_UUID_SERIAL&0xff;
		stSppInfo.connect_uuid[1] = (SDP_SCLASS_UUID_SERIAL>>8)&(0xff);
		stSppInfo.uuid_len = 2;
		stSppInfo.spp_connect_addr = *(t_bdaddr *)pAddr;
		//SPP_IF_Connect(stSppInfo);
		return SUCCESS;
	}

	return FAIL;
}


static int _RemoteAppSppDisconnect(BtAddr_t *pAddr)
{
	(void)pAddr;
	BtAddr_t addr = {{0}};
	_RemoteAppGetAddr(&addr);

	if(pAddr != NULL)
	{
		//Sunplus//BtServiceSppDisconnect(&addr);
		//SPP_IF_Disconnect();
		return SUCCESS;
	}

	return FAIL;
}



static int _RemoteAppSppConnectOkAction(stBtDevInfo *pDev)
{
	if(pDev != NULL)
	{
		_RemoteAppInfoUpdate(pDev);
		return SUCCESS;
	}

	return FAIL;
}



static int _RemoteAppSppDisconnectOkAction(stBtDevInfo *pDev)
{
	if(pDev != NULL)
	{
		_RemoteAppInfoUpdate(pDev);
		return SUCCESS;
	}

	return FAIL;
}



stBtRemoteAppAction *BtGetRemoteAppAction(void)
{
	return &RemoteAppAction;
}

#endif  //__BT_AO_REMOTE_C__

