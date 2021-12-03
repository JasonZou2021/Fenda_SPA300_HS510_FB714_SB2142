#ifndef __BT_AO_AUTO_H__
#define __BT_AO_AUTO_H__
#include "bt_common.h"

// for bt auto mrg
typedef enum AutoRole
{
	ROLE_NONE = -1,
	ROLE_PHONE,
	ROLE_HEADPHONE,
	ROLE_WSS,
	ROLE_MAX,
}AutoRole_e;



typedef struct BtAutoCon_st
{
	ENABLE_STATE_e WorkState;// auto connect work on or off
	ENABLE_STATE_e CurAutoState;
	AUTOCONN_DEX_e role;//set auto connect target role:headphone or phone
	int  AutoLatency;
	char RetryTimes;
}BtAutoConConfig;



typedef struct AutoConMgr
{
	AutoRole_e role;
	BtAddr_t address;
	ENABLE_STATE_e state;
	ENABLE_STATE_e TimeOutFlag;//for dev connect timeout
	int Latency;//1s
	int TimeCounter;
	int IsAutoConnected;//if auto connect connect ok
	char IsConnectFail;//for dev connect fail and ao continue connect
	char RetryTimes;
	char CounterRetryTimes;
}stAutoConMgr;


void BtAutoInit(BtAutoConConfig *pAutoCnfInfo);
void BtSetIsAutoConnected(int IsConnected,AutoRole_e role);
void BtSetIsAutoConnectFail(int IsConnected,AutoRole_e role);
void BtAutoClear(int role);
void BtSetAutoConnectWorkState(ENABLE_STATE_e state);
void BtSetCurAutoState(ENABLE_STATE_e state);
int BtAutoConnectCheck(AutoRole_e AutoRole);
int BtAutoCon(AutoRole_e AutoRole);
int *BtGetAutoCounterTime(AutoRole_e role);
int BtGetAutoTime(AutoRole_e role);
int BtSetAutoTime(int time500Ms,AutoRole_e role);
int BtIsAutoConnected(AutoRole_e role);
int BtIsAutoConnectFail(AutoRole_e role);
int BtSetAutoState(ENABLE_STATE_e state,AutoRole_e role);
int BtSetAutoTimeOutFlag(ENABLE_STATE_e state,AutoRole_e role);
int BtSetAutoRetryTimes(int times,AutoRole_e role);
int BtGetAutoAddress(BtAddr_t *pAddress,AutoRole_e role);
int BtAutoSet(ENABLE_STATE_e state,AutoRole_e role,BtAddr_t *pAddress);
ENABLE_STATE_e BtGetAutoState(AutoRole_e role);
ENABLE_STATE_e BtGetAutoConnectWorkState(void);
ENABLE_STATE_e BtGetCurAutoState(void);
ENABLE_STATE_e BtGetAutoTimeOutFlag(AutoRole_e role);
AUTOCONN_DEX_e BtGetSurportAutoRole(void);
int BtAutoClearCurRetryTimes(AutoRole_e role);

#endif //__BT_AO_AUTO_H__
