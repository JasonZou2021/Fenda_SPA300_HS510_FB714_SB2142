#ifndef __BT_AO_SERVICE_H__
#define __BT_AO_SERVICE_H__
#include "qp/qf_sync.h"
#include "qp/qp_pub.h"
#include "qp/qep.h"
#include "bt_common.h"



typedef enum BTCBK_EVTSIG
{
	BT_CBK_SIG,
	BT_CBK_READY_SIG = BT_CBK_SIG,
	BT_CBK_UNREADY_SIG,
	BT_CBK_TWS_SWITCH_OK_SIG,
	BT_CBK_SEARCH_RESULT_SIG,
	BT_CBK_SEARCH_FINISH_SIG,
	BT_CBK_ACL_CONNECT_SIG,
	BT_CBK_PAIRING_IND_SIG,
	BT_CBK_PAIRED_OK_SIG,
	BT_CBK_PAIRED_FAIL_SIG,
	BT_CBK_UNPAIRED_OK_SIG,
	BT_CBK_ACL_DISCONNECT_OK_SIG,
	BT_CBK_REMOTE_DEVICE_NAME_SIG,
	BT_CBK_REMOTE_DEVICE_INFO_SIG,
	BT_CBK_MUSIC_PLAY_STATUS_SIG,
	BT_CBK_MUSIC_CODEC_IND_SIG,
	BT_CBK_MUSIC_INFO_IND_SIG,
	BT_CBK_REMOTE_DEVICE_RSSI_SIG,
	BT_CBK_BASE_MAX_SIG = BT_CBK_REMOTE_DEVICE_RSSI_SIG,

	BT_CBK_A2DP_SIG,
	BT_CBK_A2DP_CONNECT_ING_SIG = BT_CBK_A2DP_SIG,
	BT_CBK_A2DP_CONNECT_FAIL_SIG,
	BT_CBK_A2DP_CONNECT_SUCCESS_SIG,
	BT_CBK_A2DP_DISCONNECT_ING_SIG,
	BT_CBK_A2DP_DISCONNECT_FAIL_SIG,
	BT_CBK_A2DP_DISCONNECT_SUCCESS_SIG,
	BT_CBK_A2DP_MAX_SIG = BT_CBK_A2DP_DISCONNECT_SUCCESS_SIG,

	BT_CBK_SPP_SIG,
	BT_CBK_SPP_CONNECT_FAIL_SIG = BT_CBK_SPP_SIG,
	BT_CBK_SPP_CONNECT_SUCCESS_SIG,
	BT_CBK_SPP_DISCONNECT_SUCCESS_SIG,
	BT_CBK_SPP_DISCONNECT_FAIL_SIG,
	BT_CBK_SPP_DATA_IN_SIG,
	BT_CBK_SPP_MAX_SIG = BT_CBK_SPP_DATA_IN_SIG,
	BT_CBK_DEV_CONNECT_TIMEOUT_SIG,
	BT_CBK_CTR_REQ_SIG,
	BT_CBK_AVRCP_CONNECTOK_SIG = BT_CBK_CTR_REQ_SIG,
	BT_CBK_AVRCP_DISCONNECTOK_SIG,
	BT_CBK_VOLUME_SET_REQ_SIG,
	BT_CBK_VOLUME_REG_SIG,
	BT_CBK_SAMSUNG_TV_FLAG_SIG,
	BT_CBK_PLAY_REQ_SIG,
	BT_CBK_PAUSE_REQ_SIG,
	BT_CBK_POWER_REQ_SIG,
	BT_CBK_VOLUMEUP_REQ_SIG,
	BT_CBK_VOLUMEDOWN_REQ_SIG,
	BT_CBK_MUTE_REQ_SIG,
	BT_CBK_NEXT_PLAY_REQ_SIG,
	BT_CBK_PREV_PLAY_REQ_SIG,
	BT_CBK_CTR_REQ_MAX_SIG = BT_CBK_PREV_PLAY_REQ_SIG,

	BT_CBK_SDP_SIG,
	BT_CBK_SDP_DID_DATA_IN_SIG,
	BT_CBK_SDP_MAX_SIG,

	BT_CBK_SOURCE_STATE_UPDATE_SIG,//phone
	BT_CBK_CUSTOM_DATA_IN_SIG,

	BT_CBK_MSG_MAX,
}BTCBK_EVTSIG_e;


typedef int (*UserCbkHandle)(void *pMsg ,BTCBK_EVTSIG_e type);

typedef struct st_UserCbkHandle
{
	UserCbkHandle CbkFunction;
}BtUserCbkHandle;


QActive *GetBtAo(void);
int BtAoSrvUserCbkRegister(BtUserCbkHandle* pMsgSendFunc);
int BtAoSrvUserCbkDelRegister(BtUserCbkHandle* pMsgSendFunc);


int BtPlatformApUpgradeInit(int(*BtApUpgradeCbk)(void));
int BtOn(void);
int BtOff(void);
int BtModeOn(void);
int BtModeOff(void);
int BtSinkModeSwitch(SINK_MODE_e mode);//connect sink role :headphone/sub

int BtScanModeSet(SCAN_MODE_e state);
int BtScanModeGet(SCAN_MODE_e *pstate);


int BtSearchStart(void);
int BtSearchStop(void);

int BtGetAddrByDevRole(BtAddr_t *BtAddr,DEVROLE_DEX_e dex);//wss,headphone,phone0,1...6
int BtGetConnectStateByDevRole(DEVROLE_DEX_e dex,BT_PROFILE_STATE_e *pState);
int BtIsPlay(void);


int BtSetPincode(char *pPinCode);
int BtReplyPincode(BtAddr_t *pBtAddr,char *pPinCode);

int BtPairSart(BtAddr_t *BtAddr);
int BtUnpair(BtAddr_t*BtAddr);

int BtA2dpConnect(BtAddr_t *BtAddr);
int BtSppConnect(BtAddr_t *BtAddr);
int SPP_IF_Connect_Samsung(BtAddr_t btAddr,unsigned char pin);
int BtA2dpDisconnect(BtAddr_t *BtAddr);
int BtSppDisconnect(BtAddr_t *BtAddr);
int BtCustomDataSend(BtAddr_t BtAddr,UINT8 *pData, int len);

int BtRejectConnectSet(BtAddr_t *pBtAddr,ENABLE_STATE_e state);


//int BtGetProfileState(BtAddr_t *BtAddr,BT_PROFILE_e profile,BT_PROFILE_STATE_e *pstate);
int BtGetProfileState(BtAddr_t *pBtAddr,BT_PROFILE_e profile,BT_PROFILE_STATE_e *pstate);
int BtCheckAllDeviceProfileState(Profile_Name_e profile,BT_PROFILE_STATE_e *pstate);
int BtGetRemoteDevInfo(BtAddr_t *BtAddr,stBtDevInfo* DevInfo);
int BtShowAllRemoteDevInfo(void);
int BtGetBondDevNums(int *pBondedNum);
int BtGetBondDevInfo(int sdBondedNum,stBtDevInfo *pBtDev);
int BtSendCustomData(BtAddr_t BtAddr,UINT8 *pData, int len);

int BtAutoConnect(AUTOCONN_DEX_e role);//phone,headphone
int BtAutoConnectCancel(AUTOCONN_DEX_e role);

int BtRfPowerSet(DBM_LEVEL_e DbmValue);
int BtUdtTestOn(void);


int BtLocalNameGet(char *name);
int BtLocalNameSet(char *name,int len);
int BASE_IF_LmpNameResChange(char* name);
int BASE_IF_WriteDevClass(unsigned int devclass);
int BASE_IF_SDP_ServiceSearch_DID(BtAddr_t address);
int Music_IF_Volume_Sync_Samsung(BtAddr_t *pAddr,UINT_8 Volume,UINT_8 ismute,UINT_8 vol_mute);
int BtLocalAddrGet(BtAddr_t *pAddr);
int BtLocalAddrSet(BtAddr_t *pAddr);
int BtGetLocalInfo(BtLocalInfo *devinfo);


void BtFree(void *ptr);
void *BtMalloc(size_t size);
void *BtZoreMalloc(size_t size);

int BtPlay(void);
int BtPause(void);
int BtNext(void);
int BtPrev(void);
int BtSetVolumeSync(int volume, DEVROLE_DEX_e role);

int BtSetWssDelayLevel(BtDelaySyncInfo DelaySyncInfo);
int BtResetAudioCodec(void);

int BtWssPause(void);
int BtWssPlay(void);
int BtAllowAudioDataIn(int dOnOff);
//Sunplus//int BtInit(void);
int BtInitStart(void);
int BtSetAppCurrentSource(SrcType_e src);
int BtAddBondDev(BTPairDev *stPairDev);
int BtClearAllBondDev(void);
int BtGetRemoteDeviceRssi(BtAddr_t *pBtAddr);

#endif //__BT_AO_SERVICE_H__

