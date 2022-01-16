#ifndef __BT_AO_PHONE_H__
#define __BT_AO_PHONE_H__


typedef void (*SourceConnect)(BtAddr_t *pBtAddr);
typedef void (*SourceDisconnect)(BtAddr_t *pBtAddr);
typedef void (*SourceConnectOk)(stBtDevInfo *pDev ,char IsTrue);
typedef void (*SourceDisconnectOk)(stBtDevInfo *pDev ,char BtModeState);
typedef void (*SourceVolumeSyncReg)(BtVolumeInfo *pVolumeInfo);
typedef void (*SourceVolumeSyncSet)(BtVolumeSet *pVolumeInfo);

typedef int (*SourceSetConnectState)(BT_PROFILE_STATE_e state);
typedef int (*SourceGetConnectState)(BT_PROFILE_STATE_e *state);
typedef int (*SourceGetAddr)(BtAddr_t *pAddr);
typedef int (*SourcePlay)(void);
typedef int (*SourcePause)(void);
typedef int (*SourceIsEqualAddr)(BtAddr_t *pAddr,int *pState);
typedef int (*SourceIsPlaying)(int *pIsTrue);


typedef struct BtDevAction
{
	SourceConnect			Connect;
	SourceDisconnect		Disconnect;
	SourceConnectOk			ConnectOk;
	SourceDisconnectOk		DisconnectOk;
	SourceGetConnectState	GetConnectState;
	SourceGetAddr			GetAddr;
	SourcePlay				Play;
	SourcePause				Pause;
	SourceIsPlaying			IsPlay;
	SourceIsEqualAddr		IsEqualAddr;
	SourceVolumeSyncReg		VolumeSyncReg;
	SourceVolumeSyncSet		VolumeSyncSet;
}stBtDevAction;

void PhoneSaveDevInfo(stBtDevInfo *pDev);
int IsPhoneAddr(BtAddr_t *pAddr,int *pState);
int PhoneGetAddr(BtAddr_t *pAddr);
void PhoneDisconnect(BtAddr_t *pAddr);
void PhoneConnect(BtAddr_t *pAddr);
int PhonePlay(void);
int PhonePause(void);
int PhonePrev(void);
int PhoneNext(void);
int PhoneConnectStateGet(BT_PROFILE_STATE_e *pState);
int PhoneAudioStateGet(BT_IR_CMD_e *pState);
int PhoneAudioPlayStateUpdate(BT_IR_CMD_e state);
void PhonePairFailAction(BTPairDev *pDev);
void PhonePairOkAction(BTPairDev *pDev);
void PhoneConnectOkAction(stBtDevInfo *pDev);
void PhoneDisconnectOkAction(stBtDevInfo *pDev);
void PhoneVolumeSyncReg(BtVolumeInfo *pVolumeInfo);
void PhoneVolumeSyncSet(BtVolumeSet *pVolumeInfo);
int PhoneIsPlaying(int *pIsTrue);
int BtSourceInit(void);
//int BtSourceInit(stBtDevAction **pAction);

extern int SDK_Music_IF_Ctrl_connect(BtAddr_t *address);
extern int SDK_Music_IF_Ctrl_disconnect(BtAddr_t *address);
extern int SDK_Music_IF_Connect(BtAddr_t *address);
extern int SDK_Music_IF_Disconnect(BtAddr_t *address);
extern int SDK_Music_IF_ForceDisconnect(BtAddr_t *address);
extern int SDK_Music_IF_Next(BtAddr_t *address);
extern int SDK_Music_IF_Prev(BtAddr_t *address);
extern int SDK_Music_IF_Pause(BtAddr_t *address);
extern int SDK_Music_IF_Stop(BtAddr_t *address);
extern int SDK_Music_IF_Play(BtAddr_t *address);
extern int SDK_Music_IF_VOLUME_UP(BtAddr_t *address);
extern int SDK_Music_IF_VOLUME_DN(BtAddr_t *address);
extern int SDK_Music_IF_PlayStatus(BtAddr_t *address);
extern int SDK_Music_IF_PlayMode(BtAddr_t *address);
extern int SDK_Music_IF_Attributes(BtAddr_t *address);

#endif

