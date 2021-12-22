#ifndef __BT_COMMON_H__
#define __BT_COMMON_H__
#include "cfg_bt.h"

#define W_64     long long
#define W_32     int
#define W_16     short
#define W_8      char

typedef unsigned W_64 UINT_64;
typedef unsigned W_32 UINT_32;
typedef unsigned W_16 UINT_16;
typedef unsigned W_8 UINT_8;

#ifndef SUCCESS
#define SUCCESS	0
#endif

#ifndef FAIL
#define FAIL	-1
#endif

#define bt_ao_printf(fmt, arg...) LOGDT(fmt, ## arg)
#define bt_ao_ntc_printf(fmt, arg...) LOGD(fmt, ## arg)
#define bt_ao_warn_printf(fmt, arg...) LOGW(fmt, ## arg)
#define bt_ao_error_printf(fmt, arg...) LOGE(fmt, ## arg)



typedef enum DBM_LEVEL
{
	MINUS_3DBM_LVL,
	MINUS_1DBM_LVL,
	POSITIVE_1DBM_LVL,
	POSITIVE_3DBM_LVL,
	POSITIVE_4DBM_LVL,
	POSITIVE_5DBM_LVL,
	POSITIVE_9DBM_LVL,
}DBM_LEVEL_e;



typedef enum BTPROFILE
{
	NONE_PROFILE=-1,
	A2DP_SINK, //!<profile a2dp sink role
    A2DP_SOURCE, //!<profile a2dp source role
	SPP_PROFILE,
	MAX_PROFILE,
}BT_PROFILE_e;


typedef enum {
	STATUS_STOP = 0,
	STATUS_PLAY = 1,
	STATUS_PAUSE = 2,
	STATUS_FWD_SEEK = 3,
	STATUS_REV_SEEK = 4,
	STATUS_UNKNOWN = 0xFF
}BtDevPlayerState_e;


typedef enum BTPROFILESTATE
{
	DISCONNECT_OK, /*!<profile dieconnected state*/
	CONNECT_ING, /*!<profile connecting state*/
	CONNECT_OK, /*!<profile connected state*/
	DISCONNECT_ING, /*!<profile disconnecting state*/
	CONNECT_FAIL,
	DISCONNECT_FAIL,
}BT_PROFILE_STATE_e;



typedef enum
{
	BT_UNREADY,
	BT_INIT,
	BT_DISABLEING,
	BT_ENABLEING,
	BT_READY,
}BT_STATE_e;


typedef enum DEVROLE_DEX
{
	UNKOWN_ROLE_ADDR = -1,
	WSS_ADDR,
	HEADPHONE_ADDR,
	REMOTEAPP_ADDR,
	PHONE_ADDR,
	DEVNUM_MAX = RECODE_REMOTE_DEV_NUN -1,
}DEVROLE_DEX_e;




typedef enum {
	PAIR_NONE,
	UNPAIR, /*!<not bonded*/
	PAIRING, /*!<bonding*/
	PAIRED /*!<bonded*/
} PAIR_STATE_e;



typedef enum SCAN_MODE
{
	SCAN_NONE,
	ONLY_CONNECTABLE,
	BOTH_DISCOVERY_CONNECTABLE,
}SCAN_MODE_e;



typedef enum
{
	AV_PLAY,
	AV_PAUSE,
	AV_NEXT,
	AV_PREV,
}BT_IR_CMD_e;



typedef enum
{
	MD_NONE = 0,
	MD_USB,
	MD_CARD,
	MD_BT,
	MD_SPDIFIN2,
	MD_SPDIFIN3,
	MD_LINEIN,
	MD_AUXIN,
	MD_ARC,
	MD_HDMI_TX,
	MD_MAX,
}SrcType_e;


typedef enum
{
    PAYLOAD_NULL,
	PAYLOAD_CMD,
	PAYLOAD_DATA,
	PAYLOAD_MAX,
}PAYLOAD_TYPE;

//value
typedef enum
{
	TYPE_UPGRADE_REQ,
	TYPE_UPGRADE_READY,
	TYPE_UPGRADE_DATA_FINISH,
	TYPE_CMD_VALUE_MAX,
}TransCmdType;


//value
typedef enum
{
	TYPE_UPGRADE_DATA_IN,
	TYPE_REMOTE_APP_DATA,
	TYPE_FUNC_VALUE_MAX,
}TransFuncType;



typedef struct stBtAddr_
{
	char address[ADDR_LENGTH];
} BtAddr_t;



typedef struct
{
	BtAddr_t addr;
	int		volume;
	DEVROLE_DEX_e	role;
}BtVolumeInfo;


typedef struct
{
	BtAddr_t addr;
	int		volume;
}BtVolumeSet;

typedef struct _Btavrcp_volume_samsung_type {
	BtAddr_t addr;
	UINT_8 	volume;
	UINT_8 	mute;
	UINT_8 	vol_mute_set;	//0: set mute; 1: Set volume
}Btavrcp_volume_samsung_type;

typedef struct
{
	SrcType_e src;
	UINT8	DelayTime;
	UINT8   WaterLevelPacketNums;
}BtDelaySyncInfo;


typedef struct _AppSendData_type_flag {
	UINT_8 	func:2;// 10:data,01:CMD
	UINT_8 	Reserved1:2;
	UINT_8	all_data_send:1;// 1..表示所有数据发送完
	UINT_8 	Reserved2:1;
	UINT_8 	Error_check:2;// 1 表示该数据需要重传机制
	UINT_8	value;// 当为DATA表示该数据功能类型,当为CMD  时表示CMD 参数
}AppSendData_Type_Flag;


typedef struct _reciverdata_type_flag
{
	UINT_16 	func:2;// 10:data,01:CMD
	UINT_16 	Reserved1:2;
	UINT_16 	all_data_send:1;// 1..表示所有数据发送完
	UINT_16 	Reserved2:1;
	UINT_16 	data_end:1;
	UINT_16 	Reserved3:1;
	UINT_16 	value:8;// 当为DATA表示该数据功能类型,当为CMD  时表示CMD 参数
}Reciverdata_Type_Flag;



typedef struct _spp_receiver_data_if
{
	Reciverdata_Type_Flag Type_Flag;
    void*    pdatabuf;      /* payload buf*/
	UINT_32 payload_len;//payload len
}Spp_receiver_Data_If;


typedef struct _spp_send_data_if
{
	AppSendData_Type_Flag  Type_Flag;
	void*    pdatabuf;      /* payload buf*/
	UINT_32 payload_len;
}Spp_Send_Data_If;


typedef struct st_BtData
{
	BtAddr_t BtAddr;
	int	len;
	void *pBtData;
}stBtData;

typedef struct st_BtSppData
{
	BtAddr_t BtAddr;
	UINT_8 port;
	int	len;
	void *pBtData;
}stBtSppData;

typedef struct st_BtSdpDidData
{
	UINT_8 result;
	BtAddr_t bdAddress;
    UINT_16 SpecificationID;
    UINT_16 VendorID;
    UINT_16 ProductID;
    UINT_16 VersionID;
    UINT_16 VendorIDSource;
}stBtSdpDidData;


typedef struct st_BtSppConnect_Samsung
{
	BtAddr_t BtAddr;
	UINT_8 pin;
}stBtSppConnect_Samsung;

typedef struct
{
    BtAddr_t BtAddr;
    char     name[NAME_LEN];
    char  	 NameLen;
	UINT_32      cod;
	//for update ui
	PAIR_STATE_e PairState;
	BT_PROFILE_STATE_e ConnectState;//a2dp
}BTSearchDevInfo;


typedef struct
{
    UINT_8	 master;
    BtAddr_t bd_addr;
	UINT_32      cod;
    char     name[NAME_LEN];
    char  name_len;
	UINT_8   linkKeyType; /* MGR_COMBINATION | MGR_LOCALUNITKEY | MGR_REMOTEUNITKEY */
    UINT_8   linkKey[LINKKEY_SIZE];
	UINT_8	 used;
	char result;
}BTPairDev;


typedef struct
{
    BtAddr_t bd_addr;
    char     name[NAME_LEN];
    char  name_len;
	int result;
	char reason;
}BTConnectReqInfo;

typedef struct
{
    BtAddr_t bd_addr;
	UINT_32 reason;
}BTDisConnectReqInfo;

typedef struct
{
    BtAddr_t bd_addr;
    char     name[NAME_LEN];
    char  name_len;
	char result;
	DEVROLE_DEX_e DevRole;
	PAIR_STATE_e 	BtPairState;
	int			  BtConnState;
	UINT_32      cod;
    UINT_8   linkKey[16];
	UINT16    spp_port;
}stBtDevInfo;

typedef struct BtDevInfo_Node
{
   	stBtDevInfo BtDevInfo;
	struct BtDevInfo_Node *next;
}stBtDevInfo_Node;

typedef struct
{
	BtAddr_t addr;
	BtDevPlayerState_e state;
}BtDevPlayState;


typedef struct stBTLocalInfo_
{
	BtAddr_t addr;
	char name[NAME_LEN];
	char pincode[4];
}BtLocalInfo;

typedef struct BtRemoteDevRssi_ {
	BtAddr_t addr; /*!< device address */
	INT8 cmd_sta;
   	INT16 conn_handle;
	INT32 value;
}BtRemoteDevRssi_t;

typedef struct
{
	UINT8   result;//0, 读取成功; 255, 由于A2DP未连接, 获取codec信息失败。失败时codec_type, sample_rate无效。
    BtAddr_t bt_addr;
    UINT16 codec_type; // 0, SBC; 2, AAC
    UINT32 sample_rate;
    UINT32 bitrate; //目前解码不在SDK实现,无法获取bitrate。保留备用, 默认填写0。
}stBtAudioPlayInfo;

#define Connected_Device_Max 5

typedef enum
{
	AVDTP,
	AVRCP,
	SPP,
}Profile_Name_e;

typedef enum
{
	ACL_CONNECTED,
	ACL_DISCONNECTED,
}Acl_State_e;

typedef struct
{
	BtAddr_t bt_addr;
	UINT8 acl_state;
	UINT8 profile_state;
	UINT8 spp_port;
}Connected_Device;

typedef struct
{
	BtAddr_t bd_addr;
	UINT8   data_length;
	UINT8   data[1];//可变长度
}stADVData;

typedef struct
{
	UINT8 status;
	BtAddr_t bd_addr;
}stBtAvrcpPlayBackStatus;

extern void *memcpy(void *, const void *, unsigned);
extern void *memset(void * pDst, int iFillValue, UINT32 uiLen);
extern int	memcmp(const void *, const void *, unsigned/*size_t*/);
extern unsigned strlen(const char *);

#endif //__BT_COMMON_H__

