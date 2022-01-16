#ifndef __BT_BACKGROUND_APP_H__
#define __BT_BACKGROUND_APP_H__
#include "bt_ao_service_api.h"
#include "bt_common.h"



typedef struct
{
	QHsm super;
	QActive *TargetAo;
	ENABLE_STATE_e BtAutoWork;
	ENABLE_STATE_e TwsWork;
	SINK_MODE_e BtTwsMode;
	int BtState;
	int IsSinkDevConnected;
	int IsDevConnected[DEVNUM_MAX];
	int iSrc; // userapp src
	int	IsSurportTwsSwitch;
	int	TwsRecodeWssVolume;
	int	TwsRecodeHeadphoneVolume;
	int BtHeadPhoneMute;
	int	IsSinkDevVolumeSyncReg;
	int IsSourceDevVolumeSyncReg;
	//if no to disconnect phone when platform switch source
	char 			IsIgnoreSwitchSrc;
	UINT8 *aUserSrc_tbl;
	UINT8 SrcEnd;
	QTimeEvt timeAutoPlayEvt;
	UINT_8 PairDevRecordIndex;
} BtBackGroundQMSM;


typedef struct
{
	QEvt stEvt;
	UINT8 vol;
} stVolSet;

typedef enum
{
	NAME,
	LINKKEY,
}DevInfoType_e;

#define BT_DEV_CLASS1     0x040414
#define BT_DEV_CLASS2     0x000414
#define BT_DEV_CLASS3     0x240414
#define BT_DEV_CLASS4     0x040404
#define BT_DEV_CLASS_T550 0x240404

void BtBackGroup_Ctor(BtBackGroundQMSM **pBtQmsm, QActive *TargetAo);
int UserAppVolumeNotifyToBt(INT16 Vol, INT8 MuteState);
int UserAppSetSubVolumeToBt(UINT8 Vol);
int BtGetRemoteAddress(BtAddr_t *pAddress);


#endif
