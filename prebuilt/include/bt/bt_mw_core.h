#ifndef __TEST_BT_MW_H__
#define __TEST_BT_MW_H__

#include "bt_common.h"


typedef unsigned short  u_int16;     /* unsigned integer, 16 bits long */

#define BIT_N(n) 			(1UL<<(n))
#define BIT_X2Y(x,y) 		( (BIT_N(y)|(BIT_N(y)-1))&(~(BIT_N(x)-1)) )

#define CLR_BITS(NumOrVal, Bits) ((NumOrVal) & (~(Bits)))

#define BT_A2DP_PROFILE_BIT  	(0)//a2dp bit:0~1
#define BT_WSS_SPP_PROFILE_BIT  	(2)//spp bit:4~5
#define BT_SPP_PROFILE_BIT  	(4)//spp bit:4~5
#define BT_AVRCP_PROFILE_BIT  	(6)//spp bit:4~5

#define BT_A2DP_PROFILE(VAL)  	(((VAL)&(BIT_X2Y(0,1)))>>BT_A2DP_PROFILE_BIT)//a2dp bit:0~1  get a2dp profile value
#define BT_SPP_PROFILE(VAL)  	(((VAL)&(BIT_X2Y(4,5)))>>BT_SPP_PROFILE_BIT)//spp bit:4~5  get spp profile value
#define BT_AVRCP_PROFILE(VAL)  	(((VAL)&(BIT_X2Y(6,7)))>>BT_AVRCP_PROFILE_BIT)//avrcp bit:6~7  get spp profile value




typedef enum
{
	TYPE_NULL,
	TYPE_UPGRADE_DATA,
	TYPE_REMOTE_DATA,
	TYPE_MAX,
}TRANSMIT_TYPE;


typedef enum
{
	UPGRADE_REQ,//SEND TO SUB(UPGRADE INFO)
	UPGRADE_READY,//SEND TO MAIN
	UPGRADE_FINNISH,//SEND TO SUB
	UPGRADE_SUCCESS,//SEND TO MAIN
	UPGRADE_FAIL,//SEND TO MAIN
	UPGRADE_DATA_IND,//SEND TO SUB
}UPGRADE_NOTY;


typedef struct st_BtConfigSet
{
	BT_PLATFORM_ROLE_e   SystemRole;
	char	DevMaxConnect;
	char	IsBtSimpleModeEn;
	char	IsBtAacCodecEn;
}stBtConfig;


typedef struct st_BtHintCnf
{
	ENABLE_STATE_e   state;
	const BYTE   	*pHintData;//decide a2dpsource:headphone or wss
	UINT32		len;
}stBtHintCnf;


typedef int (*BtReadyInfoPost)(void);
typedef int (*BtUnReadyInfoPost)(void);
typedef int (*SearchDevInfoPost)(BTSearchDevInfo *pdata);
typedef int (*SearchDevFinishPost)(void);
typedef int (*ConnectReqInfoPost)(BTConnectReqInfo *pdata);
typedef int (*PairDevInfoPost)(BTPairDev *pdata);
typedef int (*PairDevFinishPost)(BTPairDev *pdata);
typedef int (*UnPairDevPost)(BTPairDev *pdata);
typedef int (*ConnectDevInfoPost)(stBtDevInfo *pdata);
typedef int (*DisconnectDevInfoPost)(stBtDevInfo *pdata);
typedef int (*ConnectOkDevInfoPost)(stBtDevInfo *pdata);
typedef int (*DisconnectOkDevInfoPost)(stBtDevInfo *pdata);
typedef int (*BtConnectFailPost)(stBtDevInfo *pdata);
typedef int (*BtConnectTimeOutPost)(BtAddr_t *pBtAddr);
typedef int  (*BtVolumeSetPost)(BtVolumeInfo *pBtVolumeInfo);
typedef int  (*BtVolumeRegPost)(BtVolumeInfo *pBtVolumeInfo);
typedef int  (*BtSamsungTvFlagPost)(BtAddr_t *pBtAddr);
typedef int  (*BtPlayNextReqPost)(BtAddr_t *pBtAddr);
typedef int  (*BtPlayPrevReqPost)(BtAddr_t *pBtAddr);
typedef int  (*BtPlayReqPost)(BtAddr_t *pBtAddr);
typedef int  (*BtPauseReqPost)(BtAddr_t *pBtAddr);
typedef int  (*BtPowerReqPost)(BtAddr_t *pBtAddr);
typedef int  (*BtVolumeUpReqPost)(BtAddr_t *pBtAddr);
typedef int  (*BtVolumeDownReqPost)(BtAddr_t *pBtAddr);
typedef int  (*BtMuteReqPost)(BtAddr_t *pBtAddr);

typedef int (*BtEncodePlayReqPost)(void);
typedef int (*BtSourceStatePost)(BtDevPlayState *pDevPlayerInfo);
typedef int (*BtBondclearfinishsigPost)(void);

typedef int (*SppUpGradeInfoPost)(void *pdata);
typedef int (*SppConnectInfoPost)(stBtDevInfo *pdata);
typedef int (*SppDisconnectInfoPost)(stBtDevInfo *pdata);
typedef int (*SppConnectFailInfoPost)(stBtDevInfo *pdata);
typedef int (*SppDisconnectFailInfoPost)(stBtDevInfo *pdata);
typedef int (*SppDataPost)(void *pdata);
typedef int (*SppRecvCustomDataPost)(stBtData *pData);


typedef int (*AvrcpConnectOkDevInfoPost)(stBtDevInfo *pdata);
typedef int (*AvrcpDisconnectOkDevInfoPost)(stBtDevInfo *pdata);

typedef int (*BtGetRemoteDeviceRssiPost)(BtRemoteDevRssi_t *pRemoteDevRssi);


typedef struct DevSppPostIF
{
	SppUpGradeInfoPost SppUpGradePost;
	SppConnectInfoPost SppConnectPost;
	SppDisconnectInfoPost SppDisconnectPost;
	SppConnectFailInfoPost SppConnectFailPost;
	SppDisconnectFailInfoPost SppDisconnectFailPost;
	SppDataPost SppDataInPost;
	SppRecvCustomDataPost SppRecvCustomDataInPost;
}DevSppPost;


typedef int (*SdpDidDataPost)(void *pdata);

typedef struct DevSdpPostIF
{
	SdpDidDataPost SdpDidDataInPost;
}DevSdpPost;

typedef struct DevPostIF
{
	BtReadyInfoPost   BtReadyPost;
	BtUnReadyInfoPost BtUnReadyPost;
	SearchDevInfoPost SearchResultPost;
	SearchDevFinishPost SearchFinishPost;
	ConnectReqInfoPost	ConnectReqResultPost;
	PairDevFinishPost PairOkPost;
	PairDevInfoPost PairReqPost;
	UnPairDevPost UnPairPost;
	ConnectDevInfoPost ConnectReqPost;
	DisconnectDevInfoPost DisconnectReqPost;
	ConnectOkDevInfoPost ConnectOkPost;
	DisconnectOkDevInfoPost DisconnectOkPost;
	BtConnectFailPost ConnectFailPost;
	BtConnectTimeOutPost	ConnectTimeOutPost;
	BtVolumeSetPost			VolumeSetPost;
	BtVolumeRegPost			VolumeRegPost;
	BtSamsungTvFlagPost		SamsungTvFlagPost;
	BtPlayNextReqPost		PlayNextReq;
	BtPlayPrevReqPost		PlayPrevReq;
	BtPlayReqPost			PlayReq;
	BtPauseReqPost			PauseReq;
	BtPowerReqPost			PowerReq;
	BtVolumeUpReqPost		VolumeUpReq;
	BtVolumeDownReqPost		VolumeDownReq;
	BtMuteReqPost			MuteReq;
	BtEncodePlayReqPost		EncodePlayReq;
	BtSourceStatePost		SourceStatePost;
	BtBondclearfinishsigPost BondclearfinishsigPost;
	AvrcpConnectOkDevInfoPost AvrcpConnectOkPost;
	AvrcpDisconnectOkDevInfoPost AvrcpDisconnectOkPost;
	BtGetRemoteDeviceRssiPost	RemoteDevRssiReqResultPost;
}DevPostInterface;



/*int BtServiceEnable(void);
int BtServiceInit(void);
int BtServiceDelInit(void);
int BtServiceDisable(void);
void BtServiceCallBackReg(DevPostInterface * pMsgSend ,
									DevSppPost *pSppMsgSend,
									DevSdpPost *pSdpMsgSend);
void BtServiceCallBackUnReg(DevPostInterface * pMsgSend ,
									DevSppPost *pSppMsgSend,
									DevSdpPost *pSdpMsgSend);
void BtServiceGetLocalName(char *name);
void BtServiceSetLocalName(char *name);
void BtServiceSetLocalLmpName(char *name);
void BtServiceSetLocalDeviceClass(unsigned int* pDeviceClass);
void BtServiceSetLocalEirInfo(char* name,
								  unsigned int verdoridSource,
								  unsigned int deviceId_pid,
								  unsigned int deviceId_vid,
								  unsigned int deviceId_version);
void BtServiceSetLocalAvrcpCtSrdName(char* name);
void BtServiceSetLocalAvrcpCtSrvName(char* name);
void BtServiceSetLocalSDP_DeviceID( unsigned int deviceId_pid,
								  unsigned int deviceId_vid,
								  unsigned int deviceId_version);
void BtServiceGetRemoveSdp_DeviceID(BtAddr_t*BtAddr);
void BtServiceGetLocalAddr(BtAddr_t *BtAddr);
void BtServiceSetLocalAddr(BtAddr_t *BtAddr);
void BtServiceGetDevState(BtAddr_t *pBtAddr,int *state);
int BtServiceA2dpConnect(BtAddr_t *BtAddr,DEVROLE_DEX_e role);
void BtServiceSppConnect(BtAddr_t *BtAddr);
int BtServiceSppConnectSamsung(BtAddr_t *BtAddr,unsigned char pin);
void BtServiceAvrcpVolSyncSamsung(BtAddr_t *BtAddr,unsigned char volume,
														unsigned char mute,unsigned char volume_mute_set);
void BtServiceA2dpDisconnect(BtAddr_t *BtAddr,DEVROLE_DEX_e role);
void BtServiceSppDisconnect(BtAddr_t *BtAddr);
void BtServiceSppDataOut(void * pParam);
int BtServiceUdtTestOn(void);
void BtServiceWssSetMode(SINK_MODE_e mode);
void BtServiceGetAudioState(BtAddr_t *pBtAddr,BT_IR_CMD_e *pCmd);
void BtServiceAvrcpCtl(BtAddr_t *pBtAddr,BT_IR_CMD_e cmd);
void BtServiceAutoConnect(ENABLE_STATE_e state);
void BtServiceRfPowerSet(DBM_LEVEL_e DbmValue);
int BtServiceGetProfileState(BtAddr_t *pBtAddr,BT_PROFILE_e profile,BT_PROFILE_STATE_e *pstate);
int BtServiceGetRemoteDevInfo(BtAddr_t *BtAddr,stBtDevInfo* DevInfo);
//extern void BtServiceAllRemoteDevInfo(void* DevInfo);
void BtServiceGetScanMode(SCAN_MODE_e *pMode);
void BtServiceSetScanMode(SCAN_MODE_e mode);
void BtServiceRecoverSearchingScanMode(void);
int BtServiceIsSearching(void);
int BtServiceStartSearch(void);
int BtServiceStopSearch(void);
int BtServiceGetRemoteDevNum(int *pNum);
int BtServiceGetRemoteDevByIdex(int dex,stBtDevInfo *pBtDev);

void BtServicePairStart(BtAddr_t *pBtAddr);
void BtServiceUnpair(BtAddr_t *pBtAddr);
int BtServiceBtDoClearBondDev(void);
void BtServiceSendData(BtAddr_t *pBtAddr,char *pBtData,int len);
int BtServiceSourceSendStart(BtAddr_t *pBtAddr);
int BtServiceSourceSendPause(BtAddr_t *pBtAddr);

int BtServiceBondNum(int *psdBondedNum);
int BtServiceGetBondByIdex(int sdBondedNum,stBtDevInfo *pBtDev);
void BtServiceGetLocalInfo(BtLocalInfo *dev);
int BtServiceConfigSet(stBtConfig *pBtConf);
int BtServiceGetAutoHeadPhoneAddr(BtAddr_t *pAddr);
int BtServiceGetWssAddr(BtAddr_t *pAddr);

int BtServiceIsA2dpConnected(BtAddr_t *pBtAddr);
int BtServiceSubIsFirstConnect(void);
int BtServiceSubFirstConnectedSet(void);
void BtServiceSetWaterLevel(BtAddr_t *pBtAddr,int level);
int BtServiceGetAutoAddr(BtAddr_t *pBtAddr);
int BtServiceSetAutoAddr(BtAddr_t *pBtAddr);
int BtServiceSetSpeakOutDelay(int DelayTime);
int BtServiceSetMainMode(SINK_MODE_e mode);
int BtServiceSetDenyConnect(BtAddr_t *pBtAddr,ENABLE_STATE_e state);
int BtServiceAllowAudioIn(ENABLE_STATE_e eState, int dIsUpdateAudio);
int BtServiceSendAudioSync(int IsPlay,BtAddr_t *pBtAddr);
int BtAudioIsEncodeAutoSync(int IsTrue);
int BtServiceReplyPinCode(BtAddr_t* pBtAddr,char *pPinCode);
int BtServiceSetPinCode(char *pPinCode);
int BtServiceAudioCodecInit(void);
int BtServiceAvrcpNotifyVolumeChange(BtAddr_t *pBtAddr,UINT8 volume,DEVROLE_DEX_e role);
int BtServiceSubSendSyncReady(void);
void BtServiceSendUserCmd(BtAddr_t *pBtAddr,UINT8 length,UINT8 *pUserCmdData);
int BtServiceGetBondDevByAddr(BtAddr_t BtAddr,stBtDevInfo *pBtDev);
int BtServiceAllowAudioDataIn(ENABLE_STATE_e eState);
int BtServiceGetRemoteDeviceRssi(BtAddr_t *pBtAddr);*/

typedef unsigned char u_int8;
typedef unsigned int u_int32;
typedef  short s_int16;
//typedef unsigned short u_int16;

#define BTBM_ADP_MAX_NAME_LEN 48
#define LINKKEY_SIZE 16
#define SIZE_OF_BDADDR 6

typedef struct {
    u_int8 bytes[SIZE_OF_BDADDR];
}t_bdaddr;

#define BTADDR_LENGTH (6)
typedef struct BTAddr_ {
	UINT8 address[BTADDR_LENGTH];
} BTAddr_t;

typedef struct  SEARCH_RESULT
{
    t_bdaddr bd_addr;
    u_int32  cod;
    char     name[BTBM_ADP_MAX_NAME_LEN];
    u_int8  name_len;
}bt_search_result_struct;


typedef struct   PAIRED_DEVICE
{
	/* WARNING - this structure MUST match the layout of the first part of the st_t_BT_DeviceInfo structure above */
	    u_int8	 master;
        t_bdaddr bdAddress;
        u_int32  cod;
        char     name[BTBM_ADP_MAX_NAME_LEN];
        u_int8   name_len;
        u_int8   linkKeyType; /* MGR_COMBINATION | MGR_LOCALUNITKEY | MGR_REMOTEUNITKEY */
        u_int8   linkKey[LINKKEY_SIZE];
}t_paired_DeviceInfo;

typedef struct   PINCODE_INFO
{
        t_bdaddr bdAddress;
        u_int8   pincode[4];
        u_int8   pincode_lenth;
}t_pincode_info;

typedef struct _BT_DEV_ENTRY{
      t_bdaddr             dev_btaddress;
	  UINT8                service;
      UINT8                namelength;
      char                 friendname[BTBM_ADP_MAX_NAME_LEN];
      UINT32               cod;
      UINT8                paired;
      u_int8               linkKeyType; /* MGR_COMBINATION | MGR_LOCALUNITKEY | MGR_REMOTEUNITKEY */
      u_int8               linkKey[LINKKEY_SIZE];
}BtDevEntry;

typedef struct
{
    t_bdaddr bdAddress;
	u_int8 reason;
}t_disconnect_DeviceInfo;

int SDK_BASE_IF_BtServerInit();
int SDK_BASE_IF_StartSearch();
int SDK_BASE_IF_StopSearch();
int SDK_BASE_IF_StartPair(t_bdaddr *address);
int SDK_Bchs_IF_DisableBT();
int SDK_Bach_IF_EnableBT();

int SDK_BASE_IF_link_reject();
int SDK_BASE_IF_link_accept(t_paired_DeviceInfo *info);
int SDK_BASE_IF_Disconnectdevice(void);
int SDK_BASE_IF_pincode_response(t_pincode_info *info);
int SDK_BASE_IF_Remove_device(t_bdaddr address);
int SDK_BASE_IF_Write_device(BtDevEntry record);
int SDK_BASE_IF_Get_Stacklib_Version(void);
int SDK_BASE_IF_Change_Discoverable(int mode);
int SDK_BASE_IF_Change_Connectable(int mode);
int SDK_BASE_IF_Set_LocalBtName(const char* pszName);
int SDK_BASE_IF_Get_LocalBtAddr();

/**********************************************************************************************************
** SDK_BASE_IF_Set_EIRData()

*/
int SDK_BASE_IF_Set_EIRData(const char* eir_data, unsigned char eir_data_length);

/**********************************************************************************************************
** SDK_BASE_IF_Make_EIRDeviceID()

*/
UINT8 SDK_BASE_IF_Make_EIRDeviceID(UINT8* buf, UINT8 buf_len, UINT16 vendor_id_src, UINT16 vendor_id, UINT16 product_id, UINT16 version);


/**********************************************************************************************************
** SDK_BASE_IF_Make_EIRServiceUUID()
** type: EIR±ê×?ààDí
** uuids: ?éò?ê?UINT16, UINT32, ?ò??128 byteμ?UUID êy×é
** uuid_num: uuidsêyá?
*/
UINT8 SDK_BASE_IF_Make_EIRServiceUUID(UINT8* buf, UINT8 buf_len, UINT8 type, void* uuids, UINT8 uuid_num);

/**********************************************************************************************************
** SDK_BASE_IF_Make_EIRLocalName()

*/
UINT8 SDK_BASE_IF_Make_EIRLocalName(UINT8* buf, UINT8 buf_len, UINT8* name, UINT8 name_len);

int SDK_BASE_IF_SetLogLevel(int level); //level = 0 for closing lib_bluetooth.a log

int SDK_BASE_IF_Change_LocalName(const char* name);
int SDK_BASE_IF_Change_LocalLmpName(const char* name);
int SDK_BASE_IF_Set_ClassOfDevice(u_int32 class_of_device);

int SDK_SPP_IF_Connect(BtAddr_t *bd_addr, u_int16 uuid16, u_int8 *uuid128);  //SPP ??
int SDK_SPP_IF_Disconnect(u_int16 spp_port); //SPP ????
int SDK_SPP_IF_Send (u_int8* data, u_int16 length, u_int16 spp_port);  //SPP ?????????

int SDK_AVRCP_CtProviderName(u_int8* name, u_int8 len);
int SDK_AVRCP_CtServiceName(u_int8* name, u_int8 len);
///???????SDK_Bchs_IF_EnableBT()???????AVRCP????????

int SDK_AVRCP_VolSyncSamsung(BtAddr_t* bd_addr, u_int8 absolute_volume/*0~127*/, u_int8 mute, u_int8 mute_set);///mute/mute_set???samsung????????????,mute??


int SDK_SDP_IF_Set_DeviceID(u_int16 vendor_id_source, u_int16 vendor_id, u_int16 product_id, u_int16 version, u_int16 specification_id);
int SDK_SDP_IF_Remote_DeviceID(BtAddr_t* bd_addr);


int SDK_BASE_AddBondedDevice(BTPairDev *info);
int SDK_BASE_DeleteBondedDevice(BTPairDev *info);
int SDK_BASE_ClearBondedDevice(void);

/*1。 ---LICENSE*/
int SDK_BASE_IF_Write_License(u_int8 license[48]);///读取48字节license字符串，模拟烧入license.请在蓝牙初始化前调用。
int SDK_BASE_IF_Set_LocalBtAddr(u_int8 bd[6]);///设置本地蓝牙地址,请在蓝牙初始化前调用。

int SDK_BASE_IF_Set_InquiryScanActivity(u_int16 inquiry_scan_window,u_int16 inquiry_scan_interval);
int SDK_BASE_IF_link_connect(BtAddr_t* bd_addr);
int SDK_BASE_IF_ReadRemoteRSSI(BtAddr_t* bd_addr);
int SDK_Music_IF_EnableA2DPAAC(int is_enable);

#define MSG_MASK                           	0xFFFF0000

#define APP_MESSAGE_BASE            		0x00000000
#define BLUETOOTH_MESSAGE_BASE              0x00010000
#define HANDSFREE_MESSAGE_BASE          	0x00020000
#define A2D_MESSAGE_BASE                    0x00030000
#define PAC_MESSAGE_BASE                    0x00040000
#define DEBUG_MESSAGE              			0x00050000
#define SPP_MESSAGE_BASE                    0x00060000
#define HID_MESSAGE_BASE                    0x00070000
#define SDP_MESSAGE_BASE                    0x00080000

#define         MSG_SUCCESS           0
#define         MSG_FAILD             -1
#define         MSG_WAIT_TIMEOUT      -2

/*app error information*/
enum {
	BTMW_RESULT_OK = 0,                         /*0*/
	BTMW_RESULT_FAIL,                           /*1*/
};

/*************************************************************************************************************
*   ----------------------------------Bluetooth Common Message & Event here----------------------------------
**************************************************************************************************************/
typedef enum _BLUETOOTH_BASE_MSG {
	BASE_START_BCHS_TASK = BLUETOOTH_MESSAGE_BASE,
	//BASE_STOP_BCHS_TASK,
	START_BCHS_TASK_OK,
	STOP_BCHS_TASK_OK,
	BASE_BTSTACK_STARTUP_OK,
	BASE_BTSTACK_STARTUP_FAIL,
	BASE_BTSTACK_STOP_OK,
	BASE_SEARCH_START,
	BASE_SEARCH_START_OK,
	BASE_SEARCH_START_NG,
	BASE_SEARCH_CANCEL,
	BASE_PAIR_START,    ///A
	BASE_PAIR_RESPONSE,
	BASE_PAIR_SUCCESS,
	BASE_PAIR_FAIL,
	BASE_SERVICE_INFO,
	BASE_PINCODE_INDICATE,
	BASE_PINCODE_RESPONSE,//10
	BASE_LINK_ACCEPT,
	BASE_LINK_REJECT,
	BASE_REMOTE_LINK_LOCAL,
	BASE_REMOVE_DEVICE_RECORD,
	BASE_WRITE_DEVICE_RECORD,
	BASE_DISCONNECT_DEVICE,
	//BASE_DISCONNECT_DEVICE_OK,
	BASE_CHANGE_DISCOVERABLE,
	BASE_CHANGE_CONNECTABLE,
	BASE_CHANGE_NAME,
	BASE_CHANGE_SECURITY,
	BASE_CHANGE_LINKABLE,
	BASE_GET_REMOTE_NAME,
	BASE_SEARCH_RESULT,
	BASE_SEARCH_FINISHED,
	BASE_LINK_REQUEST,
	BASE_STACKLIB_VERSION,//20
	BASE_READ_RSSI,//BASE_GET_STACKLIB_VERSION,
	BASE_GET_ACL_HANDLE,
#if 1//def BLUETOOTH_SUPPORT_PBAB
	BASE_GET_PBK_FIRST,
	BASE_GET_PBK_DATA,
	BASE_GET_PBK_OK,
	//BASE_GET_PBK_LOG_OK,
	BASE_GET_PBK_FAIL,

	BASE_GET_PBK_LOG_OK,
	BASE_GET_PBK_LOG_FAIL,
#endif
	BASE_GET_LOCAL_BD_IND,/*event,param:t_bdaddr*/
	BASE_STACK_START_OK_IND,/*event,param:NULL*/

	BASE_LE_ADV_DATA,

	BASE_LICENSE_OK,
	BASE_LICENSE_FAIL,

	BASE_ENTER_DUT_MODE,


    /*Radio Tx Test*/
    BASE_CSR_RADIO_TEST_CFG_TX_POWER,
    //30
    BASE_CSR_RADIO_TEST_CFG_PKT,
    BASE_CSR_RADIO_TEST_CFG_FREQ,
    BASE_CSR_RADIO_TEST_CFG_HOPPING_SEQ,
    BASE_CSR_RADIO_TEST_TXSTART,
    BASE_CSR_RADIO_TEST_TXDATA1,
    BASE_CSR_RADIO_TEST_TXDATA2,
    BASE_CSR_RADIO_TEST_TXDATA3,
    BASE_CSR_RADIO_TEST_TXDATA4,


    /*Radio Rx Test*/
    /*TO BE DONE*/
    BASE_STOP_BCHS_TASK,
} BT_BASE_MSG_E;

/*************************************************************************************************************
*   ----------------------------------Bluetooth HFP Message & Event here----------------------------------
**************************************************************************************************************/
typedef enum _BLUETOOTH_HFP_MSG {
	START_HFP_PROFILE = HANDSFREE_MESSAGE_BASE,
	STOP_HFP_PROFILE,
	START_HFP_PROFILE_OK,
	STOP_HFP_PROFILE_OK,
	HFP_CONNECTED,
	HFP_CONNECTED_FAIL,
	HFP_DISCONNECTED,
	HFP_SCO_CONNECT_NO_CALL_STATUS,
	HFP_CALL_STATUS_INCOMING,
	HFP_CALL_STATUS_ALERT,
	HFP_CALL_STATUS_OUTGOING,//10
	HFP_CALL_STATUS_TALKING,
	HFP_CALL_HOLD_STATUS,
	HFP_CALL_STATUS_NONE,
	HFP_SCO_CONNECTED,
	HFP_SCO_DISCONNECTED,
	HFP_PHONE_STATUS,
	HFP_SPEAKER_GAIN,
	HFP_PHONE_NUMBER,
	HFP_NEWCALL,
	HFP_SWICALL,//20
	HFP_ENDCALL,
	HFP_HOLDCALL,
	HFP_SPEAKER_ADJUST,
	HFP_ACCEPTCALL,
	HFP_REJECTCALL,
	HFP_CARTOPHONE,
	HFP_RELEASESCO,
	HFP_CARTOPHONE_OK,
	HFP_3WAY_SWAP,
	HFP_3WAY_MERGE,//30
	HFP_3WAY_ENDCALL,
	HFP_3WAY_HOLD_CUR_ACPT_OTH,
	HFP_3WAY_REL_CUR_ACPT_OTH,
	HFP_3WAY_ACTIVE_CUR_REL_OTH,
	HFP_KEYBOARD_PRESS,
	HFP_CALL1_OK,
	HFP_CALL2_OK,
	HFP_CALL_FULL,
	HFP_HANG1_OK,
	HFP_HANG2_OK,//40
	HFP_HANG_ERR,

	HFP_SYNCLOG,
	HFP_SYNCLOG_OK,
	HFP_SYNCLOGSTART_OK,

	HFP_PHONEBOOK_START,
	HFP_PHONEBOOK_SETPATH,
	HFP_PHONEBOOK_GET,
	HFP_PHONEBOOK_NUM,
	HFP_PHONELOG_NUM,
	HFP_PHONEBOOK_NAME_ENCODE_TYPE,//50
	HFP_PHONEBOOK_GET_FAIL,
	HFP_PHONELOG_GET_FAIL,
	HFP_PHONEBOOK_DATA_GET,
	HFP_PHONEBOOK_DATA_END,
	HFP_PHONELOG_DATA_GET,
	HFP_PHONELOG_DATA_END,

#ifdef BQB_TEST_SCO_CALL_ID
	HFP_SNED_CALL_ID,
	HFP_CONNECT_SCO,
#endif
} BT_HFP_MSG_E;

/*************************************************************************************************************
*   ----------------------------------Bluetooth A2D Message & Event here----------------------------------
**************************************************************************************************************/
typedef enum _BLUETOOTH_A2D_MSG {
	START_A2D_PROFILE = A2D_MESSAGE_BASE,
	//STOP_A2D_PROFILE,
	STOP_A2D_PROFILE_FORCE,//swjiang added,2013.11.7
	START_A2D_PROFILE_OK,
	STOP_A2D_PROFILE_OK,
	STOP_A2D_PROFILE_FORCE_OK,//swjiang added,2013.11.7
	A2DP_CONNECT,
	A2DP_DISCONNECT,
	//  A2DP_AUDIO_DATA,
	AVRCP_CONNECT,
	AVRCP_CONNECT_OK,
	AVRCP_DISCONNECT,
	AVRCP_DISCONNECT_OK,
	A2DP_MUSIC_PAUSE,
	A2DP_MUSIC_PAUSE_OK,
	A2DP_MUSIC_PREV,
	A2DP_MUSIC_PREV_OK,
	A2DP_MUSIC_PLAY,
	A2DP_MUSIC_PLAY_OK,
	A2DP_MUSIC_NEXT,
	A2DP_MUSIC_NEXT_OK,
	A2DP_MUSIC_STOP,
	A2DP_MUSIC_STOP_OK,
	A2DP_MUSIC_VOLUME_UP,
	A2DP_MUSIC_VOLUME_UP_OK,
	A2DP_MUSIC_VOLUME_DN,
	A2DP_MUSIC_VOLUME_DN_OK,
	A2DP_MUSIC_PAUSE_IND,
	A2DP_MUSIC_FWD_SEEK,//Yuan Guochao for get from avrcp msg AVRCP_PLAYER_STATUS_IND
	A2DP_MUSIC_REV_SEEK,

	A2DP_MUSIC_PLAYSTATUS,
	A2DP_MUSIC_PLAYSTATUS_IND,
	A2DP_MUSIC_PLAYMODE,
	A2DP_MUSIC_PLAYMODE_IND,
	A2DP_MUSIC_ATTRIBUTES,
	A2DP_MUSIC_MUSICINFO_IND,
	A2DP_MUSIC_PLAYTIME_IND,

	A2DP_MUSIC_SETABSOLUTEVOLUME_UP,
	A2DP_MUSIC_SETABSOLUTEVOLUME_DOWN,

	AVRCP_VENDOR_COMPANY_ID,
	AVRCP_REMOTE_CONTROL,

	STOP_A2D_PROFILE,
} BT_A2D_MSG_E;

/*************************************************************************************************************
*   ----------------------------------Bluetooth PAC Message & Event here----------------------------------
**************************************************************************************************************/
typedef enum _BLUETOOTH_PAC_MSG {
	START_PAC_PROFILE = PAC_MESSAGE_BASE,
	STOP_PAC_PROFILE,
	START_PAC_PROFILE_OK,
	STOP_PAC_PROFILE_OK,
	PAC_SYNCPB,
	PAC_SYNCPB_OK,
	PAC_SYNCPBSTART_OK,
	PAC_SYNCLOG,
#if (defined(PHONEBOOK_DOWNLOAD_ABORT)||(defined(ABORT_PHONEBOOK_DOWNLOAD)))//hewei add 0212
	PAC_ABORT_DOWNLOAD,
#endif
	PAC_SYNCLOG_OK,
	PAC_SYNCLOGSTART_OK,
	PAC_PBK_RESUME_MUSIC,//hewei
} BT_PAC_MSG_E;

/*************************************************************************************************************
*   ----------------------------------Bluetooth SPP Message & Event here----------------------------------
**************************************************************************************************************/
typedef enum _BLUETOOTH_SPP_MSG {
	SPP_CONNECT = SPP_MESSAGE_BASE,
	SPP_DISCONNECT,
	SPP_SEND_DATA,
	SPP_CONNECT_OK,
	SPP_DISCONNECT_OK,
	SPP_APSEND_DATA,
	SPP_APSEND_DATA_OK,
	SPP_RECEIVE_DATA_TO_AP,
	SPP_RECEIVE_DATA,
	SPP_RECEIVE_DATA_PARSER,
	SPP_MSG_MAX,
} BT_SPP_MSG_E;
/*************************************************************************************************************
*   ----------------------------------Bluetooth SDP Message & Event here----------------------------------
**************************************************************************************************************/
typedef enum _BLUETOOTH_SDP_MSG {
    START_SDP_PROFILE = SDP_MESSAGE_BASE,
    SDP_SVC_INFO_DEVICE_ID= SDP_MESSAGE_BASE,
    SDP_MSG_MAX,
} BT_SDP_MSG_E;

/*************************************************************************************************************
*   ----------------------------------Bluetooth HID Message & Event here----------------------------------
**************************************************************************************************************/
typedef enum _BLUETOOTH_HID_MSG {
	START_HID_PROFILE = HID_MESSAGE_BASE,
	STOP_HID_PROFILE,
	START_HID_PROFILE_OK,
	STOP_HID_PROFILE_OK,
	HID_CONNECT,
	HID_DISCONNECT,
	HID_SEND_INPUT_DATA,//only used for device
	HID_SEND_INPUT_BUTTON_DATA,
	HID_RECEIVE_INPUT_DATA,//only used for host
	HID_REGISTER_SERVER,
	HID_UNREGISTER_SERVER,
	HID_SEND_DATA,
} BT_HID_MSG_E;

extern int init_cbk_task(void);
extern int SDK_Music_IF_PlayStatus();
extern int SDK_TEST_IF_EnterBRTestMode(void);///always return BTMW_RESULT_OK

/* SDK_TEST_IF_CFG_FREQ(txrx_freq_us,  loopback_offset,  report_freq)
配置射频tx测试的测试包发送时间间隔相关参数。
参数说明：
txrx_freq_us:
相邻两个测试包的发送时间间隔，单位是微秒。也就是测试包的发送周期。
取值范围(625~65535) 必须为625us的倍数。

loopback_offset:
回环测试中，接收回环数据包的时隙相关参数，单位是微秒。
取值范围(625~65535) 必须为625us的倍数。
loopback_offset必须小于（不能等于）设置的txrx_freq_us参数。

report_freq:
接收测试的报告周期，单位是秒。
取值范围(1~8)
*/
extern int SDK_TEST_IF_CFG_FREQ(UINT16 txrx_freq_us, UINT16 loopback_offset, UINT16 report_freq); ///event BASE_CSR_RADIO_TEST_CFG_FREQ
/*
SDK_TEST_IF_CFG_TX_POWER(power_dB)
根据用户配置来调整发射功率相关熟悉。
power_dB:
发射测试的最大发射功率。
取值范围(离散值: 0, 4, 8)
*/
extern int SDK_TEST_IF_CFG_TX_POWER(UINT16 power_dB);   ///event BASE_CSR_RADIO_TEST_CFG_TX_POWER
/*SDK_TEST_IF_CFG_PKT(bt_packet_type, packet_len)
测试蓝牙数据包类型
bt_packet_type&packet_len：
蓝牙包类型和包长一一绑定的参数，见下表(均为10进制表示)。根据不同测试模式来调整
*/
extern int SDK_TEST_IF_CFG_PKT(UINT16 bt_packet_type, UINT16 packet_len);///event BASE_CSR_RADIO_TEST_CFG_PKT
/*
SDK_TEST_IF_TXSTART(lo_freq, int_feq_lvl)
定频测试
lo_freq：
定频测试的中心频率
取值范围(2402~2480)
int_freq_lvl：
放大增益，值越大则发射功率越大
取值范围(0~63)*/
extern int SDK_TEST_IF_TXSTART(UINT16 lo_freq, UINT16 int_freq_lvl);	///event BASE_CSR_RADIO_TEST_TXSTART
/*
DK_TEST_IF_TXDATA1(lo_freq, int_feq_lvl)
数据发射测试, 需要先调用CFG_PKT来配置数据包类型。
lo_freq：
定频测试的中心频率
取值范围(2402~2480)
int_freq_lvl：
放大增益，值越大则发射功率越大
取值范围(0~63)*/

extern int SDK_TEST_IF_TXDATA1(UINT16 lo_freq, UINT16 int_freq_lvl);	///event BASE_CSR_RADIO_TEST_TXDATA1

extern int SDK_TEST_IF_CFG_TX_PA_ATTEN(UINT16 pa_level);//pa_level: 0~8, 发射增益相关
extern int SDK_TEST_IF_CFG_UAP_LAP(UINT16 UAP, UINT32 LAP);//设置发射/接收测试的蓝牙地址, 发射接收用同样的UAP/LAP设置即可。 UAP:0~0xFFFF, LAP:0~0xFFFFFF
extern int SDK_TEST_IF_RXDATA1(UINT16 lo_freq, UINT16 is_highside, UINT16 rx_atten); //rx 定频测试, lo_freq: 2402~2480, 中心频率; is_highside：1或者0,调制参数; rx_atten:0~15, 接收衰减
extern int SDK_TEST_IF_RXDATA2(UINT16 is_highside, UINT16 rx_atten);//rx 跳频测试. is_highside：1或者0,调制参数; rx_atten:0~15, 接收衰减
extern int SDK_TEST_IF_BLE_TXSTART(UINT8 channel, UINT8 payload_length, UINT8 payload_type); //BLE tx 测试. channel：0~39, BLE 频道. payload_length: 0~37, BLE数据帧长度. payload_type:0~7,测试帧类型
extern int SDK_TEST_IF_BLE_RXSTART(UINT8 channel);//BLE rx 测试. channel：0~39, BLE 频道.
extern int SDK_TEST_IF_BLE_TEST_END(void);//停止BLE测试,适用于tx和tx。对于rx 测试,会打印收到的BLE数据包数量。
extern int SDK_TEST_IF_CFG_BR_TX_PA_TRIM_BY_CHANNEL(UINT16 channel, INT16 trim);//频道功率平坦度微调接口 channel: 2402~2480, 蓝牙br/edr频道。 trim: -8~7, 功率补偿，数值越大功率越大。负数为降低功率。
extern int SDK_TEST_IF_CFG_XTAL_FTRIM(UINT16 ftrim);// 频偏补偿接口 ftrim：0~63. 默认值40.如果需要调整频偏，在此基础上微调。


/*extern int SDK_Music_IF_Ctrl_connect();
extern int SDK_Music_IF_Ctrl_disconnect();
extern int SDK_Music_IF_Connect(t_bdaddr address);
extern int SDK_Music_IF_Disconnect();
extern int SDK_Music_IF_ForceDisconnect();
extern int SDK_Music_IF_Next();
extern int SDK_Music_IF_Prev();
extern int SDK_Music_IF_Pause();
extern int SDK_Music_IF_Stop();
extern int SDK_Music_IF_Play();
extern int SDK_Music_IF_VOLUME_UP();
extern int SDK_Music_IF_VOLUME_DN();
extern int SDK_Music_IF_PlayStatus();
extern int SDK_Music_IF_PlayMode();
extern int SDK_Music_IF_Attributes();*/




#endif //__TEST_BT_MW_H__
