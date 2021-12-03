#ifndef __BT_AO_REMOTE_H__
#define __BT_AO_REMOTE_H__

#include "bt_ao_tws.h"



typedef enum eRemoteAppRole
{
	REMOTE_APP_DEV = 0,
	REMOTE_APP_MAX_DEV,
}RemoteAppRole_e;


typedef int (*ActionRemoteAppInit)(void);

typedef struct BtRemoteAppAction
{
	ActionRemoteAppInit		RemoteAppInit;
	ActionSppConnect		SppConnect;
	ActionSppDisconnect		SppDisconnect;
	ActionSppConnectOk		SppConnectOk;
	ActionSppDisconnectOk	SppDisconnectOk;
	ActionGetConnectState	GetConnectState;
	ActionGetAddr			GetAddr;
	ActionIsEqualAddr		IsEqualAddr;
	ActionAutoConnect		AutoConnect;
}stBtRemoteAppAction;



stBtRemoteAppAction *BtGetRemoteAppAction(void);

#endif //__BT_AO_REMOTE_H__

