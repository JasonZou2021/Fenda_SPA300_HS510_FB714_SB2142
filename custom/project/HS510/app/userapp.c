/******************************************************************************/
/**
*
* \file	 userapp.c
*
* \brief	This file provides the userapp middleware state machine framework \n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*\author       email@sunplus.com
*\version	 v0.01
*\date 	 2016/08/9
******************************************************************************/

#include "qpc.h"
#include "userapp.h"
#include "Display_Srv.h"
#include "HDMI_Srv.h"
#include "initcall.h"
#include "AudDspService.h"
#include "audcodec.h"
#include "spXeffect_api.h"
#include "cfg_audtool.h"
#include "dsp_hint_sound.h"
#include "amp.h"
#include <string.h>
#include "cfg_sdk.h"
#include "sdk_cfg.h"
#include "Usersetting.h"
#include "userapp_key_code_convert.inc"
#include "apupgrade.h"
#include "systemservice.h"
#ifdef USERAPP_SUPPORT_BT
	#include "bt_background_app.h"
#endif
#include "userapp_vfd_str_tbl.inc"
#include "task_attribute_user.h"
#include "qf_response.h"
#include "datarammap.h"
#include "project_cfg.h"
#include "powerservice.h"
#include "pinmux.h"
#include "karaoke.h"
#include "hw_gpio_list.h"
#include "LED_Drv.h"

#include "custom_MsgStr.inc"
#include "TvRemoteReady_cfg.h"
#include "settingmanager.h"
#include "VoiceAssist_Srv.h"
#include "systemsetting.h"
#include "touch_app.h"
#include "uart_app.h"
#include "hdmi_app.h"

#define LOG_MODULE_ID ID_APUSR
#include "log_utils.h"
#ifdef MCU_IF_RSR
#include "mcu_if_rsr.h"
#endif

#ifdef CERTTEST_IF
#include "CertTest_if.h"
#endif
#include "fsi_api.h"

#include "Repeater_Srv.h"

#ifdef SYNCCOM_WIRELESS
#include "Wireless.h"
#endif
#include "vfd_seg_map.h"
#include "wireless.h"

#define AND_VER 12

// Merged from SPA300_TPV_20210205
#define SUPPORT_DSP_VOL

#ifdef CONFIG_USB_LOGO_TEST
#include "src/include/fsi/disk_manager.h"

extern void select_usb_port_num(int port_num);
extern int umount(const char *name);
#endif

//#define SUPPORT_HINT_SOUND

extern void vTaskMemoryUsageStats(void);
extern void FullEnvDelayMs(unsigned int sdMSec);

extern int First_Power_Standby(void);

 int vfd_is_scroll(void);
int first_power_up = 0;

extern void setHdmiMode(int mode);

extern int SDK_TEST_IF_EnterBRTestMode(void);///always return BTMW_RESULT_OK

//#define HintSoundTest
#ifdef ATMOS_CERT
	#undef HintSoundTest
#endif


/*..........................................................................*/
//userapp default value
#define UPGRADE_LONG_PRESS_TIME (50) // unit: 100ms
#define UPGRADE_PERCENTAGE_MAX (100)
#define TV_REMOTE_READY_TICK (10) // unit: tick time (200ms) //iKeyTickFreq = KEY_TICK_TIME/KEY_SCAN_FREQ
/*..........................................................................*/

#define USERAPP_XEF_SIZE (100*1024) //(50*1024)  //zehai.liu todo


#define SUPPORT_AP

#ifdef SUPPORT_AP
	#include "mf_player.h"
#endif

//--{{Added by Grace to indicate USB state
#define NO_USB 0
#define USB_NORMAL 1
#define USB_PLAY 0
#define USB_PAUSE 1
#define USB_STOP 2
//--}}

//-----Source Gain Table-----
//(0):source 1
//(1):source 2
//(2):source 3
//(3):source 4
//(4):source 5
//(5):source 6
//(6):source 7
//(7):source 8
//(8):source 9

//please refer to gUserSrc_tbl to mapping source gain sequence in table
// src gain : +24dB ~ -140dB, 0.1dB step
float SrcGain_Tab[] =
{
	0, 0, 0, 0, 0, 0, 0, 0,0, 0,0,0,0,0
};


// user source switch table
// you can adjust the order and number in below source switch table
#define USER_SRC_END (0xFF)
UINT8 gUserSrc_tbl[] =
{
	
	eAp_SPDIF_Optical_Src,
	eAp_SPDIF_Coaxial_Src,
	eAp_ARC_Src,
	eAp_I2S_Src,
	eAp_USB_Src,
	eAp_AUX_Src,
	USER_SRC_END,
};


Q_DEFINE_THIS_FILE;

/*..........................................................................*/
typedef struct __userapp       /* the UserApp active object */
{
	QActive super;   /* inherit QActive */
	uint8_t num; /* private num in playList */
	char *pKeyTable; // ir key table, customized
	int iSrc; // userapp src
	hCircBuf_t *hKeyBuf; // ring buf handler (key)
	int *pKeyCode2SigTable; // key code 2 signal table

	QTimeEvt timeEvt; /* private time event for play state */

	QTimeEvt timeSwitchSrcEvt;
	QTimeEvt timePowerOffEvt;
	QTimeEvt timeRepeatSignalEvt;
	QTimeEvt timeSrvAOReadyEvt;
	QTimeEvt timeEvt_Watchdog;
	QTimeEvt timerWireless;
	QTimeEvt timerSysDetect;
	QTimeEvt timeSrcHintEvt;
	QTimeEvt timeDisplayOnce;
	QTimeEvt timeSrcReady;
	QTimeEvt timeSettingMenu;//--{{ Add by Grace for setting menu timeout 
	QTimeEvt timeHintDelayEvt;
	QTimeEvt timeHintIntervalEvt;

	QTimeEvt timeSrcDelay;
	
#ifdef CONFIG_USB_LOGO_TEST
	QTimeEvt timeDevPlay;

	int playerSource;	//USB device list number
#endif
	QTimeEvt timePassword;

	AutoSrcTest_t *stAutoSrcTest;
	AutoRepeatSignalTest_t *stAutoRepeatSignalTest;

	void *pAPInstance;	/* player instance */
	int showErr;		/* play file error flag */
	int iTrickyFactor;	/* ff/fb factor */

	uint8_t playercreat;
	uint8_t opType;

#ifdef SUPPORT_HINT_SOUND
	DSP_HINT_SOUND_P *pstHintSound;
#endif

    //--{{ Add by Heaven
	int Bass_vol;
	int Treble_vol;
	int Centel_vol;
	int Top_vol;
	int Ls_vol;
	int FTLR_vol;
	int FSLR_vol;
	int RTLR_vol;
	//int Rs_vol;
	int LR_vol;
	int dim_cnt;
	int Upgrade_cnt;
	int Version_cnt;
	int menu_flag;
	int dim_flag;
	int Up_flag;
	int HDMI_Ver;
	int BT_Ver;
	int Display;
	int dolby_status;
	
	int vol;
	int Wireless_vol;
	int mute;
	int Surround_3D;
	int EQMode;
	int DIMMER;
	int dimmer_cnt;
	int iRepeat;
	int iShuffle;
	UINT32 AUD_DPS_UserGain; //AUD_DPS_UPDATE_USRGAIN #dps
	AmpList *hAmpList;

	int UpgradeTime;
	int UpgradeCnt;
	//char Num2Str[14];

	char *SP_UPDATE_bin;
#ifdef BUILT4ISP2
	int ISPBOOOT_bin;
#endif
	int MonkeyTest_Enable;
#ifndef SDK_RELEASE
	int Memory_Check;
#endif

#ifdef HINT_SOUND
	QStateHandler Hint_PreState;
	UINT8 *pHint_Data;
	UINT32 dHintSize;
	UINT32 dHintDataType;
	UINT32 dMusic_Target_Gain;
	UINT32 dHint_Ramp_Up_Velocity;
	UINT32 dHint_Decline_Velocity;
	UINT8  dHint_In_Smp;
#else
	UINT32 dXef_Handle;
#endif
	UINT8 EP_Src_id;

	int TV_Remote_Ready_State;
	int TV_Remote_Ready_cnt;
	int HPD_State;
	int hdmi1_state;
	int hdmi2_state;

	int hdmi_src;
	
#ifdef MCU_IF_RSR
	stMCU_IF_RSR_t stMCU_IF_RSR;
#endif
#ifdef CERTTEST_IF
    stCertTest_IF_t *stCertTest_IF;
#endif
#ifdef USERAPP_SUPPORT_KARAOKE
	Karaoke_Effect *Kara_effect;
#endif
	int audio_mute_pinnum;

	int eARC_SRC;
#ifdef USERAPP_SUPPORT_BT
	BtBackGroundQMSM *pBtBakcGroundApp;
#endif
	UINT8 *aUserSrc_tbl;

#ifdef ATMOS_CERT
	sp_atmos_option_s atmos_opt;
#endif

#if (SUPPORT_VA)
	uint8_t AuxVA_Status;
	uint8_t BtVA_Status;
#endif
	uint8_t bt_connect;
	uint8_t bt_play;

#if (SUPPORT_DEMOMODE)
	uint8_t demoState; //transition for demo and other states
	uint8_t demoBackSrc; //back the source before entering the demo
#endif
	uint8_t wireless_mute_cnt;
	uint8_t wireless_mute;
	uint8_t wl_pair;
	uint16_t wl_pair_cnt;

	uint8_t usb_detect_state;
	uint8_t	usb_play_state;
	uint8_t subwoofer_vol;  // Merged from SPA300_TPV_20210205
	uint8_t spdif_format;
	eInputChannelNum channel_num;
	uint8_t mute_cnt;
	UINT8 ARC_DSP_State;

	DSP_HINT_SOUND_P *pHintCnf;
	eHint_Type hintType;
	UINT32 singlePlayMode;
	UINT8 hintPlayFlag;
	UINT8 hintDelayFlag;
	UINT32 codecType;
	eSoundMenu sound_menu;
	UINT8 PwrOnFlag;
	void (*delay_call)(struct __userapp *);
	UINT8 xeffBufSize;

	int idle_power_standby_count;


	int no_signal_detect_cnt;
	int have_signal_detect_cnt;
	int signal_demute_cnt;
	int signal_mute_cnt;
	int have_signal_flag;
	int src_blink;

	int hdmi_sw_cnt;
	eSoundMenu menu_status;
	INT64 usb_playtime;
	
} UserApp;

static UserApp l_UserApp; /* the UserApp active object */

static AutoSrcTest_t AutoSrcTestBuf;
static AutoRepeatSignalTest_t AutoRepeatSignalTestBuf;

#ifdef CERTTEST_IF
    static stCertTest_IF_t stUserapp_CertTest_IF;
#endif

#ifdef USERAPP_SUPPORT_KARAOKE
	static Karaoke_Effect l_Kara_effect;
#endif

#ifdef USERAPP_SUPPORT_BT
	static BtBackGroundQMSM stBtBackGroupApp;
#endif

QActive *const AO_UserApp = &l_UserApp.super;

typedef enum {
	AUDIO_FORMAT_DISPLAY_NONE,
	AUDIO_FORMAT_DISPLAY_ATMOS,
	AUDIO_FORMAT_DISPLAY_DOLBY_SURROUND,
	AUDIO_FORMAT_DISPLAY_DOLBY_AUDIO,
	AUDIO_FORMAT_DISPLAY_PCM,
	AUDIO_FORMAT_DISPLAY_OTHER,
	AUDIO_FORMAT_DISPLAY_MAX,
}eAUDIO_FORMAT_DISPLAY;

/*..........................................................................*/
int UserAppAmpInit(AmpList *pAmpList);
int UserAppVolumeSet(int vol, AmpList *pstAmpList);
int UserAppRepeatSet(UserApp *const me, int State);
int UserAppShuffleSet(UserApp *const me, int State);
int UserAppEQSet(QActive *me, int State);
int UserAppSurroundSet(QActive *me, int State);
int UserAppMute(UserApp *const me, int iEnable);
void UserApp_IR_Learning_Listener(QActive * const me, void *pParam);
void UserAppAudioInfo_Listener(QActive *const me, void *pParam);
void UserAppCecEvt_Listener(QActive *const me, void *pParam);
void UserAppDspCodecInfo_Listener(QActive * const me, void *pParam);
void UserAppDspCodecInfo_RSR_Listener(QActive * const me,void * pParam);
void UserApp_DimmerSet(UserApp *const me);

/* hierarchical state machine ... */
QState UserApp_initial(UserApp *const me, QEvt const *const e);
QState UserApp_home(UserApp *const me, QEvt const *const e);
QState UserApp_active(UserApp *const me, QEvt const *const e);
QState UserApp_standby(UserApp *const me, QEvt const *const e);
QState UserApp_Upgrade(UserApp *const me, QEvt const *const e);
QState UserApp_IR_Learning(UserApp *const me, QEvt const *const e);
QState UserApp_active_AudioPlayer(UserApp *const me, QEvt const *const e);
QState UserApp_active_Bt(UserApp *const me, QEvt const *const e);
#if (!SUPPORT_VA)
QState UserApp_active_AUX_in(UserApp *const me, QEvt const *const e);
#endif
QState UserApp_active_Raw_Data(UserApp *const me, QEvt const *const e);
QState UserApp_active_Line_in(UserApp *const me, QEvt const *const e);
QState UserApp_active_I2S_in(UserApp *const me, QEvt const *const e);
QState UserApp_active_SPDIF_Coaxial(UserApp *const me, QEvt const *const e);
QState UserApp_active_SPDIF_Optical(UserApp *const me, QEvt const *const e);
#ifdef USERAPP_SUPPORT_HDMI_CEC
QState UserApp_active_ARC(UserApp *const me, QEvt const *const e);
QState UserApp_active_HDMI_TX(UserApp *const me, QEvt const *const e);
#endif

#if 1
QState UserApp_active_ARC(UserApp *const me, QEvt const *const e);
QState UserApp_active_HDMI_RX(UserApp * const me, QEvt const * const e);
#endif
QState UserApp_active_HDMI(UserApp * const me, QEvt const * const e) ;
QState UserApp_AudioPlayer_play(UserApp *const me, QEvt const *const e);
QState UserApp_AudioPlayer_pause(UserApp *const me, QEvt const *const e);
QState UserApp_AudioPlayer_stop(UserApp *const me, QEvt const *const e);
QState UserApp_AudioPlayer_tricky(UserApp *const me, QEvt const *const e);
QState UserApp_Upgrade_Work(UserApp *const me, QEvt const *const e);
QState UserApp_Upgrade_Err(UserApp *const me, QEvt const *const e);
//QState UserApp_hint(UserApp *const me, QEvt const *const e) ;
int UartApp_SetKey_Sig(void *  sender,int sig, int para);
void UserApp_PopBackUp_ShowSource(UserApp *me);

QState UserApp_PowerOn(UserApp *const me, QEvt const *const e);
QState UserApp_PowerOff(UserApp *const me, QEvt const *const e);
#ifdef SYNCCOM_WIRELESS
int UserApp_WIRELESS_VolumeUp(UserApp *const me);
int UserApp_WIRELESS_VolumeDown(UserApp *const me);
#endif
void UserAppDspCodecInfoFromDspC_Listener(QActive * const me, void *pParam);
UINT8 *pbList = NULL;

typedef QState (*UserappState_t)(UserApp *const me, QEvt const *const e);

//SUNPLUS support all the sources as below
// DO NOT remove any member of it
//if you want to adjust the order of sources or remove some of them, adjust the aUserSrc_tbl in userapp.c
UserappState_t UserappStateTab[eApUserAppSrc_Num] =
{
#ifndef BUILT4ISP2
	UserApp_active_AudioPlayer,

	UserApp_active_AudioPlayer,
#endif

#if (!SUPPORT_VA)
	UserApp_active_AUX_in,
#endif

	UserApp_active_Line_in,

	UserApp_active_I2S_in,

	UserApp_active_SPDIF_Optical,

	UserApp_active_SPDIF_Coaxial,

	UserApp_active_ARC,
#if 1//def USERAPP_SUPPORT_REPEATER
    UserApp_active_HDMI_RX,

    UserApp_active_HDMI_RX,

    UserApp_active_HDMI_RX,
#else
	NULL,

	NULL,

	NULL,
#endif

#ifdef USERAPP_SUPPORT_BT
	UserApp_active_Bt,
#else
	NULL,
#endif

#ifdef USERAPP_SUPPORT_HDMI_TX
	UserApp_active_HDMI_TX,
#endif

};

#if 0
void Test_settingmanager(void)
{
	daniel_G_printf("Test_settingmanager in!!!!!!!!\n");

	int i = 1;
	int j = 0;

	static int count = 0;

	CUSTINFO_T g_TEST;

	UINT8 *pTestWriteBuf = NULL;

	SettingManagerCustomerInfoGet(&g_TEST);

	daniel_G_printf("g_TEST.dBytePerSector : %d\n", g_TEST.dBytePerSector);
	daniel_G_printf("g_TEST.dTotalSector : %d\n", g_TEST.dTotalSector);
	daniel_G_printf("g_TEST.dTotalSize : %d\n", g_TEST.dTotalSize);

	daniel_G_printf("count : %d\n", count);

	daniel_G_printf("count % 4 : %d\n", count % 4);

	pTestWriteBuf = (UINT8 *) malloc(4096 * (sizeof(UINT8)));
	memset(pTestWriteBuf, 0x11 * (count % 4), (4096 * (sizeof(UINT8))));

	//ap_printf("pTestWriteBuf : 0x%x\n",pTestWriteBuf);
	//ap_printf("pTestWriteBuf+1 : 0x%x\n",pTestWriteBuf+1);
	//ap_printf("*pTestWriteBuf : 0x%x\n",*pTestWriteBuf);
	//ap_printf("*(pTestWriteBuf+1) : 0x%x\n",*(pTestWriteBuf+1));

	SettingManagerCustomerErase(count % 4);

	if ((count % 4) == 0)
	{
		for (i = 0 ; i < 40 ; i++)
		{
			daniel_G_printf("Write Byte Start : %d\n", (4096 * 0) + (100 * i));
			SettingManagerCustomerWrite((4096 * 0) + (100 * i), pTestWriteBuf, (100 * (sizeof(UINT8))));
		}
	}
	else if ((count % 4) == 1)
	{
		for (i = 0 ; i < 40 ; i++)
		{
			daniel_G_printf("Write Byte Start : %d\n", (4096 * 1) + (100 * i));
			SettingManagerCustomerWrite((4096 * 1) + (100 * i), pTestWriteBuf, (100 * (sizeof(UINT8))));
		}
	}
	else if ((count % 4) == 2)
	{
		for (i = 0 ; i < 40 ; i++)
		{
			daniel_G_printf("Write Byte Start : %d\n", (4096 * 2) + (100 * i));
			SettingManagerCustomerWrite((4096 * 2) + (100 * i), pTestWriteBuf, (100 * (sizeof(UINT8))));
		}
	}
	else if ((count % 4) == 3)
	{
		for (i = 0 ; i < 40 ; i++)
		{
			daniel_G_printf("Write Byte Start : %d\n", (4096 * 3) + (100 * i));
			SettingManagerCustomerWrite((4096 * 3) + (100 * i), pTestWriteBuf, (100 * (sizeof(UINT8))));
		}
	}
	else
	{
		daniel_G_printf("count error\n");
	}

	for (j = 0 ; j < 4096 ; j++)
	{
		UINT8 TestReadBuf = 0;

		ap_printf("Before : Byte : %d , TestReadBuf : 0x%x\n", (4096 * (count % 4)) + j, TestReadBuf);

		SettingManagerCustomerRead((4096 * (count % 4)) + j, &TestReadBuf, 1);

		ap_printf("After : Byte : %d , TestReadBuf : 0x%x\n", (4096 * (count % 4)) + j, TestReadBuf);

	}

	free(pTestWriteBuf);

	daniel_G_printf("sector: %d test over !!!\n", (count % 4));

	count ++;


}
#endif




/******************************************************************************************/
/**
 * \fn			void AUDIO_MUTE_SET(void)
 *
 * \brief
 *
 * \param
 *
 * \return
 *
 *
 ******************************************************************************************/

void AUDIO_MUTE_SET(int *audio_mute_pinnum)
{
	int sdPinnum;
	int sdRet;
	sdPinnum = Pinmux_ModulePin_Find(AUDIO_MUTE, AUDIO_MUTE_CTL);
	ap_printf("sdPinnum = %d\n", sdPinnum);
	sdRet = Pinmux_GPIO_Alloc(sdPinnum, IO_GPIO, IO_RISC);
	if (sdRet == 0 )
	{
		GPIO_OE_Set(sdPinnum, IO_OUTPUT);
		GPIO_Output_Write(sdPinnum, IO_LOW);
		*audio_mute_pinnum = sdPinnum;
		ap_printf("MUTE AUDIO SET PASS ,  audio_mute_pinnum = %d\n", *audio_mute_pinnum);
	}
	else
	{
		ap_printf_E("MUTE AUDIO SET fail \n");
	}
}


/******************************************************************************************/
/**
 * \fn			QActive *UserApp_get(void)
 *
 * \brief		get receiver handler
 *
 * \param
 *
 * \return
 *
 *
 ******************************************************************************************/
QActive *UserApp_get(void)
{
	return AO_UserApp;
}


/******************************************************************************************/
/**
 * \fn			void UserApp_ctor(stUserAppInit_t stUserAppInit)
 *
 * \brief		initial the userapp AO paramater
 *
 * \param		stUserAppInit_t stUserAppInit
 *
 * \return
 *
 *
 ******************************************************************************************/
//---{{{ Merged from SPA300_TPV_20210205
enum
{
	DSP_HEADROOM_GAIN = 0,
	DSP_INV_HEADRO0M_GAIN,	
	DSP_MASTER_VOL,
};

enum
{
	DSP_CH_L,
	DSP_CH_R,
	DSP_CH_LS,
	DSP_CH_RS,
	DSP_CH_C,
	DSP_CH_SUB,
	DSP_CH_LRS,
	DSP_CH_RRS,
	DSP_CH_LTF,
	DSP_CH_RTF,
	DSP_CH_LTR,
	DSP_CH_RTR,
	DSP_CH_LW,
	DSP_CH_RW,
	DSP_CH_LTM,
	DSP_CH_RTM,
};


enum
{
	CMD_FILTER_TYPE,
	CMD_FREQ,
	CMD_GAIN,
	CMD_Q,
	CMD_PHASE,
	CMD_SLOPE,
	CMD_METHOD,
	CMD_ENABLE = 100,
};

#ifdef SUPPORT_DSP_VOL
const float dsp_master_vol[USERAPP_VOL_MAX+1] = 
{
	-99,

	-50, -46, -42, -39, -37, -35, -33, -31, -29, -27,
	-25, -23, -21, -19, -17, -15, -14, -13, -12, -11,
	-10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 
	0,
};

int UserApp_SetMasterVol(UserApp* const me, BYTE vol)
{
	if(vol >= (sizeof(dsp_master_vol)/sizeof(float)))
		return -1;
	
	AudDspService_Set_MasterVol(&me->super, 2, dsp_master_vol[vol]);
	return 0;
}

const float dsp_subwoofer_vol[] = 
{
	-9, -8.5, -8, -7.5, -7, -6.5, -6, -5.5, -5, 
	-4.5, -4, -3.5, -3, -2.5, -2, -1.5,  -1, -0.5, 0
};


int UserApp_SetSubwooferVol(UserApp* const me, BYTE vol)
{
	if(vol >= (sizeof(dsp_subwoofer_vol)/sizeof(float)))
		return -1;
    ap_printf("-----bass set dsp_subwoofer_vol[%d] = %f\n ",vol, dsp_subwoofer_vol[vol]);
	AudDspService_Set_ChannelVol(&me->super, 0, DSP_CH_SUB, dsp_subwoofer_vol[vol]);
	return 0;
}


const float dsp_Center_vol[] = 
{
	-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0
};


int UserApp_SetCenterVol(UserApp* const me, BYTE vol)
{
	if(vol >= (sizeof(dsp_Center_vol)/sizeof(float)))
		return -1;

	AudDspService_Set_ChannelVol(&me->super, 0, DSP_CH_C, dsp_Center_vol[vol]);
	
	ap_printf("[%s] : %d \n", __FUNCTION__, vol);
	return 0;
}

const float dsp_LR_vol[] = 
{
	-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0
};

int UserApp_SetLRVol(UserApp* const me, BYTE vol)
{
	if(vol >= (sizeof(dsp_LR_vol)/sizeof(float)))
		return -1;


	AudDspService_Set_ChannelVol(&me->super, 0, DSP_CH_L, dsp_LR_vol[vol]);
	AudDspService_Set_ChannelVol(&me->super, 0, DSP_CH_R, dsp_LR_vol[vol]);
	
	return 0;
}

const float dsp_FTLR_vol[] = 
{
	-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0
};

int UserApp_SetFTLRVol(UserApp* const me, BYTE vol)
{
	if(vol >= (sizeof(dsp_FTLR_vol)/sizeof(float)))
		return -1;

	/*调整前左顶以及前右顶的通道音量*/
	AudDspService_Set_ChannelVol(&me->super, 0, DSP_CH_LTF, dsp_FTLR_vol[vol]);
	AudDspService_Set_ChannelVol(&me->super, 0, DSP_CH_RTF, dsp_FTLR_vol[vol]);
	
	return 0;
}

const float dsp_FSLR_vol[] = 
{
	-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0
};

int UserApp_SetFSLRVol(UserApp* const me, BYTE vol)
{
	if(vol >= (sizeof(dsp_FSLR_vol)/sizeof(float)))
		return -1;

	/*调整前左环绕以及前右环绕的通道音量*/
	AudDspService_Set_ChannelVol(&me->super, 0, DSP_CH_LS, dsp_FSLR_vol[vol]);
	AudDspService_Set_ChannelVol(&me->super, 0, DSP_CH_RS, dsp_FSLR_vol[vol]);
	
	return 0;
}

const float dsp_RTLR_vol[] = 
{
	-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0
};

int UserApp_SetRTLRVol(UserApp* const me, BYTE vol)
{
	if(vol >= (sizeof(dsp_RTLR_vol)/sizeof(float)))
		return -1;

	/*调整后左顶以及后右顶的通道音量*/
	AudDspService_Set_ChannelVol(&me->super, 0, DSP_CH_LTR, dsp_RTLR_vol[vol]);
	AudDspService_Set_ChannelVol(&me->super, 0, DSP_CH_RTR, dsp_RTLR_vol[vol]);
	
	return 0;
}
const float dsp_LSRS_vol[] = 
{
	-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0
};

int UserApp_SetLsRsVol(UserApp* const me, BYTE vol)
{
	if(vol >= (sizeof(dsp_LSRS_vol)/sizeof(float)))
		return -1;


	AudDspService_Set_ChannelVol(&me->super, 0, DSP_CH_LS, dsp_LSRS_vol[vol]);
	AudDspService_Set_ChannelVol(&me->super, 0, DSP_CH_RS, dsp_LSRS_vol[vol]);
	
	return 0;
}



const float dsp_Bass_vol[] = 
{
	-18,-17,-16,-15,-14,-13,-12,-11, -10,
	-9, -8, -7, -6, -5 -4, -3, -2, -1, 0
};


int UserApp_SetBass(UserApp* const me, BYTE vol)
{
	if(vol >= (sizeof(dsp_Bass_vol)/sizeof(float)))
		return -1;

	AudDspService_Set_Filter(&me->super, 1, CMD_GAIN, DSP_CH_L, 1, dsp_Bass_vol[vol]);
	AudDspService_Set_Filter(&me->super, 1, CMD_GAIN, DSP_CH_R, 1, dsp_Bass_vol[vol]);
	AudDspService_Set_Filter(&me->super, 1, CMD_GAIN, DSP_CH_SUB, 1, dsp_Bass_vol[vol]);
	//AudDspService_Set_Filter(&me->super, 1, DSP_CH_RS, 1, dsp_Bass_vol[vol]);
	ap_printf("[%s] : %d \n", __FUNCTION__, vol);
	return 0;
}

const float dsp_Treble_vol[] = 
{
    -12,-11, -10,-9, -8, -7, -6, -5,-4, -3, -2, -1, 0,
};

int UserApp_SetTreble(UserApp* const me, BYTE vol)
{
	if(vol >= (sizeof(dsp_Treble_vol)/sizeof(float)))
		return -1;

	AudDspService_Set_Filter(&me->super, 1, CMD_GAIN, DSP_CH_L, 0, dsp_Treble_vol[vol]);
	AudDspService_Set_Filter(&me->super, 1, CMD_GAIN, DSP_CH_R, 0, dsp_Treble_vol[vol]);
	AudDspService_Set_Filter(&me->super, 1, CMD_GAIN, DSP_CH_C, 0, dsp_Treble_vol[vol]);
	ap_printf("------dsp_Treble_vol[%d] = %f\n ",vol,dsp_Treble_vol[vol]);
	ap_printf("[%s] : %d \n", __FUNCTION__, vol);
	return 0;
}

#endif
//--}}}

void UserApp_ctor(void)
{
	UserApp *const me = &l_UserApp;
	UserSetting_T UserSetting;
	CFG_KEY_T *pstKEY_CTRL_Cfg = GET_SDK_CFG(CFG_KEY_T);
	CFG_IR_T *pstIR_Cfg = GET_SDK_CFG(CFG_IR_T);
	CFG_BT_T *pstBT_Cfg = GET_SDK_CFG(CFG_BT_T);
	CFG_BTMAIN_T *pBtSdkCfg = &(pstBT_Cfg->BtSdkCfg.BtMainCfg);
	CFG_HINT_T *pstHINT_Cfg = GET_SDK_CFG(CFG_HINT_T);

	ap_printf("[%s] : \n", __FUNCTION__);

	vfd_init();

	UserSetting_Init(&UserSetting);

#ifdef Q70_DEMO
	UserSetting.Volume = 30;
	UserSetting.WIRELESS_Volume = 30;
#endif
	me->AUD_DPS_UserGain = UserSetting.AUD_DPS_UserGain; //AUD_DPS_UPDATE_USRGAIN  #dps

	QActive_ctor(&me->super, Q_STATE_CAST(&UserApp_initial));

	me->pKeyTable = aIRKeyCodeTable;

#ifdef MCU_IF_RSR
	MCU_IF_RSR_ctor(&me->stMCU_IF_RSR, &me->super);
#endif

#ifdef CERTTEST_IF

    me->stCertTest_IF = &stUserapp_CertTest_IF;
    me->stCertTest_IF->superAO = &me->super;
    CertTest_IF_ctor(me->stCertTest_IF);/* stCertTest_IF orthogonal component ctor */

#endif


#ifdef SUPPORT_HINT_SOUND
	me->pstHintSound = &HINT_TONE;

	ap_printf("me->pstHintSound->DATA_TYPE : %d\n", me->pstHintSound->DATA_TYPE);
	ap_printf("me->pstHintSound->SIZE : %d\n", me->pstHintSound->SIZE);
	ap_printf("me->pstHintSound->SAMPLE_RATE : %d\n", me->pstHintSound->SAMPLE_RATE);
	ap_printf("me->pstHintSound->MUSIC_TARGET_GAIN : 0x%x\n", me->pstHintSound->MUSIC_TARGET_GAIN);
	ap_printf("me->pstHintSound->HINT_RAMP_UP_VELOCITY : %d\n", me->pstHintSound->HINT_RAMP_UP_VELOCITY);
	ap_printf("me->pstHintSound->HINT_DECLINE_VELOCITY : %d\n", me->pstHintSound->HINT_DECLINE_VELOCITY);
	ap_printf("me->pstHintSound->input_samplerate : %d\n", me->pstHintSound->SAMPLE_RATE);

#endif

	me->iSrc = UserSetting.Src;

	if (pBtSdkCfg->SinkMode == HEADPHONE)
	{
		me->vol = UserSetting.EarVolume;
	}
	else
	{
		me->vol = UserSetting.Volume;
	}

	if(me->vol >= USERAPP_VOL_MAX)
		me->vol = 20;

//	me->Wireless_vol = UserSetting.WIRELESS_Volume;
	me->EQMode = UserSetting.EQ_State;
	
	//--{{ Add by heaven to init vol/dim value
	me->menu_status = eSoundMax;
	me->Bass_vol = UserSetting.Bass_vol;
	me->Treble_vol = UserSetting.Treble_vol;
	me->Top_vol = UserSetting.Top_vol;
	me->Centel_vol = UserSetting.Center_vol;
	me->Ls_vol = UserSetting.LsRs_vol;
	me->LR_vol = UserSetting.LR_vol;
	me->FTLR_vol = UserSetting.FTLR_vol;
	me->FSLR_vol = UserSetting.FSLR_vol;
	me->RTLR_vol = UserSetting.RTLR_vol;
	//me->Rs_vol = UserSetting.LsRs_vol;
	me->DIMMER = UserSetting.dimmer;
	me->Surround_3D = UserSetting.Surround_3D;
	me->iRepeat = UserSetting.Repeat;
	me->iShuffle = UserSetting.Shuffle;

    me->dXef_Handle = 0; //zehai.liu todo

	me->UpgradeTime = ((UPGRADE_LONG_PRESS_TIME / pstKEY_CTRL_Cfg->iPollingTime) - pstKEY_CTRL_Cfg->iLongPressTickNum) / pstKEY_CTRL_Cfg->iKeyTickFreq;
	me->UpgradeCnt = 0;
	me->num = 0;

	me->showErr = 0;
	me->opType = 0;
#ifdef CONFIG_USB_LOGO_TEST
	me->playerSource = 0; //USB device list number
#endif

	me->EP_Src_id = 0;  // for 8137 swicth Sorce  :  1 => SPIDIF ;  2 => I2S_1_lane ;  3 => I2S_4_lane

	me->SP_UPDATE_bin = NULL;
#ifdef BUILT4ISP2
	me->ISPBOOOT_bin = 0;
#endif
	me->MonkeyTest_Enable = 0;
	me->mute = eMute_off;
	me->TV_Remote_Ready_State = pstIR_Cfg->TV_Remote_Ready_Enable;
	me->TV_Remote_Ready_cnt = TV_REMOTE_READY_TICK;

#if (SUPPORT_VA)
	me->AuxVA_Status = UserSetting.AuxVA_Status;
	me->BtVA_Status = UserSetting.BtVA_Status;
	ap_printf("AuxVA_Status:%d, BtVA_Status:%d\n", me->AuxVA_Status, me->BtVA_Status);
#endif

#if (SUPPORT_DEMOMODE)
	me->demoState =0;
	me->demoBackSrc = 0;
#endif


#ifndef SDK_RELEASE
	me->Memory_Check = 0;
	QTimeEvt_ctorX(&me->timeSwitchSrcEvt, &me->super, CUSTOM_TEST_AUTOSWITCHSRC_SIG, 0U);
	QTimeEvt_ctorX(&me->timePowerOffEvt, &me->super, POWER_RELEASE_SIG, 0U);
#endif

#ifdef USERAPP_SUPPORT_KARAOKE
	me->Kara_effect = &l_Kara_effect;
	Karaoke_ctor(me->Kara_effect); /* karaoke  orthogonal component ctor */
#endif

#ifdef USERAPP_SUPPORT_BT
	me->pBtBakcGroundApp = &stBtBackGroupApp;
	me->pBtBakcGroundApp->aUserSrc_tbl = gUserSrc_tbl;
	me->pBtBakcGroundApp->SrcEnd = USER_SRC_END;
#ifndef BUILT4ISP2
	//BtBackGroup_Ctor(me->pBtBakcGroundApp, AO_UserApp);
	BtBackGroup_Ctor(&me->pBtBakcGroundApp,AO_UserApp);
#endif
#endif

	QTimeEvt_ctorX(&me->timeEvt, &me->super, SHOW_PLAYING_TIME, 0U);
	QTimeEvt_ctorX(&me->timeEvt_Watchdog, &me->super, WATCHDOG_TICK_SIG, 0U);

#ifdef CONFIG_USB_LOGO_TEST
	QTimeEvt_ctorX(&me->timeDevPlay, &me->super, TIME_DEV_PLAY_SIG, 0U);
#endif
	me->ARC_DSP_State = 0;

	me->singlePlayMode = pstHINT_Cfg->singlePlayMode;
	me->xeffBufSize = pstHINT_Cfg->xeffBufSize;
#if (POWER_ON_SRC_HINT)
	me->PwrOnFlag = 0;
#else
	me->PwrOnFlag = 1;
#endif
	QTimeEvt_ctorX(&me->timeSrcHintEvt, &me->super, SRC_HINT_TIME_SIG, 0U);
	QTimeEvt_ctorX(&me->timeHintDelayEvt, &me->super, HINT_DELAY_TIME_SIG, 0U);
	QTimeEvt_ctorX(&me->timeHintIntervalEvt, &me->super, HINT_INTERVAL_TIME_SIG, 0U);
	QTimeEvt_ctorX(&me->timeDisplayOnce, &me->super, DISPLAY_ONCE_BACK_SIG, 0U);
	QTimeEvt_ctorX(&me->timePassword, &me->super, PASSWORD_TIMEOUT_SIG, 0U);
	QTimeEvt_ctorX(&me->timeSrcDelay, &me->super, SRC_DELAY_SIG, 0U);

#ifdef ATMOS_CERT
	me->atmos_opt.dap_enable = 0;
	me->atmos_opt.direct = 0;
	me->atmos_opt.tuning_mode = 0;
	me->atmos_opt.config_file = 0;
	me->atmos_opt.config_virt_enable = 0;
	me->atmos_opt.virt_mode = 0;
	me->atmos_opt.virt_frontangle = 0;
	me->atmos_opt.virt_surrangle = 0;
	me->atmos_opt.virt_heightangle = 0;
	me->atmos_opt.virt_rearsurrangle = 0;

	me->atmos_opt.b_postproc= 1;
	me->atmos_opt.profile_id = 0;
	me->atmos_opt.b_vlamp= 1;
	me->atmos_opt.b_enable_upmixer= 1;
	me->atmos_opt.b_center_spread= 0;
	me->atmos_opt.b_height_filter= 0;
	me->atmos_opt.b_enable_loudness_mgmt= 1;
	me->atmos_opt.drc_mode = 0;
	me->atmos_opt.drc_cut = 100;
	me->atmos_opt.drc_boost = 100;
#endif
}


/******************************************************************************************/
/**
 * \fn          void UserApp_HPDState_Listener(QActive * const me, void *pParam)
 *
 * \brief      function for listener regist, system will run this function when HDMI plug in or out
 *
 * \param   QActive * const me:(Input) AO handler ,\n
 *               void *pParam : HDMI HPD pin state info structure pointer (structure type: stHDMI_HPDInfo_t)
 *
 * \return	    none.
 *
 *
 ******************************************************************************************/
void UserApp_HPDState_Listener(QActive *const me, void *pParam)
{
	stHDMI_HPDInfo_t *stHDMI_HPD = (stHDMI_HPDInfo_t * )pParam;

	((UserApp *)me)->HPD_State = stHDMI_HPD->bState;

	ap_printf("[%s] HPD state: %d\n", __FUNCTION__, stHDMI_HPD->bState);
}
#if 0
/******************************************************************************************/
/**
 * \fn			void UserApp_VolAct(QActive * const me, void *pParam)
 *
 * \brief		function for listener regist, system will run this function when USB/SD plug in or out,
 *              you shouldn't handle this event any more in you own state machine function, if you do, system would not run it,
 *			you can manage it yourself
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              pParam : (Input) event from USB driver, you can get VOLUMEPARAM with it\n
 *
 * \return	    none.
 *
 *
 ******************************************************************************************/
void UserApp_VolAct(QActive *const me, void *pParam)
{
	(void)me;  /* suppress the "unused parameter" compiler warning */

	VOLUMEPARAM *pstVolParam = (VOLUMEPARAM *)pParam;
	if (pstVolParam->eAction == VOLUME_ADD)
	{
		ap_printf("volume %s insert, should mount to %s. \n", pstVolParam->psbDevName, pstVolParam->psbDiskName);
#ifdef SUPPORT_AP
		mount(pstVolParam->psbDevName, pstVolParam->psbDiskName, "fatfs");
#endif
	}
	else
	{
		ap_printf("volume %s plug out, umount %s. \n", pstVolParam->psbDevName, pstVolParam->psbDiskName);
#ifdef SUPPORT_AP
		umount(pstVolParam->psbDiskName);
#endif
	}
}
#endif
/******************************************************************************************/
/**
 * \fn			QState UserApp_initial(UserApp * const me, QEvt const * const e)
 *
 * \brief		initial the userapp AO paramater
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e : none\n
 *
 * \return	    \e  trans to UserApp_standby state \n
 *
 *
 ******************************************************************************************/


uint32_t UserApp_Standby_Polling(uint8_t src)
{
	uint32_t res = 0;
	UserApp *const me = &l_UserApp;
	static BYTE flag = 1;
	//stIR_Info_t cust_ir;

	src = src;
	
	if(flag)
	{	
		flag = 0;
		GPIO_Output_Write(33, IO_LOW);
		TouchKey_StandbyInit();
		hdmi_cfg(HDMI_SYS_STANDBY, CMD_MODE_BT, me->vol, USERAPP_VOL_MAX);
		ap_printf(" standby init .....\n ");
	}

	if(TouchKey_Polling() == 1)
	{
		ap_printf(" Touch key power on\n ");
		res = 1;
	}
/*
	IOSrv_getKeyCode_MultiIR(&cust_ir);

	//ap_printf(" cust 0x%x, code 0x%x\n ", cust_ir.CustomerCode, cust_ir.KeyCode);
	
	if(cust_ir.CustomerCode == CUST_CUSTOMER_CODE)
	{
		if(cust_ir.KeyCode == CUST_POWER_KEY_CODE )
		{
			ap_printf(" standby IR src wakeup\n ");
			res = 1;
		}
	}
*/
	if(hdmi_get_status(HDMI_SYS_STANDBY) == 1)
		res = 1;

	return res;
}


QState UserApp_initial(UserApp *const me, QEvt const *const e)
{
	(void)e; /* avoid compiler warning about unused parameter */
	ap_printf("[%s] : \n", __FUNCTION__);

	me->hKeyBuf = &circBuf;
	me->pKeyCode2SigTable = aKeyCode2SigTable;

	me->hAmpList = &stAmpList;
	me->stAutoSrcTest = &AutoSrcTestBuf;
	me->stAutoRepeatSignalTest = &AutoRepeatSignalTestBuf;
	me->stAutoSrcTest->burn_log_enable = 1;
	me->aUserSrc_tbl = gUserSrc_tbl;
	me->eARC_SRC = eUser_ARCSRC;


#ifdef CERTTEST_IF

    QMSM_INIT(&me->stCertTest_IF->super, (QEvt *)0);/* take the top-most initial transition in the component */

#endif
#ifdef MCU_IF_RSR
	QMSM_INIT(&me->stMCU_IF_RSR.super, (QEvt *)0);
#endif

	wireless_init();
	me->wireless_mute = eMute_on;
	me->wireless_mute_cnt = 0;
	QTimeEvt_ctorX(&me->timerWireless, &me->super, WIRELESS_TIMER_SIG, 0U);
	QTimeEvt_ctorX(&me->timerSysDetect, &me->super, SYS_DETECT_TIMER_SIG, 0U);
	UserAppAmpInit(me->hAmpList);

	UserAppVolumeSet(me->vol, me->hAmpList);

	//UserAppRepeatSet(me->iRepeat);//we cannot set repeat and shuffle here because the AO which provide this service doesnt ready yet at this time
	//UserAppShuffleSet(me->iShuffle);

	QActive_subscribe(&me->super, IOP_KEY_SIG);
	QActive_subscribe(&me->super, MFP_STOP_OVER_SIG);
	QActive_subscribe(&me->super, SYSTEM_UPGRADE_PROGRESS_SIG);
	QActive_subscribe(&me->super, SYSTEM_UPGRADE_FAIL_SIG);
	QActive_subscribe(&me->super, SYSTEM_UPGRADE_END_SIG);
	QActive_subscribe(&me->super, IOP_MUTE_REQ_SIG);
	QActive_subscribe(&me->super, ODSP_RESET_START_SIG);
	QActive_subscribe(&me->super, ODSP_RESET_END_SIG);
#ifndef SDK_RELEASE
	QActive_subscribe(&me->super, AUTOSRC_TEST_OPEN_SIG); //for auto test
	QActive_subscribe(&me->super, MONKEY_TEST_OPEN_SIG); //for monkey test
	QActive_subscribe(&me->super, BURN_TEST_CLOSE_SIG); //for burn test
	QActive_subscribe(&me->super, SYSTEM_MEMORY_CHECK_OPEN_SIG); //for check system stack & heap memory
	QActive_subscribe(&me->super, AUTO_REPEAT_SIGNAL_TEST_OPEN_SIG); //for auto test the signal which user input
#endif

#ifdef USERAPP_SUPPORT_KARAOKE
	QMSM_INIT(&me->Kara_effect->super, (QEvt *)0);/* take the top-most initial transition in the component */ //rongrong
#endif

#ifdef SYNCCOM_WIRELESS
	EV01SE_Init();
	EV01SE_Pair();
#endif

 	CFG_AUD_T *pstAUD_Cfg = GET_SDK_CFG(CFG_AUD_T);

	if(pstAUD_Cfg->AUDIO_FLOW == AUDFLOW_LC)
	{
	    AudDspService_RegAudioInfoBroadcastListener(&me->super,UserAppDspCodecInfoFromDspC_Listener);
	}
	else
	{

        AudDspService_RegAudioInfoBroadcastListener(&me->super,UserAppDspCodecInfo_Listener); //chihhao: to show audio format in VFD
	}


	/*IOSrv_RegCecEvtListener(&me->super, UserAppCecEvt_Listener);
	SystemService_RegAudioInfoBroadcastListener(&me->super, UserAppAudioInfo_Listener);

	int WakeupInfo = 0;
	WakeupInfo = PowerService_GetWakeupInfo();
	ap_printf( "pWakeupInfo:0x%x\n", WakeupInfo);
	if (WakeupInfo == eWakeupInfo_GPIO)
	{
		ap_printf("wakeup from power key\n");
	}

	AUDIO_MUTE_SET(&(me->audio_mute_pinnum));*///jeff system service ready to open
	return Q_TRAN(&UserApp_home);
}
/******************************************************************************************/
/**
 * \fn			void UserApp_Start(void)
 *
 * \brief		initial the userapp AO paramater
 *
 * \param
 *
 * \return	    \e none \n
 *
 *
 ******************************************************************************************/
void UserApp_Start(void)
{
	static QEvt const *pstUserAppQueueSto[20];

	if ((CUSTOMER_PRIVATE_SIG_MAX > CUSTOMER_PRIVATE_SIG_END)
			|| (CUSTOMER_PUB_SIG_MAX > CUSTOMER_PUB_SIG_END)
	   )
	{
		while (1)
		{
			ap_printf_E("too many customer signals !!\nCUSTOMER_PRIVATE_SIG_MAX: %d, CUSTOMER_PUB_SIG_MAX: %d\n", CUSTOMER_PRIVATE_SIG_MAX, CUSTOMER_PUB_SIG_MAX);
		}
	}

	UserApp_ctor();

	QACTIVE_START(AO_UserApp,           /*!< AO to start */
				  TSK_USERAPP_PRIO, 			/*!< QP priority of the AO */
				  pstUserAppQueueSto,      /*!< event queue storage */
				  Q_DIM(pstUserAppQueueSto), /*!< queue length [events] */
				  TSK_USERAPP_NAME,             /* AO name */
				  TSK_USERAPP_STACK_DEPTH,      /*!< size of the stack [bytes] */
				  (QEvt *)0);             /*!< initialization event */
}
AO_INIT(UserApp_Start);

/******************************************************************************************/
/**
 * \fn			int UserAppNum2Str(UserApp * const me, int Str_ID)
 *
 * \brief		Api for user to translate number to string
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              int  num: number  \n
 *
 * \return	    \e  0 on success. \n
 *
 *
 ******************************************************************************************/
//int UserAppNum2Str(UserApp * const me ,int num)
int UserAppNum2Str(char *strBuf, int num)
{

	if ((strBuf == NULL)
			|| (num > VFD_MAX_NUM)
	   )
	{
		return -1;
	}

	char NumBuf1[14] = {0};
	char NumBuf2[14] = {0};
	int i, j;
	int k = 0;
	int remainder;
	for (i = 0 ; i < 14 ; i++)
	{
		remainder = num % 10 ;
		NumBuf1[i] = '0' + (remainder - 0);
		num = num / 10 ;
		if (num == 0)
		{
			for (j = (i + 1) ; j < 14 ; j++)
			{
				NumBuf1[j] = 0x20;
			}
			break;
		}
	}

	for (i = 13 ; i >= 0 ; i--)
	{
		if (NumBuf1[i] != 0x20)
		{
			NumBuf2[k] = NumBuf1[i];
			k++;
		}
	}

	memcpy(strBuf, NumBuf2, strlen(NumBuf2) + 1);

	ap_printf2("strlen(strBuf):%d, %s\n ", strlen(strBuf), strBuf);
	return 0;
}

int UserAppStrToVolDisplay(char *strBuf, int num)
{

	if(num == USERAPP_VOL_MAX)
		sprintf(strBuf, "V MAX");
	else if(num == 0)
		sprintf(strBuf, "V MIN");
	else 
		sprintf(strBuf, "V  %02d", num);

	ap_printf("strBuf %s\n", strBuf);
	
	return 0;
}

/******************************************************************************************/
/**
 * \fn			int UserAppTranSrc(int Src)
 *
 * \brief		set a UserApp Source
 *
 * \param		UINT8 Src : (Input) enum UserAppSrc
 *              UserApp* me \n

 * \return	    \e QState \n
 *
 ******************************************************************************************/
QState UserAppTranSrc(UserApp *const me, UINT8 Src)
{
	QState status = 0;

	if ((UserappStateTab[Src] != NULL)
			&& (Src < eApUserAppSrc_Num)
	   )
	{
		ap_printf("LoadAppSrc:%d\n", Src);
		status = Q_TRAN(UserappStateTab[Src]);
	}
	else
	{
		ap_printf_E("Set SOURCE ERROR!! : %d\n", Src);
		status = Q_HANDLED();
	}

	return status;
}

/******************************************************************************************/
/**
 * \fn			int UserAppDisplay(UserApp * const me, int Str_ID)
 *
 * \brief		Api for user to set vfd display in constant style
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              int Str_ID : source ID \n
 *
 * \return	    \e  0 on success. \n
 *
 *
 ******************************************************************************************/
int UserAppDisplay(UserApp *const me, int Str_ID)
{
#ifdef Q70_DEMO
	LED_Service_Display(&me->super, Str_ID);
	LED_Service_Constant(&me->super);
#endif

	VFD_Service_Display(&me->super, vfd_str[Str_ID]);
	VFD_Service_Align(&me->super, CENTER);
	VFD_Service_Constant(&me->super);
	me->delay_call = NULL;
	return 0;
}


/******************************************************************************************/
/**
 * \fn			int UserAppDisplay_Blink(UserApp * const me, int Str_ID , uint16_t bright_time ,uint16_t dark_time)
 *
 * \brief		Api for user to set vfd display in blink style
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              int Str_ID : source ID \n
 *              uint16_t bright_time : bright time ;  1 = 100ms ; 10 = 1s\n
 *              uint16_t dark_time : dark time ; 1 = 100ms ; 10 = 1s\n
 *
 * \return	    \e  0 on success. \n
 *
 *
 ******************************************************************************************/
int UserAppDisplay_Blink(UserApp *const me, int Str_ID, uint16_t bright_time, uint16_t dark_time)
{
#ifdef Q70_DEMO
	LED_Service_Display(&me->super, Str_ID);
	LED_Service_Constant(&me->super);
#endif

	VFD_Service_Display(&me->super, vfd_str[Str_ID]);
	VFD_Service_Align(&me->super, CENTER);
	VFD_Service_Blink (&me->super, bright_time, dark_time);
	me->delay_call = NULL;
	return 0;
}

int UserAppDisplay_BlinkOnce(UserApp * const me, int Str_ID, int Str_Const_ID, BYTE blink_time,  eShowOncemode_t back_mode, uint16_t once_bright_time ,uint16_t once_dark_time, uint16_t bright_time ,uint16_t dark_time)
{

    	VFD_Service_Display(&me->super ,vfd_str[Str_ID]);
    	VFD_Service_Align(&me->super, CENTER);

	if(Str_Const_ID != STR_NULL)
    		VFD_Service_BlinkOne_Constant(&me->super, vfd_str[Str_Const_ID], blink_time, back_mode, once_bright_time, once_dark_time, bright_time , dark_time);
	else
		VFD_Service_BlinkOne_Constant(&me->super, NULL, back_mode, 0,0,0, 0, 0);
	me->delay_call = NULL;
    	return 0;
}


/******************************************************************************************/
/**
 * \fn			int UserAppDisplay_Scroll(UserApp * const me, int Str_ID)
 *
 * \brief		Api for user to set vfd display in scroll style
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              int Str_ID : source ID \n
 *              eScrollmode_t scroll_mode : set scroll mode
 * \return	    \e  0 on success. \n
 *
 *
 ******************************************************************************************/
int UserAppDisplay_Scroll(UserApp *const me, int Str_ID, eScrollmode_t scroll_mode)
{
	int result;
	ap_printf("UserAppDisplay_Scroll: %s\n", vfd_str[Str_ID]);
	result = VFD_Service_Display(&me->super, vfd_str[Str_ID]);

	VFD_Service_Align(&me->super, CENTER);
	VFD_Service_Scroll(&me->super, scroll_mode);
	me->delay_call = NULL;
	return 0;
}

int UserAppDisplay_ScrollOnce(UserApp * const me, int Str_ID, int Str_Const_ID, eScrollmode_t scroll_mode, eShowOncemode_t back_mode, uint16_t bright_time ,uint16_t dark_time)
{


    	VFD_Service_Display(&me->super ,vfd_str[Str_ID]);
    	VFD_Service_Align(&me->super, CENTER);

	if(Str_Const_ID != STR_NULL)
    		VFD_Service_ScrollOne_Constant(&me->super, vfd_str[Str_Const_ID], scroll_mode, back_mode, bright_time , dark_time);
	else
		VFD_Service_ScrollOne_Constant(&me->super, NULL, scroll_mode, 0, 0, 0);

	me->delay_call = NULL;
    	return 0;
}

int UserAppDisplayString(UserApp *const me, BYTE *str)
{
	VFD_Service_Display(&me->super, str);
	VFD_Service_Align(&me->super, CENTER);
	VFD_Service_Constant(&me->super);
	me->delay_call = NULL;
	return 0;
}


int UserAppDisplayOnce(UserApp *const me, BYTE *str, BYTE time)
{
	VFD_Service_Display(&me->super, str);
	VFD_Service_Align(&me->super, CENTER);
	VFD_Service_Constant(&me->super);
	QTimeEvt_rearm(&me->timeDisplayOnce, TICKS_PER_SEC*time);
	me->delay_call = UserApp_PopBackUp_ShowSource;

	return 0;
}




void UserApp_ScrollBackUp_ShowSource(UserApp * const me, int show_id)
{

	if(me->mute == eMute_on)
	{
		UserAppDisplay_ScrollOnce(me, show_id, STR_MUTE, R2L, BACK_BLINK, 8, 8);
		return;
	}

	if(me->iSrc == APP_SRC_AUX)
	{
		UserAppDisplay_ScrollOnce(me, show_id, STR_AUXIN, R2L, BACK_CONST, 0, 0);
	}
	else if(me->iSrc == APP_SRC_ARC)
	{
		if(me->eARC_SRC == eUser_eARCSRC)
			UserAppDisplay_ScrollOnce(me, show_id, STR_EARC, R2L, BACK_CONST, 0, 0);
		else
			UserAppDisplay_ScrollOnce(me, show_id, STR_ARC, R2L, BACK_CONST, 0, 0);
	}
	else if(me->iSrc == APP_SRC_OPT)
	{
		UserAppDisplay_ScrollOnce(me, show_id, STR_OPTICAL, R2L, BACK_CONST, 0, 0);
	}
	else if(me->iSrc == APP_SRC_HDMI1)
	{
		UserAppDisplay_ScrollOnce(me, show_id, STR_HDMI, R2L, BACK_CONST, 0, 0);
	}
	else if(me->iSrc == APP_SRC_USB)
	{
		if(me->usb_detect_state == USB_NORMAL)
		{
			if(me->pAPInstance) 
			{
				UserAppDisplay_ScrollOnce(me, show_id, STR_USB, R2L, BACK_USER, 0, 0);	
	              }
			else
			{
				UserAppDisplay_ScrollOnce(me, show_id, STR_NOUSB, R2L, BACK_SCROLL, 0, 0);
			}
		}
		else  // in stop state
			UserAppDisplay_ScrollOnce(me, show_id, STR_NOUSB, R2L, BACK_SCROLL, 0, 0);
	}
	else if(me->iSrc == APP_SRC_BT)
	{	

		if(me->bt_connect == 1)
		{
			if(me->bt_play == 0)
				UserAppDisplay_ScrollOnce(me, show_id, STR_BT, R2L, BACK_BLINK, 8, 8);
			else
				UserAppDisplay_ScrollOnce(me, show_id, STR_BT, R2L, BACK_CONST, 0, 0);
		}
		else 
			UserAppDisplay_ScrollOnce(me, show_id, STR_BT_NOBT, R2L, BACK_CONST, 0, 0);
	}
	else
	{

		UserAppDisplay_ScrollOnce(me, show_id, STR_ARC, R2L, BACK_CONST, 0, 0);
	}

}



void UserApp_PopBackUp_ShowSource(UserApp *me)
{
	BYTE vfd_id = STR_USB;
	BYTE strBuf[8] = {0};
	INT64 plTime = 0;

	ap_printf_E(" UserAppDisplay_ShowDeMute \n ");

	switch(me->iSrc)
	{
		case APP_SRC_USB:
			vfd_id = STR_USB;
			break;
		case APP_SRC_AUX:
			vfd_id = STR_AUXIN;
			break;
		case APP_SRC_OPT:
			vfd_id = STR_OPTICAL;
			break;
			
		case APP_SRC_COAX:
			vfd_id = STR_COAXIAL;
			break;
		case APP_SRC_HDMI1:
			vfd_id = STR_HDMI0;
			break;
		case APP_SRC_HDMI2:
			vfd_id = STR_HDMI1;
			break;
		case APP_SRC_BT:
			vfd_id = STR_BT;
			break;
		case APP_SRC_ARC:
			me->eARC_SRC = hdmi_CheckEARC();
            if(me->eARC_SRC == eUser_ARCSRC)
                vfd_id = STR_ARC;
            else
                vfd_id = STR_EARC;
			break;
		
		default:
			return;
	}

	if(me->mute == eMute_on)
	{
		UserAppDisplay_Blink(me, STR_MUTE, 8, 8);
	}
	else if(me->iSrc == APP_SRC_BT)
	{
		if(me->bt_connect == 1)
		{
			if(me->bt_play == 0)
				UserAppDisplay_Blink(me, STR_BT, 8, 8);
			else
				UserAppDisplay(me, STR_BT);
		}
		else 
			UserAppDisplay(me,STR_BT_NOBT);
	}
	else if(me->iSrc == APP_SRC_USB)
	{
		if(me->usb_detect_state == USB_NORMAL)
		{
			if(me->pAPInstance) 
			{
				if(me->usb_play_state == USB_PLAY) // in play state
    	    			{
    	        			MFP_Ctrl(me->pAPInstance, MFP_GET_PLAY_TIME, &plTime);
					VFD_Service_Play_Time(&me->super, plTime/1000);
				}
				else if(me->usb_play_state == USB_PAUSE)// in pause state
				{
					sprintf(strBuf, "%02d-%02d",  (UINT32)(me->usb_playtime/1000/60), (UINT32)(me->usb_playtime/1000%60));				
					VFD_Service_Display(&me->super ,strBuf);
	    				VFD_Service_Align(&me->super, CENTER);
	    				VFD_Service_Blink (&me->super , 10 , 10);
				}
				else {// in stop state
					UserAppDisplay_Scroll(me, STR_NOUSB, R2L);
				}	
	        	}
			else
			{
				UserAppDisplay_Scroll(me, STR_NOUSB, R2L);
			}
		}
		else  // in stop state
		{
			UserAppDisplay_Scroll(me, STR_NOUSB, R2L);
		}
	}
	else if(me->iSrc == APP_SRC_ARC)
	{
		if(me->eARC_SRC == eUser_eARCSRC)
			UserAppDisplay(me, STR_EARC);	
		else
			UserAppDisplay(me, STR_ARC);	
	}
	else
	{
		UserAppDisplay(me, vfd_id);
	}
}


/******************************************************************************************/
/**
 * \fn			int UserAppUpdate(UserApp * const me, int Str_ID)
 *
 * \brief		Api for user to show update percent in constant style
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              int Str_ID : source ID \n
 *
 * \return	    \e  0 on success. \n
 *
 *
 ******************************************************************************************/
int UserAppUpdate(UserApp *const me, int percent)
{
	char strBuf[14];
	if ((percent < 0) || (percent > UPGRADE_PERCENTAGE_MAX))
	{
		ap_printf_E("Update Fail !!!\n");
		return -1 ;
	}
	//UserAppNum2Str(strBuf, percent);

	if(percent < 10)
		sprintf(strBuf, "UD  %d\n", percent);
	else if(percent < 100)
		sprintf(strBuf, "UD %02d\n", percent);
	else
		sprintf(strBuf, "UD%03d\n", percent);
	
	VFD_Service_Display(&me->super, strBuf);
	VFD_Service_Align(&me->super, RIGHT);
	VFD_Service_Constant(&me->super);

	return 0;
}


/******************************************************************************************/
/**
 * \fn			void System_Stdy_Disable_Output_Device(void)
 *
 * \brief		Disable output device
 *
 * \return	    none
 ******************************************************************************************/
void System_Stdy_Disable_Output_Device(void)
{
	/* STOP APP SERVICE ex: file list scan / media player / ... */
	ap_printf("stop app service\n");

	/* DISABLE AUDIO ex: DSP module / VOL / AMP / ... */
	ap_printf("disable audio volume ...\n");

	/* DISABLE BT / USB POWER / CARD POWER ... */
	ap_printf("disable other module\n");
}


int UserAppUpgrade_ISP(void)
{
	ap_printf("UserAppUpgrade_ISP!\n");

	volatile int *psdiISP_Magic = (int * )SRAM_ADDR_ISP_UPGRADE;
	*psdiISP_Magic = ISP_UPGRADE_USB;

	return 0;
}

#if(0)
/******************************************************************************************/
/**
 * \fn			void UserAppUpgradeScanExist(UserApp * const me, int Bin_type)
 *
 * \brief		adjust the null bin
 *
 * \return	    none
 ******************************************************************************************/
void UserAppUpgradeScanExist(UserApp *const me, int Bin_type)
{
	ap_printf("enter [%s]\n", __FUNCTION__);

#ifndef BUILT4ISP2
	if (Bin_type == BIN_TYPE_SP_UPDATE_BIN)
	{
		ap_printf("SP_UPDATE_bin exist!!!\n");
		me->SP_UPDATE_bin = 1;
	}
	else if (Bin_type == BIN_TYPE_ALL_BIN)
	{
		ap_printf("ALL_bin exist!!!\n");
		me->ALL_bin = 1;
	}
	else if (Bin_type == BIN_TYPE_DSP_BIN)
	{
		ap_printf("DSPTOOL_bin exist!!!\n");
		me->DSPTOOL_bin = 1;
	}
#else	/* BUILT4ISP2 */
	if (Bin_type == BIN_TYPE_ISPBOOOT_BIN)
	{
		ap_printf("ISPBOOOT.BIN exist!!!\n");
		me->ISPBOOOT_bin = 1;
	}
#endif
	else
	{
		ap_printf_E(" Bin_type is not correct\n");
	}
	return;
}

/******************************************************************************************/
/**
 * \fn		void UserAppUpgradeScanDone(QActive * const me, void *pParam)
 *
 * \brief		function for callback regist, system will run this function when userApp receive UPGRADE_SCAN_FINISH_SIG,
 *              you shouldn't handle this event any more in you own state machine function, if you do, system would not run if,
 *			you can manage it yourself
 *
 * \param  	me : (Input) the player handler here
 * \param 		pParam : (Input) event from mediaScanner, you can get UPGBINLIST_T with it
 *
 * \return		none.
 *
 * \note		show the first bin name from the bin list
 *
 * \sa          UserApp_Upgrade_ScanBin()
 *
 ******************************************************************************************/
void UserAppUpgradeScanDone(QActive *const me, void *pParam)
{
	ap_printf("enter [%s]\n", __FUNCTION__);
	UPGBINLIST_T *pstBin = (UPGBINLIST_T *)pParam;
	int i;

	if (pstBin->pbList)
	{
		pbList = (UINT8 *)malloc(sizeof(pstBin->pbList));
		if (pbList != NULL)
		{
			memcpy(pbList, pstBin->pbList, sizeof(pstBin->pbList));

			for (i = 0 ; i < BIN_TYPE_MAX ; i++)
			{
				if (pbList[i] == i)
				{
					UserAppUpgradeScanExist((UserApp *)me, i);
				}
				else
				{

				}
			}
		}
		else
		{
			ap_printf_E(" Upgrade malloc fail\n");
		}
	}
	else
	{
		pbList = NULL;
		//UserAppDisplay((UserApp*)me, STR_NOBIN);
		ap_printf("[%s] NO BIN\n", __FUNCTION__);
	}
	ap_printf("exit [%s]\n", __FUNCTION__);
}
#endif

/******************************************************************************************/
/**
 * \fn		void UserAppUpgradeWorkDone(QActive * const me, void *pParam)
 *
 * \brief		function for callback regist, system will run this function when userApp receive UPGRADE_WORK_FINISH_SIG,
 *              you shouldn't handle this event any more in you own state machine function, if you do, system would not run if,
 *			you can manage it yourself
 *
 * \param  	me : (Input) the player handler here
 * \param 		pParam : (Input) event from upgradeservice, you can get UPGRETURN_T with it
 *
 * \return		none.
 *
 * \note		show some UI for upgrade ok
 *
 * \sa          UserApp_Upgrade_ScanBin()
 *
 ******************************************************************************************/
void UserAppUpgradeWorkDone(QActive *const me, void *pParam)
{
	//(void)me;  /*!< suppress the "unused parameter" compiler warning */
	ap_printf("enter [%s][%d]\n", __FUNCTION__, __LINE__);

	UPGRETURN_T *pstreturn = (UPGRETURN_T *)pParam;


	if (pstreturn->sdRen > 0)
	{
		UserAppDisplay((UserApp *)me, STR_ERR);
		if (pstreturn->sdRen == 1)
		{
			ap_printf("Plese upgrade the bin once-[%s][%d] \n", __FUNCTION__, __LINE__);
		}
		else
		{
			ap_printf("Plese TD the rom bin-[%s][%d] \n", __FUNCTION__, __LINE__);
		}
		while (1);
	}
	else
	{
		UserAppDisplay((UserApp *)me, STR_OK);
		ap_printf("AP upgrade OK-[%s][%d] \n", __FUNCTION__, __LINE__);
	}

	ap_printf("after [%s][%d]\n", __FUNCTION__, __LINE__);
}

#if 0
/******************************************************************************************/
/**
 * \fn		void UserAppUpgradePercentage(QActive * const me, void *pParam)
 *
 * \brief		function for callback regist, system will run this function when userApp receive SYSTEM_UPGRADE_PROGRESS_SIG,
 *              you shouldn't handle this event any more in you own state machine function, if you do, system would not run if,
 *			you can manage it yourself
 *
 * \param  	me : (Input) the player handler here
 * \param 		pParam : (Input) event from mediaScanner, you can get UPGRETURN_T with it
 *
 * \return		none.
 *
 * \note		show the upgrade process
 *
 * \sa          MediaScannerService_StartScan()
 *
 ******************************************************************************************/
void UserAppUpgradePercentage(QActive *const me, void *pParam)
{
#if 0
	UPGRADEPRO *e = (UPGRADEPRO *)pParam;
	ap_printf("%s, %d, %d%%\n", __FUNCTION__, __LINE__, e->sdPro);
#else
	UserAppUpgradeEvt *e = Q_NEW(UserAppUpgradeEvt, UPGRADE_SHOW_PERCENTAGE_SIG);

	e->pUPGRETURN_T = (UPGRETURN_T *)pParam;
	e->pstSender = me ;
#ifdef BUILT4ISP2
	ap_printf("%s, %d, %d%%\n", __FUNCTION__, __LINE__, e->pUPGRETURN_T->sdRen);
#endif
	if ((e->pUPGRETURN_T->sdRen < 0) || (e->pUPGRETURN_T->sdRen > UPGRADE_PERCENTAGE_MAX))
	{
		UserAppDisplay((UserApp *)me, STR_ERR);
	}
	else
	{
		QACTIVE_POST(me, (QEvt *)e, e->pstSender);
	}
#endif
}
#endif


int UserAppUpgrade_DisableBtDone(void)
{
	ap_printf("UserAppUpgrade_DisableBtDone\n");

	return 0;
}

/******************************************************************************************/
/**
 * \fn		QState UserApp_Upgrade_Work(UserApp * const me, QEvt const * const e)
 *
 * \brief		HSM function: usb upgrade work UI
 *
 * \param  	me : (Input) the UserApp
 * \param  	e : (Input) the first event UserApp get
 *
 * \return		the value of state machine function return.
 *
 * \sa		qep.h
 *
 ******************************************************************************************/

QState UserApp_Upgrade_Work(UserApp *const me, QEvt const *const e)
{
	(void)me;
	QState status = 0;

#ifdef BUILT4ISP2
	ap_printf("%s, %d, sig: %d\n", __FUNCTION__, __LINE__, (int)e->sig);
#endif

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			ap_printf("[%s]ENTRY; \n", __FUNCTION__);
#ifdef WD_TICK_ENABLE
			iop_wd_disable();
#endif
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			ap_printf("[%s]EXIT; \n", __FUNCTION__);
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			ap_printf("[%s]INIT; \n", __FUNCTION__);

			/* upg close all key */
			CFG_ADC_T *adc_cfg = GET_SDK_CFG(CFG_ADC_T);
			adc_cfg[0].Enable = 0;

			CFG_IR_T *ir_cfg = GET_SDK_CFG(CFG_IR_T);
			ir_cfg->Cfg_Enable = 0;

			CFG_POWER_KEY_T *pwr_cfg = GET_SDK_CFG(CFG_POWER_KEY_T);
			pwr_cfg ->Cfg_Enable = 0;
			
			status = Q_HANDLED();
			break;
		}
		case SYSTEM_UPGRADE_PROGRESS_SIG:
		{
			// ap_printf("%s, %d, %d%%\n", __FUNCTION__, __LINE__, p->sdPro);
			UPGRADEPRO const *p = Q_EVT_CAST(UPGRADEPRO);
			int upgrd_percent = p->getPercent(p->pstSender);
			UserAppUpdate(me, upgrd_percent);
			status = Q_HANDLED();
			break;
		}
		case UPGRADE_SHOW_PERCENTAGE_SIG:
		{
#ifndef BUILT4ISP2
			ap_printf(".\n");
#endif
			UserAppUpgradeEvt *pUserAppUpgradeEvt = ((UserAppUpgradeEvt *)e);
			UserAppUpdate(me, pUserAppUpgradeEvt->pUPGRETURN_T->sdRen);
#ifdef BUILT4ISP2
			if ((pUserAppUpgradeEvt->pUPGRETURN_T->sdRen % 10) == 0) {
				ap_printf("%d%%\n", pUserAppUpgradeEvt->pUPGRETURN_T->sdRen);
			}
#endif
			status = Q_HANDLED();
			break;
		}
		case SYSTEM_UPGRADE_FAIL_SIG:
		{
			status = Q_TRAN(&UserApp_Upgrade_Err);
#ifdef MCU_IF_SP
			QMSM_DISPATCH(&me->stMCU_IF.super, Q_NEW(QEvt, MCUIF_UPGRD_END_SIG));
#endif
			break;
		}
		case SYSTEM_UPGRADE_END_SIG:
		{
#ifdef MCU_IF_SP
			QMSM_DISPATCH(&me->stMCU_IF.super, Q_NEW(QEvt, MCUIF_UPGRD_END_SIG));
#endif
			status = Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&UserApp_Upgrade);
			break;
		}
	}

	return status;
}

QState UserApp_Upgrade_Err(UserApp *const me, QEvt const *const e)
{
	(void)me;
	QState status = 0;

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			ap_printf("[%s]EXIT; \n", __FUNCTION__);
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			ap_printf("[%s]INIT; \n", __FUNCTION__);
			ap_printf_E("there is no correct bin file\n");
			UserAppDisplay(me, STR_ERR);
			status = Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&UserApp_Upgrade);
			break;
		}
	}

	return status;
}

/******************************************************************************************/
/**
 * \fn			QState UserApp_Upgrade(UserApp * const me, QEvt const * const e)
 *
 * \brief		the state that corresponds to USB upgrade mode
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_Upgrade_ScanBin state \n
 *			    \e status : Q_TRAN() trans to other state \n
 *              \e status : Q_SUPER() trans to UserApp_active state \n
 ******************************************************************************************/
QState UserApp_Upgrade(UserApp *const me, QEvt const *const e)
{
	QState status = 0;

#ifdef BUILT4ISP2
	ap_printf("%s, %d, sig: %d\n", __FUNCTION__, __LINE__, (int)e->sig);
#endif
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			ap_printf("[%s]ENTRY; \n", __FUNCTION__);
			UserAppDisplay(me, STR_UPGRADE);
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			ap_printf("[%s]EXIT; \n", __FUNCTION__);
			free(pbList);
			me->SP_UPDATE_bin = NULL;
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			ap_printf("[%s]INIT\n", __FUNCTION__);
#ifndef BUILT4ISP2
#ifdef USERAPP_SUPPORT_BT
			BtPlatformApUpgradeInit(UserAppUpgrade_DisableBtDone);
#endif
#endif
#if 0
			if (UpgradeService_RegGetProlLSR(me, UserAppUpgradePercentage)) {
				ap_printf_E("%s, %d\n", __FUNCTION__, __LINE__);
			}
#endif

			


			QTimeEvt_disarmX(&me->timerSysDetect);
			QTimeEvt_armX(&me->timerSysDetect, TICKS_PER_500MS, 0);

			status = Q_HANDLED();
			break;
		}

		case SYS_DETECT_TIMER_SIG:
		case PLAY_RELEASE_SIG:
		{
			ap_printf("[%s]PLAY; \n", __FUNCTION__);
			char app_bin_file_path[20] = "USB:/";
			strcat(app_bin_file_path, me->SP_UPDATE_bin? me->SP_UPDATE_bin: "spupdate.bin");
			UpgradeFileLoader_Select(me->SP_UPDATE_bin);
			status = UpgradeFileLoader_Scan()? Q_TRAN(&UserApp_Upgrade_Err): Q_TRAN(&UserApp_Upgrade_Work);
			break;
		}
		case ZERO_RELEASE_SIG:
		{
			status = Q_TRAN(&UserApp_active);
			break;
		}
		case ONE_RELEASE_SIG:
		{
			char *upFileName = GET_SDK_CFG(CFG_UPGRADE_T)->psbSpUpdateFileName;
			ap_printf("[%s]: %s; \n", __FUNCTION__, upFileName);
			UserAppDisplay_Scroll(me, STR_SP_UPDATE_BIN, R2L);
			UpgradeFileLoader_Select(upFileName);
			status = Q_HANDLED();
			break;
		}
		case UP_RELEASE_SIG:
		{
			if(!me->SP_UPDATE_bin)
			{
				UpgradeFileLoader_GetFileList();
			}
			me->SP_UPDATE_bin = UpgradeFileLoader_PreFile();
			LOGD("select the upgrade bin file -> %s\n", me->SP_UPDATE_bin);
			status = Q_HANDLED();
			break;
		}
		case DOWN_RELEASE_SIG:
		{
			if(!me->SP_UPDATE_bin)
			{
				UpgradeFileLoader_GetFileList();
			}
			me->SP_UPDATE_bin = UpgradeFileLoader_NextFile();
			LOGD("select the upgrade bin file -> %s\n", me->SP_UPDATE_bin);
			status = Q_HANDLED();
			break;
		}
#ifdef MCU_IF_SP
		case MCU_IF_USER_BAUDSET0_SIG:
		{
			ap_printf("MCU_IF_USER_BAUDSET 0 SIG\n");
			MCUIF_SP_set_baud(&me->stMCU_IF, eUART_BR_115200);
			status = Q_HANDLED();
			break;
		}
		case MCU_IF_USER_BAUDSET1_SIG:
		{
			ap_printf("MCU_IF_USER_BAUDSET 1 SIG\n");
			MCUIF_SP_set_baud(&me->stMCU_IF, eUART_BR_921600);
			status = Q_HANDLED();
			break;
		}
		case MCU_IF_USER_BAUDSET2_SIG:
		{
			ap_printf("MCU_IF_USER_BAUDSET 2 SIG\n");
			MCUIF_SP_set_baud(&me->stMCU_IF, eUART_BR_1843200);
			status = Q_HANDLED();
			break;
		}
		case MCU_IF_USER_BAUDSET3_SIG:
		{
			ap_printf("MCU_IF_USER_BAUDSET 3 SIG\n");
			MCUIF_SP_set_baud(&me->stMCU_IF, eUART_BR_3686400);
			status = Q_HANDLED();
			break;
		}
#endif
#if(0)
		case TWO_RELEASE_SIG:
		{
			ap_printf("[%s]2: all.bin; \n", __FUNCTION__);
			if (me->ALL_bin == 0)
			{
				status = Q_TRAN(&UserApp_Upgrade_Err);
			}
			else
			{
				ap_printf("ALL Bin File !!!!!!!!!!!!!!\n");
				UserAppDisplay(me, STR_ALLBIN);
				UpgradeService_Select(me, BIN_TYPE_ALL_BIN);
				status = Q_TRAN(&UserApp_Upgrade_Work);
			}
			break;
		}
		case THREE_RELEASE_SIG:
		{
			ap_printf("[%s]3: dsptool_config.xml\n", __FUNCTION__);
			if (me->DSPTOOL_bin == 0)
			{
				status = Q_TRAN(&UserApp_Upgrade_Err);
			}
			else
			{
				ap_printf("DSP Bin File !!!!!!!!!!!!!!\n");
				UserAppDisplay(me, STR_DSPBIN);
				UpgradeService_Select(me, BIN_TYPE_DSP_BIN);
				status = Q_TRAN(&UserApp_Upgrade_Work);
			}
			break;
		}
#endif
#ifdef BUILT4ISP2
		case FOUR_RELEASE_SIG:
		{
			ap_printf("[%s]4: ISPBOOOT.BIN \n", __FUNCTION__);
			if (me->ISPBOOOT_bin == 0)
			{
				status = Q_TRAN(&UserApp_Upgrade_Err);
			}
			else
			{
				ap_printf("ISPBOOOT.BIN !!!!!!!!!!!!!!\n");
				UserAppDisplay(me, STR_ISPBOOOTBIN);
				UpgradeService_Select(me, BIN_TYPE_ISPBOOOT_BIN);
				status = Q_TRAN(&UserApp_Upgrade_Work);
			}
			break;
		}
#endif
		case POWER_RELEASE_SIG:
		case POWER_OFF_SIG:
		{
			ap_printf("[%s] POWER_RELEASE_SIG -> [%d]\n", __FUNCTION__, __LINE__);
			status = Q_TRAN(&UserApp_standby);
			break;
		}
#ifdef MCU_IF_SP
		case MCUIF_UPGRADE_TERMINATE_SIG:
		{
			ap_printf("[%s] MCUIF_UPGRADE_TERMINATE_SIG -> [%d]\n", __FUNCTION__, __LINE__);
			status = Q_TRAN(&UserApp_standby);
			break;
		}
#endif
		default:
		{
			status = Q_SUPER(&UserApp_home);
			break;
		}
	}
	return status;
}

/******************************************************************************************/
/**
 * \fn			void SwUserAppSrc(UserApp * const me, QEvt const * const e)
 *
 * \brief		the state used to transform different source
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e : none\n
 *
 * \return	    \e  none \n
 *
 *
 ******************************************************************************************/
void SwUserAppSrc(UserApp *const me, QEvt const *const e)
{
	(void)e;/* avoid compiler warning about unused parameter */


	me->iSrc++;
	ap_printf("SwUserAppSrc = %d...\n", me->iSrc);

#ifdef USERAPP_SUPPORT_BT
	if (me->aUserSrc_tbl[me->iSrc] == eAp_Bt_Src)
	{
		if (me->pBtBakcGroundApp != NULL)
		{
			if (me->pBtBakcGroundApp->IsSurportTwsSwitch && me->pBtBakcGroundApp->BtTwsMode == HEADPHONE)
			{
				me->iSrc++;
			}
		}
	}
#endif

	if(me->iSrc >= APP_SRC_MAX)
	{
		me->iSrc = 0;
	}

	UserSetting_SaveSrc(me->iSrc);
}

/******************************************************************************************/
/**
 * \fn			int KeyCode2Sig(UserApp * const me, stKeyInfo_t *stKeyInfo)
 *
 * \brief		used to trans key to signal
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              stKeyInfo_t *stKeyInfo : key info structure \n
 *
 * \return	    \e signal info. \n
 *			    \e -1 on error. \n
 *
 ******************************************************************************************/
int UserAppKeyCode2Sig(UserApp *const me, stUserKeyInfo_t *pstUserKeyInfo)
{
	int cnt = 0;
	int KeySignal = 0;

	for (cnt = 0; cnt < eKey_Num; cnt++)
	{
		if (me->pKeyTable[cnt] == pstUserKeyInfo->UserKeyCode)
		{
			break;
		}
	}

	if (cnt < eKey_Num)
	{
		KeySignal = me->pKeyCode2SigTable[cnt] + pstUserKeyInfo->stKeyInfo.key_type;

#ifndef SDK_RELEASE
		//MONKEY_TEST, all power key signals are inactive

		if ((me->MonkeyTest_Enable == 1)
				&& ((KeySignal == POWER_SIG)
					|| (KeySignal == POWER_RELEASE_SIG)
					|| (KeySignal == POWER_LONG_PRESS_SIG)
					|| (KeySignal == POWER_LONG_RELEASE_SIG)
					|| (KeySignal == POWER_TICK_SIG)
					|| (KeySignal == BT_TWS_MODE_SWITCH_SIG)
					|| (KeySignal == BT_TWS_MODE_SWITCH_RELEASE_SIG)
					|| (KeySignal == BT_TWS_MODE_SWITCH_LONG_PRESS_SIG)
					|| (KeySignal == BT_TWS_MODE_SWITCH_LONG_RELEASE_SIG)
					|| (KeySignal == BT_TWS_MODE_SWITCH_TICK_SIG)
				   ))
		{
			return -1;
		}
		//MONKEY_TEST
#endif
		return KeySignal;
	}
	else
	{
		return -1;
	}

}

void LearningVolUpEntry(UserApp * const me)
{
	ap_printf("[Learning Key] vol up, ready to learn; \n");
	stSYS_IrLearningInfo_t stIR_L_info;

	int key_position = 0;
	key_position = SystemSetting_ReadIRPosition(IR_L_KEY_1);	  //read vol up position
	//  ap_printf("vol up key_position = %d\n",key_position);

	switch(key_position)
	{
		case IR_L_KEY_1:
			stIR_L_info.LearningKey_Target = IR_L_KEY_1_2;
			break;

		case IR_L_KEY_1_2:
			stIR_L_info.LearningKey_Target = IR_L_KEY_1_3;
			break;

		case IR_L_KEY_1_3:
		default:
			stIR_L_info.LearningKey_Target = IR_L_KEY_1;
			break;
	}
	IOSrv_IR_Learning_Start(&me->super, &stIR_L_info, UserApp_IR_Learning_Listener);
}

void LearningVolDownEntry(UserApp * const me)
{
	ap_printf("[Learning Key] vol down, ready to learn; \n");


	stSYS_IrLearningInfo_t stIR_L_info;

	int key_position = 0;
	key_position = SystemSetting_ReadIRPosition(IR_L_KEY_2);	//read vol up position
	//ap_printf("vol down key_position = %d\n",key_position);

	switch(key_position)
	{
		case IR_L_KEY_2:
			stIR_L_info.LearningKey_Target = IR_L_KEY_2_2;
			break;

		case IR_L_KEY_2_2:
			stIR_L_info.LearningKey_Target = IR_L_KEY_2_3;
			break;

		case IR_L_KEY_2_3:
		default:
			stIR_L_info.LearningKey_Target = IR_L_KEY_2;
			break;
	}

	IOSrv_IR_Learning_Start(&me->super, &stIR_L_info, UserApp_IR_Learning_Listener);
}

void LearningMuteEntry(UserApp * const me)
{
	ap_printf("[Learning Key] mute, ready to learn; \n");

	stSYS_IrLearningInfo_t stIR_L_info;
	int key_position = 0;
	key_position = SystemSetting_ReadIRPosition(IR_L_KEY_3);	//read mute position
	//ap_printf("mute key_position = %d\n",key_position);

	switch(key_position)
	{
		case IR_L_KEY_3:
			stIR_L_info.LearningKey_Target = IR_L_KEY_3_2;
			break;

		case IR_L_KEY_3_2:
			stIR_L_info.LearningKey_Target = IR_L_KEY_3_3;
			break;

		case IR_L_KEY_3_3:
		default:
			stIR_L_info.LearningKey_Target = IR_L_KEY_3;
			break;
	}
	IOSrv_IR_Learning_Start(&me->super, &stIR_L_info, UserApp_IR_Learning_Listener);
}

/******************************************************************************************/
/**
 * \fn			_IR_KEY_LIST UserAppKeyCodeConvert_CEC(void* pSender, UINT8 bCecKeyCode)
 *
 * \brief		CEC Key Cmd 2 user Key Code
 *
 * \param       UINT8 bCecKeyCode : CEC Key cmd \n
 *
 * \return	    \e user key code. \n
 *
 ******************************************************************************************/
_IR_KEY_LIST UserAppKeyCodeConvert_CEC(UINT8 bCecKeyCode)
{
	_IR_KEY_LIST eRes = IRC_NULL;
	int cnt = 0;

	while ((bCecKeyCode != CEC_CmdCode[cnt])
			&& (cnt < CEC_CMD_NUM)
		  )
	{
		cnt++;
	}
	ap_printf("===== UserAppKeyCodeConvert_CEC bCecKeyCode = %d. CEC_CmdCode[cnt] = %d   .cnt = %d.\n",bCecKeyCode , CEC_CmdCode[cnt],cnt);

	if (cnt < CEC_CMD_NUM)
	{
		eRes = CEC_Cmd2KeyCode[cnt];
	}
	else
	{
		eRes = IRC_NULL;
	}

	return eRes;
}

/******************************************************************************************/
/**
 * \fn			_IR_KEY_LIST UserAppKeyCodeConvert_VFD(void* pSender, UINT8 bCecKeyCode)
 *
 * \brief		VFD Key data 2 user Key Code
 *
 * \param       UINT8 bVFDKeyCode : VFD Key data \n
 *
 * \return	    \e user key code. \n
 *
 ******************************************************************************************/
_IR_KEY_LIST UserAppKeyCodeConvert_VFD(UINT8 bVFDKeyCode)
{

	_IR_KEY_LIST eRes = IRC_NULL;

	if (bVFDKeyCode < VFD_KEY_NUM)
		eRes = (_IR_KEY_LIST)VFD_KeyTab[bVFDKeyCode];

	return eRes;
}

/******************************************************************************************/
/**
 * \fn		_IR_KEY_LIST UserAppKeyCodeConvert_ADC(UINT8 bADCSrc, UINT8 bADCLv)
 *
 * \brief		ADC level 2 Key Code
 *
 * \param	UINT8 bADCSrc : ADC src, UINT8 bADCLv : ADC level \n
 *
 * \return	    \e user key code. \n
 *
 ******************************************************************************************/
_IR_KEY_LIST UserAppKeyCodeConvert_ADC(UINT8 bADCSrc, UINT8 bADCLv)
{

	_IR_KEY_LIST eRes = IRC_NULL;

	if (eKeySrc_ADC_0_Key == bADCSrc)
	{
		if (bADCLv < ADC_KEY_NUM)
			eRes = (_IR_KEY_LIST)ADC_keycode[bADCLv];
	}
	else if (eKeySrc_ADC_1_Key == bADCSrc)
	{
		if (bADCLv < ADC_KEY_NUM)
			eRes = (_IR_KEY_LIST)ADC_keycode[bADCLv];
	}
	else if (eKeySrc_ADC_2_Key == bADCSrc)
	{
		if (bADCLv < ADC_KEY_NUM)
			eRes = (_IR_KEY_LIST)ADC_keycode[bADCLv];
	}
	else if (eKeySrc_ADC_3_Key == bADCSrc)
	{
		if (bADCLv < ADC_KEY_NUM)
			eRes = (_IR_KEY_LIST)ADC_keycode[bADCLv];
	}

	return eRes;
}

/******************************************************************************************/
/**
 * \fn			_IR_KEY_LIST UserAppKeyCodeConvert_GPIO(UINT8 bGPIOKey)
 *
 * \brief		GPIO Key data 2 user Key Code
 *
 * \param		UINT8 bGPIOKey : GPIO Key \n
 *
 * \return	    \e user key code. \n
 *
 ******************************************************************************************/
_IR_KEY_LIST UserAppKeyCodeConvert_GPIO(UINT8 bGPIOKey)
{

	_IR_KEY_LIST eRes = IRC_NULL;

	if (bGPIOKey < GPIO_KEY_NUM)
		eRes = (_IR_KEY_LIST)GPIO_KeyCode[bGPIOKey];

	return eRes;
}

/******************************************************************************************/
/**
 * \fn			_IR_KEY_LIST UserAppKeyCodeConvert_IR_Learning(UINT8 bGPIOKey)
 *
 * \brief		IR learning Key data 2 user Key Code
 *
 * \param		UINT8 Key : Key \n
 *
 * \return	    \e user key code. \n
 *
 ******************************************************************************************/
_IR_KEY_LIST UserAppKeyCodeConvert_IR_Learning(UINT8 Key)
{

	_IR_KEY_LIST eRes = IRC_NULL;

	if(Key != 0)
	{
		Key = ((Key + 2) % 3) + 1;	//qingyu.wu add for vizio learning three sets remote
	}
	if (Key < IR_L_KEY_NUM)
		eRes = (_IR_KEY_LIST)IR_Learning_Key_Tbl[Key];

	return eRes;
}


/******************************************************************************************/
/**
 * \fn			_IR_KEY_LIST UserAppKeyCodeConvert_IR(UINT8 bIrKey)
 *
 * \brief		to convert main IR key code to user key code cmd
 *
 * \param		UINT8 bIrKey : ir  Key \n
 *
 * \return	    \e user key code cmd. \n
 *
 ******************************************************************************************/
_IR_KEY_LIST UserAppKeyCodeConvert_IR(UINT8 bIrKey)
{

	_IR_KEY_LIST eRes = IRC_NULL;

	//if (bIrKey < sizeof(ir_mapcode))
	eRes = (_IR_KEY_LIST)ir_mapcode[bIrKey];

	return eRes;
}

/******************************************************************************************/
/**
 * \fn			_IR_KEY_LIST UserAppKeyCodeConvert_MultiIR(stKeyInfo_t* pstKeyInfo)
 *
 * \brief		to convert main IR key code to user key code cmd
 *
 * \param		stKeyInfo_t* pstKeyInfo : ir  Key info \n
 *
 * \return	    \e user key code cmd. \n
 *
 ******************************************************************************************/
_IR_KEY_LIST UserAppKeyCodeConvert_MultiIR(stKeyInfo_t *pstKeyInfo)
{

	_IR_KEY_LIST eRes = IRC_NULL;
	stIR_KeyTbl_T *pstIRCgf_Tbl;
	int CusId_cnt = 0;
	int KeyCodeTbl_cnt = 0;
	UINT16 *pKeyCode_tbl;

	pstIRCgf_Tbl = (stIR_KeyTbl_T *)GetIR_Tbl(pstKeyInfo->IR_Fmt);

	ap_printf("UserAppKeyCodeConvert_MultiIR: IR_Fmt = 0x%x, IR_Cus_ID = 0x%x, key = 0x%x\n", pstKeyInfo->IR_Fmt, pstKeyInfo->IR_Cus_ID, pstKeyInfo->key);

	if (pstIRCgf_Tbl == NULL)
	{
		ap_printf_E("IRCgf_Tbl reference error\n");
		return IRC_NULL;
	}

	while (pstIRCgf_Tbl->IR_CusId[CusId_cnt] != IR_Cus_ID_TBL_END)
	{
		// search for key code tbl
		if (pstIRCgf_Tbl->IR_CusId[CusId_cnt] == pstKeyInfo->IR_Cus_ID)
			break;

		CusId_cnt++;
	}

	if (pstIRCgf_Tbl->IR_CusId[CusId_cnt] != IR_Cus_ID_TBL_END)
	{
		pKeyCode_tbl = (UINT16 *)pstIRCgf_Tbl->pIR_KeyCode[CusId_cnt];

		if (pKeyCode_tbl != NULL)
		{
			while (pKeyCode_tbl[KeyCodeTbl_cnt] != IR_KEY_CODE_TBL_END)
			{
				if (pKeyCode_tbl[KeyCodeTbl_cnt] == pstKeyInfo->key)
					break;

				KeyCodeTbl_cnt++;
			}

			if (pKeyCode_tbl[KeyCodeTbl_cnt] != IR_KEY_CODE_TBL_END)
			{
				eRes = pstIRCgf_Tbl->pIR_KeyCmd[KeyCodeTbl_cnt];
			}
			else
			{
				eRes = IRC_NULL;
			}
		}
	}

	ap_printf("res: 0x%x\n", eRes);

	return eRes;
}


/******************************************************************************************/
/**
 * \fn			_IR_KEY_LIST UserAppKeyCodeConvert(stKeyInfo_t* pstKeyInfo)
 *
 * \brief		IO data convert 2 user Key Code
 *
 * \param		stUserKeyInfo_t* pstUserKeyInfo : IO Code info\n
 *
 *
 * \return	    \e user key code. \n
 *
 ******************************************************************************************/
int UserAppKeyCodeConvert(UserApp *me, stUserKeyInfo_t *pstUserKeyInfo)
{
	_IR_KEY_LIST eRes = IRC_NULL;

	CFG_IR_T *pstIR_Cfg = GET_SDK_CFG(CFG_IR_T);

	ap_printf("[%s]::keysrc: %d, key: %d\n", __FUNCTION__, pstUserKeyInfo->stKeyInfo.keysrc, pstUserKeyInfo->stKeyInfo.key); // 20 ~ 60 ticks

	switch (pstUserKeyInfo->stKeyInfo.keysrc)
	{
		case eKeySrc_CEC_CMD:
			eRes = UserAppKeyCodeConvert_CEC(pstUserKeyInfo->stKeyInfo.key);
			ap_printf("UserKeyCode : %d \n",eRes);
			break;

		case eKeySrc_VFD_Key:
			eRes = UserAppKeyCodeConvert_VFD(pstUserKeyInfo->stKeyInfo.key);
			break;

		case eKeySrc_ADC_0_Key:
		case eKeySrc_ADC_1_Key:
		case eKeySrc_ADC_2_Key:
		case eKeySrc_ADC_3_Key:
			eRes = UserAppKeyCodeConvert_ADC(pstUserKeyInfo->stKeyInfo.keysrc, pstUserKeyInfo->stKeyInfo.key);
			break;

		case eKeySrc_PWR_Key:
			eRes = IRC_POWER; //UserAppKeyCodeConvert_GPIO(pstUserKeyInfo->stKeyInfo.key);
			break;

		case eKeySrc_GPIO_Key:
			eRes = UserAppKeyCodeConvert_GPIO(pstUserKeyInfo->stKeyInfo.key);
			break;

		case eKeySrc_IR_L_Key:
			if ((me->HPD_State != 1) || (me->aUserSrc_tbl[me->iSrc] != eAp_ARC_Src))
				eRes = UserAppKeyCodeConvert_IR_Learning(pstUserKeyInfo->stKeyInfo.key);
			break;

		case eKeySrc_IR:
			//eRes = UserAppKeyCodeConvert_IR(pstUserKeyInfo->stKeyInfo.key);
			if (((pstUserKeyInfo->stKeyInfo.IR_Cus_ID == pstIR_Cfg->lCustomerID) && (pstUserKeyInfo->stKeyInfo.IR_Fmt == IR_FMT_UPD6121)))
			{
				// main IR
				eRes = UserAppKeyCodeConvert_IR(pstUserKeyInfo->stKeyInfo.key);
			}
#ifdef Q70_DEMO
			else if (pstUserKeyInfo->stKeyInfo.IR_Fmt == IR_FMT_SAMSUNG)
			{
				eRes = UserAppKeyCodeConvert_MultiIR(&pstUserKeyInfo->stKeyInfo);
			}
#endif
			else if(pstUserKeyInfo->stKeyInfo.IR_Fmt == IR_FMT_MONKEY_TEST)
			{
				eRes = UserAppKeyCodeConvert_IR(pstUserKeyInfo->stKeyInfo.key);
			}
			else if ((me->HPD_State == 1) && (me->aUserSrc_tbl[me->iSrc] == eAp_ARC_Src))//if CEC is actived, disable tv remote ready
			{

			}
#ifdef USERAPP_SUPPORT_REPEATER
			else if((me->HPD_State == 1)&&((me->aUserSrc_tbl[me->iSrc] == eAp_HDMI0_Src)||(me->aUserSrc_tbl[me->iSrc] == eAp_HDMI1_Src)||(me->aUserSrc_tbl[me->iSrc] == eAp_HDMI2_Src)))
			{
				//if CEC is actived when Support repeater, disable tv remote ready

			}
#endif

			else
			{
				//ap_printf("pstUserKeyInfo->stKeyInfo.IR_Fmt: 0x%x\n", pstUserKeyInfo->stKeyInfo.IR_Fmt);
				//ap_printf("pstUserKeyInfo->stKeyInfo.IR_Cus_ID: 0x%x\n", pstUserKeyInfo->stKeyInfo.IR_Cus_ID);

				//eRes = UserAppKeyCodeConvert_MultiIR(&pstUserKeyInfo->stKeyInfo);
			}

			ap_printf("me->HPD_status: %d, me->Src_table: %d\n",me->HPD_State,me->aUserSrc_tbl[me->iSrc]); // 20 ~ 60 ticks
			break;

		default:
			ap_printf_E("err!! unsupport src: %d\n", (pstUserKeyInfo->stKeyInfo.keysrc));
			//eRes = (_IR_KEY_LIST)pstUserKeyInfo->stKeyInfo.key;
			break;
	}

	pstUserKeyInfo->UserKeyCode = eRes;

	return 0;
}

/******************************************************************************************/
/**
 * \fn			int UserAppMute(UserApp * const me, int iEnable)
 *
 * \brief		used to control volume mute
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              int iEnable : 0 --> disable mute \n
 *                            1 --> enable mute\n
 *
 * \return	    \e other on success. \n
 *			    \e -1 on error. \n
 *
 ******************************************************************************************/
int UserAppMute(UserApp *const me, int iEnable)
{
	int iRes = 0;

	if (iEnable)
	{
		ap_printf("%s on !!!\n", __FUNCTION__);

		
		iRes = AmpSet_MUTE_ON(me->hAmpList);
		wireless_mute(iEnable);
	}
	else
	{
		ap_printf("%s off !!!\n", __FUNCTION__);


		//if(me->aux_mute_flag == 0)
		{
			iRes = AmpSet_MUTE_OFF(me->hAmpList);
			wireless_mute(iEnable);
		}
		//else
		{
		//	ap_printf(" no set mute, aux mute is set\n ");

		}
	}

	if (iRes == -1)
	{
		ap_printf_E("MUTE FAIL ,PLZ CHECK!!!!\n");
		return iRes;
	}

	return iRes;
}


/******************************************************************************************/
/**
 * \fn			int UserAppAmpInit(AmpList* pAmpList)
 *
 * \brief		used to Init userapp AMP
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *
 * \return	    \e 0 on success. \n
 *			    \e -1 on error. \n
 *
 ******************************************************************************************/
int UserAppAmpInit(AmpList *pAmpList)
{
	int iRes = 0;
	int i = 0;

	ap_printf("%s !!!\n", __FUNCTION__);

	//check amp cfg! Only I2S amp needs to be controlled, it  means "peList" and each SignalModeAmpX have to match
	for (i = 0; i < pAmpList->pstAmpCfg->AmpNum; i++)
	{
		if ((*(&pAmpList->pstAmpCfg->SignalModeAmp1 + pAmpList->peList[i].eAmpID)) != eAmpSignal_I2S)
		{
			ap_printf_E("AMP config error!!\n Amp%d, SignalMode: %d\n", pAmpList->peList[i].eAmpID + 1, (*(&pAmpList->pstAmpCfg->SignalModeAmp1 + pAmpList->peList[i].eAmpID)));
			//return eRes_err;   // Modified in SPA300_TPV_20210205
		}
	}

	iRes = AmpInit(pAmpList);

	//AmpSet_Volume(&list, USERAPP_VOL_DEFAULT);

	ap_printf("%s  on !!!\n", __FUNCTION__);
	AmpSet_MUTE_ON(pAmpList);

	return iRes;
}

/******************************************************************************************/
/**
 * \fn			int UserAppEQSet(QActive* me, int State)
 *
 * \brief		used to set EQ mode
 *
 * \param		int State : eApEQ_Mode_Num(max)
 *
 *
 * \return	    \e 0 on success. \n
 *			    \e -1 on error. \n
 *
 ******************************************************************************************/
int UserAppEQSet(QActive *me, int State)
{
	int iRes = 0;
	ap_printf("%s:%d\n", __FUNCTION__, State);

	if ((State < 0) || (State >= eApEQ_Mode_Num))
	{
		ap_printf_E("%s prameter error: %d\n", __FUNCTION__, State);
		return -1;
	}

	// audio EQ set service
#ifndef BUILT4ISP2
	AudDspService_Set_AppMode(me, SET_EQ,GRP_FILTER1,State);
#endif

	if (iRes == 0)
	{
		UserSetting_SaveEQ(State);
	}
	else
	{
		ap_printf_E("%s FAIL: %d, res: %d\n", __FUNCTION__, State, iRes);
	}

	return iRes;
}

/******************************************************************************************/
/**
 * \fn			int UserAppSurroundSet(QActive *me, int State)
 *
 * \brief		used to set surround mode
 *
 * \param		int State : eApSurround_Mode_Num(max)
 *
 *
 * \return	    \e 0 on success. \n
 *			    \e -1 on error. \n
 *
 ******************************************************************************************/
int UserAppSurroundSet(QActive *me, int State)
{
	int iRes = 0;
	ap_printf("%s:%d\n", __FUNCTION__, State);

	if ((State < 0) || (State >= eApSurround_Mode_Num))
	{
		ap_printf_E("%s prameter error: %d\n", __FUNCTION__, State);
		return -1;
	}

	// audio Surround set service
#ifndef BUILT4ISP2
	AudDspService_Set_AppMode(me, SET_SS3,GRP_SURROUND0,State);
#endif

	if (iRes == 0)
	{
		UserSetting_SaveSurround(State);
	}
	else
	{
		ap_printf_E("%s FAIL: %d, res: %d\n", __FUNCTION__, State, iRes);
	}

	return iRes;
}

//AUD_DPS_UPDATE_USRGAIN #dps
#if 0
int UserAppDPSSet(UINT32 UserGain)
{
	int iRes = 0;
	LOGD("$$$$$$$$$$$$$$$$%s:%d\n", __FUNCTION__, UserGain);

	if (UserGain > 0x80000)
		UserGain = 0x80000;

	// audio DPS set service
#ifndef BUILT4ISP2
	AudDspService_Set_DPS(UserApp_get(), UserGain);
#endif

	if (iRes == 0)
	{
		UserSetting_SaveDPS(UserGain);
		//printf("[yuanchiao] save dps set \n");
	}
	else
	{
		ap_printf("%s FAIL: %d, res: %d\n", __FUNCTION__, UserGain, iRes);
	}

	return iRes;
}

#endif
/******************************************************************************************/
/**
 * \fn			int UserAppShuffleSet(UserApp * const me, int State)
 *
 * \brief		used to set shuffle mode
 *
 * \param		int State : REPEAT_MODE_NUM(max)
 *
 *
 * \return	    \e 0 on success. \n
 *			    \e -1 on error. \n
 *
 ******************************************************************************************/
int UserAppShuffleSet(UserApp *const me, int State)
{
	int iRes = 0;
	ap_printf("%s:%d\n", __FUNCTION__, State);

	if ((State < 0) || (State >= PLAY_MODE_NUM))
	{
		ap_printf_E(" prameter error: %d\n", State);
		return -1;
	}

	iRes = MFP_PlayModeSet(me->pAPInstance, State);

	if (iRes == 0)
	{
		UserSetting_SaveShuffle(State);
		switch (State)
		{
			case PLAY_MODE_ORDER:
				ap_printf("[%s:%d] set shuffle to order\n", __FUNCTION__, __LINE__);
				break;
			case PLAY_MODE_SHUFFLE:
				ap_printf("[%s:%d] set shuffle to shuffle\n", __FUNCTION__, __LINE__);
				break;
			case PLAY_MODE_RANDOM:
				ap_printf("[%s:%d] set shuffle to random\n", __FUNCTION__, __LINE__);
				break;
			case PLAY_MODE_UNKNOW:
				ap_printf("[%s:%d] set shuffle to unknow\n", __FUNCTION__, __LINE__);
				break;
			default:
				ap_printf("[%s:%d] set shuffle to %d\n", __FUNCTION__, __LINE__, State);
		}

	}
	else
	{
		ap_printf_E(" FAIL: %d, res: %d\n", State, iRes);
	}

	return iRes;
}

/******************************************************************************************/
/**
 * \fn			int UserAppRepeatSet(UserApp * const me, int State)
 *
 * \brief		used to set Repeat mode
 *
 * \param		int State : REPEAT_MODE_NUM(max)
 *
 *
 * \return	    \e 0 on success. \n
 *			    \e -1 on error. \n
 *
 ******************************************************************************************/
int UserAppRepeatSet(UserApp *const me, int State)
{
	int iRes = 0;
	ap_printf("%s:%d\n", __FUNCTION__, State);

	if ((State < 0) || (State >= REPEAT_MODE_NUM))
	{
		ap_printf_E("prameter error: %d\n", State);
		return -1;
	}

	iRes = MFP_PlayRepeatSet(me->pAPInstance, State);

	if (iRes == 0)
	{
		UserSetting_SaveRepeat(State);
		switch (State)
		{
			case REPEAT_MODE_NOTHING:
				ap_printf("[%s:%d] set repeat to nothing\n", __FUNCTION__, __LINE__);
				break;
			case REPEAT_MODE_ALL:
				ap_printf("[%s:%d] set repeat to all\n", __FUNCTION__, __LINE__);
				break;
			case REPEAT_MODE_SINGLE_NOREPEAT:
				ap_printf("[%s:%d] set repeat to single norepeat\n", __FUNCTION__, __LINE__);
				break;
			case REPEAT_MODE_SINGLE:
				ap_printf("[%s:%d] set repeat to signle\n", __FUNCTION__, __LINE__);
				break;
#if (SUPPORT_FLODER)
			case REPEAT_MODE_DIR:
				ap_printf("[%s:%d] set repeat to dir\n", __FUNCTION__, __LINE__);
				break;
#endif
			case REPEAT_MODE_USER:
				ap_printf("[%s:%d] set repeat to user\n", __FUNCTION__, __LINE__);
				break;
			default:
				ap_printf("[%s:%d] set repeat to %d\n", __FUNCTION__, __LINE__, State);
		}
	}
	else
	{
		ap_printf_E(" FAIL: %d, res: %d\n", State, iRes);
	}

	return iRes;
}

/******************************************************************************************/
/**
 * \fn			int UserAppVolumeSet(UserApp * const me, int vol)
 *
 * \brief		used to set volume
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              int vol : 40(max) , 0(min)
 *
 * \return	    \e 0 on success. \n
 *			    \e -1 on error. \n
 *
 ******************************************************************************************/
int UserAppVolumeSet(int vol, AmpList *pstAmpList)
{
	int iRes = 0;
	#ifdef SUPPORT_DSP_VOL
	UserApp* me = &l_UserApp;
	#endif
	if ((vol > USERAPP_VOL_MAX) || (vol < USERAPP_VOL_MIN))
	{
		ap_printf_E("UserAppVolumeSet FAIL: %d\n", vol);
		return -1;
	}

	ap_printf("UserAppVolumeSet: %d\n", vol);

	if ((pstAmpList->pstAmpCfg->SignalModeAmp1 == eAmpSignal_I2S)
			|| (pstAmpList->pstAmpCfg->SignalModeAmp2 == eAmpSignal_I2S)
			|| (pstAmpList->pstAmpCfg->SignalModeAmp3 == eAmpSignal_I2S)
	   )
	{
		iRes = AmpSet_Volume(pstAmpList, vol);
	}

	if (pstAmpList->pstAmpCfg->SignalModeAmp1 == eAmpSignal_ANALOG)
	{
		//to do: we need a DSP gain ctrl API here
		#ifdef SUPPORT_DSP_VOL
		UserApp_SetMasterVol(me, vol);
		#endif
	}

	if (pstAmpList->pstAmpCfg->SignalModeAmp2 == eAmpSignal_ANALOG)
	{
		//to do: we need a DSP gain ctrl API here
	}

	if (pstAmpList->pstAmpCfg->SignalModeAmp3 == eAmpSignal_ANALOG)
	{
		//to do: we need a DSP gain ctrl API here
	}

	if (iRes == 0)
	{
		return vol;
	}

	return -1;
}

/******************************************************************************************/
/**
 * \fn			void UserAppMemoryCheck(void)
 *
 * \brief		used to check system memory
 *
 * \param
 *
 *
 * \return
 *
 ******************************************************************************************/
void UserAppMemoryCheck(void)
{
	ap_printf("\n%-23s%-8s%-8s %-10s%-14s%-12s%-10s\n", "Task", "TCBNum", "status", "StackSize", "StackMaxUsage", "SysHeapUsed", "BtHeapUsed");
	vTaskMemoryUsageStats();

	return;
}

/******************************************************************************************/
/**
 * \fn			int UserAppVolumeUp(UserApp * const me)
 *
 * \brief		used to set volume up
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *
 *
 * \return	    \e 0 on success. \n
 *			    \e -1 on error. \n
 *
 ******************************************************************************************/
int UserAppVolumeUp(UserApp *const me)
{
	int iRes = -1;
	int ivol;
	//EV01SE_Init();

	if (me->vol  < USERAPP_VOL_MAX)
	{
		me->vol ++;
	}
	else
	{
		ap_printf_E("UserAppVolumeUp FAIL: %d\n", me->vol );
		return iRes;
	}

	ap_printf("UserAppVolumeUp: %d\n", me->vol );
	UserSetting_SaveVolume(me->vol);

	ivol = UserAppVolumeSet(me->vol, me->hAmpList);

	if (ivol != -1)
	{
		me->vol = ivol;
	}
	else
	{
		ap_printf_E("UserAppVolumeSet Fail\n");
	}
	return iRes;
}

/******************************************************************************************/
/**
 * \fn			int UserAppVolumeDown(UserApp * const me)
 *
 * \brief		used to set volume down
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *
 *
 * \return	    \e 0 on success. \n
 *			    \e -1 on error. \n
 *
 ******************************************************************************************/
int UserAppVolumeDown(UserApp *const me)
{
	int iRes = -1;
	int ivol;

	if (me->vol > USERAPP_VOL_MIN)
	{
		me->vol --;
	}
	else
	{
		ap_printf_E(" FAIL: %d\n", me->vol );
		return iRes;
	}

	ap_printf("UserAppVolumeDown: %d\n", me->vol );
	UserSetting_SaveVolume(me->vol);

	ivol = UserAppVolumeSet(me->vol, me->hAmpList);

	if (ivol != -1)
	{
		me->vol = ivol;
	}
	else
	{
		ap_printf_E(" Fail\n");
	}
	return iRes;
}



#ifdef SYNCCOM_WIRELESS
/******************************************************************************************/
/**
 * \fn			int UserApp_WIRELESS_VolumeUp(UserApp * const me)
 *
 * \brief		used to set WIRELESS volume up
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *
 *
 * \return	    \e 0 on success. \n
 *			    \e -1 on error. \n
 *
 ******************************************************************************************/
int UserApp_WIRELESS_VolumeUp(UserApp *const me)
{
	int iRes = -1;
	int ivol = 0;

	if (me->Wireless_vol < WIRELESS_VOL_MAX)
	{
		me->Wireless_vol ++;
	}
	else
	{
		ap_printf_E("UserApp_WIRELESS_VolumeUp FAIL: %d\n", me->Wireless_vol );
		return iRes;
	}

	ap_printf("UserApp_WIRELESS_VolumeUp: %d\n", me->Wireless_vol );

	//ivol = SetWireless_Vol_Set(me->Wireless_vol);

	if (ivol != -1)
	{
		me->Wireless_vol = ivol;
	}
	else
	{
		ap_printf_E("UserApp_WIRELESS_VolumeUp Fail\n");
	}
	return iRes;
}

/******************************************************************************************/
/**
 * \fn			int UserApp_WIRELESS_VolumeDown(UserApp * const me)
 *
 * \brief		used to set WIRELESS volume down
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *
 *
 * \return	    \e 0 on success. \n
 *			    \e -1 on error. \n
 *
 ******************************************************************************************/
int UserApp_WIRELESS_VolumeDown(UserApp *const me)
{
	int iRes = -1;
	int ivol =0 ;

	if (me->Wireless_vol  > WIRELESS_VOL_MIN)
	{
		me->Wireless_vol --;
	}
	else
	{
		ap_printf_E("UserApp_WIRELESS_VolumeDown FAIL: %d\n", me->Wireless_vol );
		return iRes;
	}

	ap_printf("UserApp_WIRELESS_VolumeDown: %d\n", me->Wireless_vol );

	//ivol = SetWireless_Vol_Set(me->Wireless_vol);

	if (ivol != -1)
	{
		me->Wireless_vol = ivol;
	}
	else
	{
		ap_printf_E("UserApp_WIRELESS_VolumeDown\n");
	}
	return iRes;
}
#endif

/******************************************************************************************/
/**
 * \fn			int UserAppKeySound(void *pSender, DSP_HINT_SOUND_P* pstKeySound)
 *
 * \brief		for key sound
 *
 * \param		void *pSender:(Input) the UserApp handler here,\n
 *                          DSP_HINT_SOUND_P* pstKeySound: key sound info structure\n
 *
 *
 * \return	    \e 0 on success. \n
 *			    \e -1 on error. \n
 *
 ******************************************************************************************/
int UserAppKeySound(void *pSender, DSP_HINT_SOUND_P *pstKeySound)
{
	int iRes = -1;

	ap_printf("KeySound\n");
#if 0
#ifndef BUILT4ISP2
	iRes = AudDspService_Hint_PlayPcm((QActive *)pSender,
									  pstKeySound->HINT_DATA,
									  pstKeySound->SIZE,
									  pstKeySound->MUSIC_TARGET_GAIN,
									  pstKeySound->HINT_RAMP_UP_VELOCITY,
									  pstKeySound->HINT_DECLINE_VELOCITY,
									  0,
									  pstKeySound->SAMPLE_RATE);
#endif
#else
	(void)pSender;
	(void)pstKeySound;
#endif
	if (iRes == -1)
	{
		ap_printf_E("KeySound fail\n");
	}

	return iRes;
}

/******************************************************************************************/
/**
 * \fn			int UserAppGetSingleKey(hCircBuf_t *c, stKeyInfo_t *data)
 *
 * \brief		used to get single  key
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              stKeyInfo_t *data : key info structure \n
 *
 * \return	    \e 0 on success. \n
 *			    \e -1 on error. \n
  *			    \e 1 on specific. \n
 *
 ******************************************************************************************/
int UserAppGetSingleKey(hCircBuf_t *c, stKeyInfo_t *data)
{
	int iRes = 0;

	iRes = circBufPop(c, data);

	if (iRes == 0)
	{
		if (c->KeyNum > 0)
			return eRes_specific1;
	}
	else
	{
		return eRes_err; //there is no key in the buf
	}

	return eRes_success;
}


/******************************************************************************************/
/**
 * \fn			int UserAppGetLatestKey(hCircBuf_t *c, stKeyInfo_t *data)
 *
 * \brief		used to get latest key
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              stKeyInfo_t *data : key info structure \n
 *
 * \return	    \e 0 on success. \n
 *			    \e -1 on error. \n
 *
 ******************************************************************************************/
int UserAppGetLatestKey(hCircBuf_t *c, stKeyInfo_t *data)
{
	int iRes = 0, cnt = 0;

	iRes = circBufPop(c, data);

	if (iRes == 0)
	{
		while ((iRes != -1) && (cnt < KEY_BUF_SIZE))
		{
			iRes = circBufPop(c, data);
			cnt++;
			//ap_printf_d("UserAppGetLatestKey get key::%d, src: %d, key_type:%d, result: %d, cnt: %d\n", data->key, data->keysrc, data->key_type, iRes, cnt);
		}
	}
	else
	{
		return eRes_err; //there is no key in the buf
	}

	return eRes_success;
}

/******************************************************************************************/
/**
 * \fn			int UserAppGetLatestKey_SpecificSrc(int iKeySrc_specific, hCircBuf_t *c, stKeyInfo_t *data)
 *
 * \brief		used to get latest key but prioritize the key from specific src
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              stKeyInfo_t *data : key info structure \n
 *
 * \return	    \e 0 on success. \n
 *			    \e -1 on error. \n
 *			    \e 1 on key sig from specific src and key buf is not empty. \n
 *
 ******************************************************************************************/
int UserAppGetLatestKey_SpecificSrc(int iKeySrc_specific, hCircBuf_t *c, stKeyInfo_t *data)
{
	int iRes = 0, cnt = 0;

	iRes = circBufPop(c, data);

	if (iRes == 0)
	{
		while ((iRes != -1)
				&& (cnt < KEY_BUF_SIZE)
				&& (iKeySrc_specific != data->keysrc)
			  )
		{
			iRes = circBufPop(c, data);
			cnt++;
		}

		ap_printf("UserAppGetLatestKey get key::%d, src: %d, key_type:%d, result: %d, cnt: %d, KeyNum: %d\n", data->key, data->keysrc, data->key_type, iRes, cnt, c->KeyNum);

		if (c->KeyNum > 0) // key sig from specific src and key buf is not empty
		{
			return eRes_specific1;
		}
	}
	else
	{
		return eRes_err; //there is no key in the buf
	}

	return eRes_success;
}

/******************************************************************************************/
/**
 * \fn			void UserAppKeyInfo(UserApp * const me,stUserKeyInfo_t data)
 *
 * \brief		used to print key info
 *
 * \param		stKeyInfo_t data
 *
 * \return	    NULL
 *
 ******************************************************************************************/
void UserAppKeyInfo(UserApp *const me, stUserKeyInfo_t data)
{
	if ((data.stKeyInfo.keysrc < eKeySrc_Num) && (data.UserKeyCode < IR_CODE_MAX_NUM))
	{
		ap_BurnTest_log("[%s] :  %s\n", KeySrc[(unsigned char)data.stKeyInfo.keysrc], IRKeyTable[data.UserKeyCode]);
	}
	else
	{
		ap_BurnTest_log("src :%d, key: 0x%x err! \n", data.stKeyInfo.keysrc, data.UserKeyCode);
	}

	if (data.stKeyInfo.key_type < eKeyStateNum)
	{
		ap_BurnTest_log("key_type: %s\n", KeyType[(int)data.stKeyInfo.key_type]);
	}
	else
	{
		ap_BurnTest_log("key_type err: %d\n", data.stKeyInfo.key_type);
	}

	return ;
}

/******************************************************************************************/
/**
 * \fn			int UserAppHintSoundEnd(QActive * const sender, void *pParam)
 *
 * \brief		call this API to previous state when hint sound play back is finish
 *
 * \param		QActive * const me:(Input) the send handler,\n
 *
 ******************************************************************************************/
void UserAppHintSoundEnd(QActive *const sender, void *pParam)
{
	(void)pParam;

	QACTIVE_POST(AO_UserApp, Q_NEW(QEvt, CUSTOM_HINT_END_SIG), sender);
}

int UserAppHintParamSet(UserApp *const me, int mode, double duck_vol, double xef_vol, UINT16 L_to_out_ch_num, UINT16 R_to_out_ch_num)
{
	int iRes = 0;

	spXeffect_ChSelect(mode, L_to_out_ch_num, R_to_out_ch_num);
	if(me->dXef_Handle == 0){
		me->dXef_Handle = spXeffect_Alloc(mode, me->xeffBufSize * 1024);
		if (me->dXef_Handle == 0) {
			ap_printf_E(" xeffect alloc fail\n");
			return -1;
		}
	}

	spXeffect_SetHintVol(me->dXef_Handle, xef_vol);
	spXeffect_SetDuckVol(me->dXef_Handle, duck_vol);
	return iRes;
}

void SrcNameHint(UserApp *const me)
{
	if(me->PwrOnFlag)
	{
		me->PwrOnFlag = 0;
	}
	else
	{
		QTimeEvt_disarmX(&me->timeSrcHintEvt);
//		QTimeEvt_armX(&me->timeSrcHintEvt, (TICKS_PER_100MS), 0);
		QTimeEvt_armX(&me->timeSrcHintEvt, (TICKS_PER_250MS), 0);
	}
}

void UserAppHintStop(UserApp *const me)
{
	if(me->hintPlayFlag)
	{
		me->hintDelayFlag = 0;
		QTimeEvt_disarmX(&me->timeHintDelayEvt);
		hintEndProcess();
		if(me->dXef_Handle)
		{
			spXeffect_Stop(me->dXef_Handle);
		}
	}
}

void UserAppHintPlay(UserApp *const me)
{
	int iRes = 0;
	int mode = 0;
	int duck_vol = -12; // default set -12 db
	int xef_vol = 0; 	// default set 0 db
	UINT16 L_to_out_ch_num = XEFFECT_L_CH;	/* source Lch default out channel is left */
	UINT16 R_to_out_ch_num = XEFFECT_R_CH;	/* source Rch default out channel is right */
	me->pHintCnf = getNextSrcData();
	ap_printf("me->pHintCnf:0x%x\n", me->pHintCnf);
	if(me->pHintCnf == NULL)
	{
		ap_printf("hint data is NULL!\n");
		return;
	}

	duck_vol = me->pHintCnf->duck_vol;
	xef_vol = me->pHintCnf->xef_vol;
	L_to_out_ch_num = me->pHintCnf->L_to_out_ch_num;
	R_to_out_ch_num = me->pHintCnf->R_to_out_ch_num;
	mode = XEFFET_USE_PATH1 | XEFFET_PLAYMODE_SINGLEPATH;

	ap_printf("Hint data size:%d\n", me->pHintCnf->SIZE);

	iRes = UserAppHintParamSet(me, mode, duck_vol, xef_vol, L_to_out_ch_num, R_to_out_ch_num);
	if(iRes < 0) {
		ap_printf_E(" xeffect set param fail\n");
		return;
	}

	me->hintPlayFlag = 1;
	spXeffect_Play(me->dXef_Handle, (UINT8 *)me->pHintCnf->HINT_DATA, me->pHintCnf->SIZE);
}

int UserAppDelayHint(UserApp *const me, eHint_Type HintType)
{
	int iRes = 0;
	me->hintDelayFlag = 0;

	ap_printf("hintPlayFlag: %d, me->hintType:%d\n", me->hintPlayFlag, me->hintType);

	iRes = hintDataAdd(HintType);
	if(iRes < 0) {
		ap_printf_E(" hint data init error\n");
		return -1;
	}
	UserAppHintPlay(me);

	return iRes;
}

int UserAppCallHint(UserApp *const me, eHint_Type HintType)
{
	int iRes = 0;

	ap_printf("hintPlayFlag: %d, HintType:%d, curHintType:%d\n", me->hintPlayFlag, HintType, me->hintType);
	if( me->hintPlayFlag &&
		(me->singlePlayMode == XEFFET_PROMPTLY_PALY
			|| HintType > eHint_Single_Max || me->hintType > eHint_Single_Max
			|| (HintType > eHint_EQ_Min && HintType < eHint_EQ_Max)))
	{
		UserAppHintStop(me);
		me->hintDelayFlag = 1;
		QTimeEvt_disarmX(&me->timeHintDelayEvt);
		QTimeEvt_armX(&me->timeHintDelayEvt, (TICKS_PER_100MS / 2), 0);
	}
	else if(me->hintPlayFlag &&
			(me->singlePlayMode == XEFFET_SMOOTH_PALY && HintType < eHint_Single_Max))
	{
		iRes = hintDataAdd(HintType);
		if(iRes < 0) {
			ap_printf_E(" hint data init error\n");
			return -1;
		}
	}
	else
	{
		iRes = UserAppDelayHint(me, HintType);
		if(iRes < 0) {
			ap_printf_E(" hint data init error\n");
			return -1;
		}
	}
	me->hintType = HintType;


	return iRes;
}

//#define MAJOR_VER 1
//#define MINOR_VER 0
//#define REVISION_VER 1
//#define BUILD_VER 1
#define FW_VERSION "1011"

#ifdef MAJOR_VER
char *getFwVersion()
{
	static char verStr[30];
	sprintf(verStr, "%d.%d.%d.%d", MAJOR_VER, MINOR_VER, REVISION_VER, BUILD_VER);
	ap_printf("version:%s\n", verStr);
	return verStr;
}
#else
char *getFwVersion()
{
	static char *verStr;
	verStr = FW_VERSION;
	ap_printf("version:%s\n", verStr);
	return verStr;
}
#endif

int getCurUserSrc()
{
	UserApp *const me = &l_UserApp;
	return me->iSrc;
}

UINT32 getCurCodecType()
{
	UserApp *const me = &l_UserApp;
	return me->codecType;
}

UINT16 getDSPC_AudioFormat()
{
	UINT16 audio_format = AUDIO_FORMAT_NONE;

	audio_format = AudDspService_Get_CurrFormat(NULL);
	switch(audio_format){
		case ID_MAIN:
			ap_printf("[DSP_C]No audio signal\n");
			break;
		case ID_DECODER_PCM:
			audio_format = AUDIO_FORMAT_PCM;
			break;
		case ID_DECODER_DDP:
			audio_format = AUDIO_FORMAT_DD_PLUS;
			break;
		case ID_DECODER_TRUEHD:
			audio_format = AUDIO_FORMAT_DOLBY_TRUEHD;
			break;
		case ID_DECODER_DTSLBR:
			audio_format = AUDIO_FORMAT_DTS_HD;
			break;
		case ID_DECODER_DTSHD:
			audio_format = AUDIO_FORMAT_DTS_HD;
			break;
		case ID_DECODER_MPEG:
			audio_format = AUDIO_FORMAT_MPEG;
			break;
		case ID_DECODER_AAC:
		case ID_DECODER_HEAAC:
			audio_format = AUDIO_FORMAT_AAC;
			break;
		case ID_DECODER_WMA:
			audio_format = AUDIO_FORMAT_WMA;
			break;
		case ID_DECODER_RA:
			audio_format = AUDIO_FORMAT_RA;
			break;
		case ID_DECODER_OGG:
			audio_format = AUDIO_FORMAT_OGG;
			break;
		case ID_DECODER_WMAPRO:
			audio_format = AUDIO_FORMAT_WMA_PRO;
			break;
		case ID_DECODER_FLAC:
			audio_format = AUDIO_FORMAT_FLAC;
			break;
		case ID_DECODER_APE:
			audio_format = AUDIO_FORMAT_APE;
			break;
		case ID_DECODER_ALAC:
			audio_format = AUDIO_FORMAT_ALAC;
			break;
		case ID_DECODER_SBC:
			audio_format = AUDIO_FORMAT_SBC;
			break;
		case ID_DECODER_DSD:
			audio_format = AUDIO_FORMAT_DSD;
			break;
		default:
			ap_printf("(Unknown format %x)\n",audio_format);
			break;
	}

	return audio_format;

}

#if defined(SUPPORT_HE_FLOW) && (SUPPORT_HE_FLOW > 0)
UINT16 getAudioFormat()
{
	UINT16 audio_format=AUDIO_FORMAT_NONE;
	int dCh_Num=0, dSampleRate=0, dFormat=0;

	switch(getCurUserSrc()){
		case eAp_USB_Src:
		case eAp_CARD_Src:
	#if (!SUPPORT_VA)
		case eAp_AUX_Src:
	#endif
		case eAp_LINE_Src:
		case eAp_I2S_Src:
	#ifdef USERAPP_SUPPORT_BT
		case eAp_Bt_Src:
	#endif
			audio_format = getDSPC_AudioFormat();
		    break;

		case eAp_SPDIF_Optical_Src:
	#if defined(CFG_COAX_ENABLE) && (CFG_COAX_ENABLE == 1)
		case eAp_SPDIF_Coaxial_Src:
	#endif
		case eAp_ARC_Src:
		case eAp_HDMI0_Src:
		case eAp_HDMI1_Src:
		case eAp_HDMI2_Src:
	#ifdef USERAPP_SUPPORT_HDMI_TX
		case eAp_HDMI_TX_Src:
	#endif
		// Get Audio Information
		SpHdsp_getAudioFormat(&dCh_Num, &dSampleRate, &dFormat);
		audio_format = dFormat & 0xff;
		switch(audio_format){
			case FORMAT_NONE:
				ap_printf("[DSP_H]No audio signal\n");
				break;
			case FORMAT_PCM:
				audio_format = AUDIO_FORMAT_PCM;
				break;
			case FORMAT_PCM_MCH:
				audio_format = AUDIO_FORMAT_PCM_MCH;
				break;
			case FORMAT_DOLBY_DIGITAL:
			case FORMAT_DOLBY_DIGITAL_EX:
				audio_format = AUDIO_FORMAT_DOLBY_DIGITAL;
				break;
			case FORMAT_DD_PLUS:
				audio_format = AUDIO_FORMAT_DD_PLUS;
				break;
			case FORMAT_DD_TRUEHD:
				audio_format = AUDIO_FORMAT_DOLBY_TRUEHD;
				break;
			case FORMAT_MAT:
				audio_format = AUDIO_FORMAT_DOLBY_MAT;
				break;
			case FORMAT_DTS:
			case FORMAT_DTS_9624:
			case FORMAT_DTS_ES_M61:
			case FORMAT_DTS_ES_D61:
			case FORMAT_DTS_ES_D8:
				audio_format = AUDIO_FORMAT_DTS;
				break;
			case FORMAT_DTS_HIRES:
			case FORMAT_DTS_MA:
			case FORMAT_DTS_LBR:
			case FORMAT_DTS_LOSSLESS:
				audio_format = AUDIO_FORMAT_DTS_HD;
				break;
			case FORMAT_DTS_UHD:
			case FORMAT_DTS_UHD_MA:
			case FORMAT_DTS_UHD_LOSSLESS:
			case FORMAT_DTS_UHD_GAME:
				audio_format = AUDIO_FORMAT_DTS_X;
				break;
			case FORMAT_HDCD:
				audio_format = AUDIO_FORMAT_HDCD;
				break;
			case FORMAT_MP3:
			case FORMAT_MPEG2:
				audio_format = AUDIO_FORMAT_MPEG;
				break;
			case FORMAT_MPEG2_AAC_ADTS:
				audio_format = AUDIO_FORMAT_AAC;
				break;
			case FORMAT_DSD:
				audio_format = AUDIO_FORMAT_DSD;
				break;
			case FORMAT_WMA_PRO:
				audio_format = AUDIO_FORMAT_WMA_PRO;
				break;
			default:
				ap_printf("(unknown format %x)\n",audio_format);
				break;
		}
		break;

		default:
			ap_printf("unknow source:%d\n",getCurUserSrc());
			break;

	}

	return audio_format;

}
#else
UINT16 getAudioFormat()
{
	return getDSPC_AudioFormat();
}
#endif
/******************************************************************************************/
/**
 * \fn          void UserAppAudioInfo_Listener(QActive * const me, void *pParam)
 *
 * \brief      function for listener regist
 *
 * \param   QActive * const me:(Input) AO handler ,\n
 *               void *pParam : audio info structure pointer (structure type: stSYS_AudioInfo_t)
 *
 * \return	    none.
 *
 *
 ******************************************************************************************/
void UserAppAudioInfo_Listener(QActive *const me, void *pParam)
{
	(void)me;
	(void)pParam;

	QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, AUDIO_INFO_BROADCAST_SIG), UserApp_get());

	ap_printf("UserAppAudioInfo_Listener callback\n");
}


/******************************************************************************************/
/**
 * \fn          void UserAppVolSet_Listener(QActive * const me, void *pParam)
 *
 * \brief      function for listener regist
 *
 * \param   QActive * const me:(Input) AO handler ,\n
 *               void *pParam : audio info structure pointer (structure type: stSYS_AudioInfo_t)
 *
 * \return	    none.
 *
 *
 ******************************************************************************************/
void UserAppVolSet_Listener(QActive *const me, void *pParam)
{
	(void)me;
	(void)pParam;
	stSYS_AudioInfo_t *pstAudioInfo = (stSYS_AudioInfo_t * )pParam;

	if (pstAudioInfo->BroadcastState & SYS_MUTE_CHG)
	{
		ap_printf("============================UserAppVolSet_Listener: AUDIO_MUTE_SET_SIG\n");
		stSYS_AudioInfo_t *e = Q_NEW(stSYS_AudioInfo_t, AUDIO_MUTE_SET_SIG);

		e->BroadcastState = pstAudioInfo->BroadcastState;
		e->Vol = pstAudioInfo->Vol;
		e->MuteState = pstAudioInfo->MuteState;
		e->MaxVol = pstAudioInfo->MaxVol;
		e->MinVol = pstAudioInfo->MinVol;

		QACTIVE_POST(me, &e->stEvt, me);
	}

	if (pstAudioInfo->BroadcastState & SYS_VOL_CHG)
	{
		stSYS_AudioInfo_t *e = Q_NEW(stSYS_AudioInfo_t, AUDIO_VOL_SET_SIG);

		e->BroadcastState = pstAudioInfo->BroadcastState;
		e->Vol = pstAudioInfo->Vol;
		e->MuteState = pstAudioInfo->MuteState;
		e->MaxVol = pstAudioInfo->MaxVol;
		e->MinVol = pstAudioInfo->MinVol;

		QACTIVE_POST(me, &e->stEvt, me);
	}

	ap_printf("============================ UserAppAudioInfo_Listener callback\n");
}

/******************************************************************************************/
/**
 * \fn          void UserAppCecEvt_Listener(QActive * const me, void *pParam)
 *
 * \brief      callback function for cec evt listener regist
 *
 * \param   QActive * const me:(Input) AO handler ,\n
 *               void *pParam : cec event info structure pointer (structure type: stCECEvt_t)
 *
 * \return	    none.
 *
 *
 ******************************************************************************************/
void UserAppCecEvt_Listener(QActive *const me, void *pParam)
{
	ap_printf2("UserAppCecEvt_Listener callback\n");

	stCECEvt_t *e = Q_NEW(stCECEvt_t, CEC_EVT_SIG);

	memcpy(&e->stCecInfo, (stCecCmdInfo_t * )pParam, sizeof(stCecCmdInfo_t));

	QACTIVE_POST(me, &e->stEvt, me);
}


void UserAppDspCodecInfo_RSR_Listener(QActive * const me,void * pParam)
{
	ap_printf("UserAppDspCodecInfo_RSR_Listener callback\n");
	(void)me;
	(void)pParam;
	QACTIVE_POST((QActive *)UserApp_get(), Q_NEW(QEvt, RSR_AudFmt_CHANGED_SIG), (void *)NULL);
}

/******************************************************************************************/
/**
 * \fn           void UserAppDspCodecInfo_Listener(QActive * const me, void *pParam)

 *
 * \brief      function for listener regist
 *
 * \param   QActive * const me:(Input) AO handler ,\n
 *               void *pParam : audio info structure pointer (structure type: stSYS_AudioInfo_t)
 *
 * \return	    none.
 *
 *\writer		chihhao.chang: for showing audio format in VFD
 ******************************************************************************************/
void UserAppDspCodecInfo_Listener(QActive * const me, void *pParam)
{
	UserApp *const pstUsr = &l_UserApp;
    UINT32 codec_id=0,proc_type=0;
	extern void vTaskDelay();
    (void)me;
    Dsp_Format_Info_t *pUserappDspCodecInfo = (Dsp_Format_Info_t*)pParam;
    codec_id = pUserappDspCodecInfo->Audio_Format;
	proc_type = pUserappDspCodecInfo->Audio_type;
	pstUsr->codecType = codec_id;
    if(codec_id)
    {
        switch((codec_id & 0xff))
        {
	        case FORMAT_PCM:
	        {
	           // VFD_Service_POP(&me->super ,"PCM",3,CENTER);
		    	ap_printf("(PCM)\n");
	            break;
	        }
	        case FORMAT_PCM_MCH:
	        {
	         //   VFD_Service_POP(&me->super ,"MPCM",3,CENTER);
				ap_printf("(M-PCM)\n");
	            break;
	        }
	        case FORMAT_DOLBY_DIGITAL:
			case FORMAT_DOLBY_DIGITAL_EX:
	        {
	       //     VFD_Service_POP(&me->super ,"DD",3,CENTER);
				ap_printf("(AC3)\n");

	            break;
	        }
			case FORMAT_DD_PLUS:
	        {
	       //     VFD_Service_POP(&me->super ,"DDP",3,CENTER);
				ap_printf("(EC3)\n");
	            break;
	        }
			case FORMAT_DD_TRUEHD:
	        {
	       //     VFD_Service_POP(&me->super ,"TRUEHD",3,CENTER);
				ap_printf("(TRUEHD)\n");
	            break;
	        }
			case FORMAT_MAT:
	        {
	       //     VFD_Service_POP(&me->super ,"MAT",3,CENTER);
				ap_printf("(MAT)\n");
	            break;
	        }
			case FORMAT_DTS:
			case FORMAT_DTS_9624:
			case FORMAT_DTS_ES_M61:
			case FORMAT_DTS_ES_D61:
			case FORMAT_DTS_ES_D8:
	        {
	       //     VFD_Service_POP(&me->super ,"DTS",3,CENTER);
				ap_printf("(DTS)\n");
	            break;
	        }
			case FORMAT_DTS_HIRES:
			case FORMAT_DTS_MA:
			case FORMAT_DTS_LBR:
			case FORMAT_DTS_LOSSLESS:
	        {
	       //     VFD_Service_POP(&me->super ,"DTSHD",3,CENTER);
				ap_printf("(DTSHD)\n");
	            break;
	        }
			case FORMAT_DTS_UHD:
			case FORMAT_DTS_UHD_MA:
			case FORMAT_DTS_UHD_LOSSLESS:
			case FORMAT_DTS_UHD_GAME:
	        {
	       //     VFD_Service_POP(&me->super ,"DTSX",3,CENTER);
				ap_printf("(DTSX)\n");
	            break;
	        }

	        default:
	            ap_printf("(unknown %x)\n",((codec_id & 0xff)));
	            break;

        }
        //VFD_Service_Align(&me->super, CENTER);
        //VFD_Service_Constant(&me->super);
    }
	if(proc_type &&
		((codec_id==FORMAT_DOLBY_DIGITAL) ||
		(codec_id==FORMAT_DOLBY_DIGITAL_EX) ||
		(codec_id==FORMAT_DD_PLUS) ||
		(codec_id==FORMAT_DD_TRUEHD) ||
		(codec_id==FORMAT_MAT))
		)
    {
        switch((proc_type & 0xff00))
        {
	        case IS_DOLBY_SURROUND:
	        {
		//		vTaskDelay(3000);
	       //     VFD_Service_POP(&me->super ,"SURROUND",3,CENTER);
		    	ap_printf("PROC TYPE (SURROUND)\n");
	            break;
	        }
	        case IS_DOLBY_ATMOS:
	        {
		//		vTaskDelay(3000);
	       //     		VFD_Service_POP(&me->super ,"ATMOS",3,CENTER);
				ap_printf("PROC TYPE (ATMOS)\n");
	            break;
	        }
        }
        //VFD_Service_Align(&me->super, CENTER);
        //VFD_Service_Constant(&me->super);
    }

    ap_printf("PGL UserAppAudioInfo_Listener callback out\n");
}

/******************************************************************************************/
/**
 * \fn           void UserAppDspCodecInfoFromDspC_Listener(QActive * const me, void *pParam)

 *
 * \brief      function for listener regist
 *
 * \param   QActive * const me:(Input) AO handler ,\n
 *               void *pParam : audio info structure pointer (structure type: stSYS_AudioInfo_t)
 *
 * \return	    none.
 *
 *\writer
 ******************************************************************************************/


void UserAppDspCodecInfoFromDspC_Listener(QActive * const me, void *pParam)
{
	UserApp *const pstUsr = &l_UserApp;
    UINT32 codec_id=0,proc_type=0;
	extern void vTaskDelay();
    (void)me;
    Dsp_Format_Info_t *pUserappDspCodecInfo = (Dsp_Format_Info_t*)pParam;
    codec_id = pUserappDspCodecInfo->Audio_Format;
	proc_type = pUserappDspCodecInfo->Audio_type;
	pstUsr->codecType = codec_id;

    if(codec_id)
    {
        switch((codec_id))
        {
	        case ID_DECODER_PCM:
	        {
	            VFD_Service_POP(&me->super ,"PCM",3,CENTER);
		    	ap_printf("(PCM)\n");
	            break;
	        }


	        case ID_DECODER_MPEG:
	        {
	            VFD_Service_POP(&me->super ,"MP3",3,CENTER);
		    	ap_printf("(MP3)\n");
	            break;
	        }

			case ID_DECODER_DDP:
	        {
	            VFD_Service_POP(&me->super ,"DDP",3,CENTER);
				ap_printf("(EC3)\n");
	            break;
	        }


			case ID_DECODER_AAC:
	        {
	            VFD_Service_POP(&me->super ,"AAC",3,CENTER);
				ap_printf("(AAC)\n");
	            break;
	        }


			case ID_DECODER_HEAAC:
	        {
	            VFD_Service_POP(&me->super ,"HEAAC",3,CENTER);
				ap_printf("(HE-AAC)\n");
	            break;
	        }

			case ID_DECODER_WMA:
	        {
	            VFD_Service_POP(&me->super ,"WMA",3,CENTER);
				ap_printf("(WMA)\n");
	            break;
	        }

			case ID_DECODER_OGG:
	        {
	            VFD_Service_POP(&me->super ,"OGG",3,CENTER);
				ap_printf("(OGG)\n");
	            break;
	        }

			case ID_DECODER_APE:
	        {
	            VFD_Service_POP(&me->super ,"APE",3,CENTER);
				ap_printf("(APE)\n");
	            break;
	        }

			case ID_DECODER_ALAC:
	        {
	            VFD_Service_POP(&me->super ,"ALAC",4,CENTER);
				ap_printf("(ALAC)\n");
	            break;
	        }
			case ID_DECODER_DSD:
	        {
	            VFD_Service_POP(&me->super ,"DSD",3,CENTER);
				ap_printf("(DSD)\n");
	            break;
	        }


			case ID_DECODER_FLAC:
	        {
	            VFD_Service_POP(&me->super ,"FLAC",4,CENTER);
				ap_printf("(FLAC)\n");
	            break;
	        }


	        default:
	            ap_printf("(unknown 0x%x)\n",codec_id);
	            break;

        }
        //VFD_Service_Align(&me->super, CENTER);
        //VFD_Service_Constant(&me->super);
    }


    ap_printf("PGL UserAppAudioInfo_Listener callback out\n");
}



/******************************************************************************************/
/**
 * \fn          void Userapp_BurnTestParameterInit(void)
 *
 * \brief      function to initial burn test parameter when burn test is enabled
 *
 * \param
 *
 *
 * \return	    none.
 *
 *
 ******************************************************************************************/
void Userapp_BurnTestParameterInit(QActive *me)
{
	BurnTest_SwitchSrcPowerOnOff stBurnTest_SwitchSrcPowerOnOff;
	int iRe = -1;
	iRe = Power_ReadBurnTestParameter(&stBurnTest_SwitchSrcPowerOnOff);

	if (iRe == 0)
	{
		Burn_AutoSrcTestEvt_t *e = Q_NEW(Burn_AutoSrcTestEvt_t, AUTOSRC_TEST_OPEN_SIG);

		e->power_off_time = stBurnTest_SwitchSrcPowerOnOff.power_off_time;
		e->power_on_time = stBurnTest_SwitchSrcPowerOnOff.power_on_time;
		e->switch_src_time = stBurnTest_SwitchSrcPowerOnOff.switch_src_time;

		ap_printf("e->switch_src_time = %d\n", e->switch_src_time);
		ap_printf("e->power_off_time = %d\n", e->power_off_time);
		ap_printf("e->power_on_time = %d\n", e->power_on_time);

		QACTIVE_POST(me, (QEvt *)e, (void *)0);
	}
	else if (iRe == 1)
	{
		ap_printf("Burn Magic Number is not equal to 0x67899876\n");
	}
	else
	{
		ap_printf("Burn Test Read Parameter Error\n");
	}
}

/******************************************************************************************/
/**
 * \fn			void UserApp_PrintIRKeySignal(UserApp * const me)
 *
 * \brief		API for user to printf IR key signal
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *
 * \return
 *
 ******************************************************************************************/

void UserApp_PrintIRKeySignal(int IR_key_sig_num)
{
	if ((IR_key_sig_num >= CUSTOMER_PRIVATE_SIG_START)
			&& (IR_key_sig_num <= CUSTOMER_KEY_SIG_MAX))
	{
		ap_printf("IR Signal name = %s\n", KeySigTable[IR_key_sig_num - CUSTOMER_PRIVATE_SIG_START]);
	}
	else
	{
		ap_printf_E("This Signal is not belog to IR key , Plz confirm...\n");
	}

	return ;
}

void UserAppFactoryReset(UserApp * const me)
{
	ap_printf("UserAppFactoryReset Start!!!\n");
		QState status = 0;
	int ret = 0;

	// step 1 Display RESET for 5s 
	UserAppDisplay(me, STR_FACTORY_RESET);

	if (ret < 0)
	{
		ap_printf("Factory reset Volume to default fail!!\n");
	}
	
	// step 4 Clear BT pairing info
	UartApp_SetKey_Sig(&me->super, RESET_RELEASE_SIG, me->iSrc);
	
	// step 5 EQ mode reset to Movie 
	UserAppEQSet((QActive *)me, eApEQ_Mode_0); // //--{{ Modified by Zanchen  at [20210205] for set EQ mode to Movie when factory reset happen��]

	// step 6 Other settings is in default mode
	UserSetting_RestoreDefault();
	
	// Step 7 Source reset to ARC
	UserSetting_SaveSrc(APP_SRC_ARC);// //--{{ Modified by Zanchen  at [20210205] for set src to ARC

	status = Q_HANDLED();
	return ;
}
/*
 * userapp source quick jump.
 */
QState UserAppSrcJump(UserApp * const me, QEvt const * const e, int iSrc)
{
	(void)e;
    QState status = 0;
	if(iSrc != me->iSrc){
		ap_printf("Switch to source: %d...\n", iSrc);

		if(me->iSrc == APP_SRC_USB/* || me->iSrc == eAp_CARD_Src*/){
	#ifdef SUPPORT_AP
			if (me->playercreat == 1){
            	MFP_Destroy(me->pAPInstance);
            	me->playercreat = 0;
            	me->pAPInstance = NULL;
            }
	#endif

			UserAppMute(me,1);
			me->showErr = 0;
			status = Q_HANDLED();
		}
		else{
        	status = UserAppTranSrc(me, me->aUserSrc_tbl[iSrc]);
		}

		me->iSrc = iSrc;
		UserSetting_SaveSrc(me->iSrc);
		/* bt ready to open
#ifdef USERAPP_SUPPORT_BT
		if (me->pBtBakcGroundApp != NULL)
		{
			QMSM_DISPATCH(&me->pBtBakcGroundApp->super, e);
		}
#endif
		*/
	}
	else{
		ap_printf("In current source ,not jump source...\n");
		status = Q_HANDLED();
	}

	return status;
}

#if (SUPPORT_VA)
void AuxVoiceAssistOn(UserApp * const me)
{
	if (!me->AuxVA_Status)
	{
		ap_printf("aux va status is off");
	}
	else
	{
		VoiceAssistService_VAStatus(NULL, me->AuxVA_Status, me->BtVA_Status);
	}
}

void AuxVoiceAssistOff(UserApp * const me)
{
	if (me->AuxVA_Status)
	{
		ap_printf("aux va status is on");
	}
	else
	{
		VoiceAssistService_VAStatus(NULL, me->AuxVA_Status, me->BtVA_Status);
	}
}

void AuxVoiceAssistSw(UserApp * const me)
{
	ap_printf("AUX VA on/off\n");

	me->AuxVA_Status = !me->AuxVA_Status;
	ap_printf("me->AuxVA_Status = %d\n", me->AuxVA_Status);
	UserSetting_SaveVAStatus(me->AuxVA_Status, me->BtVA_Status);

	if(me->AuxVA_Status)
	{
		AuxVoiceAssistOn(me);
	}
	else
	{
		AuxVoiceAssistOff(me);
	}
}

void BtVoiceAssistOn(UserApp * const me)
{
	if (!me->BtVA_Status)
	{
		ap_printf("bt va status is off");
	}
	else
	{
		VoiceAssistService_VAStatus(NULL, me->AuxVA_Status, me->BtVA_Status);
	}
}

void BtVoiceAssistOff(UserApp * const me)
{
	if (me->BtVA_Status)
	{
		ap_printf("bt va status is on");
	}
	else
	{
		VoiceAssistService_VAStatus(NULL, me->AuxVA_Status, me->BtVA_Status);
	}
}

void BtVoiceAssistSw(UserApp * const me)
{
	ap_printf("BT VA on/off\n");

	me->BtVA_Status = !me->BtVA_Status;
	ap_printf("me->BtVA_Status = %d\n", me->BtVA_Status);
	UserSetting_SaveVAStatus(me->AuxVA_Status, me->BtVA_Status);

	if(me->BtVA_Status)
	{
		BtVoiceAssistOn(me);
	}
	else
	{
		BtVoiceAssistOff(me);
	}
}

void AuxBtVAOn(UserApp * const me)
{
	ap_printf("AUX and BT VA on\n");

	ap_printf("AUX VA: %d, BT VA: %d\n", me->AuxVA_Status, me->BtVA_Status);

	me->AuxVA_Status = VA_ON;
	me->BtVA_Status = VA_ON;
	UserSetting_SaveVAStatus(me->AuxVA_Status, me->BtVA_Status);
	VoiceAssistService_VAStatus(NULL, me->AuxVA_Status, me->BtVA_Status);
}

void AuxBtVAOff(UserApp * const me)
{
	ap_printf("AUX and BT VA off\n");

	ap_printf("AUX VA: %d, BT VA: %d\n", me->AuxVA_Status, me->BtVA_Status);

	if(me->AuxVA_Status || me->BtVA_Status)
	{
		me->AuxVA_Status = VA_OFF;
		me->BtVA_Status = VA_OFF;
		UserSetting_SaveVAStatus(me->AuxVA_Status, me->BtVA_Status);
		VoiceAssistService_VAStatus(NULL, me->AuxVA_Status, me->BtVA_Status);
	}
	else
	{
		ap_printf("va is disable %d\n");
	}
}

void UserApp_WirelessUpgDisplay(QActive * const me, void *pParam)
{


	BYTE strBuf[8] = {0};
	UINT16 upgrage_status = *(UINT16 *)pParam;
	BYTE type = upgrage_status>>8;
	BYTE percent = upgrage_status & 0xff;

	if(type == WIRELESS_UPG_TX)
	{
		if(percent == WL_UPG_OK)
			sprintf(strBuf, "TX OK");
		else if(percent == WL_UPG_NG)	 
			sprintf(strBuf, "TX NG");
		else if(percent == 100)
			sprintf(strBuf, "TX%03d", percent);
		else if(percent > 9)
			sprintf(strBuf, "TX %02d", percent);
		else
			sprintf(strBuf, "TX  %01d", percent);
	}
	else if(type == WIRELESS_UPG_RX_SURROUND)
	{
		if(percent == WL_UPG_OK)
			sprintf(strBuf, "RX OK");
		else if(percent == WL_UPG_NG)	 
			sprintf(strBuf, "RX NG");
		else if(percent == 100)
			sprintf(strBuf, "RX%03d", percent);
		else if(percent > 9)
			sprintf(strBuf, "RX %02d", percent);
		else
			sprintf(strBuf, "RX  %01d", percent);
	}
	else if(type == WIRELESS_UPG_EEPROM_SURROUND)
	{
		if(percent == WL_UPG_OK)
			sprintf(strBuf, "EE OK");
		else if(percent == WL_UPG_NG)	 
			sprintf(strBuf, "EE NG");
		else if(percent == 100)
			sprintf(strBuf, "EE%03d", percent);
		else if(percent > 9)
			sprintf(strBuf, "EE %02d", percent);
		else
			sprintf(strBuf, "EE  %01d", percent);
	}
	else if(type == WIRELESS_UPG_RX_SUBWOOFER)
	{
		if(percent == WL_UPG_OK)
			sprintf(strBuf, "RS OK");
		else if(percent == WL_UPG_NG)	 
			sprintf(strBuf, "RS NG");
		else if(percent == 100)
			sprintf(strBuf, "RS%03d", percent);
		else if(percent > 9)
			sprintf(strBuf, "RS %02d", percent);
		else
			sprintf(strBuf, "RS  %01d", percent);
	}
	else if(type == WIRELESS_UPG_EEPROM_SUBWOOFER)
	{
		if(percent == WL_UPG_OK)
			sprintf(strBuf, "ES OK");
		else if(percent == WL_UPG_NG)	 
			sprintf(strBuf, "ES NG");
		else if(percent == 100)
			sprintf(strBuf, "ES%03d", percent);
		else if(percent > 9)
			sprintf(strBuf, "ES %02d", percent);
		else
			sprintf(strBuf, "ES  %01d", percent);
	}	
	VFD_Service_Display(me, strBuf);
	VFD_Service_Align(me, CENTER);
	VFD_Service_Constant(me);
}

void UserApp_SetAuxMute(void)
{
	VoiceAssistService_SetAuxMute(NULL);
}

void UserApp_SetAuxResume(void)
{
	VoiceAssistService_SetAuxResume(NULL);
}

void UserApp_SetBTMute(void)
{
	VoiceAssistService_SetBTMute(NULL);
}

void UserApp_SetBTResume(void)
{
	VoiceAssistService_SetBTResume(NULL);
}
#endif

/******************************************************************************************/
/**
 * \fn			QState UserApp_home(UserApp * const me, QEvt const * const e)
 *
 * \brief		userapp home takes charge of key signal receiver
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_active state \n
 *			    \e status : Q_TRAN() trans to other sub_state \n
 *
 ******************************************************************************************/

void UserApp_MuteCnt(UserApp *const me)
{
	if(me->mute_cnt > 0)
	{
		me->mute_cnt--;
		if(me->mute_cnt == 0)
		{
			ap_printf(" %s\n ", __func__);
			UserAppMute(me,me->mute);
		}
	}
}


UINT32 UserApp_GetAudioFormat(UserApp *const me)
{
	UINT32 dCh_Num=0, dSampleRate=0, dFormat=0;
	UINT32 cur_format = AUDIO_FORMAT_DISPLAY_MAX;
	static UINT32 last_format = 0;
	static UINT8 last_ch;

	if((me->iSrc == APP_SRC_OPT || me->iSrc == APP_SRC_ARC|| me->iSrc == APP_SRC_HDMI1 || me->iSrc == APP_SRC_COAX || me->iSrc == APP_SRC_HDMI2))
	{
		// Get Audio Information
		SpHdsp_getAudioFormat(&dCh_Num, &dSampleRate, &dFormat);

		//if(dFormat != FORMAT_NONE)
		//	ap_printf(" audio format 0x%x, sample %d, ch num %d \n ", dFormat, dSampleRate, dCh_Num);

		dFormat = dFormat&0xffff;
		
		// Format
		if(dFormat & IS_DOLBY_ATMOS)
		{
			// Dolby Atmos Audio
			cur_format = AUDIO_FORMAT_DISPLAY_ATMOS;
		}
		else if(dFormat & IS_DOLBY_SURROUND)
		{
			// Dolby Surround Decode
			cur_format = AUDIO_FORMAT_DISPLAY_DOLBY_SURROUND;
		}
		else if(dFormat == FORMAT_DOLBY_DIGITAL || dFormat == FORMAT_DOLBY_DIGITAL_EX || dFormat == FORMAT_DD_PLUS|| dFormat == FORMAT_DD_TRUEHD || dFormat == FORMAT_MAT)
		{
			// Dolby Audio
			cur_format = AUDIO_FORMAT_DISPLAY_DOLBY_AUDIO;
		}
		else if(dFormat == FORMAT_PCM || dFormat == FORMAT_PCM_MCH)
		{
			// PCM Audio
			cur_format = AUDIO_FORMAT_DISPLAY_PCM;
		}
		else if(dFormat == FORMAT_NONE)
		{
			// Other Audio
			cur_format = AUDIO_FORMAT_DISPLAY_NONE;
		}
		else 
		{
			// Other Audio
			cur_format = AUDIO_FORMAT_DISPLAY_OTHER;
		}

		if(cur_format != AUDIO_FORMAT_DISPLAY_NONE && cur_format != AUDIO_FORMAT_DISPLAY_OTHER)
		{
			//if(input_map & C_CH)
			if(cur_format == AUDIO_FORMAT_DISPLAY_ATMOS)
			{
				me->channel_num = eInput_bypass; 
			}
			else if(dCh_Num == 2)
			{
				me->channel_num = eInput_2p0_ch; // other 
			}
			else
			{
				me->channel_num = eInput_5p1_ch; // 2.0
			}
			
			if(last_ch != me->channel_num)
			{
				last_ch = me->channel_num;

				AudDspService_Set_AppMode(&me->super, SET_MMIX, GRP_MMIX0, MODE1+me->channel_num);
		
				ap_printf(" audio channel num change %d\n ", dCh_Num);
			}
		}

		me->spdif_format = cur_format;

		if(cur_format != last_format)
		{
			// to do format display or other
			last_format = cur_format;
			ap_printf(" audio format change %d\n ", cur_format);

			if(cur_format != AUDIO_FORMAT_DISPLAY_NONE && cur_format != AUDIO_FORMAT_DISPLAY_OTHER)
			{
			
				//UserApp_SetMix(me, me->channel_num); 
			}
		}
		else
		{
			cur_format = AUDIO_FORMAT_DISPLAY_MAX;
		}

	}
	else
	{
		me->spdif_format = AUDIO_FORMAT_DISPLAY_PCM;

		if(me->channel_num != eInput_2p0_ch)
		{
			ap_printf(" other source set 2.0 mix %d\n ", me->channel_num);
			AudDspService_Set_AppMode(&me->super, SET_MMIX, GRP_MMIX0, MODE1);
		}
		me->channel_num = eInput_2p0_ch;
		last_ch = eInput_2p0_ch;
	}

	return cur_format;
}


void UserApp_DisplayAudioFormat(UserApp *const me, UINT32 cur_format)
{
	ap_printf(" current format %d \n ", cur_format);

		UserApp_DimmerSet(me);

		if(cur_format == AUDIO_FORMAT_DISPLAY_ATMOS)
		{
			UserApp_ScrollBackUp_ShowSource(me, STR_DISPLAY_ATMOS);
		}
		else if(cur_format == AUDIO_FORMAT_DISPLAY_DOLBY_SURROUND)
		{
			UserApp_ScrollBackUp_ShowSource(me, STR_DISPLAY_DOLBY_SURROUND);
		}
		else if(cur_format == AUDIO_FORMAT_DISPLAY_DOLBY_AUDIO)
		{
			UserApp_ScrollBackUp_ShowSource(me, STR_DISPLAY_DOLBY_AUDIO);
		}
		else if(cur_format == AUDIO_FORMAT_DISPLAY_PCM)
		{
			UserAppDisplayOnce(me, "PCM ", 3);
		}
		else if(cur_format == AUDIO_FORMAT_DISPLAY_NONE)
		{
			ap_printf(" no signal!\n ");
		}
		else if(cur_format == AUDIO_FORMAT_DISPLAY_OTHER)
		{
			UserAppDisplayOnce(me, "ERR ", 3);
		}
		else
		{
			//VFD_Service_POP(me, "ERROR", 3, RIGHT);
		}
		
	
}



void UserApp_SignalDetect(UserApp *const me)
{
	#define IDLE_STANDBY_COUNT 15*60*5
	
	BYTE no_signal = 0;
	//static int j = 0;
	uint32_t ret_eng_vec[2] = {0};
	static BYTE last_src;

	(void)me;
	AudDspService_Get_InputEnergy(&ret_eng_vec[0], &ret_eng_vec[1]);

	if(last_src != me->iSrc)
	{
		last_src = me->iSrc;
		me->no_signal_detect_cnt = 0;
		me->have_signal_detect_cnt = 0;
		me->have_signal_flag = 0; 
	}


	switch(me->iSrc)
	{
		case APP_SRC_AUX:
			//if(ret_eng_vec[0] < 2400 && ret_eng_vec[1] < 2400)
			if(ret_eng_vec[0] < 10500 && ret_eng_vec[1] < 10500)
			{
				me->signal_mute_cnt = 5;
				no_signal = 1;
			}
			else
			{
				me->signal_demute_cnt = 5;
				no_signal = 0;
			}
			break;
		case APP_SRC_BT:
			if(me->bt_play == 0)
			{
				me->signal_mute_cnt = 5;
				no_signal = 1;
			}
			else
			{
				me->signal_demute_cnt = 5;
				no_signal = 0;
			}
			break;
		case APP_SRC_USB:
			if(ret_eng_vec[0] < 100 && ret_eng_vec[1] < 100)
			{
				me->signal_mute_cnt = 5;
				no_signal = 1;
			}
			else
			{
				me->signal_demute_cnt = 5;
				no_signal = 0;
			}
			break;
		case APP_SRC_ARC:
			if(ret_eng_vec[0] < 100 && ret_eng_vec[1] < 100)
			{
				me->signal_mute_cnt = 5;
				no_signal = 1;
			}
			else
			{
				me->signal_demute_cnt = 5;
				no_signal = 0;
			}
			break;
		case APP_SRC_HDMI1:
			if(ret_eng_vec[0] < 100 && ret_eng_vec[1] < 100)
			{
				me->signal_mute_cnt = 5;
				no_signal = 1;
			}
			else
			{
				me->signal_demute_cnt = 5;
				no_signal = 0;
			}
			break;
		case APP_SRC_HDMI2:
			if(ret_eng_vec[0] < 100 && ret_eng_vec[1] < 100)
			{
				me->signal_mute_cnt = 5;
				no_signal = 1;
			}
			else
			{
				me->signal_demute_cnt = 5;
				no_signal = 0;
			}
			break;
		case APP_SRC_OPT:
			if(ret_eng_vec[0] < 100 && ret_eng_vec[1] < 100)
			{
				me->signal_mute_cnt = 5;
				no_signal = 1;
			}
			else
			{
				me->signal_demute_cnt = 5;
				no_signal = 0;
			}
			break;
		case APP_SRC_COAX:
			if(ret_eng_vec[0] < 100 && ret_eng_vec[1] < 100)
			{
				me->signal_mute_cnt = 5;
				no_signal = 1;
			}
			else
			{
				me->signal_demute_cnt = 5;
				no_signal = 0;
			}
			break;
			
		default:
			return;
	}

	//if (j ++ %10 ==0)
	{
		//ap_printf(" audio signal engery %d-%d %d\n ", ret_eng_vec[0], ret_eng_vec[1], me->idle_power_standby_count);		
	}

	if(no_signal)
	{	
		if(me->no_signal_detect_cnt < me->signal_mute_cnt)  //5s no signal source vfd blink
		{
			me->no_signal_detect_cnt++;

			if(me->no_signal_detect_cnt == me->signal_mute_cnt)
			{
				me->have_signal_detect_cnt = 0;

				LOGD(" << no signal MUTE  >>  \n");
	
				me->have_signal_flag = 0; 

			}
		}

		if (me->idle_power_standby_count++ == IDLE_STANDBY_COUNT)
		{		
			ap_printf("System idle 15 min  direct enter standby!!!!!! ");		
			QACTIVE_POST(&me->super, Q_NEW(QEvt, POWER_OFF_SIG), (void *)0);
		}
	}
	else
	{
		if(me->have_signal_detect_cnt < me->signal_demute_cnt) 
		{
			me->have_signal_detect_cnt++;

			if(me->have_signal_detect_cnt == me->signal_demute_cnt) 
			{
				me->no_signal_detect_cnt = 0;

				me->have_signal_flag = 1; 
				
				LOGD(" << have signal DEMUTE  >>\n");

				
			}
		}

		me->idle_power_standby_count = 0;
	}
	

}


void UserApp_WirelessPairTimer(UserApp *const me)
{
	static BYTE flag = 1;

	if(flag)
	{
		flag = 0;

		if(wireless_get_syn() == 0)
		{
			me->wl_pair_cnt = 3*60*5;
			UserAppDisplay_Scroll(me, STR_PAIRING, R2L);
		}
	}

	if(me->wl_pair_cnt > 0)
	{
		me->wl_pair_cnt--;

		if(me->wl_pair_cnt == 3*60*5)
		{
			if(wireless_get_syn() == 0)
				wireless_pair(1);
		}
		else if(me->wl_pair_cnt == 0)
		{
			if(wireless_get_syn() == 0)
				UserAppDisplayOnce(me , "FAIL", 3);
		}
	}
	
}



enum
{
	PW_NUM_KEY_VALUE = 5,
	PW_NUM_COUNT,
	PW_NUM_KEY_NUM,
	PW_NUM_FUNC,
	PW_NUM_MAX,
};


typedef enum _IR_PASSWORD
{
	PW_UPG,	
	PW_MCU_UPG,
	PW_VER,
	PW_MAX

}IR_PASSWORD;


#define PW_FUNC_MAX	PW_MAX


static UINT16 PassWord[PW_FUNC_MAX][PW_NUM_MAX] =  // the password[][6]  is the count, the password[][7]  is the key number
{	
		{IRC_PREV, IRC_PREV, IRC_NEXT, IRC_NEXT, IRC_PLAY_PAUSE, IRC_NULL ,0, 5, PW_UPG},
		{IRC_PREV, IRC_PREV,IRC_PLAY_PAUSE, IRC_NEXT, IRC_NEXT,  IRC_NULL ,0, 5, PW_MCU_UPG},

		{IRC_NEXT, IRC_PLAY_PAUSE, IRC_PREV, IRC_NULL,IRC_NULL,  IRC_NULL ,0, 3, PW_VER},
};

static void UserApp_ClearPassword_Key(void)
{
	UINT16 (*password)[PW_NUM_MAX] = PassWord;
	BYTE i=0;

	//ap_printf(" UserApp_ClearPassword_Key\n ");
	
	for(i=0; i<PW_FUNC_MAX; i++)
		password[i][PW_NUM_COUNT] = 0;
}


int UserApp_Password(UserApp * const me, int rx)
{
	BYTE i=0;
	UINT16 (*password)[PW_NUM_MAX] = PassWord;
	int ret = 0;
	BYTE strbuf[10];

	// only in aux source and within 3s can check the password
	
	for(i=0; i<PW_FUNC_MAX; i++)
	{
		//ap_printf(" rx %d, pw %d\n ", rx, password[i][password[i][PW_NUM_COUNT]]);
	
		if(rx == password[i][ password[i][PW_NUM_COUNT] ])
		{
			password[i][PW_NUM_COUNT]++;	
		}
		else
		{
			password[i][PW_NUM_COUNT]=0;
		}
		
		//ap_printf(" conut %d, num %d\n ", password[i][PW_NUM_COUNT], password[i][PW_NUM_KEY_NUM]);

		if(password[i][PW_NUM_COUNT] == password[i][PW_NUM_KEY_NUM] && password[i][PW_NUM_KEY_NUM] != 0)
		{

			ap_printf(" get the password index : %d\n ", i);
		
			UserApp_ClearPassword_Key();
			
			switch(password[i][PW_NUM_FUNC])
			{
			
				case PW_UPG:
					ap_printf(" src : %d\n ", me->iSrc);
					if(me->iSrc == APP_SRC_AUX)
					{
						mount("USB", "USB:", "fatfs");
					
						BYTE UPG_SPA100[] = {"USB:spupdate.bin"};
						int upg_fd = open(UPG_SPA100, O_RDONLY, 0);
						ap_printf(" upg_fd : %d\n ", upg_fd);
						if(upg_fd > 0)
						{
							close(upg_fd);
							QACTIVE_POST(&me->super, Q_NEW(QEvt, APP_UPG_SIG), (void *)0);
							ret = 1;
						}
					}
					break;

				case PW_VER:
					if(me->iSrc == APP_SRC_AUX)
					{
						sprintf(strbuf, "V %02d", AND_VER);
						UserAppDisplayOnce(me, strbuf, 3);
						ret = 1;
					}
					break;

				case PW_MCU_UPG:
					ap_printf(" src : %d\n ", me->iSrc);
					if(me->iSrc == APP_SRC_AUX)
					{
						mount("USB", "USB:", "fatfs");
					
						BYTE UPG_SPA100[] = {"USB:MCU.bin"};
						int upg_fd = open(UPG_SPA100, O_RDONLY, 0);

						if(upg_fd > 0)
						{
							close(upg_fd);
							QACTIVE_POST(&me->super, Q_NEW(QEvt, MCU_UPG_SIG), (void *)0);
							ret = 1;
						}
					}
					

					
					break;

						
				default:
					return 0;
			}

			return ret;
		}

		if(password[i][PW_NUM_COUNT] > 0)
		{
			QTimeEvt_rearm(&me->timePassword, TICKS_PER_SEC*2);
		}
	}

	return ret;
}


void UserApp_DimmerSet(UserApp *const me)
{
	if(me->DIMMER == DIMMER_OFF)
	{
		vfd_dimmer(DIMMER_HIGH);
		me->dimmer_cnt = 3*5; // 3s
	}
}

void UserApp_DimmerTimeout(UserApp *const me)
{
	if(me->DIMMER == DIMMER_OFF)
	{
		if(me->dimmer_cnt > 0)
		{
			me->dimmer_cnt--;
			if(me->dimmer_cnt == 0)
			{
				vfd_dimmer(DIMMER_OFF);
			}
		}
	}
	
}

QState UserApp_home(UserApp *const me, QEvt const *const e)
{
#ifdef BUILT4ISP2
	ap_printf("%s: %d, sig: %d\n", __FUNCTION__, __LINE__, (int)e->sig);
#endif

	QState status = 0;
	
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			ap_BurnTest_log("ENTRY\n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			ap_BurnTest_log("EXIT\n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			ap_BurnTest_log("INIT\n");
			QTimeEvt_ctorX(&me->timeSrvAOReadyEvt, &me->super, SRV_AO_READY_SIG, 0U);
			QTimeEvt_armX(&me->timeSrvAOReadyEvt, (TICKS_PER_SEC / 100), 0);
			//--{{ Added by Grace for setting menu timeout event
			QTimeEvt_ctorX(&me->timeSettingMenu, &me->super, SETTING_MENU_TIMER_SIG, 0U);
			status =  Q_HANDLED();
			break;
		}
		case SRV_AO_READY_SIG:
		{
			ap_printf("[%s]@@@@@@@ SRV_AO_READY_SIG @@@@@@@; \n", __FUNCTION__);
			QTimeEvt_disarm(&me->timeSrvAOReadyEvt);
			UserAppMute(me, 1);

			UserAppEQSet((QActive *)me, me->EQMode);
			UserAppSurroundSet((QActive *)me, me->Surround_3D);

#ifdef USERAPP_SUPPORT_HDMI_CEC
			HDMISrv_RegHPDListener(&me->super, UserApp_HPDState_Listener);
#endif
			QTimeEvt_armX(&me->timeEvt_Watchdog, TICKS_PER_SEC * 3, 0);
#ifdef WD_TICK_ENABLE
			iop_wd_set_timeout(100);
			iop_wd_enable();
#endif

#ifndef SDK_RELEASE
			Userapp_BurnTestParameterInit(&me->super);
#endif

			first_power_up = First_Power_Standby();

			if(first_power_up == 1)
			{
				me->vol = 20;
				UserSetting_SaveVolume(me->vol);
				return Q_TRAN(&UserApp_standby);
			}
		

			status = Q_TRAN(&UserApp_active);
			break;
		}

		case PASSWORD_TIMEOUT_SIG:
			UserApp_ClearPassword_Key();
			ap_printf(" PASSWORD_TIMEOUT_SIG \n");
			status = Q_HANDLED();
			break;

		case WATCHDOG_TICK_SIG:
		{
			ap_printf2("WATCHDOG_TICK_SIG\n");
#ifdef WD_TICK_ENABLE
			iop_wd_tick();
#endif
#ifdef SYNCCOM_WIRELESS
			EV01SE_Polling();
#endif
			QTimeEvt_armX(&me->timeEvt_Watchdog, TICKS_PER_SEC * 3, 0);
			status = Q_HANDLED();
			break;
		}

		case IOP_KEY_SIG:
		{
			stUserKeyInfo_t stUserKeyInfo;
			int result = 0;

			//result = circBufPop(me->hKeyBuf,&stUserKeyInfo.stKeyInfo); // get 1 key only from key buf
			//result = UserAppGetSingleKey(me->hKeyBuf,&stUserKeyInfo.stKeyInfo);
			//result |= UserAppGetLatestKey(me->hKeyBuf,&stUserKeyInfo.stKeyInfo); // get the latest key in key buf
			result = UserAppGetLatestKey_SpecificSrc(eKeySrc_CEC_CMD, me->hKeyBuf, &stUserKeyInfo.stKeyInfo); // get the latest key in key buf but prioritize the key from specific src

			if (result == eRes_specific1) //key buf is not empty
			{
				ap_printf("Key eRes_specific1\n");
				QF_PUBLISH(Q_NEW(QEvt, IOP_KEY_SIG), me); // re-get key
			}

			if (result >= eRes_success)
			{
#ifdef AP_DBG
				UINT32 UserApp_KeySTC;
				extern unsigned int GetSTC(void);
				UserApp_KeySTC = GetSTC();
				UserApp_KeySTC -= stUserKeyInfo.stKeyInfo.Key_STCStamp;
				ap_printf("UserApp_KeySTC::%d\n", UserApp_KeySTC); // 20 ~ 60 ticks
#endif
				stUserKeyInfo.UserKeyCodeSrc = stUserKeyInfo.stKeyInfo.keysrc;
				result = UserAppKeyCodeConvert(me, &stUserKeyInfo);

#if 1//def AP_DBG
				//UserAppKeyInfo(me, stUserKeyInfo);
#endif

				result = UserAppKeyCode2Sig(me, &stUserKeyInfo);

				if(stUserKeyInfo.stKeyInfo.key_type == eKeyRelease )
				{	
					if(UserApp_Password(me, stUserKeyInfo.UserKeyCode) == 1)
					{
						status = Q_HANDLED();
		            			break;
					}

				}


				ap_BurnTest_log("KeyCode2Sig::%d\n", result);
				if (result != -1)
				{
					//UserApp_PrintIRKeySignal(result);
				}

				if (result != -1)
				{

#ifdef SUPPORT_HINT_SOUND
					/*if (stUserKeyInfo.stKeyInfo.key_type == eKeyPress) // issue key sound when key is pressed only
					{
						if (me->pstHintSound->DATA_TYPE == eType_PCM)
						{
							UserAppKeySound(&me->super, me->pstHintSound);
						}
					}*/
#endif
					UserAppKeyEvt *e = Q_NEW(UserAppKeyEvt, result);

					e->User_KeyCode = stUserKeyInfo.UserKeyCode;

					e->User_KeyCodeSrc = stUserKeyInfo.UserKeyCodeSrc;

					e->pstKeyInfo_t = &(stUserKeyInfo.stKeyInfo);

					me->idle_power_standby_count = 0;

					if(stUserKeyInfo.UserKeyCode != IRC_DIM)
						UserApp_DimmerSet(me);

					QACTIVE_POST(&me->super, (QEvt *)e, (void *)0);
				}
			}
			else
			{
				ap_printf("Key buf empty\n");
			}

			status = Q_HANDLED();
			break;
		}
#ifndef SDK_RELEASE
		case CUSTOM_TEST_AUTOSWITCHSRC_SIG:
		{
			ap_printf("[%s]CUSTOM_TEST_AUTOSWITCHSRC_SIG; \n", __FUNCTION__);
			QTimeEvt_armX(&me->timeSwitchSrcEvt, (me->stAutoSrcTest->switch_src_time)*TICKS_PER_SEC, 0);
			QACTIVE_POST(&me->super, Q_NEW(QEvt, SOURCE_SW_RELEASE_SIG), (void *)0);
			status = Q_HANDLED();
			break;
		}
		case CUSTOM_TEST_POWEROFF_SIG:
		{
			ap_printf("[%s]CUSTOM_TEST_POWEROFF_SIG; \n", __FUNCTION__);
			QTimeEvt_armX(&me->timePowerOffEvt, (me->stAutoSrcTest->power_off_time)*TICKS_PER_SEC, 0);
			status = Q_HANDLED();
			break;
		}
		case CUSTOM_TEST_AUTO_REPEAT_SIGNAL_SIG:
		{
			//Test_settingmanager();
			ap_printf("[%s]CUSTOM_TEST_AUTO_REPEAT_SIGNAL_SIG ;\n", __FUNCTION__);
			UserApp_PrintIRKeySignal(me->stAutoRepeatSignalTest->private_signal_number);
			UserAppMemoryCheck();
			QTimeEvt_armX(&me->timeRepeatSignalEvt, (me->stAutoRepeatSignalTest->private_signal_repeat_time)*TICKS_PER_SEC, 0);
			QACTIVE_POST(&me->super, Q_NEW(QEvt, me->stAutoRepeatSignalTest->private_signal_number), (void *)0);
			status = Q_HANDLED();
			break;
		}
#endif
		//for upgrade feature{
#ifdef BUILT4ISP2
		case SYSTEM_BUILT4ISP2_SIG:
		{
			ap_printf("\n\n\n%s: SYSTEM_BUILT4ISP2_SIG recived\n\n\n", __FUNCTION__);
			VFD_Service_Show_Dark(me);
			status = Q_TRAN(&UserApp_Upgrade);
			break;
		}
#endif

/*
		case POWER_TICK_SIG:
		{
			ap_printf("[%s] POWER_TICK_SIG ->\n", __FUNCTION__);

			if (me->UpgradeCnt < (me->UpgradeTime))
			{
				me->UpgradeCnt++;
				ap_printf("UpgradeCnt: %d; \n", me->UpgradeCnt);
				status = Q_HANDLED();
			}
			else if (me->UpgradeCnt == (me->UpgradeTime))
			{
				me->UpgradeCnt++;
				ap_printf("enter Upgrade mode; \n");
				VFD_Service_Show_Dark(me);
				status = Q_TRAN(&UserApp_Upgrade);
			}
			else
			{
				status = Q_HANDLED();
			}

			break;
		}
*/
		case POWER_LONG_RELEASE_SIG:
		{
			ap_printf("[%s] POWER_LONG_RELEASE_SIG ->\n", __FUNCTION__);

			me->UpgradeCnt = 0;

			status = Q_HANDLED();
			break;
		}
			//for upgrade feature}
#ifndef SDK_RELEASE
		case AUTOSRC_TEST_OPEN_SIG:
		{
			ap_printf("[%s] AUTOSRC_TEST_OPEN_SIG ->\n", __FUNCTION__);

			Burn_AutoSrcTestEvt_t *pAutoSrcTestEvt = ((Burn_AutoSrcTestEvt_t *)e) ;
			me->stAutoSrcTest->switch_src_time = pAutoSrcTestEvt->switch_src_time;
			me->stAutoSrcTest->power_off_time  = pAutoSrcTestEvt->power_off_time;
			me->stAutoSrcTest->power_on_time = pAutoSrcTestEvt->power_on_time;

			Power_WriteBurnTestParameter(me->stAutoSrcTest->switch_src_time, me->stAutoSrcTest->power_off_time, me->stAutoSrcTest->power_on_time);

			if (me->stAutoSrcTest->switch_src_time != 0)
			{
				QTimeEvt_disarm(&me->timeSwitchSrcEvt);
				QACTIVE_POST(&me->super, Q_NEW(QEvt, CUSTOM_TEST_AUTOSWITCHSRC_SIG), (void *)0);
			}

			if (me->stAutoSrcTest->power_off_time != 0)
			{
				QTimeEvt_disarm(&me->timePowerOffEvt);
				QACTIVE_POST(&me->super, Q_NEW(QEvt, CUSTOM_TEST_POWEROFF_SIG), (void *)0);
			}
			status = Q_HANDLED();
			break;
		}
		case BURN_TEST_CLOSE_SIG:
		{
			ap_printf("[%s] BURN_TEST_CLOSE_SIG ->\n", __FUNCTION__);

			int *psdiMagicBurnChk = ( int * )SRAM_ADDR_BURN_CHECK;
			int *pPowerOnCounter = ( int * )SRAM_ADDR_BURN_PWR_CNT;

			QTimeEvt_disarm(&me->timeSwitchSrcEvt);
			QTimeEvt_disarm(&me->timePowerOffEvt);
			QTimeEvt_disarm(&me->timeRepeatSignalEvt);

			CFG_PMC_T *pstCFG_PMC = GET_SDK_CFG(CFG_PMC_T);
			pstCFG_PMC->wRtcwakeupMode = 0;
			pstCFG_PMC->wRtcIntervalTime = 10;

			*psdiMagicBurnChk = 0x0;
			*pPowerOnCounter = 0;
			me->MonkeyTest_Enable = 0;
			me->Memory_Check = 0;
			status = Q_HANDLED();
			break;
		}

		case MONKEY_TEST_OPEN_SIG:
		{
			ap_printf("userapp MONKEY_TEST_OPEN_SIG enable\n");
			me->MonkeyTest_Enable = 1;
			status = Q_HANDLED();
			break;
		}
		case SYSTEM_MEMORY_CHECK_OPEN_SIG:
		{
			ap_printf("[%s] SYSTEM_MEMORY_CHECK_OPEN_SIG ->\n", __FUNCTION__);
			me->Memory_Check = 1;
			status = Q_HANDLED();
			break;
		}
		case AUTO_REPEAT_SIGNAL_TEST_OPEN_SIG:
		{
			ap_printf("[%s] AUTO_REPEAT_SIGNAL_TEST_OPEN_SIG ->\n", __FUNCTION__);
			Burn_AutoSignalTestEvt_t *pAutoSignalTestEvt = ((Burn_AutoSignalTestEvt_t *)e) ;
			me->stAutoRepeatSignalTest->private_signal_number = pAutoSignalTestEvt->private_signal_number;
			me->stAutoRepeatSignalTest->private_signal_repeat_time = pAutoSignalTestEvt->private_signal_repeat_time;

			if (me->stAutoRepeatSignalTest->private_signal_repeat_time != 0)
			{
				ap_printf("Signal Re-send Time = %d\n", me->stAutoRepeatSignalTest->private_signal_repeat_time);
				ap_printf("CUSTOMER_PRIVATE_SIG_START = %d\n", CUSTOMER_PRIVATE_SIG_START);
				ap_printf("Signal number = %d\n", me->stAutoRepeatSignalTest->private_signal_number);
				UserApp_PrintIRKeySignal(me->stAutoRepeatSignalTest->private_signal_number);
				QTimeEvt_ctorX(&me->timeRepeatSignalEvt, &me->super, CUSTOM_TEST_AUTO_REPEAT_SIGNAL_SIG, 0U);
				QACTIVE_POST(&me->super, Q_NEW(QEvt, CUSTOM_TEST_AUTO_REPEAT_SIGNAL_SIG), (void *)0);
			}
			status = Q_HANDLED();
			break;
		}
#endif
		case ZERO_RELEASE_SIG:
		{
			ap_printf("[%s] ZERO_RELEASE_SIG -> [%d]\n", __FUNCTION__, __LINE__);
			UserAppMemoryCheck();
			status = Q_HANDLED();
			break;
		}

		case APP_UPG_SIG:
			status = Q_TRAN(&UserApp_Upgrade);
			break;

			
		case OPT_RELEASE_SIG:
			if(me->iSrc != APP_SRC_OPT )
			{
				#ifdef SUPPORT_AP
				if(me->iSrc == APP_SRC_USB)
				{
			             if (me->playercreat == 1)
					{
						MFP_Ctrl(me->pAPInstance, MFP_SET_SWSRC_FLAG, (void *)1);
						MFP_Destroy(me->pAPInstance);
			    			me->playercreat = 0;
			    			me->pAPInstance = NULL;
			    		}

				}
				#endif

				me->iSrc = APP_SRC_OPT;//4
				SrcNameHint(me);
				UserSetting_SaveSrc(me->iSrc);
				ap_printf("user switch to opt\n");
				status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);
			}
 			else
				status = Q_HANDLED();
			break;

		
		case COAXIAL_RELEASE_SIG:
			if(me->iSrc != APP_SRC_COAX )
			{
				#ifdef SUPPORT_AP
				if(me->iSrc == APP_SRC_USB)
				{
					if (me->playercreat == 1)
					{
						MFP_Ctrl(me->pAPInstance, MFP_SET_SWSRC_FLAG, (void *)1);
						MFP_Destroy(me->pAPInstance);
			    			me->playercreat = 0;
			    			me->pAPInstance = NULL;
			    		}
				}
				#endif

				me->iSrc = APP_SRC_COAX;//4
				SrcNameHint(me);
				UserSetting_SaveSrc(me->iSrc);
				ap_printf("user switch to opt\n");
				status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);
			}
 			else
				status = Q_HANDLED();
			break;

		case BT_RELEASE_SIG:
			if(me->iSrc != APP_SRC_BT)
			{
				#ifdef SUPPORT_AP
				if(me->iSrc == APP_SRC_USB)
				{
					if (me->playercreat == 1)
					{
						MFP_Ctrl(me->pAPInstance, MFP_SET_SWSRC_FLAG, (void *)1);
						MFP_Destroy(me->pAPInstance);
			    			me->playercreat = 0;
			    			me->pAPInstance = NULL;
			    		}
				}
				#endif

				me->iSrc = APP_SRC_BT;//4
				SrcNameHint(me);
				UserSetting_SaveSrc(me->iSrc);
				ap_printf("user switch to BT\n");
				status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);
			}
			else
				status = Q_HANDLED();
			break;

			
		case HDMI_RELEASE_SIG:
				#ifdef SUPPORT_AP
				if(me->iSrc == APP_SRC_USB)
				{
					if (me->playercreat == 1)
					{
						MFP_Ctrl(me->pAPInstance, MFP_SET_SWSRC_FLAG, (void *)1);
						MFP_Destroy(me->pAPInstance);
			    			me->playercreat = 0;
			    			me->pAPInstance = NULL;
			    		}
				}
				#endif

			if(me->iSrc != APP_SRC_ARC)
			{
				me->iSrc = APP_SRC_ARC;
				SrcNameHint(me);
				UserSetting_SaveSrc(me->iSrc);
				ap_printf("user switch to hdmi\n");
				status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);
			}
			else
			{
				me->iSrc = APP_SRC_HDMI1;//4
				SrcNameHint(me);
				UserSetting_SaveSrc(me->iSrc);
				ap_printf("user switch to ARC\n");
				status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);
			}	
			break;

		case ARC_SRC_SIG:
			if(me->iSrc != APP_SRC_ARC)
			{
				#ifdef SUPPORT_AP
				if(me->iSrc == APP_SRC_USB)
				{
					if (me->playercreat == 1)
					{
						MFP_Ctrl(me->pAPInstance, MFP_SET_SWSRC_FLAG, (void *)1);
						MFP_Destroy(me->pAPInstance);
			    			me->playercreat = 0;
			    			me->pAPInstance = NULL;
			    		}
				}
				#endif

			
				UserApp_DimmerSet(me);
				SrcNameHint(me);
				me->iSrc = APP_SRC_ARC;//4
				UserSetting_SaveSrc(me->iSrc);
				ap_printf("user switch to ARC\n");
				status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);
			}
			else
				status = Q_HANDLED();
			break;	

		case HDMI1_SRC_SIG:
			if(me->iSrc != APP_SRC_HDMI1)
			{
				#ifdef SUPPORT_AP
				if(me->iSrc == APP_SRC_USB)
				{
					if (me->playercreat == 1)
					{
						MFP_Ctrl(me->pAPInstance, MFP_SET_SWSRC_FLAG, (void *)1);
						MFP_Destroy(me->pAPInstance);
			    			me->playercreat = 0;
			    			me->pAPInstance = NULL;
			    		}
				}
				#endif
				UserApp_DimmerSet(me);
				SrcNameHint(me);
				me->iSrc = APP_SRC_HDMI1;//5;
				UserSetting_SaveSrc(me->iSrc);
				ap_printf("user switch to HDMI 1\n");
				status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);
			}
			else
				status = Q_HANDLED();
			break;

		case HDMI2_SRC_SIG:
			if(me->iSrc != APP_SRC_HDMI2)
			{
				#ifdef SUPPORT_AP
				if(me->iSrc == APP_SRC_USB)
				{
					if (me->playercreat == 1)
					{
						MFP_Ctrl(me->pAPInstance, MFP_SET_SWSRC_FLAG, (void *)1);
						MFP_Destroy(me->pAPInstance);
			    			me->playercreat = 0;
			    			me->pAPInstance = NULL;
			    		}
				}
				#endif
				UserApp_DimmerSet(me);
				SrcNameHint(me);
				me->iSrc = APP_SRC_HDMI2;//6;
				UserSetting_SaveSrc(me->iSrc);
				ap_printf("user switch to HDMI 2\n");
				status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);
			}
			else
				status = Q_HANDLED();
			break;
		

		case SYS_DETECT_TIMER_SIG:
		{
			UINT32 cur_format;
			cur_format = UserApp_GetAudioFormat(me);
			if(cur_format < AUDIO_FORMAT_DISPLAY_MAX)
			{
				UserApp_DisplayAudioFormat(me, cur_format);
			}
			UserApp_SignalDetect(me);

			UserApp_DimmerTimeout(me);

			QTimeEvt_armX(&me->timerSysDetect, TICKS_PER_200MS, 0);
			
			status = Q_HANDLED();
			break;
		}

		case WIRELESS_TIMER_SIG:{

			UserApp_MuteCnt(me);

			if(me->hdmi_sw_cnt > 0)
				me->hdmi_sw_cnt--;

			QTimeEvt_armX(&me->timerWireless, TICKS_PER_100MS, 0);

			int ret  = wireless_polling();

			//UserApp_WirelessPairTimer(me);
		
			if(ret == 1)    
			{	
				ap_printf(" wireless is connect , mute state %d \n", me->mute);			
			
				UserApp_ScrollBackUp_ShowSource(me, STR_SW_CONNECTED);
				
				me->wl_pair_cnt = 0;
				me->wl_pair = 0;

				//if(me->mute == eMute_off)
				//	UserAppMute(me, 0);
								
				me->wireless_mute = eMute_off;
				
				me->wireless_mute_cnt = 0;
			}
			else if(ret == 0)   
			{
				#define WIRELESS_SYN_CNT	5
			
				if(me->wireless_mute_cnt < WIRELESS_SYN_CNT)  //	1.5s
				{
					ap_printf_E(" wireless wait for connecting  %d...\n ", me->wireless_mute_cnt);
					me->wireless_mute_cnt++;
					if(me->wireless_mute_cnt == WIRELESS_SYN_CNT)  // timeout no connect need mute off
					{
						me->wireless_mute = eMute_off;
						if(me->mute == eMute_off)
							UserAppMute(me, 0);
					}
				}
			}

			//if(wireless_get_syn())
				//wireless_surround_vol(me->iSrc); // set gain according to source
			
			status = Q_HANDLED();
			break;
		}

#ifdef MCU_IF_RSR
		case RSR_TIMER_200ms_SIG:
		{
			ap_printf("[%s] RSR_TIMER_200ms_SIG -> [%d]\n", __FUNCTION__, __LINE__);

			stMCU_IF_RSR_Evt *evt;
			evt = Q_NEW(stMCU_IF_RSR_Evt, RSR_TIMER_200ms_SIG);
			QMSM_DISPATCH(Q_MSM_UPCAST(&me->stMCU_IF_RSR), &evt->super);
			QF_gc((QEvt *)evt);

			status = Q_HANDLED();
			break;
		}

		case RSR_AudFmt_CHANGED_SIG:
		{
			ap_printf("[%s] RSR_AudFmt_CHANGED_SIG -> [%d]\n", __FUNCTION__, __LINE__);

			stMCU_IF_RSR_Evt *evt;
			evt = Q_NEW(stMCU_IF_RSR_Evt, RSR_AudFmt_CHANGED_SIG);
			QMSM_DISPATCH(Q_MSM_UPCAST(&me->stMCU_IF_RSR), &evt->super);
			QF_gc((QEvt *)evt);
			status = Q_HANDLED();
			break;
		}

#endif
		default:
		{
			status = Q_SUPER(&QHsm_top);
			break;
		}
	}
	return status;
}

/******************************************************************************************/
/**
 * \fn			QState UserApp_standby(UserApp * const me, QEvt const * const e)
 *
 * \brief		the userapp stand_by state
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_standby state \n
 *			    \e status : Q_TRAN() trans to other state \n
 *
 ******************************************************************************************/
QState UserApp_standby(UserApp *const me, QEvt const *const e)
{
	QState status;

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			VFD_DotCtrl(me, VFD_DOT_PWR, VFD_DOT_OP_CLR);

			AmpSet_PowerOff(me->hAmpList);

			UserApp_DimmerSet(me);
			
			if(first_power_up != 1)
				UserAppDisplay(me, STR_GOODBYE);
			else
				vfd_off();
			
				Hdmi_SetHandleSig(&me->super, CMD_ARC_SYS_CLOSE, 0, 0);
				FullEnvDelayMs(200);
				Hdmi_SetHandleSig(&me->super, CMD_POWER_STANDBY, 0, 0);
			

			UartApp_SetKey_Sig(&me->super, POWER_OFF_RELEASE_SIG, me->iSrc);

			ap_BurnTest_log("ENTRY\n");

			QTimeEvt_disarm(&me->timerWireless);
			
		
///* jeff standby flow ready then open
#if !(defined(ROMTER) || defined(BUILT4ISP2))
			UserSetting_SaveALL();
#endif
			System_Stdy_Disable_Output_Device();
#ifdef USERAPP_SUPPORT_REPEATER
			RepeaterSrv_PowerDown();
#endif
			FullEnvDelayMs(800);
			vfd_off();
			//FullEnvDelayMs(50);

			//QACTIVE_POST(SystemService_GetHandler(), Q_NEW(QEvt, SYSTEM_POWER_STANDBY_SIG), me);
			PowerService_SysPowerOff(&me->super);
//*/
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			ap_BurnTest_log("EXIT\n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			ap_BurnTest_log("INIT\n");
			status = Q_HANDLED();
			break;
		}

		case ARC_SRC_SIG:
		case HDMI1_SRC_SIG:
		case HDMI2_SRC_SIG:
			ap_printf(" standby, sw ARC/HDMI not do\n ");
			status = Q_HANDLED();
			break;
		// marked for spa100 IQA3 mantis 0302306
		/*
		case POWER_RELEASE_SIG:
		{
			ap_printf("UserApp_standby -> active; \n");
			ap_printf("[%s] -> active in[%d]\n", __FUNCTION__, __LINE__);
			status = Q_TRAN(&UserApp_active);
			break;
		}
		*/

		case POWER_RELEASE_SIG:
		case POWER_OFF_RELEASE_SIG:
			status = Q_HANDLED();
			break;

		
		default:
		{
			status = Q_SUPER(&UserApp_home);
			break;
		}
	}

	return status;
}

/******************************************************************************************/
/**
 * \fn			QState UserApp_hint(UserApp * const me, QEvt const * const e)
 *
 * \brief		the userapp stand_by state
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_hint state \n
 *			    \e status : Q_TRAN() trans to other state \n
 *
 ******************************************************************************************/
/*
QState UserApp_hint(UserApp *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			ap_BurnTest_log("EXIT\n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			status = Q_HANDLED();
			break;
		}
		case SOURCE_SW_RELEASE_SIG:
		case BT_READY_SOUNDHINT_SIG:
		case BT_CONNECT_SOUNDHINT_SIG:
		{
			ap_printf("[%s]: src sw; \n", __FUNCTION__);

			status = Q_TRAN(me->Hint_PreState);
			QACTIVE_POST(&me->super, Q_NEW(QEvt, e->sig), (void *)0);
			break;
		}
		default:
		{
			status = Q_SUPER(&UserApp_active);
			break;
		}
	}
	return status;
}
*/
/******************************************************************************************/
/**
 * \fn			QState UserApp_active(UserApp * const me, QEvt const * const e)
 *
 * \brief		the userapp active state
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_active state \n
 *			    \e status : Q_TRAN() trans to other sub_state \n
 *
 ******************************************************************************************/

void UserApp_CleanResume()
{	
	UserApp* const me = &l_UserApp;
	MFP_Ctrl(me->pAPInstance, MFP_SET_CLEAN_RESUME, (void *)1);
	//QACTIVE_POST(me, Q_NEW(QEvt, ONE_RELEASE_SIG), (void *)0);
}


QState UserApp_active(UserApp *const me, QEvt const *const e)
{
	QState status = 0;
	static char strBuf[14];
	BYTE cec_src = CMD_MODE_BT;
	static BYTE cnt=0;

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			ap_BurnTest_log("ENTRY\n");
			//VFD_DotCtrl(me, VFD_DOT_PWR, VFD_DOT_OP_SET);
#ifdef USERAPP_SUPPORT_KARAOKE
			QACTIVE_POST(&me->super, Q_NEW(QEvt, KOK_TURN_ON_SIG), (void *)0);
#endif
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			ap_BurnTest_log("EXIT\n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			ap_BurnTest_log("INIT\n");

			QTimeEvt_armX(&me->timerWireless, TICKS_PER_100MS, 0);
			QTimeEvt_armX(&me->timerSysDetect, TICKS_PER_100MS, 0);
			QTimeEvt_armX(&me->timePassword, TICKS_PER_100MS, 0);
			QTimeEvt_armX(&me->timeDisplayOnce, TICKS_PER_100MS, 0);
			QTimeEvt_armX(&me->timeSrcDelay, TICKS_PER_100MS, 0);

			// TDM setting, woofer MIX LsRs
			AudDspService_Set_CustomTDM(&me->super, ODSP_CUSTOMTDM_ENABLE, ODSP_CUSTOMTDM_LsRs);


#if 0
			//  add for hdmi init src
			volatile int *pIteHdmiSrcRAM = (int * )SRAM_ADDR_EP_WAKE_UP_FLAG;
			if(*pIteHdmiSrcRAM == HDMI_WAKEUP_HDMI_1)	
			{
				me->iSrc = APP_SRC_ARC;
				me->hdmi_src = APP_SRC_HDMI1;
			}
			else if(*pIteHdmiSrcRAM == HDMI_WAKEUP_HDMI_2)
			{
				me->iSrc = APP_SRC_ARC;
				me->hdmi_src = APP_SRC_HDMI2;
			}
			else if(*pIteHdmiSrcRAM == HDMI_WAKEUP_ARC)
			{
				me->iSrc = APP_SRC_ARC;//4
				me->hdmi_src = APP_SRC_ARC;
			}
			ap_printf(" pIteHdmiSrcRAM 0x%x\n ", *pIteHdmiSrcRAM);

			 if(me->hdmi_src == APP_SRC_HDMI1)
				cec_src = CMD_MODE_HDMI_1;
			 else  if(me->hdmi_src == APP_SRC_HDMI2)
				cec_src = CMD_MODE_HDMI_2;
			 else  if(me->hdmi_src == APP_SRC_ARC)
				cec_src = CMD_MODE_ARC;
			 else
			  	cec_src = CMD_MODE_BT;
			  
			  me->hdmi_src = APP_SRC_MAX;
			  
#else
			//  add for hdmi init src
			volatile int *pIteHdmiSrcRAM = (int * )SRAM_ADDR_EP_WAKE_UP_FLAG;
			if(*pIteHdmiSrcRAM == HDMI_WAKEUP_HDMI_1)	
				me->iSrc = APP_SRC_HDMI1;//5;
			else if(*pIteHdmiSrcRAM == HDMI_WAKEUP_HDMI_2)
				me->iSrc = APP_SRC_HDMI2;//6;
			else if(*pIteHdmiSrcRAM == HDMI_WAKEUP_ARC)
				me->iSrc = APP_SRC_ARC;//4

			ap_printf(" pIteHdmiSrcRAM 0x%x\n ", *pIteHdmiSrcRAM);

			 if(me->aUserSrc_tbl[me->iSrc] == eAp_HDMI0_Src)
				cec_src = CMD_MODE_HDMI_1;
			 else  if(me->aUserSrc_tbl[me->iSrc] == eAp_HDMI1_Src)
				cec_src = CMD_MODE_HDMI_2;
			  else  if(me->aUserSrc_tbl[me->iSrc] == eAp_ARC_Src)
				cec_src = CMD_MODE_ARC;
			  else
			  	cec_src = CMD_MODE_BT;
#endif
			  me->hdmi_src = APP_SRC_MAX;
			
			Hdmi_SetInitSig(&me->super, cec_src, me->vol, USERAPP_VOL_MAX);

			if(*pIteHdmiSrcRAM != 0 )
			{
				UserSetting_SaveSrc(me->iSrc);
				me->hdmi_sw_cnt = 20; // 2s
			}
			*pIteHdmiSrcRAM = 0;

			#if 0//def USERAPP_SUPPORT_REPEATER
			RepeaterWakeup_t Repeater_State;
			RepeaterSrv_UserPowerUp(&Repeater_State);
			ap_printf("[%s]:Repeater_State.power_state:%d; \n", __FUNCTION__,Repeater_State.power_state);

			me->HPD_State = Repeater_State.HPD_state;
			me->eARC_SRC = Repeater_State.eARC_state;
			if(Repeater_State.power_state == AudSYS_wakeup_to_Repeater)
			{
				ap_printf("[%s]:AudSYS_wakeup_to_Repeater; \n", __FUNCTION__);
				int src_cnt = 0;


				for (src_cnt = 0; me->aUserSrc_tbl[src_cnt] != USER_SRC_END; src_cnt++)
				{
					if(Repeater_State.Source == me->aUserSrc_tbl[src_cnt])
						{
						if((Repeater_State.Source >= eAp_ARC_Src)&&(Repeater_State.Source <= eAp_HDMI2_Src))
						   	me->iSrc = src_cnt;
						}
				}
				ap_printf(" me->iSrc = %d \n",me->iSrc);

				UserSetting_SaveSrc(me->iSrc);

			}
			else
			{
				ap_printf("Not HDMI Source me->iSrc = %d \n",me->iSrc);
			 	if(me->iSrc < eAp_ARC_Src || me->iSrc > eAp_HDMI2_Src)
				{
					ap_printf(" ======= eRepeater_Src_Other =======\n");
					RepeaterSrv_SrcSwitch(eRepeater_Src_Other);
				}

			}
			#endif


			#if 0
			if(HDMISrv_Check_ARCWakeUp()==1)
			{
				ap_printf("[%s]:HDMI_ARC_WakeUp; \n", __FUNCTION__);
				me->iSrc=eAp_ARC_Src;
				ap_printf(" me->iSrc = %d \n",me->iSrc);
				UserSetting_SaveSrc(me->iSrc);
			}
			#endif
			//SystemService_AudioInfoBroadcast(&me->super, SYS_AUDIO_INFO_REQ,  me->mute, me->vol, USERAPP_VOL_MAX, USERAPP_VOL_MIN);
            //LoadUserAppSrc(me, 0);
#if 0

			if(me->iSrc == eAp_HDMI0_Src || me->iSrc == eAp_HDMI1_Src || me->iSrc == eAp_HDMI2_Src )
			{
				RepeaterSrv_SetAudPath(eRepeater_Audio_System);
			}
#endif


#if (SUPPORT_VA)
			/*Userapp init va status*/
			ap_printf("Userapp init va status\n");
			VoiceAssistService_APSource(NULL, SRC_OTHER);
			if(me->AuxVA_Status || me->BtVA_Status)
			{
				VoiceAssistService_VAStatus(NULL, me->AuxVA_Status, me->BtVA_Status);
			}
#endif

			me->sound_menu = eSoundMax;


			SpHdsp_Set_DownmixConfig(&me->super, SPKCONFIG_LR_BIT|SPKCONFIG_C_BIT|SPKCONFIG_LSRS_BIT|SPKCONFIG_SUB_BIT);
			SpHdsp_Set_UpmixConfig(&me->super, SPKCONFIG_LR_BIT|SPKCONFIG_C_BIT|SPKCONFIG_LSRS_BIT|SPKCONFIG_SUB_BIT);
			
			SpHdsp_setAtmos(&me->super, SP_ATMOS_VIRT, 0, 1); // if 2.x 3.x 4.x 5.x output, must set this function, can dialsy atmos format
			SpHdsp_setAtmos(&me->super, SP_ATMOS_CONFIG, 0, 1);
			SrcNameHint(me);

			UserApp_DimmerSet(me);

			AudDspService_Set_AppMode(&me->super, SET_MMIX, GRP_MMIX0, MODE1);
		//	AudDspService_Set_AppMode(&me->super, SET_EQ, GRP_FILTER1, MODE1);
		//	AudDspService_Set_AppMode(&me->super, SET_BASS, GRP_BASS0, MODE1);

		//	UserApp_SetBass(me, me->Bass_vol);
		//	UserApp_SetTreble(me, me->Treble_vol);
		//	UserApp_SetCenterVol(me, me->Centel_vol);
		//	UserApp_SetLRVol(me, me->Top_vol);
		//	UserApp_SetLsRsVol(me, me->Ls_vol);

		//	UserApp_SetSubwooferVol(me, me->Bass_vol);
			
		//	AudDspService_Set_AppMode(&me->super, SET_EQ, GRP_FILTER2, MODE1);  // for subwoofer lowpass
			
		//  add for hdmi init src	
            		ap_printf("---power on\n");
		//	status = Q_TRAN(&UserApp_PowerOn);
			status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);

			break;
		}
		
		case DISPLAY_ONCE_BACK_SIG:
			if(me->delay_call)
			{
				me->delay_call(me);
				me->delay_call = NULL;
			}
			status = Q_HANDLED();
			break;
	
		case BASS_UP_TICK_SIG:
		case BASS_UP_RELEASE_SIG:
			if(me->Bass_vol < 10)
			{
				me->Bass_vol++;
				UserSetting_SaveBass(me->Bass_vol);
			//	UserApp_SetBass(me, me->Bass_vol);
				UserApp_SetSubwooferVol(me, me->Bass_vol);
			}
			if(me->Bass_vol > 5)
				sprintf(strBuf, "BAS+%d", me->Bass_vol-5);
			else if(me->Bass_vol < 6)
				sprintf(strBuf, "BAS-%d", 5-me->Bass_vol);
			else	
				sprintf(strBuf, "BAS %d", 5-me->Bass_vol);
			
			UserAppDisplayOnce(me, strBuf, 3);
			
			status = Q_HANDLED();
			break;

		case BASS_DN_TICK_SIG:
		case BASS_DN_RELEASE_SIG:
			
			if(me->Bass_vol > 0)
			{
				me->Bass_vol--;
				UserSetting_SaveBass(me->Bass_vol);

				//UserApp_SetBass(me, me->Bass_vol);
				UserApp_SetSubwooferVol(me, me->Bass_vol);
			}

			if(me->Bass_vol > 5)
				sprintf(strBuf, "BAS+%d", me->Bass_vol-5);
			else if(me->Bass_vol < 5)
				sprintf(strBuf, "BAS-%d", 5-me->Bass_vol);
			else	
				sprintf(strBuf, "BAS %d", 5-me->Bass_vol);
			UserAppDisplayOnce(me, strBuf, 3);
			status = Q_HANDLED();
			break;

		case TREBLE_UP_TICK_SIG:
		case TREBLE_UP_RELEASE_SIG:
			if(me->Treble_vol < 10)
			{
				me->Treble_vol++;
				UserSetting_SaveTreble(me->Treble_vol);

				UserApp_SetTreble(me, me->Treble_vol);
			}

			if(me->Treble_vol > 5)
				sprintf(strBuf, "TRE+%d", me->Treble_vol-5);
			else if(me->Treble_vol == 5)
				sprintf(strBuf, "TRE %d", 5-me->Treble_vol);
			else
				sprintf(strBuf, "TRE-%d", 5-me->Treble_vol);
		
			UserAppDisplayOnce(me, strBuf, 3);
			status = Q_HANDLED();
			break;

		case TREBLE_DN_TICK_SIG:
		case TREBLE_DN_RELEASE_SIG:
			if(me->Treble_vol > 0)
			{
				me->Treble_vol--;
				UserSetting_SaveTreble(me->Treble_vol);

				UserApp_SetTreble(me, me->Treble_vol);
			}

			if(me->Treble_vol > 5)
				sprintf(strBuf, "TRE+%d", me->Treble_vol-5);
			else if(me->Treble_vol == 5)
				sprintf(strBuf, "TRE %d", 5-me->Treble_vol);
			else
				sprintf(strBuf, "TRE-%d", 5-me->Treble_vol);
		
			UserAppDisplayOnce(me, strBuf, 3);
			status = Q_HANDLED();
			break;


		
		case PLAY_TICK_SIG:
			if(cnt++ == 10)
			{
				wireless_pair(1);
				UserAppDisplayOnce(me, "PAIR", 3);
				
			}
			status = Q_HANDLED();
			break;

		case MCU_UPG_SIG:
			UserAppDisplay(me, STR_HDMI_UPG);
			Hdmi_SetUpgSig(&me->super);
			status = Q_HANDLED();
			break;
			
			
		case HDMI_UPG_SIG:{

			HdmiEvt *evt = (HdmiEvt *)e;
			UserAppMute(me, 1);
			if(evt->vol == HDMI_UPG_OK)
			{
				sprintf(strBuf, "HD OK");
				me->Up_flag = 0;
				UserAppMute(me, 0);
				QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, POWER_RELEASE_SIG), NULL);
			}
			else if(evt->vol == HDMI_UPG_NG)
			{
				sprintf(strBuf, "HD NG");
				me->Up_flag = 1;
			}
			else if(evt->vol == 100)
			{
				sprintf(strBuf, "HD%03d", evt->vol);
				me->Up_flag = 1;
			}
			else if(evt->vol > 9)
			{
				sprintf(strBuf, "HD %02d", evt->vol);
				me->Up_flag = 1;
			}
			else
			{
				sprintf(strBuf, "HD  %01d", evt->vol);
				me->Up_flag = 1;
			}
			VFD_Service_Display(&me->super, strBuf);
			VFD_Service_Align(&me->super, CENTER);
			VFD_Service_Constant(&me->super);
			status = Q_HANDLED();	
			break;
		}
			
		case PLAY_SIG:
			cnt = 0;
			status = Q_HANDLED();
			break;
			
		
		case WOOFER_VOL_UP_TICK_SIG:
		case WOOFER_VOL_UP_RELEASE_SIG:
		{
			// ---{{{ Merged from SPA300_TPV_20210205
			if(me->subwoofer_vol < 10)
			{
				me->subwoofer_vol++;
				UserApp_SetSubwooferVol(me, me->subwoofer_vol);
			}
			if(me->subwoofer_vol > 5)
				sprintf(strBuf, "SUB+%d", me->subwoofer_vol-5);
			else if(me->subwoofer_vol == 5)
				sprintf(strBuf, "SUB %d", 5-me->subwoofer_vol);
			else
				sprintf(strBuf, "SUB-%d", 5-me->subwoofer_vol);
		
			status = Q_HANDLED();
			break;
		}
		case WOOFER_VOL_DOWN_TICK_SIG:
		case WOOFER_VOL_DOWN_RELEASE_SIG:
		{
			// ---{{{ Merged from SPA300_TPV_20210205
			if(me->subwoofer_vol >0)
			{
				me->subwoofer_vol--;
				UserApp_SetSubwooferVol(me, me->subwoofer_vol);
			}
			if(me->subwoofer_vol > 5)
				sprintf(strBuf, "SUB+%d", me->subwoofer_vol-5);
			else if(me->subwoofer_vol == 5)
				sprintf(strBuf, "SUB %d", 5-me->subwoofer_vol);
			else
				sprintf(strBuf, "SUB-%d", 5-me->subwoofer_vol);
		
			status = Q_HANDLED();
		break;
		}

		case TOUCH_CALL_SIG:{
			
			vfd_dot(ICON_CLEAN, ICON_DOT2);
			if(me->dim_flag == 1)
			{
				//me->DIMMER = DIMMER_HIGH;
				//UserAppDIMSet(me->DIMMER);
				vfd_dimmer(DIMMER_HIGH);
				QTimeEvt_disarmX(&me->timeSettingMenu);
				QTimeEvt_armX(&me->timeSettingMenu, TICKS_PER_SEC*3, 0);
			}
			
			TouchCallEvt *evt  = (TouchCallEvt *)e;	
			QACTIVE_POST(&me->super,  Q_NEW(QEvt, evt->sig), (void*)0);
			status = Q_HANDLED();
			break;
		}

		case BT_SIG:
		case SOURCE_SW_SIG:
			cnt = 0;
			status = Q_HANDLED();
		    ap_printf("BT_SIG = %d ,SOURCE_SW_SIG =%d\n ",BT_SIG,SOURCE_SW_SIG);
			break;
		case BT_LONG_RELEASE_SIG:
		case SOURCE_SW_LONG_RELEASE_SIG:
		if(cnt >= 20)
		{
			wireless_pair(1);
			UserApp_ScrollBackUp_ShowSource(me, STR_SW_PAIRING);
		}
		else if(me->iSrc == APP_SRC_BT)
		{
			if((cnt >= 5)&&(cnt<20))
			{  
				if(me->bt_connect == 1)
				UserAppDisplay_Scroll(me, STR_BT_PAIR, R2L);
				UartApp_SetKey_Sig(&me->super, PAIR_RELEASE_SIG, me->iSrc);
			}
		}
		ap_printf("BT_LONG_RELEASE_SIG = %d ,SOURCE_SW_LONG_RELEASE_SIG =%d\n ",BT_LONG_RELEASE_SIG,SOURCE_SW_LONG_RELEASE_SIG);
		status = Q_HANDLED();
		break;
		case BT_TICK_SIG:
		case SOURCE_SW_TICK_SIG:

			cnt++;		
		    ap_printf("BT_TICK_SIG = %d ,SOURCE_SW_TICK_SIG =%d\n ",BT_TICK_SIG,SOURCE_SW_TICK_SIG);
			status = Q_HANDLED();
			break;
		
		case SOURCE_SW_RELEASE_SIG: {
			ap_printf("[%s]: src sw; \n", __FUNCTION__);
#ifdef SUPPORT_AP
		if(me->iSrc == APP_SRC_USB)
		{
			MFP_Ctrl(me->pAPInstance, MFP_SET_SWSRC_FLAG, (void *)1);
			MFP_Destroy(me->pAPInstance);
			me->pAPInstance = NULL;
			//UserAppMute(me,1);
			me->showErr = 0;
		}
#endif
			SwUserAppSrc(me, 0);
			SrcNameHint(me);
/* jeff bt ready to open
#ifdef USERAPP_SUPPORT_BT
			if (me->pBtBakcGroundApp != NULL)
			{
				QMSM_DISPATCH(&me->pBtBakcGroundApp->super, e);
			}
#endif
*/
			status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);
			break;
		}

		case POWER_OFF_SIG:
		case HDMI_CEC_Standby_SIG:
		case CEC_POWER_OFF_SIG:
		case POWER_RELEASE_SIG:
		{
			ap_printf("[%s] -> standby; \n", __FUNCTION__);
			status = Q_TRAN(&UserApp_PowerOff);
			//status = Q_TRAN(&UserApp_standby);
			break;
		}
		case SURROUND_3D_RELEASE_SIG:
		{
			ap_printf("[%s] 3D surround; \n", __FUNCTION__);

			if (me->Surround_3D == eApSurround_Mode_0)
			{
				me->Surround_3D = eApSurround_Mode_1;
			}
			else
			{
				me->Surround_3D = eApSurround_Mode_0;
			}

			UserApp_ScrollBackUp_ShowSource(me, STR_SU0 + me->Surround_3D);

			UserAppSurroundSet((QActive *)me, me->Surround_3D);
			status = Q_HANDLED();
			break;
		}
		case EQ_RELEASE_SIG:
		{
			ap_printf("[%s] EQ RELEASE; \n", __FUNCTION__);

			if (me->EQMode < 4)
			{
				me->EQMode++;
			}
			else
			{
				me->EQMode = eApEQ_Mode_0;
			}
			UserAppDisplayOnce(me, vfd_str[STR_EQ0 + me->EQMode],3);
			//UserAppEQSet((QActive *)me, me->EQMode);
			status = Q_HANDLED();
			break;
		}

		case DOLBY_RELEASE_SIG:
			me->dolby_status ^= 1;
			if(me->dolby_status == 1)
			{
				SpHdsp_setAtmos(&me->super, SP_ATMOS_DAP, 1 ,0);
				UserAppDisplayOnce(me, " ON ", 3);
			}
			else
			{
				SpHdsp_setAtmos(&me->super, SP_ATMOS_DAP, 0 ,0);
				UserAppDisplayOnce(me, " OFF", 3);
			}
			status = Q_HANDLED();
			break;
		
		case DIM_RELEASE_SIG:
			if(me->DIMMER == DIMMER_OFF)
			{
				me->DIMMER = DIMMER_HIGH;
				me->dimmer_cnt = 3*5; // 3s
			}
			else
			{
				me->DIMMER = DIMMER_OFF;
			}
			vfd_dimmer(me->DIMMER);
			status = Q_HANDLED();
			break;



		case DIM_UP_RELEASE_SIG:
			if(me->DIMMER == DIMMER_OFF)
			{
				me->DIMMER = DIMMER_LOW;
			}
			else if(me->DIMMER == DIMMER_LOW)
			{
				me->DIMMER = DIMMER_HIGH;
			}
			vfd_dimmer(me->DIMMER);
			status = Q_HANDLED();
			break;

		case DIM_DN_RELEASE_SIG:
			if(me->DIMMER == DIMMER_HIGH)
			{
				me->DIMMER = DIMMER_LOW;
			}
			else if(me->DIMMER == DIMMER_LOW)
			{
				me->DIMMER = DIMMER_OFF;
			}
			vfd_dimmer(me->DIMMER);
			status = Q_HANDLED();
			break;

		case MUSIC_RELEASE_SIG:
			if(me->EQMode != 0)
			{
				me->EQMode = 0;
				UserSetting_SaveEQ(me->EQMode);
			}
			UserAppDisplayOnce(me, "MUSIC", 3);
			status = Q_HANDLED();
			break;

		case MOVIE_RELEASE_SIG:
			if(me->EQMode != 1)
			{
				me->EQMode = 1;
				UserSetting_SaveEQ(me->EQMode);
			}
			UserAppDisplayOnce(me, "MOVIE", 3);
			status = Q_HANDLED();
			break;

		case NEWS_RELEASE_SIG:
			if(me->EQMode != 2)
			{
				me->EQMode = 2;
				UserSetting_SaveEQ(me->EQMode);
			}
			UserAppDisplayOnce(me, "NEWS ", 3);
			status = Q_HANDLED();
			break;

		case SPORT_RELEASE_SIG:
			if(me->EQMode != 3)
			{
				me->EQMode = 3;
				UserSetting_SaveEQ(me->EQMode);
			}
			UserAppDisplayOnce(me, "SPORT", 3);
			status = Q_HANDLED();
			break;

		case NIGHT_RELEASE_SIG:
			if(me->EQMode != 4)
			{
				me->EQMode = 4;
				UserSetting_SaveEQ(me->EQMode);
			}
			UserAppDisplayOnce(me, "NIGHT", 3);
			status = Q_HANDLED();
			break;


/*
		case HDMI_HPD_SIG:{
			HdmiEvt *evt = (HdmiEvt *)e;
			ap_printf(" HDMI HPD change %d %d\n ", evt->vol, me->HPD_State);
			if(evt->vol == 1 && me->HPD_State == 0)
			{
				UserApp_DimmerSet(me);
				me->hdmi_sw_cnt = 20;
				me->iSrc = APP_SRC_ARC;//5;
				me->HPD_State = evt->vol;
				UserSetting_SaveSrc(me->iSrc);
				ap_printf("user switch to ARC\n");
				status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);
				break;
			}
			me->HPD_State = evt->vol;
			
			status = Q_HANDLED();
			break;
		}

		case HDMI1_IN_PLUGIN_SIG:{
			HdmiEvt *evt = (HdmiEvt *)e;
			ap_printf(" HDMI 1 Plugin %d %d\n ", evt->vol, me->hdmi1_state);
			if(evt->vol == 2 && me->hdmi1_state == 0 && me->hdmi_sw_cnt == 0)
			{
				UserApp_DimmerSet(me);
				me->iSrc = APP_SRC_HDMI1;//5;
				me->hdmi1_state = evt->vol;
				UserSetting_SaveSrc(me->iSrc);
				ap_printf("user switch to HDMI 1\n");
				status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);
				break;
			}
			me->hdmi1_state = evt->vol;
			status = Q_HANDLED();
			break;
		}
		case HDMI2_IN_PLUGIN_SIG:{
			HdmiEvt *evt = (HdmiEvt *)e;
			
			ap_printf(" HDMI 2 Plugin %d, %d\n ", evt->vol, me->hdmi2_state);

			if(evt->vol == 2 && me->hdmi2_state == 0 && me->hdmi_sw_cnt == 0)
			{
				UserApp_DimmerSet(me);
				me->iSrc = APP_SRC_HDMI2;//5;
				me->hdmi2_state = evt->vol;
				UserSetting_SaveSrc(me->iSrc);
				ap_printf("user switch to HDMI 2\n");
				status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);
				break;
			}
			me->hdmi2_state = evt->vol;
			status = Q_HANDLED();
			break;
		}

*/
		case HDMI_VOL_SIG:{

			 if((me->aUserSrc_tbl[me->iSrc] <= eAp_HDMI2_Src)&&(me->aUserSrc_tbl[me->iSrc] >= eAp_ARC_Src))
			 {
				UserApp_DimmerSet(me);
			 
				HdmiEvt *evt = (HdmiEvt *)e;

				ap_printf(" HDMI mute %d, vol %d\n ", evt->mute, evt->vol);
				
				if(me->mute != evt->mute)
				{
					ap_printf(" HDMI set mute %d\n ", evt->mute);
				
					me->mute = evt->mute;
				
				if (me->mute == eMute_off)
				{
					//UserAppNum2Str(strBuf, me->vol);
					UserAppStrToVolDisplay(strBuf, me->vol);
					UserAppDisplayOnce(me, strBuf, 3);
				}
				else
				{
					UserAppDisplay_Blink(me, STR_MUTE, 8, 8);
				}

					UserAppMute(me, me->mute);

					
				}
				
				if(me->vol != evt->vol)
				{

					ap_printf(" HDMI set vol %d, old vol %d\n ", evt->vol, me->vol);
					
					me->vol = evt->vol;

					UserSetting_SaveVolume(me->vol);

					UserAppVolumeSet(me->vol, me->hAmpList);

					//UartApp_SetKey_Sig(&me->super, VOL_SET_SIG, me->vol);
				
					//UserAppNum2Str(strBuf, me->vol);
					UserAppStrToVolDisplay(strBuf, me->vol);
					UserAppDisplayOnce(me, strBuf, 3);
	         	
	   
				}
			 }
			status = Q_HANDLED();
			break;
		}
		case HDMI_CEC_ARCSOURCE_SIG:
		{
			ap_printf("[%s]:HDMI_CEC_ARCSOURCE_SIG; \n", __FUNCTION__);
			if(me->aUserSrc_tbl[me->iSrc] != eAp_ARC_Src)
			{
				ap_printf(" This is Other Source  \n");
				stCecSetSrc_t *ARCSRC_State = (stCecSetSrc_t *)e;
				if(ARCSRC_State->bCecSrc == 1)
				{
					me->iSrc = eAp_ARC_Src;
					UserSetting_SaveSrc(me->iSrc);
					status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);

				}else
				{
					status = Q_HANDLED();
				}

			}
			else
			{
				ap_printf(" This is ARC Source  \n");
				status = Q_HANDLED();
			}

			break;

		}
		case MUTE_RELEASE_SIG:
		{
			ap_printf("[%s]:MUTE_RELEASE_SIG mute; \n", __FUNCTION__);

			UserAppKeyEvt *KeyEvt = (UserAppKeyEvt *)e;

			if ((KeyEvt->User_KeyCodeSrc ==  eKeySrc_CEC_CMD) && (me->aUserSrc_tbl[me->iSrc] != eAp_ARC_Src))
			{
				ap_printf("This Mute Signal Send From CEC When Src is not ARC , We Drop it!!!\n");
			}
			else
			{
				if (me->mute == eMute_on)
				{
					me->mute = eMute_off;
					//UserAppNum2Str(strBuf, me->vol);
					UserAppStrToVolDisplay(strBuf, me->vol);
					UserAppDisplayOnce(me, strBuf, 3);
				}
				else
				{
					me->mute = eMute_on;
					UserAppDisplay_Blink(me, STR_MUTE, 8, 8);
				}

				UserAppMute(me, me->mute);

				Hdmi_SetHandleSig(&me->super, CMD_VOLUME_MUTE, me->mute, 0);
				
#ifdef USERAPP_SUPPORT_BT
				//UserAppVolumeNotifyToBt(me->vol, me->mute);
#endif

#ifdef USERAPP_SUPPORT_REPEATER
				RepeaterSrv_MuteSet(me->mute);
#endif
#ifdef USERAPP_SUPPORT_HDMI_CEC

				//IOSrv_AudInfo_report(me, me->mute, me->vol, USERAPP_VOL_MAX);
				SystemService_AudioInfoBroadcast(&me->super, SYS_MUTE_CHG, me->mute, me->vol, USERAPP_VOL_MAX, USERAPP_VOL_MIN);
#endif
				
			}

			status = Q_HANDLED();
			break;
		}

		case SETTING_MENU_TIMER_SIG:
		case RETURN_RELEASE_SIG:
		{
				
			me->menu_status = eSoundMax;

			status = Q_HANDLED();
			break;
		}

		case VOL_UP_TICK_SIG:
		case VOL_UP_RELEASE_SIG:
		{
			UINT8 State = SYS_VOL_CHG;

			{
						

			if (me->mute == eMute_on)
			{
				UserAppMute(me, 0);
				me->mute = eMute_off;
				State |= SYS_MUTE_CHG;
			}

			UserAppVolumeUp(me);


				UartApp_SetKey_Sig(&me->super, VOL_SET_SIG, me->vol);

				Hdmi_SetHandleSig(&me->super, CMD_VOLUME_UP, me->mute, me->vol);
				
				//UserAppNum2Str(strBuf, me->vol);
				UserAppStrToVolDisplay(strBuf, me->vol);
				UserAppDisplayOnce(me, strBuf, 3);

	
			}
			status = Q_HANDLED();
			break;
		}
		case VOL_DOWN_TICK_SIG:
		case VOL_DOWN_RELEASE_SIG:
		{
			UINT8 State = SYS_VOL_CHG;

			{
			if (me->mute == eMute_on)
			{
				UserAppMute(me, 0);
				me->mute = eMute_off;
				State |= SYS_MUTE_CHG;
			}

			UserAppVolumeDown(me);

				UartApp_SetKey_Sig(&me->super, VOL_SET_SIG, me->vol);

				Hdmi_SetHandleSig(&me->super, CMD_VOLUME_DOWN, me->mute, me->vol);

				UserAppStrToVolDisplay(strBuf, me->vol);
				UserAppDisplayOnce(me, strBuf, 3);
			}
			status = Q_HANDLED();
			break;
		}
#ifdef USERAPP_SUPPORT_REPEATER
			case REPEATER_eARCSRC_CHANGE_SIG:
			{
				ap_printf("REPEATER_eARCSRC_CHANGE_SIG\n");
				RepeatereARCSrcEvt_t *eARC = ((RepeatereARCSrcEvt_t *)e);
				me->eARC_SRC = eARC->eARCSRC;
				//ap_printf("==== me->iSrc  %d====\n",me->iSrc);
 				if(me->iSrc == APP_SRC_ARC)
				{
					if(me->eARC_SRC == eUser_eARCSRC)
					{
						ap_printf("==== STR_EARC ====\n");
						UserAppDisplay(me, STR_EARC);

						#ifndef BUILT4ISP2
						RepeaterSrcState_t repeaterstatus;
						repeaterstatus.Source_ID = 0;
						RepeaterSrv_SourceIDChk(&repeaterstatus);
						AudDspService_SPDIF_STOP(&me->super);
						AudDspService_Flow_Stop(&me->super);

                        if(repeaterstatus.Source_ID == Source_HDMISTD)
			                AudDspService_Prepare_8CH_IISIN(&me->super, IIS8CH_HDMISTD);
                        else if(repeaterstatus.Source_ID == Source_HDMIHBR)
			                AudDspService_Prepare_8CH_IISIN(&me->super, IIS8CH_HDMIHBR);

						AudDspService_Set_SourceGain(&me->super, SrcGain_Tab[eAp_ARC_Src]);
						AudDspService_Flow_Play(&me->super);
						AudDspService_SPDIF_PLAY(&me->super);
						#endif
					}
					else
					{
						ap_printf("==== STR_ARC ====\n");
						UserAppDisplay(me, STR_ARC);

						#ifndef BUILT4ISP2

		  				AudDspService_SPDIF_STOP(&me->super);
		   				AudDspService_Flow_Stop(&me->super);
						#if defined(CFG_EMU_V3_SPDIF) && (CFG_EMU_V3_SPDIF == 1)
							AudDspService_Prepare_SPDIF(&me->super, SPDIF_IEC0);
						#else
							AudDspService_Prepare_SPDIF(&me->super, SPDIF_IEC1);
						#endif
		   				AudDspService_Set_SourceGain(&me->super, SrcGain_Tab[eAp_ARC_Src]);
		  				AudDspService_Flow_Play(&me->super);
		  				AudDspService_SPDIF_PLAY(&me->super);

						#endif
					}
				}
				status = Q_HANDLED();
				break;
			}


#endif
		case ODSP_REPORT_MSMARTM_SIG:
		{
			char*smartmode_name[3] =
			{
				"music",
				"movie",
				"voice",
			};
			UINT8 msmartM = ((OdspMsmartMEvt*)e)->bMsmartmode;
			ap_printf("#Warning# main smartmode=%d:%s\n",msmartM,smartmode_name[msmartM]);
 			status = Q_HANDLED();
			break;
		}

		case AUDIO_MUTE_SET_SIG:
		{
			int iRes = -1;
			ap_printf("recieve AUDIO_MUTE_SET_SIG\n");
			stSYS_AudioInfo_t *pstAudioInfo = ((stSYS_AudioInfo_t *)e);

			iRes = UserAppMute(me, pstAudioInfo->MuteState);
			if (iRes != -1)
			{
				me->mute = pstAudioInfo->MuteState;
			}

			if (me->mute == eMute_off)
			{
				UserAppNum2Str(strBuf, me->vol);
			}
			else
			{
				UserAppNum2Str(strBuf, 0);
			}
#ifdef USERAPP_SUPPORT_BT
			//UserAppVolumeNotifyToBt(me->vol, me->mute);
#endif
#ifdef USERAPP_SUPPORT_HDMI_CEC

			SystemService_AudioInfoBroadcast(&me->super, SYS_MUTE_CHG, me->mute, me->vol, USERAPP_VOL_MAX, USERAPP_VOL_MIN);
#endif
			//VFD_Service_POP(me, strBuf, 3, RIGHT);

			status = Q_HANDLED();
			break;
		}

		case AUDIO_VOL_SET_SIG:
		{
			ap_printf("recieve AUDIO_VOL_SET_SIG\n");
			stSYS_AudioInfo_t *pstAudioInfo = ((stSYS_AudioInfo_t *)e);

			if (me->mute == eMute_on)
			{
				UserAppMute(me, 0);
				me->mute = eMute_off;
			}

			UserAppVolumeSet(pstAudioInfo->Vol, me->hAmpList);
			me->vol = pstAudioInfo->Vol;
#ifdef USERAPP_SUPPORT_BT
			//UserAppVolumeNotifyToBt(me->vol, me->mute);
#endif
			//UserAppNum2Str(strBuf, me->vol);
			UserAppStrToVolDisplay(strBuf, me->vol);
			//VFD_Service_POP(me, strBuf, 2, RIGHT);
#ifdef USERAPP_SUPPORT_HDMI_CEC
			SystemService_AudioInfoBroadcast(&me->super, SYS_VOL_CHG, me->mute, me->vol, USERAPP_VOL_MAX, USERAPP_VOL_MIN);
#endif
			status = Q_HANDLED();
			break;
		}
/*
		case CUSTOM_HINT_START_SIG:
		{

			ap_printf("go to hint state\n");
			status = Q_TRAN(&UserApp_hint);
			break;
		}
*/
		case AUDIO_INFO_BROADCAST_SIG:
		{
			ap_printf("AUDIO_INFO_BROADCAST_SIG\n");
		#ifdef USERAPP_SUPPORT_HDMI_CEC

			SystemService_AudioInfoBroadcast(&me->super, SYS_AUDIO_INFO_REQ, me->mute, me->vol, USERAPP_VOL_MAX, USERAPP_VOL_MIN);
		#endif
			status = Q_HANDLED();
			break;
		}
		case BT_APP_SIG:
		case BT_START_SEARCH_RELEASE_SIG:
		case BT_TWS_MODE_SWITCH_RELEASE_SIG:
		case BT_VOL_SYNC_SET_SIG:
		{
			//post
#ifdef USERAPP_SUPPORT_BT
			if (me->pBtBakcGroundApp != NULL)
			{
				QMSM_DISPATCH(&me->pBtBakcGroundApp->super, e);
			}
#endif
			status = Q_HANDLED();
			break;
		}
		case TWO_LONG_RELEASE_SIG: // go to ir learning mode
		{
			if (me->MonkeyTest_Enable == 1) //do not enter ir learning mode in test mode
			{
				status = Q_HANDLED();
			}
			else
			{
				status = Q_TRAN(&UserApp_IR_Learning);
			}
			break;
		}
#if 0 //every source can use
		case FOUR_LONG_RELEASE_SIG:
		{
			LearningVolUpEntry(me);
			status = Q_HANDLED();
			break;
		}
		case FIVE_LONG_RELEASE_SIG:
		{
			LearningVolDownEntry(me);
			status = Q_HANDLED();
			break;
		}
		case SIX_LONG_RELEASE_SIG:
		{
			LearningMuteEntry(me);
			status = Q_HANDLED();
			break;
		}
#endif
		case IR_LEARNING_DONE_SIG:{
        ap_printf("IR learning done,\n");

        stSYS_IrLearningInfo_t stIR_L_info;
        stIR_L_info.LearningKey_Target = -1; // unnecessary
        IOSrv_IR_Learning_Terminate(&me->super, &stIR_L_info);

		status = Q_HANDLED();
            break;
  		}
		case SET_BT_SOURCE_SIG:
		{
			ap_printf("BT_SET_SOURCE_SIG...Userapp_active \n");
			int src_cnt = 0;

			while (me->aUserSrc_tbl[src_cnt] != USER_SRC_END)
			{
			    #ifdef USERAPP_SUPPORT_BT
				if (me->aUserSrc_tbl[src_cnt] == eAp_Bt_Src)
				{
					break;
				}
				#endif
				src_cnt++;
			}

			me->iSrc = src_cnt;

			UserSetting_SaveSrc(me->iSrc);

			status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);
			break;
		}
		case USERAPP_CLI_SRC_USB_SIG:
		{
			ap_printf("userapp receive USERAPP_CLI_SRC_USB_SIG\n");

			status = UserAppSrcJump(me, e, eAp_USB_Src);
			break;
		}
		#if (!SUPPORT_VA)
		case USERAPP_CLI_SRC_AUX_SIG:
		{
			ap_printf("userapp receive USERAPP_CLI_SRC_AUX_SIG\n");

			status = UserAppSrcJump(me, e, eAp_AUX_Src);
			break;
		}
		#endif
		case USERAPP_CLI_SRC_LINE_SIG:
		{
			ap_printf("userapp receive USERAPP_CLI_SRC_LINE_SIG\n");

			status = UserAppSrcJump(me, e, eAp_LINE_Src);
			break;
		}
		case USERAPP_CLI_SRC_OPTICAL_SIG:
		{
			ap_printf("userapp receive USERAPP_CLI_SRC_OPTICAL_SIG\n");

			status = UserAppSrcJump(me, e, eAp_SPDIF_Optical_Src);
			break;
		}
		case USERAPP_CLI_SRC_COAXIAL_SIG:
		{
			ap_printf("userapp receive USERAPP_CLI_SRC_COAXIAL_SIG\n");

			status = UserAppSrcJump(me, e, eAp_SPDIF_Coaxial_Src);
			break;
		}
		case USERAPP_CLI_SRC_ARC_SIG:
		{
			ap_printf("userapp receive USERAPP_CLI_SRC_ARC_SIG\n");

			status = UserAppSrcJump(me, e, eAp_ARC_Src);
			break;
		}
#ifdef USERAPP_SUPPORT_REPEATER
		case USERAPP_CLI_SRC_HDMI0_SIG:
		{
			ap_printf("userapp receive USERAPP_CLI_SRC_HDMI0_SIG\n");

			status = UserAppSrcJump(me, e, eAp_HDMI0_Src);
			break;
		}
		case USERAPP_CLI_SRC_HDMI1_SIG:
		{
			ap_printf("userapp receive USERAPP_CLI_SRC_HDMI1_SIG\n");

			status = UserAppSrcJump(me, e, eAp_HDMI1_Src);
			break;
		}
		case USERAPP_CLI_SRC_HDMI2_SIG:
		{
			ap_printf("userapp receive USERAPP_CLI_SRC_HDMI2_SIG\n");

			status = UserAppSrcJump(me, e, eAp_HDMI2_Src);
			break;
		}
#endif
#ifdef USERAPP_SUPPORT_BT
		case USERAPP_CLI_SRC_BT_SIG:
		{
			ap_printf("userapp receive USERAPP_CLI_SRC_BT_SIG\n");

			status = UserAppSrcJump(me, e, eAp_Bt_Src);
			break;
		}
		case USERAPP_CLI_BT_CLEAR_ALLBONDDEV_SIG:
		{
			ap_printf("userapp receive USERAPP_CLI_BT_CLEAR_ALLBONDDEV_SIG\n");

            BtClearAllBondDev();
			status = Q_HANDLED();
			break;
		}
		case USERAPP_CLI_BT_ENTER_DUT_SIG:
		{
			ap_printf("userapp receive USERAPP_CLI_BT_ENTER_DUT_SIG\n");

            SDK_TEST_IF_EnterBRTestMode();
			status = Q_HANDLED();
			break;
		}
#endif
		case USERAPP_CLI_SRC_CARD_SIG:
		{
			ap_printf("userapp receive USERAPP_CLI_SRC_CARD_SIG\n");

			status = UserAppSrcJump(me, e, eAp_CARD_Src);
			break;
		}
		case USERAPP_CLI_SRC_I2S_SIG:
		{
			ap_printf("userapp receive USERAPP_CLI_SRC_CARD_SIG\n");

			status = UserAppSrcJump(me, e, eAp_I2S_Src);
			break;
		}
		case USERAPP_CLI_SRC_EARC_SIG:
		{
			ap_printf("userapp receive USERAPP_CLI_SRC_EARC_SIG\n");

			status = UserAppSrcJump(me, e, eAp_ARC_Src);
			break;
		}

#if (SUPPORT_DEMOMODE)
		case USERAPP_CLI_SRC_RAW_DATA_SIG:
		{
			ap_printf("userapp receive USERAPP_CLI_SRC_RAW_DATA_SIG\n");
			if(!me->demoState)
			{
				me->demoBackSrc = me->iSrc;
				me->demoState = 1;
				me->iSrc = eApUserAppSrc_Num;
				if (me->playercreat == 1)
				{
					MFP_Destroy(me->pAPInstance);
					me->playercreat = 0;
					me->pAPInstance = NULL;
				}
				UserAppMute(me,1);
				status = Q_TRAN(UserApp_active_Raw_Data);
			}
			else
			{
				me->demoState = 0;
				status = UserAppSrcJump(me, e, me->demoBackSrc);
			}
			break;
		}
#endif

#if (SUPPORT_VA)
		case AUXVA_SW_SIG:
		{
			ap_printf("userapp receive AUXVA_SW_SIG\n");

			AuxVoiceAssistSw(me);
			status = Q_HANDLED();
			break;
		}
		case BTVA_SW_SIG:
		{
			ap_printf("userapp receive BTVA_SW_SIG\n");

			BtVoiceAssistSw(me);
			status = Q_HANDLED();
			break;
		}
		case ALL_VAON_SIG:
		{
			ap_printf("userapp receive ALL_VAON_SIG\n");

			AuxBtVAOn(me);
			status = Q_HANDLED();
			break;
		}
		case ALL_VAOFF_SIG:
		{
			ap_printf("userapp receive ALL_VAOFF_SIG\n");

			AuxBtVAOff(me);
			status = Q_HANDLED();
			break;
		}
		case GET_ENERGY_STATE_SIG:
		{
    		UINT32 energy_aux_va, energy_bt_va, energy_bgm;
			energy_aux_va = AudDspService_VoiceAssist_Contrl(NULL, VA_Get_VA_Enr, 0, 0);
			energy_bt_va = AudDspService_VoiceAssist_Contrl(NULL, VA_Get_VA_Enr, 1, 0);
			energy_bgm = AudDspService_VoiceAssist_Contrl(NULL, VA_Get_BGM_Enr, 0, 0);

			ap_printf("energy_aux_va:0x%x, energy_bt_va:0x%x, energy_bgm:0x%x\n", energy_aux_va, energy_bt_va, energy_bgm);
			status =  Q_HANDLED();
			break;
		}
#endif

#ifdef CONFIG_USB_LOGO_TEST
		case USERAPP_CLI_DEV_LIST_SIG:
		{
			int i;
			int j = 0;
			int devnum = 0;
			DeviceInfo_t *device;
			DeviceInfo_t uDevice[MAX_DEVICE_NUM];
			for(i = 0; i < MAX_DEVICE_NUM;  ++i)
			{
				devnum = 0;
				device = DevList(i);
				if(device == NULL)
				{
					continue;
				}
				strncpy(uDevice[i].devName, device->devName, MAX_DEVICE_NAME_NUM);
				strncpy(uDevice[i].diskName, device->diskName, MAX_DISK_NAME_NUM);
				if (strcasecmp(device->diskName, "USB0:") == 0){
					devnum = 0 + 1;
				} else if (strcasecmp(device->diskName, "USB1:") == 0){
					devnum = 1 + 1;
				} else if (strcasecmp(device->diskName, "USB2:") == 0){
					devnum = 2 + 1;
				} else if (strcasecmp(device->diskName, "USB3:") == 0) {
					devnum = 3 + 1;
				}

				ap_printf("%d:devName:%s\n", devnum, uDevice[i].devName);
				j++;
				//ap_printf("%d:devName:%s, diskName:%s\n",++j, uDevice[i].devName, uDevice[i].diskName);
			}
			ap_printf("USB Dev Num:%d\n", j);

			status = Q_HANDLED();
			break;
		}
		case TIME_DEV_PLAY_SIG:
		{
			int ret = umount("USB:");
			if (ret < 0)
			{
				ap_printf("unmount device faild\n");
			}
			else
			{
				ap_printf("unmount device success\n");
			}
			select_usb_port_num(me->playerSource);
			QTimeEvt_disarmX(&me->timeDevPlay);
			AudDspService_Flow_Stop(NULL);
			QActive *QPPlayerGet(void);
			QACTIVE_POST(QPPlayerGet(), Q_NEW(QEvt, MFP_USBIF_SOURCE_IN_SIG), (void *)0);
			status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);
			break;
		}
		case USERAPP_CLI_PLAY_DEV_NUM1_SIG:
		{
#ifdef SUPPORT_AP
	     	if(me->playercreat == 1)
			{
	    		MFP_Destroy(me->pAPInstance);
	    		me->pAPInstance = NULL;
				me->playercreat = 0;
	            me->showErr = 0;
			}
#endif
			me->playerSource = 0;
			me->iSrc = APP_SRC_USB;//eAp_USB_Src;
			QTimeEvt_disarmX(&me->timeDevPlay);
			QTimeEvt_armX(&me->timeDevPlay, (TICKS_PER_SEC), 0);
			//status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);
			status = Q_HANDLED();
			break;
		}
		case USERAPP_CLI_PLAY_DEV_NUM2_SIG:
		{
#ifdef SUPPORT_AP
	     	if(me->playercreat == 1)
			{
	    		MFP_Destroy(me->pAPInstance);
	    		me->pAPInstance = NULL;
				me->playercreat = 0;
	            me->showErr = 0;
			}
#endif
			me->playerSource = 1;
			me->iSrc = APP_SRC_USB;//eAp_USB_Src;
			QTimeEvt_disarmX(&me->timeDevPlay);
			QTimeEvt_armX(&me->timeDevPlay, (TICKS_PER_SEC), 0);
			//status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);
			status = Q_HANDLED();
			break;
		}
		case USERAPP_CLI_PLAY_DEV_NUM3_SIG:
		{
#ifdef SUPPORT_AP
	     	if(me->playercreat == 1)
			{
	    		MFP_Destroy(me->pAPInstance);
	    		me->pAPInstance = NULL;
				me->playercreat = 0;
	            me->showErr = 0;
			}
#endif
			me->playerSource = 2;
			me->iSrc = APP_SRC_USB;//eAp_USB_Src;
			QTimeEvt_disarmX(&me->timeDevPlay);
			QTimeEvt_armX(&me->timeDevPlay, (TICKS_PER_SEC), 0);
			//status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);
			status = Q_HANDLED();
			break;
		}
		case USERAPP_CLI_PLAY_DEV_NUM4_SIG:
		{
#ifdef SUPPORT_AP
	     	if(me->playercreat == 1)
			{
	    		MFP_Destroy(me->pAPInstance);
	    		me->pAPInstance = NULL;
				me->playercreat = 0;
	            me->showErr = 0;
			}
#endif
			me->playerSource = 3;
			me->iSrc = APP_SRC_USB;//eAp_USB_Src;
			QTimeEvt_disarmX(&me->timeDevPlay);
			QTimeEvt_armX(&me->timeDevPlay, (TICKS_PER_SEC), 0);
			//status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);
			status = Q_HANDLED();
			break;
		}
#endif

        //case POWER_SIG...USB_TICK_SIG:
#ifdef CERTTEST_IF
        case ZERO_RELEASE_SIG:
        case ONE_RELEASE_SIG:
        case TWO_RELEASE_SIG:
        case THREE_RELEASE_SIG:
        case FOUR_RELEASE_SIG:
        case FIVE_RELEASE_SIG:
		case SIX_RELEASE_SIG:
		case SEVEN_RELEASE_SIG:
		case EIGHT_RELEASE_SIG:
		case NINE_RELEASE_SIG:
		case PRE_RELEASE_SIG:
		case NEXT_RELEASE_SIG:
		case FB_RELEASE_SIG:
        case FF_RELEASE_SIG:
        case STOP_RELEASE_SIG:
        case KOK_ECHO_DN_RELEASE_SIG:
		case KOK_KEY_SHIFT_DN_RELEASE_SIG:
		case FOUR_LONG_RELEASE_SIG:
		case FIVE_LONG_RELEASE_SIG:
		case SIX_LONG_PRESS_SIG:
		case SEVEN_LONG_RELEASE_SIG:
		case EIGHT_LONG_RELEASE_SIG:
        case NINE_LONG_RELEASE_SIG:
        {
            ap_printf("get CertTest Signal...Userapp_active \n");
            QMSM_DISPATCH((QMsm *)me->stCertTest_IF, e);
            status = Q_HANDLED();
            break;
        }
#endif
		case STOP_LONG_RELEASE_SIG:
		{
			// disable version check
			ap_printf("disable version check!\n");
			UpgradeService_SetVersionCheck(&me->super, FALSE);
			status = Q_HANDLED();
		}

#ifdef MCU_IF_SP
		case MCU_IF_USER_VOLSET_SIG:
		{
			ap_printf("MCU_IF_USER_VOLSET_SIG...Userapp_active \n");
			MCUIF_Vol_Evt_t *pstMCUIF_Vol_Evt_t = ((MCUIF_Vol_Evt_t *)e) ;
			me->vol = pstMCUIF_Vol_Evt_t->vol_data.volume;

			UserAppVolumeSet(me->vol, me->hAmpList);
			UserAppNum2Str(strBuf, me->vol);
			VFD_Service_POP(me, strBuf, 2, RIGHT);
			status = Q_HANDLED();
			break;
		}
		case MCU_IF_USER_UPGRADE_START_SIG:
		{
			ap_printf("MCU_IF_USER_UPGRADE_START_SIG...Userapp_active \n");
			PowerService_Reset(&me->super);
			QACTIVE_POST(&me->super, Q_NEW(QEvt, POWER_RELEASE_SIG), (void *)0);
			status = Q_HANDLED();
			break;
		}
		case MCU_IF_USER_SWITCHSRC_SIG:
		{
			ap_printf("MCU_IF_USER_SWITCHSRC_SIG...Userapp_active \n");
			MCUIF_Src_Evt_t *pstMCUIF_Src_Evt_t = ((MCUIF_Src_Evt_t *)e) ;

			if (pstMCUIF_Src_Evt_t->src_data.src < eApUserAppSrc_Num)
			{
				int src_cnt = 0;

				for (src_cnt = 0; me->aUserSrc_tbl[src_cnt] != USER_SRC_END; src_cnt++)
				{
					if (pstMCUIF_Src_Evt_t->src_data.src == me->aUserSrc_tbl[src_cnt])
					{
						break;
					}
				}
				me->iSrc = src_cnt;

				UserSetting_SaveSrc(me->iSrc);

				status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);
			}
			else
			{
				ap_printf("MCU_IF_UART1_SWITCHSRC_SIG...fail : %d\n", pstMCUIF_Src_Evt_t->src_data.src);
				status = Q_HANDLED();
			}
			break;
		}
		case MCU_IF_USER_BAUDSET0_SIG:
		{
			ap_printf("MCU_IF_USER_BAUDSET 0 SIG\n");
			MCUIF_SP_set_baud(&me->stMCU_IF, eUART_BR_115200);
			status = Q_HANDLED();
			break;
		}
		case MCU_IF_USER_BAUDSET1_SIG:
		{
			ap_printf("MCU_IF_USER_BAUDSET 1 SIG\n");
			MCUIF_SP_set_baud(&me->stMCU_IF, eUART_BR_921600);
			status = Q_HANDLED();
			break;
		}
		case MCU_IF_USER_BAUDSET2_SIG:
		{
			ap_printf("MCU_IF_USER_BAUDSET 2 SIG\n");
			MCUIF_SP_set_baud(&me->stMCU_IF, eUART_BR_1843200);
			status = Q_HANDLED();
			break;
		}
		case MCU_IF_USER_BAUDSET3_SIG:
		{
			ap_printf("MCU_IF_USER_BAUDSET 3 SIG\n");
			MCUIF_SP_set_baud(&me->stMCU_IF, eUART_BR_3686400);
			status = Q_HANDLED();
			break;
		}

#ifdef MCU_MASTER

		case ONE_RELEASE_SIG:
		{
			MCUIF_SP_set_vol(&me->stMCU_IF, 23);
			status = Q_HANDLED();
			break;
		}
		case TWO_RELEASE_SIG:
		{
			MCUIF_SP_set_src(&me->stMCU_IF, eAp_USB_Src);
			status = Q_HANDLED();
			break;
		}
		case THREE_RELEASE_SIG:
		{
			MCUIF_SP_set_src(&me->stMCU_IF, eAp_CARD_Src);
			status = Q_HANDLED();
			break;
		}
		case FOUR_RELEASE_SIG:
		{
			MCUIF_SP_set_src(&me->stMCU_IF, eAp_AUX_Src);
			status = Q_HANDLED();
			break;
		}
		case FIVE_RELEASE_SIG:
		{
			MCUIF_SP_set_src(&me->stMCU_IF, eAp_LINE_Src);
			status = Q_HANDLED();
			break;
		}
		case SIX_RELEASE_SIG:
		{
			MCUIF_SP_set_src(&me->stMCU_IF, eAp_I2S_Src);
			status = Q_HANDLED();
			break;
		}
		case SEVEN_RELEASE_SIG:
		{
			MCUIF_SP_set_src(&me->stMCU_IF, eAp_SPDIF_Optical_Src);
			status = Q_HANDLED();
			break;
		}
#if defined(CFG_COAX_ENABLE) && (CFG_COAX_ENABLE == 1)
		case EIGHT_RELEASE_SIG:
		{
			MCUIF_SP_set_src(&me->stMCU_IF, eAp_SPDIF_Coaxial_Src);
			status = Q_HANDLED();
			break;
		}
#endif
		case NINE_RELEASE_SIG:
		{
#if 1
			char* app_bin_file_path = "USB:/spupdate.bin";
			int sdRet = MCUIF_SP_upgrade_isp(&me->stMCU_IF, app_bin_file_path);
			if (!sdRet)
			{
				LOGD("%s: ISP Upgrade_Start \n", __FUNCTION__);
			}
			else
			{
				LOGD("%s: ISP Upgrade_Fail \n", __FUNCTION__);
			}
#else
			MCUIF_SP_set_src(&me->stMCU_IF, eAp_ARC_Src);
#endif
			status = Q_HANDLED();
			break;
		}
		case ZERO_RELEASE_SIG:
		{
			char* app_bin_file_path = "USB:/spupdate.bin";
			int sdRet = MCUIF_SP_upgrade(&me->stMCU_IF, app_bin_file_path);
			if (!sdRet)
			{
				LOGD("%s: Upgrade_Start \n", __FUNCTION__);
			}
			else
			{
				LOGD("%s: Upgrade_Failed \n", __FUNCTION__);
			}
			status = Q_HANDLED();
			break;
		}
#endif
#endif

			case REPEATER_USER_SWITCHSRC_SIG:{
			ap_printf("REPEATER_USER_SWITCHSRC_SIG...Userapp_active \n");
#ifdef SUPPORT_AP
			if(me->playercreat == 1)
			{
    			MFP_Destroy(me->pAPInstance);
				me->playercreat = 0;
    			me->pAPInstance = NULL;
			}
#endif

			RepeaterSrcEvt_t *pstRepeater_Src_t = ((RepeaterSrcEvt_t *)e) ;
			ap_printf("pstRepeater_Src_t->src_user = %d \n",pstRepeater_Src_t->src_user);

			int src_cnt = 0;


			for (src_cnt = 0; me->aUserSrc_tbl[src_cnt] != USER_SRC_END; src_cnt++)
			{
				if(pstRepeater_Src_t->src_user == me->aUserSrc_tbl[src_cnt])
					me->iSrc = src_cnt;
			}
			ap_printf(" me->iSrc = %d \n",me->iSrc);

			UserSetting_SaveSrc(me->iSrc);
			//LoadUserAppSrc(me, 0);

			status =  Q_TRAN(UserappStateTab[me->aUserSrc_tbl[me->iSrc]]);
			break;
		}
		case REPEATER_HDMI_HPD_SIG:{
			RepeaterHPDEvt_t * RepEv = (RepeaterHPDEvt_t*)e;
			ap_printf("REPEATER_HDMI_HPD_SIG\n");
			me->HPD_State = RepEv->Hpd_status;
			status = Q_HANDLED();
			break;
		}
		case REPEATER_MUTE_AUDIO_OUTPUT_SIG:{
			RepeaterAudOutputEvt_t *Repeater_Routing_t = ((RepeaterAudOutputEvt_t *)e) ;
			if( Repeater_Routing_t->Aud_output == eRepeater_TV_Speaker)
			{
				ap_printf("TV Speaker output audio,  mute Amp \n");
			}
			else
			{
				ap_printf("Audio system output audio , demute Amp\n");
			}
			status = Q_HANDLED();
			break;
		}

#ifdef HintSoundTest // for hint sound test
		case SIX_RELEASE_SIG:
		{
			ap_printf("SIX_RELEASE_SIG\n");
			UserAppCallHint(me, eHint_BtConnect);
			status = Q_HANDLED();
			break;
		}
#endif

#if (SUPPORT_SRC_NAME)
		case SRC_HINT_TIME_SIG:{
			ap_printf("recv SRC_HINT_TIME_SIG\n");
			switch(me->aUserSrc_tbl[me->iSrc]){
				case eAp_USB_Src:
					UserAppCallHint(me, eHint_SrcUSB);
					break;
				case eAp_CARD_Src:
					UserAppCallHint(me, eHint_SrcCard);
					break;
					
#if (!SUPPORT_VA)
				case eAp_AUX_Src:
					UserAppCallHint(me, eHint_SrcAux);
					break;
#endif
				case eAp_LINE_Src:
					UserAppCallHint(me, eHint_SrcLine);
					break;
				case eAp_I2S_Src:
					UserAppCallHint(me, eHint_SrcBT);
					break;
				case eAp_SPDIF_Optical_Src:
					UserAppCallHint(me, eHint_SrcOpt);
					break;
#if defined(CFG_COAX_ENABLE) && (CFG_COAX_ENABLE == 1)
				case eAp_SPDIF_Coaxial_Src:
					UserAppCallHint(me, eHint_SrcCoax);
					break;
#endif
				case eAp_ARC_Src:
					//if(me->eARC_SRC == eUser_eARCSRC)
					//{
				//		UserAppCallHint(me, eHint_SrceARC);
				//	}
				//	else
					{
						UserAppCallHint(me, eHint_SrcARC);
					}
					break;
#if 1//def USERAPP_SUPPORT_REPEATER
				case eAp_HDMI0_Src:
					UserAppCallHint(me, eHint_SrcHDMI0);
					break;
				case eAp_HDMI1_Src:
					UserAppCallHint(me, eHint_SrcHDMI1);
					break;
				case eAp_HDMI2_Src:
					UserAppCallHint(me, eHint_SrcHDMI2);
					break;
#endif
#ifdef USERAPP_SUPPORT_BT
				case eAp_Bt_Src:
					UserAppCallHint(me, eHint_SrcBT);
					break;
#endif
				default:
					break;
			}
			status = Q_HANDLED();
			break;
		}
#endif
		case HINT_DELAY_TIME_SIG:{
			ap_printf("recv HINT_DELAY_TIME_SIG, hintPlayFlag:%d\n", me->hintPlayFlag);
			if(me->hintPlayFlag)
			{
				QTimeEvt_disarmX(&me->timeHintDelayEvt);
				QTimeEvt_armX(&me->timeHintDelayEvt, (TICKS_PER_100MS / 5), 0);
			}
			else
			{
				if(me->hintDelayFlag == 1)
				{
					int ret = UserAppDelayHint(me, me->hintType);
					if(ret < 0) {
						ap_printf_E(" hint data init error\n");
						status = Q_HANDLED();
						break;
					}
				}
				else
				{
					UserAppHintPlay(me);
				}
			}
			status = Q_HANDLED();
			break;
		}
		case HINT_INTERVAL_TIME_SIG:{
			ap_printf("recv HINT_INTERVAL_TIME_SIG\n");
			UserAppHintPlay(me);
			status = Q_HANDLED();
			break;
		}
		case HINT_CONNECT_SIG:{
			UserAppCallHint(me, eHint_BtConnect);
			status = Q_HANDLED();
			break;
		}
		case HINT_DISCONNECT_SIG:{
			UserAppCallHint(me, eHint_BtDisconnect);
			status = Q_HANDLED();
			break;
		}
		case HINT_SPEAKER_TEST_SIG:{
			UserAppCallHint(me, eHint_Speaker);
			status = Q_HANDLED();
			break;
		}
		case HINT_FW_VERSION_SIG:{
			UserAppCallHint(me, eHint_FwVersion);
			status = Q_HANDLED();
			break;
		}
		case HINT_INFO_SIG:{
			UserAppCallHint(me, eHint_Info);
			status = Q_HANDLED();
			break;
		}
		case HINT_BT_SEARCH_SIG:{
			UserAppCallHint(me, eHint_BtSearching);
			status = Q_HANDLED();
			break;
		}
		case HINT_DETECT_SEARCH_SIG:{
			UserAppCallHint(me, eHint_DetectSearching);
			status = Q_HANDLED();
			break;
		}
		case HINT_EQMOCIE_SIG:{
			UserAppCallHint(me, eHint_EQMovie);
			status = Q_HANDLED();
			break;
		}
		case HINT_EQMUSIC_SIG:{
			UserAppCallHint(me, eHint_EQMusic);
			status = Q_HANDLED();
			break;
		}
		case HINT_EQGAME_SIG:{
			UserAppCallHint(me, eHint_EQGame);
			status = Q_HANDLED();
			break;
		}
		case HINT_STOP_SIG:{
			ap_printf("recv HINT_STOP_SIG\n");
			UserAppHintStop(me);
			status = Q_HANDLED();
			break;
		}
		case HINT_END_SIG:{
			ap_printf("recv HINT_END_SIG\n");
			me->hintPlayFlag = 0;
			hintEndProcess();
			status = Q_HANDLED();
			break;
		}
		case HINT_NEXT_SIG:{
			ap_printf("recv HINT_NEXT_SIG\n");
			int interval_time = getCurHintInterval();
			if(interval_time == 0)
			{
				UserAppHintPlay(me);
			}
			else
			{
				QTimeEvt_disarmX(&me->timeHintIntervalEvt);
				QTimeEvt_armX(&me->timeHintIntervalEvt, (TICKS_PER_100MS/2) * interval_time, 0);
			}
			status = Q_HANDLED();
			break;
		}

#ifdef USERAPP_SUPPORT_KARAOKE
		case KOK_TURN_ON_SIG:
		case KOK_TURN_OFF_SIG:
		case KOK_BGM_VOL_UP_TICK_SIG:
		case KOK_BGM_VOL_UP_RELEASE_SIG:
		case KOK_BGM_VOL_DN_TICK_SIG:
		case KOK_BGM_VOL_DN_RELEASE_SIG:
		case KOK_MIC1_VOL_UP_TICK_SIG:
		case KOK_MIC1_VOL_UP_RELEASE_SIG:
		case KOK_MIC1_VOL_DN_TICK_SIG:
		case KOK_MIC1_VOL_DN_RELEASE_SIG:
		case KOK_MIC2_VOL_UP_TICK_SIG:
		case KOK_MIC2_VOL_UP_RELEASE_SIG:
		case KOK_MIC2_VOL_DN_TICK_SIG:
		case KOK_MIC2_VOL_DN_RELEASE_SIG:
		case NINE_TICK_SIG:      //KOK_KEY_SHIFT_UP_TICK_SIG:
		case NINE_RELEASE_SIG:   //KOK_KEY_SHIFT_UP_RELEASE_SIG:
		case KOK_KEY_SHIFT_DN_TICK_SIG:
		case KOK_KEY_SHIFT_DN_RELEASE_SIG:
		case SEVEN_TICK_SIG:     //KOK_ECHO_UP_TICK_SIG:
		case SEVEN_RELEASE_SIG:  //KOK_ECHO_UP_RELEASE_SIG:
		case KOK_ECHO_DN_TICK_SIG:
		case KOK_ECHO_DN_RELEASE_SIG:
		case KOK_VC_LVL_RELEASE_SIG:
		case KOK_VC_LVL_TICK_SIG:
		case KOK_NR_RELEASE_SIG:
		{
			ap_printf("DISPATCH to karaoke state\n");
			QMSM_DISPATCH((QMsm *)me->Kara_effect, e);

			status = Q_HANDLED();
			break;
		}
#endif
		case UP_RELEASE_SIG:
		case UP_TICK_SIG:   //{{Added by Heaven
		{
			switch(me->menu_status)
			{
				default:
				case eSoundTreble:
					if(me->Treble_vol < 12)
					{
						me->Treble_vol++;
						UserSetting_SaveTreble(me->Treble_vol);

						UserApp_SetTreble(me, me->Treble_vol);
					}

					if(me->Treble_vol > 6)
						sprintf(strBuf, "TRE+%d", me->Treble_vol-6);
					else if(me->Treble_vol < 6)
						sprintf(strBuf, "TRE-%d", 6-me->Treble_vol);
					else	
						sprintf(strBuf, "TRE %d", 6-me->Treble_vol);
					break;
					
				case eSoundFront:
					if(me->LR_vol< 10)
					{
						me->LR_vol++;
						UserSetting_SaveLRVol(me->LR_vol);

						UserApp_SetLRVol(me, me->LR_vol);
					}

					if(me->LR_vol > 5)
						sprintf(strBuf, "FLR+%d", me->LR_vol-5);
					else if(me->LR_vol < 5)
						sprintf(strBuf, "FLR-%d", 5-me->LR_vol);
					else	
						sprintf(strBuf, "FLR %d", 5-me->LR_vol);
					break;
					
				case eSoundCenter:
					if(me->Centel_vol< 10)
					{
						me->Centel_vol++;
						UserSetting_SaveCenterVol(me->Centel_vol);

						UserApp_SetCenterVol(me, me->Centel_vol);
					}

					if(me->Centel_vol > 5)
						sprintf(strBuf, "CEN+%d", me->Centel_vol-5);
					else if(me->Centel_vol < 5)
						sprintf(strBuf, "CEN-%d", 5-me->Centel_vol);
					else	
						sprintf(strBuf, "CEN %d", 5-me->Centel_vol);
					break;

				case eSoundLsRs:
					if(me->Ls_vol< 10)
					{
						me->Ls_vol++;
						UserSetting_SaveLsRs(me->Ls_vol);

						UserApp_SetLsRsVol(me, me->Ls_vol);
					}

					if(me->Ls_vol > 5)
						sprintf(strBuf, "SLR+%d", me->Ls_vol-5);
					else if(me->Ls_vol < 5)
						sprintf(strBuf, "SLR-%d", 5-me->Ls_vol);
					else	
						sprintf(strBuf, "SLR %d", 5-me->Ls_vol);
					break;
				
			}
			UserAppDisplayOnce(me, strBuf, 3);
			QTimeEvt_disarmX(&me->timeSettingMenu);
			QTimeEvt_armX(&me->timeSettingMenu, TICKS_PER_SEC*3, 0);
			status = Q_HANDLED();
			break;
		}
		case DOWN_RELEASE_SIG:
		case DOWN_TICK_SIG: //{{Added by Heaven
		{
			switch(me->menu_status)
			{
				default:
				case eSoundTreble:
					if(me->Treble_vol > 0)
					{
						me->Treble_vol--;
						UserSetting_SaveTreble(me->Treble_vol);

						UserApp_SetTreble(me, me->Treble_vol);
					}

					if(me->Treble_vol > 6)
						sprintf(strBuf, "TRE+%d", me->Treble_vol-6);
					else if(me->Treble_vol < 6)
						sprintf(strBuf, "TRE-%d", 6-me->Treble_vol);
					else	
						sprintf(strBuf, "TRE %d", 6-me->Treble_vol);
					break;
					
				case eSoundFront:
					if(me->LR_vol> 0)
					{
						me->LR_vol--;
						UserSetting_SaveLRVol(me->LR_vol);

						UserApp_SetLRVol(me, me->LR_vol);
					}

					if(me->LR_vol > 5)
						sprintf(strBuf, "FLR+%d", me->LR_vol-5);
					else if(me->LR_vol < 5)
						sprintf(strBuf, "FLR-%d", 5-me->LR_vol);
					else	
						sprintf(strBuf, "FLR %d", 5-me->LR_vol);
					break;
					
				case eSoundCenter:
					if(me->Centel_vol> 0)
					{
						me->Centel_vol--;
						UserSetting_SaveCenterVol(me->Centel_vol);

						UserApp_SetCenterVol(me, me->Centel_vol);
					}

					if(me->Centel_vol > 5)
						sprintf(strBuf, "CEN+%d", me->Centel_vol-5);
					else if(me->Centel_vol < 5)
						sprintf(strBuf, "CEN-%d", 5-me->Centel_vol);
					else	
						sprintf(strBuf, "CEN %d", 5-me->Centel_vol);
					break;
					
				case eSoundLsRs:
					if(me->Ls_vol > 0)
					{
						me->Ls_vol--;
						UserSetting_SaveLsRs(me->Ls_vol);

						UserApp_SetLsRsVol(me, me->Ls_vol);
					}

					if(me->Ls_vol > 5)
						sprintf(strBuf, "SLR+%d", me->Ls_vol-5);
					else if(me->Ls_vol < 5)
						sprintf(strBuf, "SLR-%d", 5-me->Ls_vol);
					else	
						sprintf(strBuf, "SLR %d", 5-me->Ls_vol);
					break;
			
			}
			UserAppDisplayOnce(me, strBuf, 3);
			QTimeEvt_disarmX(&me->timeSettingMenu);
			QTimeEvt_armX(&me->timeSettingMenu, TICKS_PER_SEC*3, 0);
			status = Q_HANDLED();
			break;
		}

		case MENU_RELEASE_SIG:
		case MENU_LONG_RELEASE_SIG: //{{Added by Heaven
		{
			me->menu_status++;
			if(me->menu_status >= eSoundMax)
				me->menu_status = eSoundTreble;
						
			switch(me->menu_status)
			{
				default:
				case eSoundTreble:
					if(me->Treble_vol > 6)
						sprintf(strBuf, "TRE+%d", me->Treble_vol-6);
					else if(me->Treble_vol < 6)
						sprintf(strBuf, "TRE-%d", 6-me->Treble_vol);
					else	
						sprintf(strBuf, "TRE %d", 6-me->Treble_vol);
					break;
					
				case eSoundFront:
					ap_printf("-----me->LR_vol = %d\n",me->LR_vol);
					if(me->LR_vol > 5)
						sprintf(strBuf, "FLR+%d", me->LR_vol-5);
					else if(me->LR_vol < 5)
						sprintf(strBuf, "FLR-%d", 5-me->LR_vol);
					else	
						sprintf(strBuf, "FLR %d", 5-me->LR_vol);
					break;
					
				case eSoundCenter:

					if(me->Centel_vol > 5)
						sprintf(strBuf, "CEN+%d", me->Centel_vol-5);
					else if(me->Centel_vol < 5)
						sprintf(strBuf, "CEN-%d", 5-me->Centel_vol);
					else	
						sprintf(strBuf, "CEN %d", 5-me->Centel_vol);
					break;

				case eSoundLsRs:

					if(me->Ls_vol > 5)
						sprintf(strBuf, "SLR+%d", me->Ls_vol-5);
					else if(me->Ls_vol < 5)
						sprintf(strBuf, "SLR-%d", 5-me->Ls_vol);
					else	
						sprintf(strBuf, "SLR %d", 5-me->Ls_vol);
					break;
		
			}
			UserAppDisplayOnce(me, strBuf, 3);
			QTimeEvt_disarmX(&me->timeSettingMenu);
			QTimeEvt_armX(&me->timeSettingMenu, TICKS_PER_SEC*3, 0);
			
			status = Q_HANDLED();
			break;
		}
		case ODSP_RESET_START_SIG:
		{
			QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, STOP_RELEASE_SIG), NULL);

			AudDspService_Flow_Stop(NULL);
			AudDspService_SPDIF_STOP(NULL);

			status = Q_HANDLED();
			break;
		}
		case ODSP_RESET_END_SIG:
		{

			status = UserAppSrcJump(me, e, me->iSrc);
			if(me->aUserSrc_tbl[me->iSrc] == eAp_USB_Src || me->aUserSrc_tbl[me->iSrc] == eAp_CARD_Src){
	#ifdef SUPPORT_AP
			if (me->playercreat == 1){
				MFP_Destroy(me->pAPInstance);
				me->playercreat = 0;
				me->pAPInstance = NULL;
			}
	#endif

			UserAppMute(me,1);
			me->showErr = 0;
			status = Q_HANDLED();
			}
			else{
				status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);
			}

			UserSetting_SaveSrc(me->iSrc);
			//status = Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&UserApp_home);
			break;
		}
	}
	return status;
}

void UserAppNotify(void *user, mfpMsg_t msg, INT32 param)
{
	UserApp *const me = &l_UserApp;
	user = user;
	static char strBuf[14];

	ap_printf("[UserAPP]msg:%d, param:%d\n", msg, param);
	switch (msg)
	{
		case MFP_MSG_ERR:
			ap_printf("[UserAPP]MFP_MSG_ERR\n");
			me->showErr = 1;
			switch (param)
			{
				case MFP_ERR_NO_DEVICE:
					if (me->aUserSrc_tbl[me->iSrc] == eAp_USB_Src)
					{
						UserAppDisplay_Scroll(me, STR_NOUSB, R2L);
					}
					if (me->aUserSrc_tbl[me->iSrc] == eAp_CARD_Src)
					{
						UserAppDisplay_Scroll(me, STR_NOCARD, R2L);
					}
					me->usb_play_state = USB_STOP;
					UserApp_DimmerSet(me);
					break;
				case MFP_ERR_UNSUPPORT:
				case MFP_ERR_PARSE_UNSUPPORT:
				case MFP_ERR_DECODE_UNSUPPORT:
					UserAppDisplay_Scroll(me, STR_UNSUPPORT, L2R);
					break;
				case MFP_MSG_NO_FILE:
					{
						LOGD("************** MFP_MSG_NO_FILE ********************\n");
					}
					break;
				default:
					me->showErr = 0;
					break;
			}
			break;
		case MFP_MSG_STATE_CHANGE:
			ap_printf("[UserAPP]MFP_MSG_STATE_CHANGE\n");
			switch (param)
			{
				case MFP_STATE_PERPARE:
					UserApp_DimmerSet(me);

					UserAppMute(me, me->mute);
					
					me->usb_detect_state = USB_NORMAL;
					UserAppDisplay(me, STR_LOAD);
					break;
				default:
					break;
			}
			break;
		case MFP_MSG_UPDATE_FILE:
			if (!me->opType)
			{
				if(me->mute == eMute_off)
				{
					sprintf(strBuf, "T%04d", param);
				UserAppNum2Str(strBuf, param);
				VFD_Service_POP(me, strBuf, 1, RIGHT);
				}
			}
			break;
		case MFP_MSG_UPDATE_FOLDER:
			if (me->opType)
			{
				//UserAppNum2Str(strBuf, param);
				//VFD_Service_POP(me, strBuf, 1, LEFT);
				me->opType = 0;
			}
			break;
		case MFP_MSG_AUTO_PLAY:
			switch (param)
			{
				case PLAY_SRC_USB:
					me->usb_play_state = USB_PLAY;//--{{Added by Grace to save USB state
					UserAppDisplay(me, STR_USB);
					break;
				case PLAY_SRC_CARD:
					UserAppDisplay(me, STR_CARD);
					break;
				default:
					break;
			}
			break;
		case MFP_MSG_SEEK_FINISH:
			break;
		case MFP_MSG_ERR_DEVICE:
			ap_printf("[UserAPP]MFP_MSG_ERR_DEVICE\n");
			me->showErr = 1;
			UserAppDisplay_Scroll(me, STR_UNSUPPORT, L2R);
			break;
		default:
			return;
	};
}

#ifndef CONFIG_USB_LOGO_TEST
/******************************************************************************************/
/**
 * \fn			void UserApp_Source_Exit(QActive * const me, void *pParam)
 *
 * \brief		to proc source exit signal
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              void *pParam : not used\n
 *
 * \return	    none \n
 ******************************************************************************************/
static void UserApp_Source_Exit(QActive *const me, void *pParam)
{
	pParam = pParam;
	if (me)
	{
		ap_printf("%s>>>\n", __FUNCTION__);
		QACTIVE_POST(me, Q_NEW(QEvt, SERVICE_DESTROY_DONE_SIG), (void *)0);
	}
}
#endif

/******************************************************************************************/
/**
 * \fn			QState UserApp_active_AudioPlayer(UserApp * const me, QEvt const * const e)
 *
 * \brief		the state that corresponds to USB source
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_active_AudioPlayer state \n
 *			    \e status : Q_TRAN() trans to other state \n
 *              \e status : Q_SUPER() trans to UserApp_active state \n
 ******************************************************************************************/
QState UserApp_active_AudioPlayer(UserApp *const me, QEvt const *const e)
{
	QState status = 0;

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			ap_BurnTest_log("ENTRY\n");
///*jeff usb&SD card ready to open
			if (me->aUserSrc_tbl[me->iSrc] == eAp_USB_Src)
			{
				UserAppDisplay(me, STR_USB);
				VFD_DotCtrl(me, VFD_DOT_USB, VFD_DOT_OP_SET);
			}
			if (me->aUserSrc_tbl[me->iSrc] == eAp_CARD_Src)
			{
				UserAppDisplay(me, STR_CARD);
			}
#ifndef SDK_RELEASE
			ap_Memory_check();
#endif
//*/
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{

			UserAppMute(me, 1);
			
			ap_BurnTest_log("EXIT\n");
#ifndef BUILT4ISP2
			SpHdsp_Stop(NULL,0);
#endif
			me->codecType = 0;
			UserAppHintStop(me);
#ifdef SUPPORT_AP
			if (me->playercreat == 1)
			{
    				MFP_Destroy(me->pAPInstance);
    				me->playercreat = 0;
    				me->pAPInstance = NULL;
    			}
#endif
			VFD_DotCtrl(me, VFD_DOT_USB, VFD_DOT_OP_CLR);
//*/

			status = Q_HANDLED();
			break;
		}
///* jeff usb&SD card ready to open
		case SOURCE_SW_RELEASE_SIG:
		{
			ap_BurnTest_log("SOURCE_SW_RELEASE_SIG\n");
#ifdef SUPPORT_AP
            if (me->playercreat == 1)
			{
			MFP_Ctrl(me->pAPInstance, MFP_SET_SWSRC_FLAG, (void *)1);
			MFP_Destroy(me->pAPInstance);
    			me->playercreat = 0;
    			me->pAPInstance = NULL;
    		}
#endif

			UserAppMute(me, 1);
			me->showErr = 0;

			SwUserAppSrc(me, 0);
			SrcNameHint(me);
/*	jeff bt background app not ready
#ifdef USERAPP_SUPPORT_BT
			if (me->pBtBakcGroundApp != NULL)
			{
				QMSM_DISPATCH(&me->pBtBakcGroundApp->super, e);
			}
#endif
*/
			status = Q_HANDLED();
			break;
		}

#ifdef MCU_IF_SP
		case MCU_IF_USER_SWITCHSRC_SIG:
		{
			ap_printf("MCU_IF_USER_SWITCHSRC_SIG...UserApp_active_AudioPlayer \n");
		#ifdef SUPPORT_AP
			if (me->playercreat == 1)
			{
				MFP_Destroy(me->pAPInstance);
				me->playercreat = 0;
				me->pAPInstance = NULL;
			}
		#endif

			UserAppMute(me, 1);
			me->showErr = 0;
			MCUIF_Src_Evt_t *pstMCU_IF_Evt_t = ((MCUIF_Src_Evt_t *)e);

			if (pstMCU_IF_Evt_t->src_data.src < eApUserAppSrc_Num)
			{
				int src_cnt = 0;
				for (src_cnt = 0; me->aUserSrc_tbl[src_cnt] != USER_SRC_END; src_cnt++)
				{
					if (pstMCU_IF_Evt_t->src_data.src == me->aUserSrc_tbl[src_cnt])
						break;
				}
				me->iSrc = src_cnt;
			}
			else
			{
				ap_printf_E("switch src fail : %d\n", pstMCU_IF_Evt_t->src_data);
			}

			UserSetting_SaveSrc(me->iSrc);
			status = Q_HANDLED();
			break;
		}
#endif

		case SERVICE_DESTROY_DONE_SIG:
		{
			status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]);
			break;
		}
		case Q_INIT_SIG:
		{
			//GPIO_Output_Write(34, IO_HIGH);	
			me->mute = eMute_off;
			UserAppMute(me, 0);
			UartApp_SetKey_Sig(&me->super, SOURCE_SW_RELEASE_SIG, me->iSrc);

			AmpSet_SetSrcGain(me->hAmpList, me->iSrc);
			
			ap_BurnTest_log("INIT\n");
			AudDspService_Set_MediaEnable(&me->super);

#ifndef BUILT4ISP2
			AudDspService_Set_SourceGain(&me->super, SrcGain_Tab[me->iSrc]);
#endif
			setHdmiMode(0);
			me->mute = eMute_off;

			int source = -1;

#ifdef SUPPORT_AP
			switch (me->aUserSrc_tbl[me->iSrc])
			{
				case eAp_USB_Src:
					source = PLAY_SRC_USB;
					break;
				case eAp_CARD_Src:
					source = PLAY_SRC_CARD;
					break;
			}
			
            if(source > -1) {
                if(MFP_Create(&me->pAPInstance, "PlayerQP", source, (void *)&me->super) == MFP_ERR_NO) {
                    me->playercreat = 1;
                }
            }
#endif
            if(me->pAPInstance) {
                MFP_PlayModeSet(me->pAPInstance, me->iShuffle);
					me->iRepeat = REPEAT_MODE_ALL;
                MFP_PlayRepeatSet(me->pAPInstance, me->iRepeat);

                mfpListener_t mfpListener = {me->pAPInstance, UserAppNotify};
                MFP_Ctrl(me->pAPInstance, MFP_SET_LISTENER, (void *)&mfpListener);

#ifndef CONFIG_USB_LOGO_TEST
                MFP_Ctrl(me->pAPInstance, MFP_SET_DESTORY_LISTENER, UserApp_Source_Exit);
#endif

                CFG_PLAYER_T *pstPlayerCfg = GET_SDK_CFG(CFG_PLAYER_T);
                if(pstPlayerCfg->nAutoPlay & CFG_AUTOPLAY_USB_ENABLE
                   || pstPlayerCfg->nAutoPlay & CFG_AUTOPLAY_CARD_ENABLE) {
                    if(MFP_PlayNext(me->pAPInstance, PLAY_CUR_FILE) != MFP_ERR_NO) {
                        if(me->aUserSrc_tbl[me->iSrc] == eAp_USB_Src) {
                            UserAppDisplay_Scroll(me, STR_NOUSB, R2L);
                        }
                        if(me->aUserSrc_tbl[me->iSrc] == eAp_CARD_Src) {
                            UserAppDisplay_Scroll(me, STR_NOCARD, R2L);
                        }
                    }
                }
            }


#if (SUPPORT_VA)
			VoiceAssistService_VAStatusReset(NULL, me->AuxVA_Status, me->BtVA_Status);
			if(!me->BtVA_Status)
			{
				UserApp_SetBTMute();
			}
			if(!me->AuxVA_Status)
			{
				UserApp_SetAuxMute();
			}
#endif

			status =  Q_TRAN(&UserApp_AudioPlayer_stop);
			break;
		}
		case MFP_SWITCH_NEXT_SIG:
		{
			ap_printf("[%s]NEXT; \n", __FUNCTION__);
			MFP_PlayNext(me->pAPInstance, PLAY_NEXT_FILE);
			status = Q_HANDLED();
			break;
		}
		//case BT_MUSIC_PREV_REQ_SIG:
		case PRE_RELEASE_SIG:
		{
			ap_printf("[%s]Pre; \n", __FUNCTION__);
			MFP_PlayNext(me->pAPInstance, PLAY_PREV_FILE);
			status = Q_HANDLED();
			break;
		}
		//case BT_MUSIC_NEXT_REQ_SIG:
		case NEXT_RELEASE_SIG:
		{
			ap_printf("[%s]Next; \n", __FUNCTION__);
			MFP_PlayNext(me->pAPInstance, PLAY_NEXT_FILE);
			status = Q_HANDLED();
			break;
		}
#if (SUPPORT_FLODER)
		case UP_RELEASE_SIG:
		{
			ap_printf("[%s]Prev Floder; \n", __FUNCTION__);
			me->opType = 1;
			MFP_PlayNext(me->pAPInstance, PLAY_PREV_FOLDER);
			status = Q_HANDLED();
			break;
		}
		case DOWN_RELEASE_SIG:
		{
			ap_printf("[%s]Next Floder; \n", __FUNCTION__);
			me->opType = 1;
			MFP_PlayNext(me->pAPInstance, PLAY_NEXT_FOLDER);
			status = Q_HANDLED();
			break;
		}
#endif
		case STOP_RELEASE_SIG:
		{
			ap_printf("[%s] -> stop; \n", __FUNCTION__);

#ifdef SUPPORT_AP
			MFP_Stop(me->pAPInstance);
#endif
			if (me->aUserSrc_tbl[me->iSrc] == eAp_USB_Src)
			{
				UserAppDisplay(me, STR_USB);
			}
			if (me->aUserSrc_tbl[me->iSrc] == eAp_CARD_Src)
			{
				UserAppDisplay(me, STR_CARD);
			}
			status = Q_TRAN(&UserApp_AudioPlayer_stop);
			break;
		}
		case MFP_USB_STOP_SIG:
		{
			if(me->aUserSrc_tbl[me->iSrc] == eAp_USB_Src){
				ap_printf("[%s] -> stop; \n", __FUNCTION__);

#ifdef SUPPORT_AP
				MFP_Stop(me->pAPInstance);
#endif
				if (me->aUserSrc_tbl[me->iSrc] == eAp_USB_Src)
				{
					UserAppDisplay(me, STR_USB);
				}
				if (me->aUserSrc_tbl[me->iSrc] == eAp_CARD_Src)
				{
					UserAppDisplay(me, STR_CARD);
				}
				status = Q_TRAN(&UserApp_AudioPlayer_stop);
			}
			else{
				ap_printf("Play Card not stop...\n", __FUNCTION__);
				status = Q_HANDLED();
			}
			break;
		}
		case SHUFFLE_RELEASE_SIG:
		{
			me->iShuffle++;
			if (me->iShuffle > PLAY_MODE_SHUFFLE)
				me->iShuffle = PLAY_MODE_ORDER;

			UserAppShuffleSet(me, me->iShuffle);
			VFD_Service_POP(me, vfd_str[STR_OFF + me->iShuffle], 2, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case REPEAT_RELEASE_SIG:
		{
			me->iRepeat++;
			if (me->iRepeat >= REPEAT_MODE_USER)
				me->iRepeat = REPEAT_MODE_NOTHING;

			UserAppRepeatSet(me, me->iRepeat);
			VFD_Service_POP(me, vfd_str[STR_NONE + me->iRepeat], 2, RIGHT);
			status = Q_HANDLED();
			break;
		}
		case MFP_STOP_OVER_SIG:
		{
			ap_printf("[%s]: MFP_STOP_OVER_SIG \n", __FUNCTION__);
			status = Q_HANDLED();
			break;
		}
		case MFP_SOURCE_OUT_SIG:
		{
			ap_printf("[%s]: MFP_SOURCE_OUT_SIG \n", __FUNCTION__);
			me->showErr = 0;
			status = Q_TRAN(&UserApp_AudioPlayer_stop);
			break;
		}
		default:
		{
			status = Q_SUPER(&UserApp_active);
			break;
		}
	}
	return status;
}

#ifdef USERAPP_SUPPORT_BT
/******************************************************************************************/
/**
 * \fn			QState UserApp_active_Bt(UserApp * const me, QEvt const * const e)
 *
 * \brief		the state that corresponds to BT source
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_active_Bt state \n
 *			    \e status : Q_TRAN() trans to other state \n
 *              \e status : Q_SUPER() trans to UserApp_active state \n
 ******************************************************************************************/
QState UserApp_active_Bt(UserApp *const me, QEvt const *const e)
{
	QState status = 0;
	BtAddr_t BtAddr = {{0}};
	//BT_PROFILE_STATE_e state;
	BT_PROFILE_STATE_e state = DISCONNECT_OK;

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{

			//BtGetAddrByDevRole(&BtAddr, PHONE_ADDR);
			//BtGetProfileState(&BtAddr, A2DP_SINK, &state);

			BtCheckAllDeviceProfileState(AVDTP,&state);

			BtModeOn();

			if (state != CONNECT_OK)
			{
				UserAppDisplay_Blink(me, STR_BT, 10, 10);
				BtScanModeSet(BOTH_DISCOVERY_CONNECTABLE);
			}
			else
			{
				UserAppDisplay(me, STR_BT);
			}

			ap_BurnTest_log("ENTRY\n");
#ifndef SDK_RELEASE
			ap_Memory_check();
#endif

			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			ap_BurnTest_log("EXIT\n");
			me->codecType = 0;
			UserAppHintStop(me);
#ifndef BUILT4ISP2
			SpHdsp_Stop(NULL,0);
#endif

			BtModeOff();
			BtAllowAudioDataIn(OFF);

			UserAppMute(me, 1);
			AudDspService_Flow_Stop(NULL);
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			ap_BurnTest_log("INIT\n");
#ifndef BUILT4ISP2
			AudDspService_Set_SourceGain(&me->super, SrcGain_Tab[me->iSrc]);
#endif
			setHdmiMode(0);
			AudDspService_Set_SoundMode(NULL,1); //1:AUDIO_SOURCE_STREAM1
#if (SUPPORT_VA)
			VoiceAssistService_VAStatusReset(NULL, me->AuxVA_Status, me->BtVA_Status);
			if(!me->BtVA_Status)
			{
				UserApp_SetBTMute();
			}
			if(!me->AuxVA_Status)
			{
				UserApp_SetAuxMute();
			}
#endif
			UserAppMute(me, me->mute);

			status =  Q_HANDLED();
			break;
		}
		case BT_MUSIC_PREV_REQ_SIG:
		case PRE_RELEASE_SIG:
		{
			//BtGetAddrByDevRole(&BtAddr, PHONE_ADDR);
			//BtGetProfileState(&BtAddr, A2DP_SINK, &state);
			BtCheckAllDeviceProfileState(AVDTP,&state);

			if (state == CONNECT_OK)
			{
				UserAppDisplayOnce(me, vfd_str[STR_PREV], 1);
				BtPrev();
			}

			status = Q_HANDLED();
			break;
		}
		case BT_MUSIC_NEXT_REQ_SIG:
		case NEXT_RELEASE_SIG:
		{
			//BtGetAddrByDevRole(&BtAddr, PHONE_ADDR);
			//BtGetProfileState(&BtAddr, A2DP_SINK, &state);
			BtCheckAllDeviceProfileState(AVDTP,&state);

			if (state == CONNECT_OK)
			{
				UserAppDisplayOnce(me, vfd_str[STR_NEXT], 1);
				BtNext();
			}

			status = Q_HANDLED();
			break;
		}
		case BT_AUTO_PLAY_SIG:
		case SELECT_RELEASE_SIG:
		case PLAY_RELEASE_SIG:
		{
			//BtGetAddrByDevRole(&BtAddr, PHONE_ADDR);
			//BtGetProfileState(&BtAddr, A2DP_SINK, &state);
			BtCheckAllDeviceProfileState(AVDTP,&state);

			if (state == CONNECT_OK)
			{
				if (!BtIsPlay() || e->sig == BT_AUTO_PLAY_SIG)
				{
					UserAppDisplayOnce(me, vfd_str[STR_PLAY], 1);
					BtPlay();
				}
				else
				{
					UserAppDisplayOnce(me, vfd_str[STR_PAUSE], 1);
					BtPause();
				}
			}

			status = Q_HANDLED();
			break;
		}
		case PROGRAM_RELEASE_SIG:
		case BT_SOURCE_DISCONNECT_SIG:
		{
			//BtGetAddrByDevRole(&BtAddr, PHONE_ADDR);
			//BtGetProfileState(&BtAddr, A2DP_SINK, &state);
			BtCheckAllDeviceProfileState(AVDTP,&state);
			ap_BurnTest_log("\n state: %d\n", state);
			if (state == CONNECT_OK)
			{
				BtA2dpDisconnect(&BtAddr);
			}

			status = Q_HANDLED();
			break;
		}
		/*case CUSTOM_HINT_START_SIG:
		{
			HintSrcSwitch = TRUE;
			status = Q_SUPER(&UserApp_active);
			break;
		}*/
		case BT_READY_SOUNDHINT_SIG:
		{
			CFG_BT_T *pBtConf = GET_SDK_CFG(CFG_BT_T);
			CFG_BTMAIN_T *pBtSdkCfg = &(pBtConf->BtSdkCfg.BtMainCfg);

			if (pBtSdkCfg->HintWorkState == ON)
			{
				UserAppCallHint(me, eHint_BtReady);
			}

			UserAppDisplay_Blink(me, STR_BT, 10, 10);

			status = Q_HANDLED();
			break;
		}
		case BT_CONNECT_SOUNDHINT_SIG:
		{
			CFG_BT_T *pBtConf = GET_SDK_CFG(CFG_BT_T);
			CFG_BTMAIN_T *pBtSdkCfg = &(pBtConf->BtSdkCfg.BtMainCfg);

			if (pBtSdkCfg->HintWorkState == ON)
			{
				UserAppCallHint(me, eHint_BtConnect);
			}

			UserAppDisplay(me, STR_BT);

			if (me->stAutoSrcTest->burn_log_enable == 1)
			{

#ifdef USERAPP_SUPPORT_BT
				if (me->pBtBakcGroundApp != NULL)
				{
					QTimeEvt_rearm(&me->pBtBakcGroundApp->timeAutoPlayEvt, (TICKS_PER_500MS * 7));
				}
#endif
			}

			status = Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&UserApp_active);
			break;
		}
	}
	return status;
}
#endif

#if (!SUPPORT_VA)
/******************************************************************************************/
/**
 * \fn			QState UserApp_active_AUX_in(UserApp * const me, QEvt const * const e)
 *
 * \brief		the state that corresponds to Aux_in source
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_active_AUX_in state \n
 *			    \e status : Q_TRAN() trans to other state \n
 *              \e status : Q_SUPER() trans to UserApp_active state \n
 ******************************************************************************************/
QState UserApp_active_AUX_in(UserApp *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			UserAppDisplay(me, STR_AUXIN);
#ifndef BUILT4ISP2

#ifdef USERAPP_SUPPORT_KARAOKE
			QACTIVE_POST(&me->super, Q_NEW(QEvt, KOK_TURN_OFF_SIG), (void *)0);
#endif
			ap_BurnTest_log("ENTRY\n");
#ifndef SDK_RELEASE
			ap_Memory_check();
#endif
#endif

			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			me->codecType = 0;

			UserAppMute(me, 1);
			
			UserAppHintStop(me);
#ifndef BUILT4ISP2
			SpHdsp_Stop(NULL,0);
			ap_BurnTest_log("EXIT\n");
			AudDspService_Flow_Stop(&me->super);
		

#ifdef USERAPP_SUPPORT_KARAOKE
			QACTIVE_POST(&me->super, Q_NEW(QEvt, KOK_TURN_ON_SIG), (void *)0);
#endif
#endif
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			ap_BurnTest_log("INIT\n");
			ap_printf("\n======== main : ADC(AUX) initialize========\n");
			/*CFG_AUD_T *pstAUD_Cfg = GET_SDK_CFG(CFG_AUD_T);*/
			UserAppMute(me, 1);
			UartApp_SetKey_Sig(&me->super, SOURCE_SW_RELEASE_SIG, me->iSrc);
			
#if (SUPPORT_VA)
			VoiceAssistService_VAStatusReset(NULL, me->AuxVA_Status, me->BtVA_Status);
			if(!me->AuxVA_Status)
			{
				AudDspService_Prepare_ADC(&me->super, AIN1);
			}
			else
			{
				AudDspService_Prepare_ADC(&me->super, AIN0);
			}
#else
			AudDspService_Prepare_ADC(&me->super, AIN0);
#endif
			AudDspService_Flow_Play(&me->super);

			setHdmiMode(0);
			
			AmpSet_SetSrcGain(me->hAmpList, me->iSrc);
			AudDspService_Set_SourceGain(&me->super, SrcGain_Tab[me->iSrc]);
			
			me->mute = eMute_off;
			UserAppMute(me, me->mute);
			status =  Q_HANDLED();
			break;
		}

		default:
		{
			status = Q_SUPER(&UserApp_active);
			break;
		}
	}
	return status;
}
#endif

#if (SUPPORT_DEMOMODE)
/******************************************************************************************/
/**
 * \fn			QState UserApp_active_Raw_Data(UserApp * const me, QEvt const * const e)
 *
 * \brief		the state that corresponds to Raw_data source
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_active_AUX_in state \n
 *			    \e status : Q_TRAN() trans to other state \n
 *              \e status : Q_SUPER() trans to UserApp_active state \n
 ******************************************************************************************/
QState UserApp_active_Raw_Data(UserApp *const me, QEvt const *const e)
{
	QState status = 0;
	switch(e->sig)
		{
			case Q_ENTRY_SIG:
			{
				UserAppDisplay(me, STR_RAWDATA);
				ap_BurnTest_log("ENTRY\n");
				status = Q_HANDLED();
				break;
			}
			case Q_EXIT_SIG:
			{
				ap_BurnTest_log("EXIT\n");
				SpHdsp_Stop(NULL,0);
#ifdef SUPPORT_AP
				if (me->playercreat == 1)
				{
					MFP_Destroy(me->pAPInstance);
					me->playercreat = 0;
					me->pAPInstance = NULL;
				}
#endif
				UserAppMute(me, 1);
				status = Q_HANDLED();
				break;
			}
			case Q_INIT_SIG:
			{
				ap_BurnTest_log("INIT\n");

				if(MFP_Create(&me->pAPInstance, "playerRD", PLAY_SRC_RAWDATA, (void *)&me->super) == MFP_ERR_NO)
				{
					me->playercreat = 1;
				}

				mfpOpaque_t sparam;
				mfpAudInfo_t rdinfo;

				rdinfo.fmt = CDC_AUD_EAC3;
				rdinfo.channels = 2;
				rdinfo.sample_rate = 96000;
				rdinfo.block_align = 6;
				rdinfo.sample_size = 16;
				sparam.size = sizeof(mfpAudInfo_t);
				sparam.opaque = &rdinfo;
				MFP_Ctrl(me->pAPInstance, MFP_SET_CODEC_INFO, &sparam);

				MFP_PlayNext(me->pAPInstance, PLAY_CUR_FILE);

				UserAppMute(me, me->mute);
				status = Q_HANDLED();
				break;
			}
			case SOURCE_SW_RELEASE_SIG:
			{
				status = Q_HANDLED();
				break;
			}
			default:
			{
				status = Q_SUPER(&UserApp_active);
				break;
			}
		}
	return status;
}
#endif

/******************************************************************************************/
/**
 * \fn			QState UserApp_active_Line_in(UserApp * const me, QEvt const * const e)
 *
 * \brief		the state that corresponds to Line_in source
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_active_Line_in state \n
 *			    \e status : Q_TRAN() trans to other state \n
 *              \e status : Q_SUPER() trans to UserApp_active state \n
 ******************************************************************************************/
QState UserApp_active_Line_in(UserApp *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			UserAppDisplay(me, STR_LINEIN);
#ifndef BUILT4ISP2
#ifdef USERAPP_SUPPORT_KARAOKE
			QACTIVE_POST(&me->super, Q_NEW(QEvt, KOK_TURN_OFF_SIG), (void *)0);
#endif
			ap_BurnTest_log("ENTRY\n");
#ifndef SDK_RELEASE
			ap_Memory_check();
#endif
#endif
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			ap_BurnTest_log("EXIT\n");
#ifndef BUILT4ISP2
			me->codecType = 0;
			UserAppHintStop(me);
			SpHdsp_Stop(NULL,0);
			AudDspService_Flow_Stop(&me->super);
			UserAppMute(me, 1);

#ifdef USERAPP_SUPPORT_KARAOKE
			QACTIVE_POST(&me->super, Q_NEW(QEvt, KOK_TURN_ON_SIG), (void *)0);
#endif
#endif

#if (SUPPORT_VA)
			VoiceAssistService_APSource(NULL, SRC_OTHER);
			if(!me->AuxVA_Status)
			{
				UserApp_SetAuxMute();
			}
#endif
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			ap_BurnTest_log("INIT\n");
			ap_printf("\n======== main : ADC initialize========\n");
			/*CFG_AUD_T *pstAUD_Cfg = GET_SDK_CFG(CFG_AUD_T);*/
#if (SUPPORT_VA)
			if(me->AuxVA_Status)
			{
				UserApp_SetAuxMute();
			}
			VoiceAssistService_APSource(NULL, SRC_AUX);
			VoiceAssistService_VAStatusReset(NULL, me->AuxVA_Status, me->BtVA_Status);
#endif
#ifndef BUILT4ISP2
			AudDspService_Prepare_ADC(&me->super, AIN0);
			AudDspService_Flow_Play(&me->super);
			AudDspService_Set_SourceGain(&me->super, SrcGain_Tab[me->iSrc]);
			AudDspService_Set_OutSampleRate(&me->super, 48000);
			setHdmiMode(0);
#endif
			UserAppMute(me, me->mute);

			status =  Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&UserApp_active);
			break;
		}
	}
	return status;
}

/******************************************************************************************/
/**
 * \fn			QState UserApp_active_I2S_in(UserApp * const me, QEvt const * const e)
 *
 * \brief		the state that corresponds to I2S_in source
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_active_Line_in state \n
 *			    \e status : Q_TRAN() trans to other state \n
 *              \e status : Q_SUPER() trans to UserApp_active state \n
 ******************************************************************************************/
QState UserApp_active_I2S_in(UserApp *const me, QEvt const *const e)
{
	QState status = 0;
	static BYTE cnt=0;
	static BYTE src_rdy = 0;
	static BYTE strBuf[10];
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
				UserAppDisplay(me, STR_BT);

#ifndef BUILT4ISP2
#ifdef USERAPP_SUPPORT_KARAOKE
			QACTIVE_POST(&me->super, Q_NEW(QEvt, KOK_TURN_OFF_SIG), (void *)0);
#endif
			ap_BurnTest_log("ENTRY\n");
#ifndef SDK_RELEASE
			ap_Memory_check();
#endif
#endif
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			ap_BurnTest_log("EXIT\n");

			UserAppMute(me, 1);
#ifndef BUILT4ISP2
			me->codecType = 0;
			me->bt_connect = 0;
			me->bt_play = 0;
			UserAppHintStop(me);
			SpHdsp_Stop(NULL,0);
			AudDspService_Flow_Stop(&me->super);
			

#ifdef USERAPP_SUPPORT_KARAOKE
			QACTIVE_POST(&me->super, Q_NEW(QEvt, KOK_TURN_ON_SIG), (void *)0);
#endif
#endif

			
#if (SUPPORT_VA)
			VoiceAssistService_APSource(NULL, SRC_OTHER);
			if(!me->BtVA_Status)
			{
				UserApp_SetBTMute();
			}
#endif
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			ap_BurnTest_log("INIT\n");
			ap_printf("\n======== main : ADC initialize========\n");

			UserAppMute(me, 1);
			UartApp_SetKey_Sig(&me->super, SOURCE_SW_RELEASE_SIG, me->iSrc);

			/*CFG_AUD_T *pstAUD_Cfg = GET_SDK_CFG(CFG_AUD_T);*/
#ifndef BUILT4ISP2
#if (SUPPORT_VA)
			if(me->BtVA_Status)
			{
				UserApp_SetBTMute();
			}
			VoiceAssistService_APSource(NULL, SRC_BT);
			VoiceAssistService_VAStatusReset(NULL, me->AuxVA_Status, me->BtVA_Status);
#endif
			AudDspService_Prepare_2CH_IISIN(&me->super);
			AudDspService_Flow_Play(&me->super);
			AmpSet_SetSrcGain(me->hAmpList, me->iSrc);
			AudDspService_Set_SourceGain(&me->super, SrcGain_Tab[me->iSrc]);
			setHdmiMode(0);
#endif
			me->mute = eMute_off;
			UserAppMute(me, me->mute);

			//AudDspService_Set_AppMode(&me->super, SET_MMIX, GRP_MMIX0, MODE1);
			
			QTimeEvt_rearm(&me->timeSrcDelay, TICKS_PER_SEC*3);

			status =  Q_HANDLED();
			break;
		}

		case SRC_DELAY_SIG:
			src_rdy = 1;	
			if(me->aUserSrc_tbl[me->iSrc] == eAp_I2S_Src)
			{	
				if(me->bt_connect == 0)
					UserAppDisplay(me, STR_BT_NOBT);
			}
			status =  Q_HANDLED();
			break;
		
		
		case BT_READY_SOUNDHINT_SIG:
		{
			if(me->aUserSrc_tbl[me->iSrc] == eAp_I2S_Src)
			{
				ap_BurnTest_log("BT_READY_SOUNDHINT_SIG\n");
				me->bt_connect = 0;
				me->bt_play = 0;
				UserAppDisplay(me, STR_BT_NOBT);
				//UserAppCallHint(me, eHint_BtReady);
				UserApp_DimmerSet(me);
				status = Q_HANDLED();
			}
			break;
		}
		case BT_CONNECT_SOUNDHINT_SIG:
		{
			if(me->aUserSrc_tbl[me->iSrc] == eAp_I2S_Src)
			{
				ap_BurnTest_log("BT_CONNECT_SOUNDHINT_SIG\n");
				me->bt_connect = 1;
				UserAppCallHint(me, eHint_BtConnect);
				UserAppDisplay(me, STR_BT_CONNECTED);
				UserApp_DimmerSet(me);
			}
			status = Q_HANDLED();
			break;
		}

		case BT_PLAY_STATUS_SIG:
			me->bt_play = 1;
			status = Q_HANDLED();
			break;
			
		case BT_PAUSE_STATUS_SIG:
			me->bt_play = 0;
			status = Q_HANDLED();
			break;	
		
		case PRE_RELEASE_SIG:
			if(me->aUserSrc_tbl[me->iSrc] == eAp_I2S_Src)
			{
				UartApp_SetKey_Sig(&me->super, PRE_RELEASE_SIG, me->iSrc);
			}
			status = Q_HANDLED();
			break;
		case NEXT_RELEASE_SIG:
			if(me->aUserSrc_tbl[me->iSrc] == eAp_I2S_Src)
			{
				UartApp_SetKey_Sig(&me->super, NEXT_RELEASE_SIG, me->iSrc);
			}
				status = Q_HANDLED();
			break;
		case PLAY_RELEASE_SIG:
				
			if(me->aUserSrc_tbl[me->iSrc] == eAp_I2S_Src)
			{
				UartApp_SetKey_Sig(&me->super, PLAY_RELEASE_SIG, me->iSrc);
			}
				status = Q_HANDLED();
			break;
		case BT_VOL_SIG:
		{
			if(me->aUserSrc_tbl[me->iSrc] == eAp_I2S_Src)
			{
				BTEvt *evt = (BTEvt *)e;
				ap_printf("BT vol set %d\n", evt->vol);

				me->vol = evt->vol;
				UserAppVolumeSet(me->vol,me->hAmpList);
				      
				UserAppStrToVolDisplay(strBuf, me->vol);
				UserAppDisplayOnce(me, strBuf, 3);

				 if (me->mute == eMute_on) 
				 {
				 	me->mute = eMute_off; // move here
		            UserAppMute(me,0);
		         } 
			}
			status = Q_HANDLED();
			break;
		}
		case PLAY_TICK_SIG:
			if(me->aUserSrc_tbl[me->iSrc] == eAp_I2S_Src)
			{
				if(cnt++ == 10)
				{
					UartApp_SetKey_Sig(&me->super, PAIR_RELEASE_SIG, me->iSrc);
					UserAppDisplayOnce(me, "PAIR", 3);
					me->bt_play = 0;
					me->bt_connect = 0;			
				
				}
			}
			status = Q_HANDLED();
			break;
			
		case PLAY_SIG:
			cnt = 0;
			status = Q_HANDLED();
			break;

		default:
		{
			status = Q_SUPER(&UserApp_active);
			break;
		}
	}
	return status;
}

/******************************************************************************************/
/**
 * \fn			QState UserApp_active_SPDIF_Coaxial(UserApp * const me, QEvt const * const e)
 *
 * \brief		the state that corresponds to SPDIF_Coaxial source
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_active_SPDIF_Coaxial state \n
 *			    \e status : Q_TRAN() trans to other state \n
 *              \e status : Q_SUPER() trans to UserApp_active state \n
 ******************************************************************************************/
QState UserApp_active_SPDIF_Coaxial(UserApp *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			UserAppDisplay(me, STR_COAXIAL);
			ap_BurnTest_log("ENTRY\n");
#ifndef SDK_RELEASE
			ap_Memory_check();
#endif
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			ap_BurnTest_log("EXIT\n");
			me->codecType = 0;
			UserAppHintStop(me);
			UserAppMute(me, 1);
#ifndef BUILT4ISP2
			AudDspService_SPDIF_STOP(&me->super);
			AudDspService_Flow_Stop(NULL);
#endif
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			ap_BurnTest_log("INIT\n");
#ifndef BUILT4ISP2
			AudDspService_SPDIF_STOP(&me->super);
			AudDspService_Flow_Stop(&me->super);
			AudDspService_Prepare_SPDIF(&me->super, SPDIF_IEC2);
			AudDspService_Set_SourceGain(&me->super, SrcGain_Tab[me->iSrc]);
			AudDspService_Flow_Play(&me->super);
			AudDspService_SPDIF_PLAY(&me->super);
			//AudDspService_Set_SourceGain(&me->super, SrcGain_Tab[me->iSrc]);
#endif

#if (SUPPORT_VA)
			VoiceAssistService_VAStatusReset(NULL, me->AuxVA_Status, me->BtVA_Status);
			if(!me->BtVA_Status)
			{
				UserApp_SetBTMute();
			}
			if(!me->AuxVA_Status)
			{
				UserApp_SetAuxMute();
			}
#endif
			me->mute = eMute_off;
			UserAppMute(me, me->mute);

			status =  Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&UserApp_active);
			break;
		}
	}
	return status;
}

/******************************************************************************************/
/**
 * \fn			QState UserApp_active_SPDIF_Optical(UserApp * const me, QEvt const * const e)
 *
 * \brief		the state that corresponds to SPDIF_Optical source
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_active_SPDIF_Optical state \n
 *			    \e status : Q_TRAN() trans to other state \n
 *              \e status : Q_SUPER() trans to UserApp_active state \n
 ******************************************************************************************/
QState UserApp_active_SPDIF_Optical(UserApp *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			UserAppDisplay(me, STR_OPTICAL);
			ap_BurnTest_log("ENTRY\n");
#ifndef SDK_RELEASE
			ap_Memory_check();
#endif
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			ap_BurnTest_log("EXIT\n");
			
				UserAppMute(me, 1);
			me->codecType = 0;
			UserAppHintStop(me);
		

#ifndef BUILT4ISP2
			AudDspService_SPDIF_STOP(NULL);
			AudDspService_Flow_Stop(NULL);
#endif

			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			ap_BurnTest_log("INIT\n");	
			
#ifndef BUILT4ISP2
			UartApp_SetKey_Sig(&me->super, SOURCE_SW_RELEASE_SIG, me->iSrc);
			AudDspService_SPDIF_STOP(&me->super);
			AudDspService_Flow_Stop(&me->super);
			#if defined(CFG_EMU_V3_SPDIF) && (CFG_EMU_V3_SPDIF == 1)
				AudDspService_Prepare_SPDIF(&me->super, SPDIF_IEC1);
			#else
				AudDspService_Prepare_SPDIF(&me->super, SPDIF_IEC0);
			#endif
			AudDspService_Set_SourceGain(&me->super, SrcGain_Tab[me->iSrc]);
			//AudDspService_Set_OutSampleRate(&me->super, 48000);
			AudDspService_Flow_Play(&me->super);
			AudDspService_SPDIF_PLAY(&me->super);
			//AudDspService_Set_SourceGain(&me->super, SrcGain_Tab[me->iSrc]);
#endif
#if (SUPPORT_VA)
			VoiceAssistService_VAStatusReset(NULL, me->AuxVA_Status, me->BtVA_Status);
			if(!me->BtVA_Status)
			{
				UserApp_SetBTMute();
			}
			if(!me->AuxVA_Status)
			{
				UserApp_SetAuxMute();
			}
#endif
			me->mute = eMute_off;
			UserAppMute(me, me->mute);

			status =  Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&UserApp_active);
			break;
		}
	}
	return status;
}
#ifdef USERAPP_SUPPORT_HDMI_CEC
/******************************************************************************************/
/**
 * \fn			QState UserApp_active_ARC(UserApp * const me, QEvt const * const e)
 *
 * \brief		the state that corresponds to ARC source
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_active_ARC state \n
 *			    \e status : Q_TRAN() trans to other state \n
 *              \e status : Q_SUPER() trans to UserApp_active state \n
 ******************************************************************************************/
QState UserApp_active_ARC(UserApp *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			UserAppDisplay(me, STR_ARC);
			ap_BurnTest_log("ENTRY\n");
#ifndef SDK_RELEASE
			ap_Memory_check();
#endif
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			ap_BurnTest_log("EXIT\n");
			me->codecType = 0;
			UserAppHintStop(me);
			me->ARC_DSP_State = 0;

			UserAppMute(me, 1);
			//IOSrv_AudInfo_report(me, me->mute, me->vol, USERAPP_VOL_MAX);

			HDMISrv_ARC_Terminate(&me->super);

			if (me->aUserSrc_tbl[me->iSrc] != eAp_ARC_Src) //after IOSrv_ARC_Terminate()
			{
				#ifdef USERAPP_SUPPORT_HDMI_TX
				if (me->aUserSrc_tbl[me->iSrc] == eAp_HDMI_TX_Src)
					HDMISrv_CEC_SetSrc(&me->super, eIOP_CecSrc_HDMI_TX);
				else

				#endif

					HDMISrv_CEC_SetSrc(&me->super, eIOP_CecSrc_Others);
			}

#ifndef BUILT4ISP2
			AudDspService_SPDIF_STOP(&me->super);
			AudDspService_Flow_Stop(&me->super);
#endif

			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			ap_BurnTest_log("INIT\n");
			me->ARC_DSP_State = 0;
			HDMISrv_CEC_SetSrc(&me->super, eIOP_CecSrc_ARC);//before IOSrv_ARC_Initiate()
			HDMISrv_ARC_Initiate(&me->super);
			SystemService_AudioInfoBroadcast(&me->super, SYS_MUTE_CHG, me->mute, me->vol, USERAPP_VOL_MAX, USERAPP_VOL_MIN);

#if (SUPPORT_VA)
			VoiceAssistService_VAStatusReset(NULL, me->AuxVA_Status, me->BtVA_Status);
			if(!me->BtVA_Status)
			{
				UserApp_SetBTMute();
			}
			if(!me->AuxVA_Status)
			{
				UserApp_SetAuxMute();
			}
#endif
			UserAppMute(me, me->mute);

			status =  Q_HANDLED();
			break;
		}
		case HDMI_CEC_SETDSP_SIG:
		{
			ap_printf("HDMI_CEC_SETDSP_SIG: \n");
			stCecSetDSP_t *pstCEC_DSP = ((stCecSetDSP_t *)e) ;
			if(me->ARC_DSP_State != pstCEC_DSP->DSP_state)
			{
			#ifndef BUILT4ISP2
				AudDspService_Prepare_SPDIF(&me->super, SPDIF_HDMI_ARC);
				me->ARC_DSP_State = pstCEC_DSP->DSP_state;
				AudDspService_Flow_Play(&me->super);
				AudDspService_Set_SourceGain(&me->super, SrcGain_Tab[me->iSrc]);
				AudDspService_SPDIF_PLAY(&me->super);
			#endif
			}

			status =  Q_HANDLED();
			break;

		}
		case IOP_MUTE_REQ_SIG:
		{
			ap_printf("mute request; \n");
			char strBuf[14];
			int MuteState = 0;

			MuteState = Q_EVT_CAST(stSYS_MuteReq_t)->MuteState;

			ap_printf("mute request; %d \n", MuteState);

			UserAppMute(me, MuteState);

			me->mute = MuteState;
			//IOSrv_AudInfo_report(me, me->mute, me->vol, USERAPP_VOL_MAX);
			SystemService_AudioInfoBroadcast(&me->super, SYS_MUTE_CHG, me->mute, me->vol, USERAPP_VOL_MAX, USERAPP_VOL_MIN);

			if (MuteState == eMute_on)
			{
				UserAppNum2Str(strBuf, 0);
			}
			else
			{
				UserAppNum2Str(strBuf, me->vol);
			}

			VFD_Service_POP(me, strBuf, 3, RIGHT);

			status = Q_HANDLED();
			break;
		}

#ifdef CEC_CTS_TEST
		case PLAY_RELEASE_SIG:
		{
			ap_printf("ARC:play key released,set SAC off\n");
			HDMISrv_CEC_Set_SAC(&me->super, 0);
			status = Q_HANDLED();
			break;
		}

		case PROGRAM_RELEASE_SIG:
		{
			ap_printf("ARC:bt key released,set SAC on\n");
			HDMISrv_CEC_Set_SAC(&me->super, 1);
			status = Q_HANDLED();
			break;
		}
#endif

		case CEC_EVT_SIG:
		{
			ap_printf("get CEC_EVT_SIG\n");
			stCECEvt_t *pstCEC_Evt = ((stCECEvt_t *)e) ;
			ap_printf("opcode: 0x%x, size:%d\n", pstCEC_Evt->stCecInfo.opcode, pstCEC_Evt->stCecInfo.size);
			status = Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&UserApp_active);
			break;
		}
	}
	return status;
}

	#ifdef USERAPP_SUPPORT_HDMI_TX

/******************************************************************************************/
/**
 * \fn			QState UserApp_active_HDMI_TX(UserApp * const me, QEvt const * const e)
 *
 * \brief		the state that corresponds to hdmi tx source
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_active_Line_in state \n
 *			    \e status : Q_TRAN() trans to other state \n
 *              \e status : Q_SUPER() trans to UserApp_active state \n
 ******************************************************************************************/
QState UserApp_active_HDMI_TX(UserApp *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			UserAppDisplay(me, STR_HDMI);
			ap_BurnTest_log("ENTRY\n");

			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			ap_BurnTest_log("EXIT\n");
			me->codecType = 0;
			UserAppHintStop(me);
			UserAppMute(me, 1);

			HDMISrv_HDMI_TX_Terminate(&me->super);

			if (me->aUserSrc_tbl[me->iSrc] != eAp_HDMI_TX_Src)//after IOSrv_HDMI_TX_Terminate()
			{
				if (me->aUserSrc_tbl[me->iSrc] == eAp_ARC_Src)
					HDMISrv_CEC_SetSrc(&me->super, eIOP_CecSrc_ARC);
				else
					HDMISrv_CEC_SetSrc(&me->super, eIOP_CecSrc_Others);
			}

#ifdef USERAPP_SUPPORT_BT
			UserAppVolumeNotifyToBt(me->vol, me->mute); // restored volume value
#endif
#ifndef BUILT4ISP2
			AudDspService_Flow_Stop(&me->super);
			AudDspService_Sine_Set(&me->super, 0, 0);
#endif
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			ap_BurnTest_log("INIT\n");
			HDMISrv_CEC_SetSrc(&me->super, eIOP_CecSrc_HDMI_TX);//before IOSrv_HDMI_TX_Initiate()
			HDMISrv_HDMI_TX_Initiate(&me->super);

#ifdef USERAPP_SUPPORT_BT
#ifndef BUILT4ISP2
			//AudDspService_Set_VolumeChannel(&me->super, CH_SUB, 0); //mute sub speaker, and it will be restored at EXIT by UserAppVolumeNotifyToBt()
#endif
#endif
#ifndef BUILT4ISP2
			AudDspService_Prepare_SINE(&me->super);
			AudDspService_Flow_Play(&me->super);

			AudDspService_Set_SourceGain(&me->super, 0); //set DSP output fixed 0dB in HDMI_TX
#endif
#if (SUPPORT_VA)
			VoiceAssistService_VAStatusReset(NULL, me->AuxVA_Status, me->BtVA_Status);
			if(!me->BtVA_Status)
			{
				UserApp_SetBTMute();
			}
			if(!me->AuxVA_Status)
			{
				UserApp_SetAuxMute();
			}
#endif
			UserAppMute(me, me->mute);

			status =  Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&UserApp_active);
			break;
		}
		case CEC_EVT_SIG:
		{
			ap_printf("get CEC_EVT_SIG\n");
			stCECEvt_t *pstCEC_Evt = ((stCECEvt_t *)e) ;

			ap_printf("opcode: 0x%x, size:%d\n", pstCEC_Evt->stCecInfo.opcode, pstCEC_Evt->stCecInfo.size);

			status = Q_HANDLED();
			break;
		}
	}
	return status;
}
#endif

#endif

#if 1//def USERAPP_SUPPORT_REPEATER

/******************************************************************************************/
/**
 * \fn			QState UserApp_active_ARC(UserApp * const me, QEvt const * const e)
 *
 * \brief		the state that corresponds to ARC source
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_active_ARC state \n
 *			    \e status : Q_TRAN() trans to other state \n
 *              \e status : Q_SUPER() trans to UserApp_active state \n
 ******************************************************************************************/
QState UserApp_active_ARC(UserApp * const me, QEvt const * const e) {
    QState status = 0;
#ifdef ATMOS_CERT
	extern void vTaskDelay();
#endif

	static BYTE last_SrcData;

    switch (e->sig) {
        case Q_ENTRY_SIG: {
            ap_BurnTest_log("ENTRY\n");

			#ifndef SDK_RELEASE
            ap_Memory_check();
            #endif


            status = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG: {
            ap_BurnTest_log("EXIT\n");
		UserAppMute(me,1);
			me->codecType = 0;
			UserAppHintStop(me);
            
            //IOSrv_AudInfo_report(me, me->mute, me->vol, USERAPP_VOL_MAX);//mingjin close

            		if((me->aUserSrc_tbl[me->iSrc] > eAp_HDMI2_Src)||(me->aUserSrc_tbl[me->iSrc] < eAp_ARC_Src))
			{
		
				Hdmi_SetHandleSig(&me->super, CMD_MODE_BT, me->mute, 0);
			}


#ifndef BUILT4ISP2
		    AudDspService_SPDIF_STOP(&me->super);
			AudDspService_Flow_Stop(&me->super);
#endif
            status = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            ap_BurnTest_log("INIT\n");

			UartApp_SetKey_Sig(&me->super, SOURCE_SW_RELEASE_SIG, me->iSrc);

			AmpSet_SetSrcGain(me->hAmpList, me->iSrc);

			me->eARC_SRC = hdmi_CheckEARC();
			if(me->eARC_SRC == eUser_eARCSRC)
			{
				ap_BurnTest_log("==== *** STR_EARC *** ====\n");
				UserAppDisplay(me, STR_EARC);
				
				#ifndef BUILT4ISP2
				last_SrcData = Source_HDMIHBR;
				AudDspService_SPDIF_STOP(&me->super);
				AudDspService_Flow_Stop(&me->super);
				AudDspService_Prepare_8CH_IISIN(&me->super, IIS8CH_HDMIHBR);
		       	AudDspService_Set_SourceGain(&me->super, SrcGain_Tab[eAp_ARC_Src]);
		        	AudDspService_Flow_Play(&me->super);
		        	AudDspService_SPDIF_PLAY(&me->super);
				#endif
			}
			else
			{
				ap_BurnTest_log("==== *** STR_ARC *** ====\n");
				UserAppDisplay(me, STR_ARC);
				
				#ifndef BUILT4ISP2
				last_SrcData = Source_ARC;
		        	AudDspService_SPDIF_STOP(&me->super);
		        	AudDspService_Flow_Stop(&me->super);
				AudDspService_Prepare_SPDIF(&me->super, SPDIF_IEC1);
		       	AudDspService_Set_SourceGain(&me->super, SrcGain_Tab[eAp_ARC_Src]);
		        	AudDspService_Flow_Play(&me->super);
		        	AudDspService_SPDIF_PLAY(&me->super);
				#endif
			}

			AudDspService_Set_SourceGain(&me->super, SrcGain_Tab[me->iSrc]);

			ap_BurnTest_log("me->mute: %d\n",me->mute);

		#if (SUPPORT_VA)
			VoiceAssistService_VAStatusReset(NULL, me->AuxVA_Status, me->BtVA_Status);
			if(!me->BtVA_Status)
			{
				UserApp_SetBTMute();
			}
			if(!me->AuxVA_Status)
			{
				UserApp_SetAuxMute();
			}
		#endif
		
			me->mute = eMute_off;
			UserAppMute(me,me->mute);

			Hdmi_SetHandleSig(&me->super, CMD_MODE_ARC, me->mute, 0);


			status = Q_HANDLED();
			break;
		}

        case IOP_MUTE_REQ_SIG: {
            ap_printf("mute request; \n");
            char strBuf[14];
            int MuteState = 0;

            MuteState = Q_EVT_CAST(stSYS_MuteReq_t)->MuteState;

            ap_printf("mute request; %d \n", MuteState);

            UserAppMute(me,MuteState);

			me->mute = MuteState;
            //IOSrv_AudInfo_report(me, me->mute, me->vol, USERAPP_VOL_MAX);
           // SystemService_AudioInfoBroadcast(&me->super, SYS_MUTE_CHG, me->mute, me->vol, USERAPP_VOL_MAX, USERAPP_VOL_MIN);//mingjin close

            if (MuteState == eMute_on)
            {
                UserAppNum2Str(strBuf , 0);
            }
            else
            {
                UserAppNum2Str(strBuf , me->vol);
            }

            VFD_Service_POP(me , strBuf, 3, RIGHT);

            status = Q_HANDLED();
            break;
        }
		case REPEATER_AUD_SRC_SIG:{

			RepeaterAudSrcEvt_t *pstCEC_Evt = ((RepeaterAudSrcEvt_t *)e);
		ap_printf("REPEATER_AUD_SRC_SIG ==> Aud_I2S_Lane:%d\n",pstCEC_Evt->Aud_I2S_Lane);

			status = Q_HANDLED();
			break;
		}
	case HDMI_D4_MODE_SIG:
		if(last_SrcData != Source_HDMIHBR)
		{	
			ap_BurnTest_log("==== *** STR_eARC *** ==== HDMI  HBR \n");
		
			last_SrcData = Source_HDMIHBR;
			me->eARC_SRC = eUser_eARCSRC;
			UserAppDisplay(me, STR_EARC);
			AudDspService_SPDIF_STOP(&me->super);
			AudDspService_Flow_Stop(&me->super);
			AudDspService_Prepare_8CH_IISIN(&me->super, IIS8CH_HDMIHBR);
			//AudDspService_Set_OutSampleRate(&me->super, 48000);
			AudDspService_Flow_Play(&me->super);
			AudDspService_SPDIF_PLAY(&me->super);
		}
		status = Q_HANDLED();
		break;

	case HDMI_SPDIF_MODE_SIG:
		if(last_SrcData != Source_ARC)
		{	
			ap_BurnTest_log("==== *** STR_ARC *** ==== HDMI  spdif \n");
			me->eARC_SRC = eUser_ARCSRC;
			last_SrcData = Source_ARC;
			UserAppDisplay(me, STR_ARC);
			AudDspService_SPDIF_STOP(&me->super);
			AudDspService_Flow_Stop(&me->super);
			AudDspService_Prepare_SPDIF(&me->super, SPDIF_IEC1);
			AudDspService_Flow_Play(&me->super);
			AudDspService_SPDIF_PLAY(&me->super);
		}
		status = Q_HANDLED();
		break;

	case HDMI_D0_MODE_SIG:
		if(last_SrcData != Source_HDMISTD)
		{	
			ap_BurnTest_log("==== *** STR_eARC *** ==== HDMI STD \n");
			me->eARC_SRC = eUser_eARCSRC;
			last_SrcData = Source_HDMISTD;
			UserAppDisplay(me, STR_EARC);
			 AudDspService_SPDIF_STOP(&me->super);
			AudDspService_Flow_Stop(&me->super);
			AudDspService_Prepare_8CH_IISIN(&me->super, IIS8CH_HDMISTD);
			AudDspService_Flow_Play(&me->super);
			AudDspService_SPDIF_PLAY(&me->super);
		}
		status = Q_HANDLED();
		break;
		case REPEATER_LPCMCHANNEL_NUM_SIG:
		{
			ap_printf("REPEATER_LPCMCHANNEL_NUM_SIG \n");
			RepeatereLPCMNumEvt_t *pstLPCMNumEvt = ((RepeatereLPCMNumEvt_t *)e);

			UINT8 channel_num = pstLPCMNumEvt->LPCM_channelNum;
			UINT8 channel_mapping = pstLPCMNumEvt->LPCM_ChannelMapping;
			AudDspService_SPDIF_SetParam(NULL, ODSP_SPDIF_CHANNEL, channel_num, channel_mapping);
			status = Q_HANDLED();
			break;
		}
		case REPEATER_AUD_ARC_SRC_SIG:{

			RepeaterAudSrcEvt_t *pstCEC_Evt = ((RepeaterAudSrcEvt_t *)e);
		  ap_printf("REPEATER_AUD_ARC_SRC_SIG ==> Aud_I2S_Lane:%d\n",pstCEC_Evt->Aud_I2S_Lane);
			status = Q_HANDLED();
			break;
		}

		#ifdef CEC_CTS_TEST
		case PLAY_RELEASE_SIG: {
	        ap_printf("ARC:play key released,set SAC off\n");
            IOSrv_CEC_Set_SAC(&me->super,0);
	        status = Q_HANDLED();
	        break;
        }
		case PROGRAM_RELEASE_SIG: {
	        ap_printf("ARC:bt key released,set SAC on\n");
            IOSrv_CEC_Set_SAC(&me->super,1);
	        status = Q_HANDLED();
	        break;
        }
		#endif

        case CEC_EVT_SIG: {
            ap_printf("get CEC_EVT_SIG\n");
            stCECEvt_t *pstCEC_Evt = ((stCECEvt_t *)e) ;
            ap_printf("opcode: 0x%x, size:%d\n",pstCEC_Evt->stCecInfo.opcode, pstCEC_Evt->stCecInfo.size);
            status = Q_HANDLED();
            break;
        }
		case ONE_RELEASE_SIG:
		{
			ap_printf(" ===== ONE_RELEASE_SIG =====\n");

		#if (USERAPP_SUPPORT_REPEATER == 0)
		UserAppDisplay(me, STR_REPEATER_UPGRADE);

		RepeaterSrv_EPUpgrade(eRepeater_Upgrade_EN);

		#endif

			status = Q_HANDLED();
			break;
		}

#ifdef ATMOS_CERT
		case ONE_RELEASE_SIG:
		{

			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_DIRECT], 1, CENTER);
			me->atmos_opt.direct += 1;
			if(me->atmos_opt.direct > 1)
			{
				me->atmos_opt.direct = 0;
			}
			ap_printf("Direct %s\n",vfd_str[STR_OFF + me->atmos_opt.direct]);
			SpHdsp_setAtmos(NULL,SP_ATMOS_DIRECT,me->atmos_opt.direct,0);
			SpHdsp_Reset(NULL,0);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, vfd_str[STR_OFF + me->atmos_opt.direct], 3, RIGHT);


			status = Q_HANDLED();
			break;
		}
		case TWO_RELEASE_SIG:
		{
			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_HFT], 1, CENTER);
			me->atmos_opt.b_height_filter += 1;
			if(me->atmos_opt.b_height_filter > 1)
			{
				me->atmos_opt.b_height_filter = 0;
			}
			ap_printf("Height Cut Filter %s\n",vfd_str[STR_OFF + me->atmos_opt.b_height_filter]);
			SpHdsp_setAtmos(NULL,SP_ATMOS_HEIGHT_FILTER,me->atmos_opt.b_height_filter,0);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, vfd_str[STR_OFF + me->atmos_opt.b_height_filter], 3, RIGHT);
			status = Q_HANDLED();
			break;
		}
		case THREE_RELEASE_SIG:
		{
			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_LM], 1, CENTER);
			me->atmos_opt.b_enable_loudness_mgmt += 1;
			if(me->atmos_opt.b_enable_loudness_mgmt > 1)
			{
				me->atmos_opt.b_enable_loudness_mgmt = 0;
			}
			ap_printf("Loudness managemant %s\n",vfd_str[STR_OFF + me->atmos_opt.b_enable_loudness_mgmt]);
			SpHdsp_setAtmos(NULL,SP_ATMOS_LM,me->atmos_opt.b_enable_loudness_mgmt,0);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, vfd_str[STR_OFF + me->atmos_opt.b_enable_loudness_mgmt], 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case FOUR_RELEASE_SIG:
		{
			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_VLAMP], 1, CENTER);
			me->atmos_opt.b_vlamp += 1;
			if(me->atmos_opt.b_vlamp > 1)
			{
				me->atmos_opt.b_vlamp = 0;
			}
			ap_printf("Volume Leveler %s\n",vfd_str[STR_OFF + me->atmos_opt.b_vlamp]);
			SpHdsp_setAtmos(NULL,SP_ATMOS_VLAMP,me->atmos_opt.b_vlamp,0);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, vfd_str[STR_OFF + me->atmos_opt.b_vlamp], 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case FIVE_RELEASE_SIG:
		{
			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_UPMIX], 1, CENTER);
			me->atmos_opt.b_enable_upmixer += 1;
			if(me->atmos_opt.b_enable_upmixer > 1)
			{
				me->atmos_opt.b_enable_upmixer = 0;
			}
			ap_printf("Surround Decoder %s\n",vfd_str[STR_OFF + me->atmos_opt.b_enable_upmixer]);
			SpHdsp_setAtmos(NULL,SP_ATMOS_UPMIX,me->atmos_opt.b_enable_upmixer,0);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, vfd_str[STR_OFF + me->atmos_opt.b_enable_upmixer], 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case SIX_RELEASE_SIG:
		{
			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_CNTSPD], 1, CENTER);
			me->atmos_opt.b_center_spread += 1;
			if(me->atmos_opt.b_center_spread > 1)
			{
				me->atmos_opt.b_center_spread = 0;
			}
			ap_printf("Center Spread %s\n",vfd_str[STR_OFF + me->atmos_opt.b_center_spread]);
			SpHdsp_setAtmos(NULL,SP_ATMOS_CSPREAD,me->atmos_opt.b_center_spread,0);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, vfd_str[STR_OFF + me->atmos_opt.b_center_spread], 3, RIGHT);


			status = Q_HANDLED();
			break;
		}
		case SEVEN_RELEASE_SIG:
		{
			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_POST], 1, CENTER);
			me->atmos_opt.b_postproc += 1;
			if(me->atmos_opt.b_postproc > 1)
			{
				me->atmos_opt.b_postproc = 0;
			}
			ap_printf("Post Processing %s\n",vfd_str[STR_OFF + me->atmos_opt.b_postproc]);
			SpHdsp_setAtmos(NULL,SP_ATMOS_POST,me->atmos_opt.b_postproc,0);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, vfd_str[STR_OFF + me->atmos_opt.b_postproc], 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case EIGHT_RELEASE_SIG:
		{
			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_VIRT], 1, CENTER);
			me->atmos_opt.virt_mode += 1;
			if(me->atmos_opt.virt_mode > 1)
			{
				me->atmos_opt.virt_mode = 0;
			}
			ap_printf("Virtualization %s\n",vfd_str[STR_OFF + me->atmos_opt.virt_mode]);
			SpHdsp_setAtmos(NULL,SP_ATMOS_VIRT,0,me->atmos_opt.virt_mode);
			SpHdsp_Reset(NULL,0);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, vfd_str[STR_OFF + me->atmos_opt.virt_mode], 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case NINE_RELEASE_SIG:
		{
			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_XML], 1, CENTER);
			me->atmos_opt.config_file += 1;
			if(me->atmos_opt.config_file > 1)
			{
				me->atmos_opt.config_file = 0;
			}
			ap_printf("XML %s\n",vfd_str[STR_OFF + me->atmos_opt.config_file]);
			SpHdsp_setAtmos(NULL,SP_ATMOS_CONFIG,0,me->atmos_opt.config_file);
			SpHdsp_Reset(NULL,0);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, vfd_str[STR_OFF + me->atmos_opt.config_file], 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case ZERO_RELEASE_SIG:
		{
			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_DRC], 1, CENTER);
			me->atmos_opt.drc_mode += 1;
			if(me->atmos_opt.drc_mode > 2)
			{
				me->atmos_opt.drc_mode = 0;
			}
			ap_printf("DRC %s\n",vfd_str[STR_ATMOS_DRC_OFF + me->atmos_opt.drc_mode]);
			SpHdsp_setAtmos(NULL,SP_ATMOS_DRC,0,me->atmos_opt.drc_mode);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_DRC_OFF + me->atmos_opt.drc_mode], 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case KOK_ECHO_DN_RELEASE_SIG:
		case KOK_ECHO_DN_TICK_SIG:
		{
			char str[14];
			me->atmos_opt.drc_cut += 5;
			if(me->atmos_opt.drc_cut > 100)
			{
				me->atmos_opt.drc_cut = 0;
			}
			ap_printf("DRC Cut %d\n",me->atmos_opt.drc_cut);
			SpHdsp_setAtmos(NULL,SP_ATMOS_DRC,1,me->atmos_opt.drc_cut);
			UserAppNum2Str(str,me->atmos_opt.drc_cut);
			VFD_Service_POP(&me->super,str, 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case KOK_KEY_SHIFT_DN_RELEASE_SIG:
		case KOK_KEY_SHIFT_DN_TICK_SIG:
		{
			char str[14];
			me->atmos_opt.drc_boost += 5;
			if(me->atmos_opt.drc_boost > 100)
			{
				me->atmos_opt.drc_boost = 0;
			}
			ap_printf("DRC Boost %d\n",me->atmos_opt.drc_boost);
			SpHdsp_setAtmos(NULL,SP_ATMOS_DRC,2,me->atmos_opt.drc_boost);
			UserAppNum2Str(str,me->atmos_opt.drc_boost);
			VFD_Service_POP(&me->super,str, 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case PRE_RELEASE_SIG:
		{
			char str[14];
			me->atmos_opt.virt_frontangle += 1;
			if(me->atmos_opt.virt_frontangle > 30)
			{
				me->atmos_opt.virt_frontangle = 0;
			}
			ap_printf("Virtualizer Front Angle %d\n",me->atmos_opt.virt_frontangle);
			SpHdsp_setAtmos(NULL,SP_ATMOS_VIRT,1,me->atmos_opt.virt_frontangle);
			SpHdsp_Reset(NULL,0);
			UserAppNum2Str(str,me->atmos_opt.virt_frontangle);
			VFD_Service_POP(&me->super,str, 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case NEXT_RELEASE_SIG:

		{
			char str[14];
			me->atmos_opt.virt_surrangle += 1;
			if(me->atmos_opt.virt_surrangle >30)
			{
				me->atmos_opt.virt_surrangle = 0;
			}
			ap_printf("Virtualizer Surround Angle %d\n",me->atmos_opt.virt_surrangle);
			SpHdsp_setAtmos(NULL,SP_ATMOS_VIRT,2,me->atmos_opt.virt_surrangle);
			SpHdsp_Reset(NULL,0);
			UserAppNum2Str(str,me->atmos_opt.virt_surrangle);
			VFD_Service_POP(&me->super,str, 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case FB_RELEASE_SIG:
		{
			char str[14];
			me->atmos_opt.virt_heightangle += 1;
			if(me->atmos_opt.virt_heightangle >30)
			{
				me->atmos_opt.virt_heightangle = 0;
			}
			ap_printf("Virtualizer Height Angle %d\n",me->atmos_opt.virt_heightangle);
			SpHdsp_setAtmos(NULL,SP_ATMOS_VIRT,3,me->atmos_opt.virt_heightangle);
			SpHdsp_Reset(NULL,0);
			UserAppNum2Str(str,me->atmos_opt.virt_heightangle);
			VFD_Service_POP(&me->super,str, 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case FF_RELEASE_SIG:
		{
			char str[14];
			me->atmos_opt.virt_rearsurrangle += 1;
			if(me->atmos_opt.virt_rearsurrangle >30)
			{
				me->atmos_opt.virt_rearsurrangle = 0;
			}
			ap_printf("Virtualizer Rear Sourround Angle %d\n",me->atmos_opt.virt_rearsurrangle);
			SpHdsp_setAtmos(NULL,SP_ATMOS_VIRT,4,me->atmos_opt.virt_rearsurrangle);
			SpHdsp_Reset(NULL,0);
			UserAppNum2Str(str,me->atmos_opt.virt_rearsurrangle);
			VFD_Service_POP(&me->super,str, 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case STOP_RELEASE_SIG:
		{
			int profile_max = 3;
			me->atmos_opt.profile_id += 1;
			if(me->atmos_opt.config_file)
			{
				profile_max = 11;
			}

			if(me->atmos_opt.profile_id > profile_max)
			{
				me->atmos_opt.profile_id = 0;
			}

			ap_printf("Profile %s\n",vfd_str[STR_ATMOS_PROFILE_OFF + me->atmos_opt.profile_id]);
			SpHdsp_setAtmos(NULL,SP_ATMOS_PROFILE,me->atmos_opt.profile_id,0);
			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_PROFILE_OFF + me->atmos_opt.profile_id], 3, LEFT);

			status = Q_HANDLED();
			break;
		}
#endif

        default: {
            status = Q_SUPER(&UserApp_active);
            break;
        }
    }
    return status;
}

/******************************************************************************************/
/**
 * \fn			QState UserApp_active_HDMI_P0(UserApp * const me, QEvt const * const e)
 *
 * \brief		the state that corresponds to ARC source
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_active_ARC state \n
 *			    \e status : Q_TRAN() trans to other state \n
 *              \e status : Q_SUPER() trans to UserApp_active state \n
 ******************************************************************************************/
QState UserApp_active_HDMI_RX(UserApp * const me, QEvt const * const e) {
    QState status = 0;
#ifdef ATMOS_CERT
	extern void vTaskDelay();
#endif

	static BYTE last_SrcData;

    switch (e->sig) {
        case Q_ENTRY_SIG: {


			ap_BurnTest_log("ENTRY:\n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG: {
			ap_BurnTest_log("EXIT\n");
			UserAppMute(me,1);
			me->codecType = 0;
			UserAppHintStop(me);
			
	
			if((me->aUserSrc_tbl[me->iSrc] > eAp_HDMI2_Src)||(me->aUserSrc_tbl[me->iSrc] < eAp_ARC_Src))
			{
		
				Hdmi_SetHandleSig(&me->super, CMD_MODE_BT, me->mute, 0);
			}
#ifndef BUILT4ISP2
			 AudDspService_SPDIF_STOP(&me->super);
			 AudDspService_Flow_Stop(&me->super);
#endif
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG: {
			ap_BurnTest_log("INIT\n");

			UartApp_SetKey_Sig(&me->super, SOURCE_SW_RELEASE_SIG, me->iSrc);
			
			AmpSet_SetSrcGain(me->hAmpList, me->iSrc);
			
					if (me->aUserSrc_tbl[me->iSrc] == eAp_HDMI0_Src)
			{
				ap_BurnTest_log("eAp_HDMI0_Src\n");
				
				Hdmi_SetHandleSig(&me->super, CMD_MODE_HDMI_1, me->mute, 0);
			
				UserAppDisplay(me, STR_HDMI0);

			}
			else if (me->aUserSrc_tbl[me->iSrc] == eAp_HDMI1_Src)
			{
				ap_BurnTest_log("eAp_HDMI1_Src\n");

				Hdmi_SetHandleSig(&me->super, CMD_MODE_HDMI_2, me->mute, 0);
				
				UserAppDisplay(me, STR_HDMI1);

			}
			else if (me->aUserSrc_tbl[me->iSrc] == eAp_HDMI2_Src)
			{
				ap_BurnTest_log("eAp_HDMI2_Src\n");
				UserAppDisplay(me, STR_HDMI2);
			}
			else
			{
				ap_printf_E("HDMI PORT error \n");
			}

			#ifndef SDK_RELEASE
			ap_Memory_check();
			#endif

	
#ifndef BUILT4ISP2

				last_SrcData = Source_HDMIHBR;
				AudDspService_SPDIF_STOP(&me->super);
				AudDspService_Flow_Stop(&me->super);
				AudDspService_Prepare_8CH_IISIN(&me->super, IIS8CH_HDMIHBR);
		       	AudDspService_Set_SourceGain(&me->super, SrcGain_Tab[eAp_ARC_Src]);
		        	AudDspService_Flow_Play(&me->super);
		        	AudDspService_SPDIF_PLAY(&me->super);
#endif
#if (SUPPORT_VA)
			VoiceAssistService_VAStatusReset(NULL, me->AuxVA_Status, me->BtVA_Status);
			if(!me->BtVA_Status)
			{
				UserApp_SetBTMute();
			}
			if(!me->AuxVA_Status)
			{
				UserApp_SetAuxMute();
			}
#endif
	      me->mute = eMute_off;
            UserAppMute(me,me->mute);

            status =  Q_HANDLED();
            break;
        }
	case HDMI_D4_MODE_SIG:
		
		if(last_SrcData != Source_HDMIHBR)
		{	
			ap_BurnTest_log("==== *** STR_HDMI *** ==== HDMI  HBR \n");
		
			last_SrcData = Source_HDMIHBR;
			AudDspService_SPDIF_STOP(&me->super);
			AudDspService_Flow_Stop(&me->super);
			AudDspService_Prepare_8CH_IISIN(&me->super, IIS8CH_HDMIHBR);
			AudDspService_Flow_Play(&me->super);
			AudDspService_SPDIF_PLAY(&me->super);
			UserAppMute(me,me->mute);
		}
		status = Q_HANDLED();
		break;

	case HDMI_D0_MODE_SIG:
		if(last_SrcData != Source_HDMISTD)
		{	

			ap_BurnTest_log("==== *** STR_HDMI *** ==== HDMI  STD \n");
		
			last_SrcData = Source_HDMISTD;
			AudDspService_SPDIF_STOP(&me->super);
			AudDspService_Flow_Stop(&me->super);
			AudDspService_Prepare_8CH_IISIN(&me->super, IIS8CH_HDMISTD);
			AudDspService_Flow_Play(&me->super);
			AudDspService_SPDIF_PLAY(&me->super);
			UserAppMute(me,me->mute);
		}
		status = Q_HANDLED();
		break;


        case IOP_MUTE_REQ_SIG: {
            ap_printf("mute request; \n");
            char strBuf[14];
            int MuteState = 0;

            MuteState = Q_EVT_CAST(stSYS_MuteReq_t)->MuteState;

            ap_printf("mute request; %d \n", MuteState);

            UserAppMute(me,MuteState);

			me->mute = MuteState;
            //IOSrv_AudInfo_report(me, me->mute, me->vol, USERAPP_VOL_MAX);
            SystemService_AudioInfoBroadcast(&me->super, SYS_MUTE_CHG, me->mute, me->vol, USERAPP_VOL_MAX, USERAPP_VOL_MIN);

            if (MuteState == eMute_on)
            {
                UserAppNum2Str(strBuf , 0);
            }
            else
            {
                UserAppNum2Str(strBuf , me->vol);
            }

            VFD_Service_POP(me , strBuf, 3, RIGHT);

            status = Q_HANDLED();
            break;
        }
		case REPEATER_AUD_SRC_SIG:{

			RepeaterAudSrcEvt_t *pstCEC_Evt = ((RepeaterAudSrcEvt_t *)e);
			ap_printf("REPEATER_AUD_SRC_SIG ==> Aud_I2S_Lane:%d\n",pstCEC_Evt->Aud_I2S_Lane);
#ifndef BUILT4ISP2
			AudDspService_SPDIF_STOP(&me->super);
			AudDspService_Flow_Stop(&me->super);

			if(pstCEC_Evt->Aud_I2S_Lane == Source_ARC)
			{
				#if defined(CFG_EMU_V3_SPDIF) && (CFG_EMU_V3_SPDIF == 1)
					AudDspService_Prepare_SPDIF(&me->super, SPDIF_IEC0);
				#else
					AudDspService_Prepare_SPDIF(&me->super, SPDIF_IEC1);
				#endif
			}
            else if(pstCEC_Evt->Aud_I2S_Lane == Source_HDMISTD)
			    AudDspService_Prepare_8CH_IISIN(&me->super, IIS8CH_HDMISTD);
            else if(pstCEC_Evt->Aud_I2S_Lane == Source_HDMIHBR)
			    AudDspService_Prepare_8CH_IISIN(&me->super, IIS8CH_HDMIHBR);


			AudDspService_Flow_Play(&me->super);
			AudDspService_SPDIF_PLAY(&me->super);

#endif
			status = Q_HANDLED();
			break;

		}
		case REPEATER_LPCMCHANNEL_NUM_SIG:
		{

			ap_printf("REPEATER_LPCMCHANNEL_NUM_SIG \n");
			RepeatereLPCMNumEvt_t *pstLPCMNumEvt = ((RepeatereLPCMNumEvt_t *)e);
			UINT8 channel_num = pstLPCMNumEvt->LPCM_channelNum;
			UINT8 channel_mapping = pstLPCMNumEvt->LPCM_ChannelMapping;
			AudDspService_SPDIF_SetParam(NULL, ODSP_SPDIF_CHANNEL, channel_num, channel_mapping);
			status = Q_HANDLED();
			break;
		}
		#ifdef CEC_CTS_TEST
		case PLAY_RELEASE_SIG: {
	        ap_printf("ARC:play key released,set SAC off\n");
            IOSrv_CEC_Set_SAC(&me->super,0);
	        status = Q_HANDLED();
	        break;
        }
		case PROGRAM_RELEASE_SIG: {
	        ap_printf("ARC:bt key released,set SAC on\n");
            IOSrv_CEC_Set_SAC(&me->super,1);
	        status = Q_HANDLED();
	        break;
        }
		#endif

        case CEC_EVT_SIG: {
            ap_printf("get CEC_EVT_SIG\n");
            stCECEvt_t *pstCEC_Evt = ((stCECEvt_t *)e) ;
            ap_printf("opcode: 0x%x, size:%d\n",pstCEC_Evt->stCecInfo.opcode, pstCEC_Evt->stCecInfo.size);
            status = Q_HANDLED();
            break;
        }

#ifdef ATMOS_CERT
		case ONE_RELEASE_SIG:
		{

			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_DIRECT], 1, CENTER);
			me->atmos_opt.direct += 1;
			if(me->atmos_opt.direct > 1)
			{
				me->atmos_opt.direct = 0;
			}
			ap_printf("Direct %s\n",vfd_str[STR_OFF + me->atmos_opt.direct]);
			SpHdsp_setAtmos(NULL,SP_ATMOS_DIRECT,me->atmos_opt.direct,0);
			SpHdsp_Reset(NULL,0);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, vfd_str[STR_OFF + me->atmos_opt.direct], 3, RIGHT);


			status = Q_HANDLED();
			break;
		}
		case TWO_RELEASE_SIG:
		{
			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_HFT], 1, CENTER);
			me->atmos_opt.b_height_filter += 1;
			if(me->atmos_opt.b_height_filter > 1)
			{
				me->atmos_opt.b_height_filter = 0;
			}
			ap_printf("Height Cut Filter %s\n",vfd_str[STR_OFF + me->atmos_opt.b_height_filter]);
			SpHdsp_setAtmos(NULL,SP_ATMOS_HEIGHT_FILTER,me->atmos_opt.b_height_filter,0);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, vfd_str[STR_OFF + me->atmos_opt.b_height_filter], 3, RIGHT);
			status = Q_HANDLED();
			break;
		}
		case THREE_RELEASE_SIG:
		{
			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_LM], 1, CENTER);
			me->atmos_opt.b_enable_loudness_mgmt += 1;
			if(me->atmos_opt.b_enable_loudness_mgmt > 1)
			{
				me->atmos_opt.b_enable_loudness_mgmt = 0;
			}
			ap_printf("Loudness managemant %s\n",vfd_str[STR_OFF + me->atmos_opt.b_enable_loudness_mgmt]);
			SpHdsp_setAtmos(NULL,SP_ATMOS_LM,me->atmos_opt.b_enable_loudness_mgmt,0);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, vfd_str[STR_OFF + me->atmos_opt.b_enable_loudness_mgmt], 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case FOUR_RELEASE_SIG:
		{
			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_VLAMP], 1, CENTER);
			me->atmos_opt.b_vlamp += 1;
			if(me->atmos_opt.b_vlamp > 1)
			{
				me->atmos_opt.b_vlamp = 0;
			}
			ap_printf("Volume Leveler %s\n",vfd_str[STR_OFF + me->atmos_opt.b_vlamp]);
			SpHdsp_setAtmos(NULL,SP_ATMOS_VLAMP,me->atmos_opt.b_vlamp,0);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, vfd_str[STR_OFF + me->atmos_opt.b_vlamp], 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case FIVE_RELEASE_SIG:
		{
			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_UPMIX], 1, CENTER);
			me->atmos_opt.b_enable_upmixer += 1;
			if(me->atmos_opt.b_enable_upmixer > 1)
			{
				me->atmos_opt.b_enable_upmixer = 0;
			}
			ap_printf("Surround Decoder %s\n",vfd_str[STR_OFF + me->atmos_opt.b_enable_upmixer]);
			SpHdsp_setAtmos(NULL,SP_ATMOS_UPMIX,me->atmos_opt.b_enable_upmixer,0);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, vfd_str[STR_OFF + me->atmos_opt.b_enable_upmixer], 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case SIX_RELEASE_SIG:
		{
			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_CNTSPD], 1, CENTER);
			me->atmos_opt.b_center_spread += 1;
			if(me->atmos_opt.b_center_spread > 1)
			{
				me->atmos_opt.b_center_spread = 0;
			}
			ap_printf("Center Spread %s\n",vfd_str[STR_OFF + me->atmos_opt.b_center_spread]);
			SpHdsp_setAtmos(NULL,SP_ATMOS_CSPREAD,me->atmos_opt.b_center_spread,0);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, vfd_str[STR_OFF + me->atmos_opt.b_center_spread], 3, RIGHT);


			status = Q_HANDLED();
			break;
		}
		case SEVEN_RELEASE_SIG:
		{
			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_POST], 1, CENTER);
			me->atmos_opt.b_postproc += 1;
			if(me->atmos_opt.b_postproc > 1)
			{
				me->atmos_opt.b_postproc = 0;
			}
			ap_printf("Post Processing %s\n",vfd_str[STR_OFF + me->atmos_opt.b_postproc]);
			SpHdsp_setAtmos(NULL,SP_ATMOS_POST,me->atmos_opt.b_postproc,0);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, vfd_str[STR_OFF + me->atmos_opt.b_postproc], 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case EIGHT_RELEASE_SIG:
		{
			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_VIRT], 1, CENTER);
			me->atmos_opt.virt_mode += 1;
			if(me->atmos_opt.virt_mode > 1)
			{
				me->atmos_opt.virt_mode = 0;
			}
			ap_printf("Virtualization %s\n",vfd_str[STR_OFF + me->atmos_opt.virt_mode]);
			SpHdsp_setAtmos(NULL,SP_ATMOS_VIRT,0,me->atmos_opt.virt_mode);
			SpHdsp_Reset(NULL,0);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, vfd_str[STR_OFF + me->atmos_opt.virt_mode], 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case NINE_RELEASE_SIG:
		{
			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_XML], 1, CENTER);
			me->atmos_opt.config_file += 1;
			if(me->atmos_opt.config_file > 1)
			{
				me->atmos_opt.config_file = 0;
			}
			ap_printf("XML %s\n",vfd_str[STR_OFF + me->atmos_opt.config_file]);
			SpHdsp_setAtmos(NULL,SP_ATMOS_CONFIG,0,me->atmos_opt.config_file);
			SpHdsp_Reset(NULL,0);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, vfd_str[STR_OFF + me->atmos_opt.config_file], 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case ZERO_RELEASE_SIG:
		{
			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_DRC], 1, CENTER);
			me->atmos_opt.drc_mode += 1;
			if(me->atmos_opt.drc_mode > 2)
			{
				me->atmos_opt.drc_mode = 0;
			}
			ap_printf("DRC %s\n",vfd_str[STR_ATMOS_DRC_OFF + me->atmos_opt.drc_mode]);
			SpHdsp_setAtmos(NULL,SP_ATMOS_DRC,0,me->atmos_opt.drc_mode);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_DRC_OFF + me->atmos_opt.drc_mode], 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case KOK_ECHO_DN_RELEASE_SIG:
		case KOK_ECHO_DN_TICK_SIG:
		{
			char str[14];
			me->atmos_opt.drc_cut += 5;
			if(me->atmos_opt.drc_cut > 100)
			{
				me->atmos_opt.drc_cut = 0;
			}
			ap_printf("DRC Cut %d\n",me->atmos_opt.drc_cut);
			SpHdsp_setAtmos(NULL,SP_ATMOS_DRC,1,me->atmos_opt.drc_cut);
			UserAppNum2Str(str,me->atmos_opt.drc_cut);
			VFD_Service_POP(&me->super,str, 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case KOK_KEY_SHIFT_DN_RELEASE_SIG:
		case KOK_KEY_SHIFT_DN_TICK_SIG:
		{
			char str[14];
			me->atmos_opt.drc_boost += 5;
			if(me->atmos_opt.drc_boost > 100)
			{
				me->atmos_opt.drc_boost = 0;
			}
			ap_printf("DRC Boost %d\n",me->atmos_opt.drc_boost);
			SpHdsp_setAtmos(NULL,SP_ATMOS_DRC,2,me->atmos_opt.drc_boost);
			UserAppNum2Str(str,me->atmos_opt.drc_boost);
			VFD_Service_POP(&me->super,str, 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case PRE_RELEASE_SIG:
		{
			char str[14];
			me->atmos_opt.virt_frontangle += 1;
			if(me->atmos_opt.virt_frontangle > 30)
			{
				me->atmos_opt.virt_frontangle = 0;
			}
			ap_printf("Virtualizer Front Angle %d\n",me->atmos_opt.virt_frontangle);
			SpHdsp_setAtmos(NULL,SP_ATMOS_VIRT,1,me->atmos_opt.virt_frontangle);
			SpHdsp_Reset(NULL,0);
			UserAppNum2Str(str,me->atmos_opt.virt_frontangle);
			VFD_Service_POP(&me->super,str, 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case NEXT_RELEASE_SIG:

		{
			char str[14];
			me->atmos_opt.virt_surrangle += 1;
			if(me->atmos_opt.virt_surrangle >30)
			{
				me->atmos_opt.virt_surrangle = 0;
			}
			ap_printf("Virtualizer Surround Angle %d\n",me->atmos_opt.virt_surrangle);
			SpHdsp_setAtmos(NULL,SP_ATMOS_VIRT,2,me->atmos_opt.virt_surrangle);
			SpHdsp_Reset(NULL,0);
			UserAppNum2Str(str,me->atmos_opt.virt_surrangle);
			VFD_Service_POP(&me->super,str, 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case FB_RELEASE_SIG:
		{
			char str[14];
			me->atmos_opt.virt_heightangle += 1;
			if(me->atmos_opt.virt_heightangle >30)
			{
				me->atmos_opt.virt_heightangle = 0;
			}
			ap_printf("Virtualizer Height Angle %d\n",me->atmos_opt.virt_heightangle);
			SpHdsp_setAtmos(NULL,SP_ATMOS_VIRT,3,me->atmos_opt.virt_heightangle);
			SpHdsp_Reset(NULL,0);
			UserAppNum2Str(str,me->atmos_opt.virt_heightangle);
			VFD_Service_POP(&me->super,str, 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case FF_RELEASE_SIG:
		{
			char str[14];
			me->atmos_opt.virt_rearsurrangle += 1;
			if(me->atmos_opt.virt_rearsurrangle >30)
			{
				me->atmos_opt.virt_rearsurrangle = 0;
			}
			ap_printf("Virtualizer Rear Sourround Angle %d\n",me->atmos_opt.virt_rearsurrangle);
			SpHdsp_setAtmos(NULL,SP_ATMOS_VIRT,4,me->atmos_opt.virt_rearsurrangle);
			SpHdsp_Reset(NULL,0);
			UserAppNum2Str(str,me->atmos_opt.virt_rearsurrangle);
			VFD_Service_POP(&me->super,str, 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case STOP_RELEASE_SIG:
		{
			int profile_max = 3;
			me->atmos_opt.profile_id += 1;
			if(me->atmos_opt.config_file)
			{
				profile_max = 11;
			}

			if(me->atmos_opt.profile_id > profile_max)
			{
				me->atmos_opt.profile_id = 0;
			}

			ap_printf("Profile %s\n",vfd_str[STR_ATMOS_PROFILE_OFF + me->atmos_opt.profile_id]);
			SpHdsp_setAtmos(NULL,SP_ATMOS_PROFILE,me->atmos_opt.profile_id,0);
			VFD_Service_POP(&me->super, vfd_str[STR_ATMOS_PROFILE_OFF + me->atmos_opt.profile_id], 3, LEFT);

			status = Q_HANDLED();
			break;
		}
#endif
        default: {
            status = Q_SUPER(&UserApp_active);
            break;
        }
    }
    return status;
}

#endif
#if 0
/******************************************************************************************/
/**
 * \fn			QState UserApp_active_HDMI_TX(UserApp * const me, QEvt const * const e)
 *
 * \brief		the state that corresponds to hdmi tx source
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_active_Line_in state \n
 *			    \e status : Q_TRAN() trans to other state \n
 *              \e status : Q_SUPER() trans to UserApp_active state \n
 ******************************************************************************************/
QState UserApp_active_HDMI_TX(UserApp * const me, QEvt const * const e) {
    QState status = 0;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            UserAppDisplay(me, STR_HDMI);
            ap_BurnTest_log("ENTRY\n");

            status = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG: {
            ap_BurnTest_log("EXIT\n");
            UserAppMute(me,1);
/* HDMI ready to open
			IOSrv_HDMI_TX_Terminate(&me->super);

			if (me->iSrc != eAp_HDMI_TX_Src)//after IOSrv_HDMI_TX_Terminate()
			{
				if(me->iSrc == eAp_ARC_Src)
					IOSrv_CEC_SetSrc(&me->super, eIOP_CecSrc_ARC);
				else
					IOSrv_CEC_SetSrc(&me->super, eIOP_CecSrc_Others);
			}

			UserAppVolumeNotifyToBt(me->vol,me->mute);// restored volume value
#ifndef BUILT4ISP2
			AudDspService_Flow_Stop(&me->super);
			AudDspService_Sine_Set(&me->super, 0, 0);
#endif
*/
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG: {
			ap_BurnTest_log("INIT\n");
/* HDMI ready to open
			UINT8 SrcData = 0;
			SrcData = me->iSrc;
			SendData2MCU(me->stMCU_IF, &SrcData, eUserCmdSrc, 1);
			QTimeEvt_disarmX(&me->timeEvtMCU);
			QTimeEvt_armX(&me->timeEvtMCU, TICKS_PER_SEC, 0);

			IOSrv_CEC_SetSrc(&me->super, eIOP_CecSrc_HDMI_TX);//before IOSrv_HDMI_TX_Initiate()
			IOSrv_HDMI_TX_Initiate(&me->super);

#ifndef BUILT4ISP2
			AudDspService_Set_VolumeChannel(&me->super,CH_SUB,0);  //mute sub speaker, and it will be restored at EXIT by UserAppVolumeNotifyToBt()
			AudDspService_Prepare_SINE(&me->super);
			AudDspService_Flow_Play(&me->super);
			//AudDspService_Set_SourceGain(&me->super, 400); //set DSP output fixed 0dB in HDMI_TX
#endif
			//UserAppMute(me,me->mute);
*/
			status =  Q_HANDLED();
			break;
		}
		default: {
			status = Q_SUPER(&UserApp_active);
			break;
		}
		case CEC_EVT_SIG: {
			ap_printf("get CEC_EVT_SIG\n");
			stCECEvt_t *pstCEC_Evt = ((stCECEvt_t *)e) ;

			ap_printf("opcode: 0x%x, size:%d\n",pstCEC_Evt->stCecInfo.opcode, pstCEC_Evt->stCecInfo.size);

			status = Q_HANDLED();
			break;
	}
	}
	return status;
}
#endif

/******************************************************************************************/
/**
 * \fn			QState UserApp_AudioPlayer_play(UserApp * const me, QEvt const * const e)
 *
 * \brief		the state that corresponds to USB play mode
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_AudioPlayer_play state \n
 *			    \e status : Q_TRAN() trans to other state \n
 *              \e status : Q_SUPER() trans to UserApp_active state \n
 ******************************************************************************************/
QState UserApp_AudioPlayer_play(UserApp *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			ap_BurnTest_log("ENTRY\n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			ap_BurnTest_log("EXIT\n");
			VFD_DotCtrl(me, VFD_DOT_PLAY, VFD_DOT_OP_CLR);

			QTimeEvt_disarmX(&me->timeEvt);
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			ap_BurnTest_log("INIT showErr:%d\n", me->showErr);
			VFD_DotCtrl(me, VFD_DOT_PLAY, VFD_DOT_OP_SET);
			QTimeEvt_armX(&me->timeEvt, TICKS_PER_250MS, 0);
			me->usb_play_state = USB_PLAY; //--{{Added by Grace to save USB state
			status = Q_HANDLED();
			break;
		}
		//case BT_MUSIC_PUASE_REQ_SIG:
		//case BT_MUSIC_PLAY_REQ_SIG:
		case PLAY_RELEASE_SIG:
		case PAUSE_RELEASE_SIG:
		{
			ap_printf("[%s]-> pause; \n", __FUNCTION__);

			UserAppDisplayOnce(me, vfd_str[STR_PAUSE], 1);

			if(me->playercreat == 1)
			{

#ifdef SUPPORT_AP
				ap_printf("Pause\n");
				MFP_Pause(me->pAPInstance);
#endif

				status = Q_TRAN(&UserApp_AudioPlayer_pause);
			}
			else
			{
				status = Q_HANDLED();
			}
			break;
		}
		//case BT_MUSIC_NEXT_REQ_SIG:
		case NEXT_RELEASE_SIG:
		{
			ap_printf("[%s]NEXT; \n", __FUNCTION__);
			if(me->playercreat == 1)
			{
				MFP_PlayNext(me->pAPInstance, PLAY_NEXT_FILE);
			}
			status = Q_HANDLED();
			break;
		}
		//case BT_MUSIC_PREV_REQ_SIG:
		case PRE_RELEASE_SIG:
		{
			ap_printf("[%s]Pre; \n", __FUNCTION__);
			if(me->playercreat == 1)
			{
				MFP_PlayNext(me->pAPInstance, PLAY_PREV_FILE);
			}
			status = Q_HANDLED();
			break;
		}
#if (SUPPORT_FLODER)
		case UP_RELEASE_SIG:
		{
			ap_printf("[%s]Prev Floder; \n", __FUNCTION__);
			me->opType = 1;
			if(me->playercreat == 1)
			{
				MFP_PlayNext(me->pAPInstance, PLAY_PREV_FOLDER);
			}
			status = Q_HANDLED();
			break;
		}
		case DOWN_RELEASE_SIG:
		{
			ap_printf("[%s]Next Floder; \n", __FUNCTION__);
			me->opType = 1;
			if(me->playercreat == 1)
			{
				MFP_PlayNext(me->pAPInstance, PLAY_NEXT_FOLDER);
			}
			status = Q_HANDLED();
			break;
		}
#endif
		case FF_RELEASE_SIG:
		{
			AudDspService_Set_FF_FB_EndingCtrl(NULL);
			ap_printf("[%s]: FF; \n", __FUNCTION__);

#ifdef SUPPORT_AP
			me->iTrickyFactor = MFP_SPEED_FF2X;
			ap_printf("[%s]: FF%dx; \n", __FUNCTION__, me->iTrickyFactor);
			MFP_Tricky(me->pAPInstance, me->iTrickyFactor);
#endif

			status = Q_TRAN(&UserApp_AudioPlayer_tricky);
			break;
		}
		case FB_RELEASE_SIG:
		{
			AudDspService_Set_FF_FB_EndingCtrl(NULL);
			ap_printf("[%s]: BF; \n", __FUNCTION__);

#ifdef SUPPORT_AP
			me->iTrickyFactor = MFP_SPEED_FB2X;
			ap_printf("[%s]: FB%dx; \n", __FUNCTION__, me->iTrickyFactor);
			MFP_Tricky(me->pAPInstance, me->iTrickyFactor);
#endif

			status = Q_TRAN(&UserApp_AudioPlayer_tricky);
			break;
		}
		case MFP_SWITCHTO_STOP_SIG:
		{
			ap_printf("[%s] play to stop\n", __FUNCTION__);
			status = Q_TRAN(&UserApp_AudioPlayer_stop);
			break;
		}
		case MFP_STATE_SIG:
		{
			AUTORETURNEVT *pe = (AUTORETURNEVT *)e;
			UINT32 state = (UINT32)pe->pParam;
			ap_printf("[%s]Player State change to %d\n", __FUNCTION__, state);
			switch (state)
			{
				case MFP_STATE_PAUSE:
					status = Q_TRAN(UserApp_AudioPlayer_pause);
					break;
				case MFP_STATE_STOP:
					status = Q_TRAN(UserApp_AudioPlayer_stop);
					break;
				case MFP_STATE_PLAY:
					if (me->showErr)
					{
						ap_printf("[%s]Restart show time\n", __FUNCTION__);
						QTimeEvt_disarmX(&me->timeEvt);
						QTimeEvt_armX(&me->timeEvt, TICKS_PER_250MS, 0);
						me->showErr = 0;
					}
					status = Q_HANDLED();
					break;
				default:
					status = Q_HANDLED();
					break;
			}
			break;
		}
		case SHOW_PLAYING_TIME:
		{
			if (!me->showErr)
			{
				QTimeEvt_armX(&me->timeEvt, TICKS_PER_250MS, 0);

				if(me->delay_call == NULL && me->mute == eMute_off && vfd_is_scroll() == 0)  // {{Added by Grace to disable playtime display in setting menu
				{
				INT64 plTime = 0;
				if (me->pAPInstance)
				{
					MFP_Ctrl(me->pAPInstance, MFP_GET_PLAY_TIME, &plTime);
						me->usb_playtime = plTime;
					VFD_Service_Play_Time(&me->super, plTime / 1000);
					//ap_printf("[%s]ShowTime:%llds  ->    %lldms\n", __FUNCTION__, plTime/1000, plTime);
				}
			}
			}
			status = Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&UserApp_active_AudioPlayer);
			break;
		}
	}
	return status;
}
/******************************************************************************************/
/**
 * \fn			QState UserApp_AudioPlayer_pause(UserApp * const me, QEvt const * const e)
 *
 * \brief		the state that corresponds to USB pause mode
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_AudioPlayer_pause state \n
 *			    \e status : Q_TRAN() trans to other state \n
 *              \e status : Q_SUPER() trans to UserApp_active state \n
 ******************************************************************************************/
QState UserApp_AudioPlayer_pause(UserApp *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			ap_BurnTest_log("ENTRY\n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			ap_BurnTest_log("EXIT\n");
			VFD_DotCtrl(me, VFD_DOT_PAUSE, VFD_DOT_OP_CLR);
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			ap_BurnTest_log("INIT\n");
			//VFD_DotCtrl(me, VFD_DOT_PAUSE, VFD_DOT_OP_SET);
			BYTE strBuf[8]={0};
			if(me->pAPInstance)
			{
				sprintf(strBuf, "%02d-%02d",  (UINT32)(me->usb_playtime/1000/60), (UINT32)(me->usb_playtime/1000%60));		
				VFD_Service_Display(&me->super ,strBuf);
	    			VFD_Service_Align(&me->super, CENTER);
	    			VFD_Service_Blink(&me->super, 10, 10);
			}
			me->usb_play_state = USB_PAUSE;//--{{Added by Grace to save USB state
			status = Q_HANDLED();
			break;
		}
	//	case BT_MUSIC_PLAY_REQ_SIG:
		case PLAY_RELEASE_SIG:
		{
			ap_printf("[%s]-> play; \n", __FUNCTION__);

			UserAppDisplayOnce(me, vfd_str[STR_PLAY], 1);

			if(me->playercreat == 1)
			{

#ifdef SUPPORT_AP
				ap_printf("Play\n");
				MFP_Play(me->pAPInstance);
#endif

				status = Q_TRAN(&UserApp_AudioPlayer_play);
			}
			else
			{
				status = Q_HANDLED();
			}
			break;
		}
	//	case BT_MUSIC_PUASE_REQ_SIG:
		case PAUSE_RELEASE_SIG:
		{
			ap_printf("Now is pause, do nothing; \n");
			status = Q_HANDLED();
			break;
		}
		case FF_RELEASE_SIG:
		{
			ap_printf("[%s]: FF; \n", __FUNCTION__);
			status = Q_HANDLED();
			break;
		}
		case FB_RELEASE_SIG:
		{
			ap_printf("[%s]: BF; \n", __FUNCTION__);
			status = Q_HANDLED();
			break;
		}
		case MFP_STATE_SIG:
		{
			AUTORETURNEVT *pe = (AUTORETURNEVT *)e;
			UINT32 state = (UINT32)pe->pParam;
			ap_printf("[%s]Player State change to %d\n", __FUNCTION__, state);
			switch (state)
			{
				case MFP_STATE_STOP:
					status = Q_TRAN(&UserApp_AudioPlayer_stop);
					break;
				case MFP_STATE_PLAY:
					status = Q_TRAN(&UserApp_AudioPlayer_play);
					break;
				default:
					status = Q_HANDLED();
					break;
			}
			break;
		}
		default:
		{
			status = Q_SUPER(&UserApp_active_AudioPlayer);
			break;
		}
	}
	return status;
}

/******************************************************************************************/
/**
 * \fn			QState UserApp_AudioPlayer_stop(UserApp * const me, QEvt const * const e)
 *
 * \brief		the state that corresponds to USB stop mode
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_AudioPlayer_stop state \n
 *			    \e status : Q_TRAN() trans to other state \n
 *              \e status : Q_SUPER() trans to UserApp_active state \n
 ******************************************************************************************/
QState UserApp_AudioPlayer_stop(UserApp *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			ap_BurnTest_log("ENTRY\n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			ap_BurnTest_log("EXIT\n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			ap_BurnTest_log("INIT\n");
			status = Q_HANDLED();
			break;
		}
		case MFP_SWITCH_NEXT_SIG:
		{
			ap_printf("[%s]FLApp_play play start  \n", __FUNCTION__);
			if(me->playercreat == 1)
			{
				MFP_PlayNext(me->pAPInstance, PLAY_NEXT_FILE);
			}
			status = Q_HANDLED();
			break;
		}
		case MFP_AUTO_PLAY_SIG:
		{
			ap_printf("[%s] Auto Play\n", __FUNCTION__);
			if(me->playercreat == 1)
			{
				MFP_PlayNext(me->pAPInstance, PLAY_CUR_FILE);
			}
			status = Q_HANDLED();
			break;
		}
		//case BT_MUSIC_PLAY_REQ_SIG:
		case PLAY_RELEASE_SIG:
		{
			ap_printf("[%s] Play\n", __FUNCTION__);

			UserAppDisplayOnce(me, vfd_str[STR_PLAY], 1);

			if(me->playercreat == 1)
			{
				MFP_PlayNext(me->pAPInstance, PLAY_CUR_FILE);
			}

			status = Q_HANDLED();
			break;
		}
		case MFP_STATE_SIG:
		{
			AUTORETURNEVT *pe = (AUTORETURNEVT *)e;
			UINT32 state = (UINT32)pe->pParam;
			ap_printf("[%s]Player State change to %d\n", __FUNCTION__, state);
			switch (state)
			{
				case MFP_STATE_STOP:
					status = Q_HANDLED();
					break;
				case MFP_STATE_PLAY:
					me->showErr = 0;
					status = Q_TRAN(&UserApp_AudioPlayer_play);
					break;
				default:
					status = Q_HANDLED();
					break;
			}
			break;
		}

		default:
		{
			status = Q_SUPER(&UserApp_active_AudioPlayer);
			break;
		}
	}
	return status;
}

/******************************************************************************************/
/**
 * \fn			QState UserApp_AudioPlayer_tricky(UserApp * const me, QEvt const * const e)
 *
 * \brief		the state that corresponds to USB tricky mode
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_AudioPlayer_tricky state \n
 *			    \e status : Q_TRAN() trans to other state \n
 *              \e status : Q_SUPER() trans to UserApp_active state \n
 ******************************************************************************************/
QState UserApp_AudioPlayer_tricky(UserApp *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			ap_BurnTest_log("ENTRY\n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			ap_BurnTest_log("EXIT\n");

			QTimeEvt_disarmX(&me->timeEvt);
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			ap_BurnTest_log("INIT\n");

			QTimeEvt_armX(&me->timeEvt, TICKS_PER_250MS, 0);
			status =  Q_HANDLED();
			break;
		}
		case MFP_STATE_SIG:
		{
			AUTORETURNEVT *pe = (AUTORETURNEVT *)e;
			UINT32 state = (UINT32)pe->pParam;
			ap_printf("[%s]Player State change to %d\n", __FUNCTION__, state);
			switch (state)
			{
				case MFP_STATE_STOP:
					status = Q_TRAN(&UserApp_AudioPlayer_stop);
					break;
				case MFP_STATE_PLAY:
					status = Q_TRAN(&UserApp_AudioPlayer_play);
					break;
				default:
					status = Q_HANDLED();
					break;
			}
			break;
		}
		case BT_MUSIC_PLAY_REQ_SIG:
		case MFP_TRICKYTO_PLAY_SIG:
		case PLAY_RELEASE_SIG:
		{
			ap_printf("[%s]-> play; \n", __FUNCTION__);

#ifdef SUPPORT_AP
			ap_printf("tricky to play\n");
			me->iTrickyFactor = MFP_SPEED_1X;
			MFP_Play(me->pAPInstance);
#endif
			status = Q_HANDLED();
			break;
		}
		case FF_RELEASE_SIG:
		{
			ap_printf("[%s]: FF; \n", __FUNCTION__);

#ifdef SUPPORT_AP
			if (me->iTrickyFactor < MFP_SPEED_FF8X)
			{
				me->iTrickyFactor++;

				ap_printf("[%s]: FF%dx; \n", __FUNCTION__, me->iTrickyFactor);
				MFP_Tricky(me->pAPInstance, me->iTrickyFactor);

				status = Q_HANDLED();
			}
			else
			{
				MFP_Play(me->pAPInstance);
				status = Q_TRAN(&UserApp_AudioPlayer_play);
			}
#endif
			break;
		}
		case FB_RELEASE_SIG:
		{
			ap_printf("[%s]: FB; \n", __FUNCTION__);

#ifdef SUPPORT_AP
			if (me->iTrickyFactor == MFP_SPEED_FB8X)
			{
				MFP_Play(me->pAPInstance);
				status = Q_TRAN(&UserApp_AudioPlayer_play);
			}
			else if (me->iTrickyFactor < MFP_SPEED_FB2X)
			{
				me->iTrickyFactor = MFP_SPEED_FB2X;

				ap_printf("[%s]: FB%dx; \n", __FUNCTION__, me->iTrickyFactor);
				MFP_Tricky(me->pAPInstance, me->iTrickyFactor);

				status = Q_HANDLED();
			}
			else
			{
				me->iTrickyFactor++;

				ap_printf("[%s]: FB%dx; \n", __FUNCTION__, me->iTrickyFactor);
				MFP_Tricky(me->pAPInstance, me->iTrickyFactor);

				status = Q_HANDLED();
			}
#endif
			break;
		}
		case SHOW_PLAYING_TIME:
		{
			QTimeEvt_armX(&me->timeEvt, TICKS_PER_250MS, 0);

			INT64 plTime = 0;
			MFP_Ctrl(me->pAPInstance, MFP_GET_PLAY_TIME, &plTime);
			VFD_Service_Play_Time(&me->super, plTime / 1000);
			//ap_printf("[%s]ShowTime:%llds  ->    %lldms\n", __FUNCTION__, plTime/1000, plTime);

			status = Q_HANDLED();
			break;

		}
		case VOL_UP_TICK_SIG:
		case VOL_UP_RELEASE_SIG:
		{
			status = Q_HANDLED();
			break;
		}
		case VOL_DOWN_TICK_SIG:
		case VOL_DOWN_RELEASE_SIG:
		{
			status = Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&UserApp_active_AudioPlayer);
			break;
		}
	}
	return status;
}

/******************************************************************************************/
/**
 * \fn          void UserApp_IR_Learning_Listener(QActive * const me, void *pParam)
 *
 * \brief      function for listener regist, system will run this function when a new IR key array is received
 *
 * \param   QActive * const me:(Input) AO handler ,\n
 *               void *pParam :ir learning info structure pointer (structure type: stIR_L_Info_t)
 *
 * \return	    none.
 *
 *
 ******************************************************************************************/
void UserApp_IR_Learning_Listener(QActive *const me, void *pParam)
{
	stIR_L_Info_t *pstIR_L_Info = (stIR_L_Info_t * )pParam;
	stSYS_IrLearningInfo_t stIR_L_Target;


	if((pstIR_L_Info->IR_Fmt == 0x01)
			&&(pstIR_L_Info->CustomerCode == 0X7f))
	{
		switch(pstIR_L_Info->KeyCode)
		{
			case 0x4F:

				return;

			default:
				break;

		}
	}
	stIR_L_Target.LearningKey_Target = 1;//do not care

	IOSrv_IR_Learning_Confirm(me, &stIR_L_Target);
	ap_printf("[Learning Key] a key has been learned  Fmt: 0x%x, Cus ID: 0x%x, Key code: 0x%x\n", pstIR_L_Info->IR_Fmt, pstIR_L_Info->CustomerCode, pstIR_L_Info->KeyCode);
	UserAppDisplay((UserApp *)me, STR_IR);

	QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, IR_LEARNING_DONE_SIG), UserApp_get());
}


/******************************************************************************************/
/**
 * \fn			QState UserApp_IR_Learning(UserApp * const me, QEvt const * const e)
 *
 * \brief		the userapp ir learning state
 *
 * \param		QActive * const me:(Input) the UserApp handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in UserApp_IR_Learning state \n
 *			    \e status : Q_TRAN() trans to other state \n
 *
 ******************************************************************************************/
QState UserApp_IR_Learning(UserApp *const me, QEvt const *const e)
{
	QState status;

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			ap_BurnTest_log("ENTRY\n");
			ap_printf("[Learning Key] enter IR learning mode\n");
			ap_printf("plz press the IR key you want to learn\n\n");
			UserAppDisplay(me, STR_IR);
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			ap_BurnTest_log("EXIT\n");
			stSYS_IrLearningInfo_t stIR_L_info;
			stIR_L_info.LearningKey_Target = -1; // unnecessary
			IOSrv_IR_Learning_Terminate(&me->super, &stIR_L_info);
			ap_printf("[Learning Key] exit IR learning mode\n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			ap_BurnTest_log("INIT\n");
			status = Q_HANDLED();
			break;
		}
		case TWO_LONG_RELEASE_SIG:
		{

			status = Q_TRAN(&UserApp_active);
			break;
		}
		case VOL_UP_RELEASE_SIG:
		{
			ap_printf("[Learning Key] vol up, ready to learn; \n");
			LearningVolUpEntry(me);
			UserAppUpdate(me, 1);
			VFD_Service_Blink (me, 10, 10);

			status = Q_HANDLED();
			break;
		}
		case VOL_DOWN_RELEASE_SIG:
		{
			ap_printf("[Learning Key] vol down, ready to learn; \n");
			LearningVolDownEntry(me);
			UserAppUpdate(me, 2);
			VFD_Service_Blink (me, 10, 10);

			status = Q_HANDLED();
			break;
		}
		case MUTE_RELEASE_SIG:
		{
			ap_printf("[Learning Key] mute, ready to learn; \n");
			LearningMuteEntry(me);
			UserAppUpdate(me, 3);
			VFD_Service_Blink (me, 10, 10);

			status = Q_HANDLED();
			break;
		}

		case POWER_RELEASE_SIG:
		{
			ap_printf("[Learning Key] power, ready to learn; \n");
			stSYS_IrLearningInfo_t stIR_L_info;
			stIR_L_info.LearningKey_Target = IR_L_KEY_0;
			IOSrv_IR_Learning_Start(&me->super, &stIR_L_info, UserApp_IR_Learning_Listener);

			UserAppUpdate(me, 0);
			VFD_Service_Blink (me, 10, 10);

			status = Q_HANDLED();
			break;
		}

		default:
		{
			status = Q_SUPER(&UserApp_home);
			break;
		}
	}

	return status;
}

int Userapp_Set_Source_id(UINT8 source_id)
{
	UserApp* const me = &l_UserApp;

	if((source_id == Source_ARC) || (source_id == Source_HDMISTD) || (source_id == Source_HDMIHBR))
	{
		me->EP_Src_id = source_id;

		return 1;

	}

	return 0;
}
UINT8 Userapp_Get_Source_id()
{

	UINT8 source_id = 0;
	UserApp* const me = &l_UserApp;

	if((me->EP_Src_id == Source_ARC) || (me->EP_Src_id == Source_HDMISTD) || (me->EP_Src_id == Source_HDMIHBR))
	{

		source_id = me->EP_Src_id;

		return source_id;

	}

	return 0;

}

int Userapp_Get_System_Memory_Source(void)
{
	UserApp* const me = &l_UserApp;

	int memory_source;

	if((me->iSrc >= APP_SRC_AUX) && (me->iSrc < APP_SRC_MAX))
	{

		memory_source = me->iSrc;

		return memory_source;

	}

	return -1;

}


QState UserApp_PowerOff(UserApp *const me, QEvt const *const e)
{
	QState status;

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			UserAppDisplay_Scroll(me, STR_GOODBYE, R2L);
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			UserAppMute(me, 1);
			UserAppHintStop(me);
			SpHdsp_Stop(NULL,0);
			AudDspService_Flow_Stop(&me->super);
			ap_BurnTest_log("EXIT\n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			ap_BurnTest_log("INIT\n");
			AudDspService_Prepare_ADC(&me->super, AIN1);
			AudDspService_Flow_Play(&me->super);
			AudDspService_Set_SourceGain(&me->super, SrcGain_Tab[me->iSrc]);
			setHdmiMode(0);
			UserAppMute(me, 0);
		//	UserAppCallHint(me, eHint_PowerOff);

			
			QTimeEvt_disarmX(&me->timeSrcHintEvt);
			QTimeEvt_armX(&me->timeSrcHintEvt, (TICKS_PER_500MS*3), 0);
			status = Q_HANDLED();
			break;
		}

		case IOP_KEY_SIG:
		{
			stUserKeyInfo_t stUserKeyInfo;
			int result = 0;
			result = UserAppGetLatestKey_SpecificSrc(eKeySrc_CEC_CMD, me->hKeyBuf, &stUserKeyInfo.stKeyInfo); // get the latest key in key buf but prioritize the key from specific src

			if (result == eRes_specific1) //key buf is not empty
			{
				ap_printf("Key eRes_specific1\n");
				QF_PUBLISH(Q_NEW(QEvt, IOP_KEY_SIG), me); // re-get key
			}

			status = Q_HANDLED();
			break;
		}

		case SRC_HINT_TIME_SIG://SCROLL_BACK_TIMER_SIG:
		{
			status = Q_TRAN(&UserApp_standby);
			break;
		}
		default:
		{
			status = Q_HANDLED();
			break;
		}
	}

	return status;
}


QState UserApp_PowerOn(UserApp *const me, QEvt const *const e)
{
	QState status;

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			UserAppDisplay(me, STR_HELLO);
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			UserAppHintStop(me);
			SpHdsp_Stop(NULL,0);
			AudDspService_Flow_Stop(&me->super);
			UserAppMute(me, 1);
			ap_BurnTest_log("EXIT\n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			ap_BurnTest_log("INIT\n");
			AudDspService_Prepare_ADC(&me->super, AIN1);
			AudDspService_Flow_Play(&me->super);
			setHdmiMode(0);
			UserAppMute(me, 0);
			//UserAppCallHint(me, eHint_PowerOn);
			
			QTimeEvt_disarmX(&me->timeSrcHintEvt);
			QTimeEvt_armX(&me->timeSrcHintEvt, (TICKS_PER_500MS*3), 0);
			
			status = Q_HANDLED();
			break;
		}


		case IOP_KEY_SIG:
		{
			stUserKeyInfo_t stUserKeyInfo;
			int result = 0;
			result = UserAppGetLatestKey_SpecificSrc(eKeySrc_CEC_CMD, me->hKeyBuf, &stUserKeyInfo.stKeyInfo); // get the latest key in key buf but prioritize the key from specific src

			if (result == eRes_specific1) //key buf is not empty
			{
				ap_printf("Key eRes_specific1\n");
				QF_PUBLISH(Q_NEW(QEvt, IOP_KEY_SIG), me); // re-get key
			}

			status = Q_HANDLED();
			break;
		}

		case SRC_HINT_TIME_SIG://SCROLL_BACK_TIMER_SIG:
		{
		
			
			AudDspService_Set_AppMode(&me->super, SET_MMIX, GRP_MMIX0, MODE1);
			
		    	ap_printf("----HINT_END_TIME_SIG\n");
			SrcNameHint(me);
			status = UserAppTranSrc(me, me->aUserSrc_tbl[me->iSrc]); // ?????��???
			break;
		}
		
		default:
		{
			status = Q_SUPER(&UserApp_active);
			break;
		}
	}

	return status;
}


// Setting for vim, please keep these 2 lines at end of the file.
// vim: shiftwidth=4 softtabstop=4 tabstop=4 noexpandtab
