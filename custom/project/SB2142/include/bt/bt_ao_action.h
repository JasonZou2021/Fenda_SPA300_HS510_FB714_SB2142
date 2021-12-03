#ifndef __BT_AO_ACTION_H__
#define __BT_AO_ACTION_H__

#include "types.h"
#include "bt_common.h"
#include "bt_ao_comment_action.h"



typedef enum
{
    idle,
    inquiring,
    pairing,
    paired,
    connecting,
    connected,
} BTBaseStates_t;



//for bt local current info
typedef struct st_BTInfo
{
	BT_PLATFORM_ROLE_e PlatformRole;
	BT_STATE_e		BtState;
	BtAddr_t		LocalAddr;
	char			LocalName[NAME_LEN];
	//char 			BtName[NAME_LEN];
	const char 		**pBTDesNameList;
	char 			*pBTDesName;
	char  			BtSurportAutoRun;//on, off
	char 			IsSurportSwitch;
	char			IsBtSimpleModeEn;
	char 			IsBtAacCodecEn;
	char 			IsIgnoreSrcSwitch;
	char 			IsIgnoreTimeout;
	char 			DevMaxConnect;
	char 			DevConnectCounter;
	char  			IsBtMode;
	char			IsBtSourceVolumeSyncReg;
	char			IsBtSinkVolumeSyncReg;
	char 			IsBtInSearching;
	char 			CancelAutoConnectFalg;
	char  			BtSurportStandBy;
	char            BtSurportTransMode;
	char  			BtSurportHintPlay;
	char  			BtSurportAutoConnet;//ON OFF
	int 			AutoLatency;
	char 			AutoRole;
	char			RetryTimes;
	char  			IsConnectedSetDelay;
	char   			BtSurportTws;
	SINK_MODE_e  	BtTwsMode;
	int 			SwitchFlag;
	BTBaseStates_t  BaseState;
	//BtAddr_t 		BtAddr;
	PAIR_STATE_e 	BtPairState;
	//int				BtConnState;//profile
	//BT_PROFILE_e    AddrRole;//phone ,heagphone,wss   main
	unsigned int    verdoridSource;
	unsigned int	BtDeviceId_Pid;
	unsigned int	BtDeviceId_Vid;
	unsigned int	BtDeviceId_Version;
	//avrcp
	char avrcpCtServiceName[25];	//<25
	char avrcpCtProviderName[25];	//<25
	char license[48];
}stBTInfo;



typedef void (*BtConfigInitAction)(void *pBtConf);
typedef int (*BtModeOnDoAction)(void);
typedef int (*BtModeOffDoAction)(void);
typedef int  (*BtActionInit)(void);
typedef int  (*BtActionDelInit)(void);
typedef int  (*BtActionDisable)(int *pEnableResult);
typedef int  (*BtActionEnable)(int *pEnableResult,Init_Parm st_Init_Parm);
typedef void (*BtReadyOkAction)(int IsBtModeOn);
typedef  int (*BtIsDiscovering)(int *pIsTrue);
typedef void (*BtActionStartSearch)(void);
typedef void (*BtActionStopSearch)(void);
typedef void (*BtActionPairStart)(BtAddr_t *pBtAddr);
typedef void (*BtActionUnpair)(BtAddr_t *pBtAddr);
typedef void (*BtActionConnect)(BtAddr_t *pAddr);

typedef void (*BtActionDisconnect)(BtAddr_t *pAddr);
typedef void (*BtActionAvrcpVolumeSyncSamsung)(BtAddr_t *BtAddr,unsigned char volume,
														unsigned char mute,unsigned char volume_mute_set);

typedef void (*BtActionSppConnect)(BtAddr_t *BtAddr);
typedef int (*BtActionSppConnectSamsung)(BtAddr_t *pBtAddr,unsigned char pin);
typedef void (*BtActionSppDisconnect)(BtAddr_t *BtAddr);
typedef void (*BtActionSppDataSend)(void* pParam);
typedef void (*BtSearchResultAction)(const char **pDesNameList,BTSearchDevInfo *pBtSearchInfo);
typedef void (*BtSearchFinishAction)(void);
typedef void (*BtConnectReqResultAction)(BTConnectReqInfo *pReslutInfo);
typedef void (*BtPairingAction)(BTPairDev *pPairInfo);
typedef void (*BtPairFinishAction)(BTPairDev *pPairInfo);
typedef void (*BtConnectingAction)(stBtDevInfo *pDev);
typedef void (*BtConnectokAction)(stBtDevInfo *pDev);
typedef void (*BtConnectFailAction)(stBtDevInfo *pDev);
typedef void (*BtDisconnectingAction)(stBtDevInfo *pDev);
typedef void (*BtDisconnectokAction)(stBtDevInfo *pDev);
typedef void (*BtSppDisconnectokAction)(stBtDevInfo *pDev);
typedef void (*BtSppConnectokAction)(stBtDevInfo *pDev);
typedef void (*BtSetRfPower)(DBM_LEVEL_e DbmValue);
typedef int  (*BtOpenUdtTest)(void);
typedef void (*BtActionAutoConnectCheck)(int *pResult,int *pRole);
typedef void (*BtActionAutoConnect)(int AutoRole);

typedef void (*BtAutoConnectSet)(ENABLE_STATE_e state);
typedef void (*BtActionSetScanMode)(SCAN_MODE_e mode);
typedef void (*BtActionGetScanMode)(SCAN_MODE_e *pMode);
typedef void (*BtSetStandby)(ENABLE_STATE_e state);

typedef void (*BtPowerOffRes)(void);
typedef int (*BtActionSwitchSinkMode)(SINK_MODE_e SinkMode);
typedef int (*BtActionSwitchTwsFinish)(SINK_MODE_e SinkMode);
typedef int (*BtActionConnectTimeOut)(BtAddr_t *pAddress);


typedef void (*BtDoPlayAction)(void);//(BtAddr_t *pBtAddr);
typedef void (*BtDoPauseAction)(void);//(BtAddr_t *pBtAddr);
typedef void (*BtDoNextAction)(void);//(BtAddr_t *pBtAddr);
typedef void (*BtDoPrevAction)(void);//(BtAddr_t *pBtAddr);
typedef  int (*BtIsPlaying)(int *pIsTrue);

typedef void (*BtSendWssPlayAction)(void);//(BtAddr_t *pBtAddr);
typedef void (*BtSendWssPauseAction)(void);//(BtAddr_t *pBtAddr);

typedef int (*BtSetAudioState)(void);
typedef int (*BtGetAudioState)(void);

typedef int (*BtSetPinCodeAction)(char *pPinCode);
typedef int (*BtReplyPinCodeAction)(BtAddr_t* pBtAddr,char *pPinCode);

typedef void (*BtGetAddrByRole)(BtAddr_t *pBtAddr,DEVROLE_DEX_e dex);
typedef void (*BtGetConnectStateByRole)(DEVROLE_DEX_e dex,BT_PROFILE_STATE_e *pState);
typedef int (*BtProfileState)(BtAddr_t *pBtAddr,BT_PROFILE_e profile,BT_PROFILE_STATE_e *pstate);
typedef int (*BtGetDevInfo)(BtAddr_t *pBtAddr,stBtDevInfo *pBtDev);
typedef int (*BtGetRemoteDevByIdex)(int dex,stBtDevInfo *pBtDev);
typedef int(*BtGetLocalName)(char *pName);
typedef int (*BtSetLocalName)(char *pName);
typedef int (*BtSetLocalLmpName)(char *pName);
typedef int (*BtSetLocalDevideClass)(unsigned int * pDeviceClass);
typedef int (*BtGetRemoveSdpDeviceID)(BtAddr_t*pBtAddr);
typedef int (*BtGetLocalAddr)(BtAddr_t*pBtAddr);
typedef int (*BtSetLocalAddr)(BtAddr_t *pBtAddr);
typedef int (*BtLocalInfoGet)(BtLocalInfo *pdev);
typedef int (*BtBondDevNum)(int *pBondedNum);
typedef int (*BtGetBondByIdex)(int sdBondedNum,stBtDevInfo *pBtDev);
typedef int (*BtRemoteDevNum)(int *pDevNum) ;

typedef int (*BtVolumeSyncReg)(BtVolumeInfo *pVolumeInfo);
typedef int (*BtVolumeSyncSet)(BtVolumeSet *pVolumeInfo);
typedef void (*BtSendUserCmd)(BtAddr_t *pBtAddr,UINT8 length,UINT8 *pUserCmdData);
typedef int (*BtDenyConnectSet)(BtAddr_t *pBtAddr,ENABLE_STATE_e state);
typedef void (*BtActionAddBondDev)(BTPairDev *stPairDev);
typedef void (*BtActionClearBondDev)(void);
typedef int (*BtActionClearAutoInfo)(void);
typedef int (*BtGetRemoteDevRssiAction)(BtAddr_t *pBtAddr);





typedef struct st_BTAction
{
	BtConfigInitAction BtConfigInit;
	BtModeOnDoAction BtModeOnAction;
	BtModeOffDoAction BtModeOffAction;
	BtActionInit BtInit;
	BtActionDelInit BtDeInit;
	BtActionEnable BtEnable;
	BtActionDisable BtDisable;
	BtReadyOkAction BtReadyOkDo;
	BtIsDiscovering		BtIsSearching;
	BtActionStartSearch BtStartSearch;
	BtActionStopSearch BtStopSearch;
	BtSearchResultAction SearchResultDo;
	BtSearchFinishAction SearchFinishDo;
	BtConnectReqResultAction ConnectReqResult;
	BtActionPairStart BtPairStart;
	BtActionUnpair BtUnpair;
	BtPairingAction PairingDo;
	BtPairFinishAction PairFinishDo;
	BtActionConnect Connect;
	BtDenyConnectSet DenyConnectSet;
	BtConnectingAction ConnectingAction;
	BtConnectokAction	ConnectOkAction;
	BtConnectFailAction	ConnectFailAction;
	BtActionDisconnect Disonnect;
	BtDisconnectingAction DisconnectingAction;
	BtDisconnectokAction	DisconnectokAction;
	BtActionAvrcpVolumeSyncSamsung BtAvrcpVolSyncSamsung;
	BtActionSppConnect BtSppConnect;
	BtActionSppConnectSamsung BtSppConnectSamsung;
	BtSppConnectokAction SppConnectokAction;
	BtActionSppDisconnect BtSppDisconnect;
	BtSppDisconnectokAction	SppDisconnectokAction;
	BtActionSppDataSend	BtSppDataOut;
	BtOpenUdtTest 	BtUdtTest;
	BtSetRfPower 	BtRfSetPower;
	BtActionAutoConnectCheck BtAutoConCheck;
	BtActionAutoConnect AutoConnect;
	BtAutoConnectSet BtAutoConSet;
	BtActionSetScanMode	BtSetScanMode;
	BtActionGetScanMode BtGetScanMode;
	BtActionSwitchSinkMode BtSinkSwitchMode;
	BtActionSwitchTwsFinish BtTwsSwitchFinish;
	BtSetStandby BtSetStandbyWork;
	BtPowerOffRes	BtPowerOffResAction;
	BtDoPlayAction BtPlayAtionDo;
	BtDoPauseAction BtPauseAtionDo;
	BtDoNextAction	BtNextAtionDo;
	BtDoPrevAction	BtPrevAtionDo;
	BtIsPlaying		IsBtPlaying;
	BtSendWssPlayAction		SendWssPlay;
	BtSendWssPauseAction	SendWssPause;
	BtActionConnectTimeOut BtConnectTimeOutHandle;
	BtSetPinCodeAction		BtSetPinCode;
	BtReplyPinCodeAction	BtReplyPinCode;
	BtGetAddrByRole			GetBtAddrByDevRole;
	BtGetConnectStateByRole	GetConnectState;
	BtProfileState			GetProfileState;
	BtGetDevInfo			GetRemoteDevInfo;
	BtGetRemoteDevByIdex	GetRemoteDevByIdex;
	BtGetLocalName			GetLocalName;
	BtSetLocalName			SetLocalName;
	BtSetLocalLmpName		SetLocalLmpName;
	BtSetLocalDevideClass	SetLocalDeviceClass;
	BtGetRemoveSdpDeviceID	GetRemoveSdpDeviceID;
	BtGetLocalAddr			GetLocalAddr;
	BtSetLocalAddr			SetLocalAddr;
	BtLocalInfoGet			GetLocalInfo;
	BtBondDevNum			GetBondNum;
	BtGetBondByIdex         GetBondByIdex;
	BtRemoteDevNum			GetRemoteDevNum;
	BtVolumeSyncReg			VolumeSyncReg;
	BtVolumeSyncSet			VolumeSyncSet;
	BtSendUserCmd           SendUserCmd;
	BtActionAddBondDev		AddBondDev;
	BtActionClearBondDev	ClearBondDev;
	BtActionClearAutoInfo	ClearAutoInfo;
	BtGetRemoteDevRssiAction	BtGetRemoteDevRssi;
}stBTAction;




typedef int (*TransOffAction)(void);
typedef int (*TransOnAction)(void);

typedef struct st_BtTransAction
{
	TransOffAction TransModeOff;
	TransOnAction TransModeOn;
}stBtTransAction;


extern void BtActionGet(stBTAction **pBtAction);
extern void BtActionDel(stBTAction **pBtAction);

extern void BtTransActionInit(stBtTransAction **pTransAction);

extern void *memcpy(void *, const void *, unsigned);
extern void *memset(void * pDst, int iFillValue, UINT32 uiLen);
extern int	memcmp(const void *, const void *, unsigned/*size_t*/);
extern unsigned strlen(const char *);
extern unsigned int GetSTC(void);

#endif //__BT_AO_ACTION_H__

