#ifndef __BT_AO_TWS_H__
#define __BT_AO_TWS_H__
#include "bt_common.h"


typedef void (*ReadyOkAction)(void);
typedef void (*ActionSearchResult)(const char **pBTDesName,BTSearchDevInfo *pBtSearchInfo);
typedef void (*ActionSearchFinish)(void);
typedef void (*ActionPairing)(BTPairDev *pPairInfo);
typedef void (*ActionPairFinish)(BTPairDev *pPairInfo);
typedef void (*ActionConnect)(BtAddr_t *pBtAddr);
typedef void (*ActionDisconnect)(BtAddr_t *pBtAddr);
typedef void (*ActionConnectOk)(stBtDevInfo *pDev);
typedef void (*ActionDisconnectOk)(stBtDevInfo *pDev);
typedef int (*ActionSppConnect)(BtAddr_t *pAddr);
typedef int (*ActionSppDisconnect)(BtAddr_t *pAddr);
typedef int (*ActionSppConnectOk)(stBtDevInfo *pDev);
typedef int (*ActionSppDisconnectOk)(stBtDevInfo *pDev);
typedef int (*ActionGetConnectState)(BT_PROFILE_STATE_e *state);
typedef int (*ActionGetSppConnectState)(BT_PROFILE_STATE_e *pState);
typedef  int (*ActionGetAddr)(BtAddr_t *pAddr);
typedef  int (*ActionPlay)(void);
typedef  int (*ActionPause)(void);
typedef int (*ActionIsEqualAddr)(BtAddr_t *pAddr,int *pState);
typedef int (*ActionIsFirstConnected)(void);
typedef void (*ActionAutoConnect)(char *pResult);
typedef void (*ActionVolumeSyncReg)(BtVolumeInfo *pVolumeInfo);
typedef void (*ActionVolumeSyncSet)(BtVolumeSet *pVolumeInfo);


typedef struct BtTwsAction
{
	ReadyOkAction			ReadyOk;
	ActionSearchResult		SearchResult;
	ActionSearchFinish		SearchFinish;
	ActionPairing			Pairing;
	ActionPairFinish		PairFinish;
	ActionConnect			Connect;
	ActionDisconnect		Disconnect;
	ActionConnectOk			ConnectOk;
	ActionDisconnectOk		DisconnectOk;
	ActionSppConnect		SppConnect;
	ActionSppDisconnect		SppDisconnect;
	ActionSppConnectOk		SppConnectOk;
	ActionSppDisconnectOk	SppDisconnectOk;
	ActionGetConnectState	GetConnectState;
	ActionGetSppConnectState GetSppConnectState;
	ActionGetAddr			GetAddr;
	ActionPlay				Play;
	ActionPause				Pause;
	ActionIsEqualAddr		IsEqualAddr;
	ActionIsFirstConnected	IsFirstConnected;
	ActionAutoConnect		AutoConnect;
	ActionVolumeSyncReg		VolumeSyncReg;
	ActionVolumeSyncSet		VolumeSyncSet;
}stBtTwsAction;


typedef struct TwsCbkIF
{
	int (*BtTwsSwichFinishSend)(SINK_MODE_e eMode);
}TwsCbkPost;


typedef struct BtTwsCtrl
{
	void (*BtTwsInit)(SINK_MODE_e role,stBtTwsAction **pAction,char IsSurportSwitch);
	void (*BtTwsSwitchActionInit)(SINK_MODE_e role);
	void (*BtTwsGetSinkAction)(stBtTwsAction **pAction);
}stBtTwsCtrlApi;


stBtTwsCtrlApi * BtGetTwsAction(void);
int BtTwsRegisterCbk(TwsCbkPost *pTwsCbk);

#endif

