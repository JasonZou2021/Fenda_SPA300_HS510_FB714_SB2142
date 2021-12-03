
#ifndef _REPEATER_SRV_

#define _REPEATER_SRV_

#include "userapp.h"

#if defined(USERAPP_SUPPORT_REPEATER) && (USERAPP_SUPPORT_REPEATER == 1)
#include "ITE_hdmi_repeater.h"
#else
#include "EP_hdmi_repeater.h"
#endif

typedef struct {     /* the repeater active object */
    QHsm super;
    QActive* superAO;
    stHdmiRepeatHandler_t stRepeatHandler;
    int HPD_state;
    int RepeaterSrc;
	int RepeatersrcState;
	int TV_Switch_Source; //add to distinguish TV/8107 switching source      1 -> TV    0 -> 8107
	int Mute_state;
	int Repeater_power_state;
	int EX_INTR_last;
	int EX_INTR_New;
	int eARC_state;
	int Audio_path;
	int AudSys_User_control;
	int AudSys_TV_control;
	int EP_check_Counter;
	int First_PowerUp_Flag;
	QTimeEvt timeEvt_I2cPolling;

}RepeaterSrv_t;

typedef struct { // vol sig
    QEvt super;

    int Hpd_status;
    int Arc_status;
} RepeaterHPDEvt_t;

typedef struct { // DSP Source ID
    QEvt super;

    int DSPSourceID;
} RepeatereDSPSrcIDEvt_t;

typedef struct { //LPCM channel number
    QEvt super;

    UINT8 LPCM_channelNum;
	UINT32 LPCM_ChannelMapping;

} RepeatereLPCMNumEvt_t;

typedef enum
{
    eRepeater_Src_HDMI_P0 = 0,
    eRepeater_Src_HDMI_P1,
    eRepeater_Src_HDMI_P2,
    eRepeater_Src_ARC = 5,
    eRepeater_Src_Other,

    eRepeater_Src_Max,
}eRepeater_Src_t;

typedef enum
{

	eRepeater_Upgrade_EN = 0,
    eRepeater_Upgrade_VER,


}eRepeater_Upgrade_t;

typedef enum
{

	eRepeater_AudSrc_Disable = 0,
    eRepeater_AudSrc_Enable,

}eRepeater_AudSrc_t;

typedef enum
{

	eRepeater_ARCSRC = 0,
    eRepeater_eARCSRC,

}eRepeater_EARCSrc_t;

typedef enum
{

	TV_wakeup_to_Repeater = 0,
    AudSYS_wakeup_to_Repeater,

}eRepeater_WakeUPstate_t;

typedef enum
{

	eRepeater_Aud_PCM = 0,
    eRepeater_Aud_COMPRESSED,

}eRepeater_AudPCM_t;

typedef enum
{
	eRepeater_uesrSrc_ARC = eAp_ARC_Src,
    eRepeater_userSrc_HDMI_P0 = eAp_HDMI0_Src,
    eRepeater_userSrc_HDMI_P1,
    eRepeater_userSrc_HDMI_P2,

    eRepeater_userSrc_Other,

    eRepeater_userSrc_Max,
}eRepeater_Uear_Src_t;

typedef enum
{
    eRepeater_TV_Speaker = 0,
    eRepeater_Audio_System,

    eRepeater_Path_Max,
}eRepeater_AudioPath_t;

typedef enum
{
    eRepeater_Power_off = 0,
    eRepeater_Power_on = 1,

}eRepeater_Power_t;

enum
{
    eRepeater_ARC_OFF = 0,
    eRepeater_ARC_ON,
};

typedef struct { // Upgrade Signal
    QEvt super;

    int UPgrade_Mode;
} RepeaterUPgradeEvt_t;

typedef struct { // source sig
    QEvt super;

    eRepeater_Src_t src_sel;
	eRepeater_Uear_Src_t src_user;
} RepeaterSrcEvt_t;

typedef struct { // source sig
    QEvt super;
     UINT8 Aud_I2S_Lane;
	 //UINT8 Aud_channel;
} RepeaterAudSrcEvt_t;


typedef struct { // vol sig
    QEvt super;

    int vol;
} RepeaterVolEvt_t;

typedef struct { // mute sig
    QEvt super;

    int mute;
} RepeaterMuteEvt_t;

typedef struct { // eARC Source
    QEvt super;

    int eARCSRC;
} RepeatereARCSrcEvt_t;

typedef struct { // Audio path sig
    QEvt super;

    int Aud_output;
} RepeaterAudOutputEvt_t;

typedef struct { // HPD sig
    QEvt super;

    UINT8 HPD_State;
	UINT8 Repeater_source;
} RepeaterPowerONEvt_t;

typedef struct { // Audio path sig
    QEvt super;

    int Aud_path;
} RepeaterAudPathEvt_t;

typedef struct { // DSP Source  ID

  int Source_ID;
  int eARC_SRC;
} RepeaterSrcState_t;

typedef struct { //User app power on state,and switch source

	int power_state;
	int Source;
	int HPD_state;
	int EP_Reday;
	int eARC_state;
} RepeaterWakeup_t;

void RepeaterSrv_ctor(RepeaterSrv_t * const me);
int RepeaterSrv_UserSwitch(eRepeater_Src_t src);


// the APIs for userapp as below
int RepeaterSrv_PowerUp(void);
int RepeaterSrv_UserPowerUp(RepeaterWakeup_t *Repeater_State);

int RepeaterSrv_PowerDown(void);
int RepeaterSrv_VolSet(int vol);
int RepeaterSrv_MuteSet(int Mute);
int RepeaterSrv_RegInfo(void);
int RepeaterSrv_AudInfo(void);
int RepeaterSrv_VolConvert(UINT8 tbVol, UINT8 tbMaxVol);
int RepeaterSrv_SrcSwitch(eRepeater_Src_t src_sel);
int RepeaterSrv_Standby(void);
int RepeaterSrv_SendAudInfo(void);
int RepeaterSrv_AudOutput(eRepeater_AudioPath_t audio_output);
int RepeaterSrv_SetAudFmt(stHdmiRepeatHandler_t* pstRepeatHandler);
int RepeaterSrv_SetAudPath(int Audio_Path);
int RepeaterSrv_EP_Init(void);
int RepeaterSrv_HPDStatus(int Hpd_status, int ARC_status);
int RepeaterSrv_SourceIDChk(RepeaterSrcState_t *repeaterstatus);
int RepeaterSrv_WakeupFlash(void);
int RepeaterSrv_eARCSRC(int eARCSRC);
int RepeaterSrv_DSPSRCID(int SRCID);

int RepeaterSrv_UserDSPSRCID(int SRCID);
int RepeaterSrv_User_ARC_DSPSRCID(int SRCID);

int RepeaterSrv_eARCScyn(int SRCID);
void RepeaterSrv_EP_Reinit(void);
int RepeaterSrv_EP_ErroCheck(void);
int RepeaterSrv_MuteSYN(int Mute);
int RepeaterSrv_EPUpgrade(UINT8 ugrade_mode);

int RepeaterSrv_WakeupCheckHandler(void);

int RepeaterSrv_EP_RegCB(void);
int RepeaterSrv_REPEATER_RegCB(void);
int RepeaterSrv_UserMuteSet(void);
int RepeaterSrv_UserVolUp(void);
int RepeaterSrv_UserVolDown(void);
int RepeaterSrv_PCMChannel(UINT8 LPcm_channel,UINT32 LPCM_ch_mapping);



#endif
