#ifndef __BT_AO_COMMENT_ACTION_H__
#define __BT_AO_COMMENT_ACTION_H__
typedef enum {
	BT_EIR_DATATYPE_FLAGS =                                                 0x01,
	BT_EIR_DATATYPE_INCOMPLETE_LIST_OF_16BIT_SERVICE_UUIDS =          		0x02,
	BT_EIR_DATATYPE_COMPLETE_LIST_OF_16BIT_SERVICE_UUIDS =            		0x03,
	BT_EIR_DATATYPE_INCOMPLETE_LIST_OF_32BIT_SERVICE_UUIDS =         		0x04,
	BT_EIR_DATATYPE_COMPLETE_LIST_OF_32BIT_SERVICE_UUIDS =            		0x05,
	BT_EIR_DATATYPE_INCOMPLETE_LIST_OF_128BIT_SERVICE_UUIDS =         		0x06,
	BT_EIR_DATATYPE_COMPLETE_LIST_OF_128BIT_SERVICE_UUIDS =           		0x07,
	BT_EIR_DATATYPE_SHORTENED_LOCAL_NAME =                                  0x08,
	BT_EIR_DATATYPE_COMPLETE_LOCAL_NAME =                                   0x09,
	BT_EIR_DATATYPE_TX_POWER_LEVEL =                                        0x0A,
	BT_EIR_DATATYPE_CLASS_OF_DEVICE =                                       0x0D,
	BT_EIR_DATATYPE_SIMPLE_PAIRING_HASH_C =                                 0x0E,
	BT_EIR_DATATYPE_SIMPLE_PAIRING_RANDOMIZER_R =                           0x0F,
	BT_EIR_DATATYPE_DEVICE_ID =                                             0x10,
	BT_EIR_DATATYPE_SECURITY_MANAGER_TK_VALUE =                             0x10,
	BT_EIR_DATATYPE_SECURITY_MANAGER_OUT_OF_BAND_FLAGS =                    0x11,
	BT_EIR_DATATYPE_SLAVE_CONNECTION_INTERVAL_RANGE =                       0x12,
	BT_EIR_DATATYPE_LIST_OF_16BIT_SERVICE_SOLICITATION_UUIDS =              0x14,
	BT_EIR_DATATYPE_LIST_OF_128BIT_SERVICE_SOLICITATION_UUIDS =             0x15,
	BT_EIR_DATATYPE_SERVICE_DATA =                                          0x16,
	BT_EIR_DATATYPE_PUBLIC_TARGET_ADDRESS =                                 0x17,
	BT_EIR_DATATYPE_RANDOM_TARGET_ADDRESS =                                 0x18,
	BT_EIR_DATATYPE_APPEARANCE =                                            0x19,
	BT_EIR_DATATYPE_ADVERTISING_INTERVAL =                                  0x1A,
	BT_EIR_DATATYPE_LE_BLUETOOTH_DEVICE_ADDRESS =                           0x1B,
	BT_EIR_DATATYPE_LE_ROLE =                                               0x1C,
	BT_EIR_DATATYPE_SIMPLE_PAIRING_HASH_C256 =                              0x1D,
	BT_EIR_DATATYPE_SIMPLE_PAIRING_RANDOMIZER_R256 =                        0x1E,
	BT_EIR_DATATYPE_LIST_OF_32BIT_SERVICE_SOLICITATION_UUIDS =              0x1F,
	BT_EIR_DATATYPE_SERVICE_DATA_32BIT_UUID =                               0x20,
	BT_EIR_DATATYPE_SERVICE_DATA_128BIT_UUID =                              0x21,
	BT_EIR_DATATYPE_3D_INFORMATION_DATA =                                   0x3D,
	BT_EIR_DATATYPE_MANUFACTURER_SPECIFIC_DATA =                            0xFF
} tBT_EIRDataType;

#define BT_SERVICE_CLASS_SERIAL_PORT			0x1101
#define BT_SERVICE_CLASS_AUDIO_SOURCE			0x110A
#define BT_SERVICE_CLASS_AUDIO_SINK				0x110B
#define BT_SERVICE_CLASS_AVRCP_CT				0x110E
#define BT_SERVICE_CLASS_HANSFREE_AG			0x111F
#define BT_SERVICE_CLASS_GATT					0x1801

typedef struct
{
	BtAddr_t		LocalAddr;
	char			LocalName[NAME_LEN];
	unsigned int    verdoridSource;
	unsigned int	BtDeviceId_Pid;
	unsigned int	BtDeviceId_Vid;
	unsigned int	BtDeviceId_Version;
	//avrcp
	char avrcpCtServiceName[25];	//<25
	char avrcpCtProviderName[25];	//<25
	char license[48];
}Init_Parm;

extern int BtDoInit(void);
extern int BtDoDeInit(void);
extern int BtDoEnable(int *pResult,Init_Parm st_Init_Parm);
extern int BtDoDisable(int *pResult);
extern int IsBtDoSearching(int *pIsTrue);
extern void BtDoStartSearch(void);
extern void BtDoStopSearch(void);
extern void BtDoPairStart(BtAddr_t *pBtAddr);
extern void BtDoUnpair(BtAddr_t *pBtAddr);
extern void BtDoAddBondDev(BTPairDev *stPairDev);
extern void BtDoClearBondDev(void);
extern void BtDoClearAutoInfo(void);
extern void BtDoSppConnect(BtAddr_t *pBtAddr);
extern 	int BtDoSppConnectSamsung(BtAddr_t *pBtAddr,unsigned char pin);
extern void BtDoSppDisconnect(BtAddr_t *pBtAddr);
extern void BtDoSppDataOut(void* pParam);
extern int BtDoUdtTest(void);
extern void BtDoRfPowerSet(DBM_LEVEL_e level);
extern void BtDoScanModeSet(SCAN_MODE_e mode);
extern void BtDoScanModeGet(SCAN_MODE_e *pMode);
extern int BtSetPinCode(char *pPinCode);
extern int BtReplyPinCode(BtAddr_t* pBtAddr,char *pPinCode);
extern int BtDoGetBondDevNum(int *pBondedNum);
extern int BtDoGetRemoteDevNum(int *pBondedNum) ;
extern int BtDoGetProfileState(BtAddr_t *pBtAddr,BT_PROFILE_e profile,BT_PROFILE_STATE_e *pstate);
extern int BtDoGetRemoteDevInfo(BtAddr_t *pBtAddr,stBtDevInfo *pBtDev);
extern int BtDoGetRemoteDevByIdex(int dex,stBtDevInfo *pBtDev);
extern int BtDoLocalNameSet(char *pName);
extern int BtDoLocalLmpNameSet(char *pName);
extern int BtDoLocalNameGet(char *pName);
extern int BtDoLocalDeviceClassSet(unsigned int *pDeviceClass);
extern int BtDoRemoveSdpDeviceIDGet(BtAddr_t*pBtAddr);
extern int BtDoLocalAddrGet(BtAddr_t*pBtAddr);
extern int BtDoLocalAddrSet(BtAddr_t *pBtAddr);
extern int BtDoGetLocalInfo(BtLocalInfo *pdev);
extern int BtDoGetBondByIdex(int sdBondedNum,stBtDevInfo *pBtDev);
extern void BtDoSendUserCmd(BtAddr_t *pBtAddr,UINT8 length,UINT8 *pUserCmdData);
extern int BtDenyConnect(BtAddr_t *pBtAddr,ENABLE_STATE_e state);
extern int BtDoGetRemoteDevRssi(BtAddr_t *pBtAddr);


#endif

