#ifndef __BT_AO_AUDIO_H__
#define __BT_AO_AUDIO_H__
#include "audDspService/AudDspService.h"
#include "bt_common.h"


typedef struct MainAudioCtrlAction
{
	int (*BtAudioSetSpeakAndWaterDelay)(BtDelaySyncInfo *pDelaySyncInfo);
	int (*BtAudioEncodePlaySync)(int IsTure);
	int (*BtAudioDelayEncodePlay)(int IsTure);
}BtMainAudioCtrlAction;


typedef struct SubAudioCtrlAction
{
	int (*BtDelaySendEncodePlayCmd)(int IsTure);
}BtSubAudioCtrlAction;


typedef union
{
	BtMainAudioCtrlAction MainAudio;
	BtSubAudioCtrlAction  SubAudio;
}BtAudioCtr;


typedef struct
{
	int (*BtAudioHandle)(QActive *me, QEvt const * const e,SINK_MODE_e role,SrcType_e src);
	int (*BtAudioCodecInit)(void);
	int (*BtAudioAllow)(int IsTrue);
	BtAudioCtr BtAudioAction;
}BtAudioMgrApi;

typedef struct
{
	int PlatformRole;
	int TwsWorkEn;
	int SinkMode;
	int IsConnectedSetDelay;
}BtAudioInitInfo;


int BtAudioInit(BtAudioMgrApi **pBtAudioAction, BtAudioInitInfo *pAudioInfoInit);

#endif

