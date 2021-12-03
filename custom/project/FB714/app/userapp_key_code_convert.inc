/**************************************************************************/
/**
*
* \file	    userapp_key_code_convert.inc
*
* \brief
*
* \note  	Copyright (c) 2016 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author
* \version	 v0.01
* \date 	 2016/08/23
***************************************************************************/
/*---------------------------------------------------------------------------*
*                            INCLUDE   DECLARATIONS                         *
*---------------------------------------------------------------------------*/
#ifndef USERAPP_KEY_CODE_CONVERT_INC
#define USERAPP_KEY_CODE_CONVERT_INC

#include "ir_key.h"
#include "cec_user_ctl.h"

//IR key table selection
//#define SUNPLUS_IR501
//#define SUNPLUS_IR401
//#define SUNPLUS_IR400
//#define SUNPLUS_IR300
//#define SUNPLUS_IR200
//#define SUNPLUS_IR200S

#define CUST_IR

#if defined(IR_PHILIPS_EASTECH)
	#include "IRmap/ircode_philips_eastech.inc"
#elif defined(SUNPLUS_IR400)
	#include "IRmap/ircode_sunplus_IR400.inc"
#elif defined(SUNPLUS_IR300)
	#include "IRmap/ircode_sunplus_IR300.inc"
#elif defined(SUNPLUS_IR200)
	#include "IRmap/ircode_sunplus_IR200.inc"
#elif defined(SUNPLUS_IR200S)
	#include "IRmap/ircode_sunplus_IR200S.inc"
#elif defined(SUNPLUS_IR401)
	#include "IRmap/ircode_sunplus_IR401.inc"
#elif defined(SUNPLUS_IR501)
	#include "IRmap/ircode_sunplus_IR501.inc"
#elif defined(CUST_IR)
	#include "IRmap/ircode_fenda.inc"
#else
	#error: "no IR table"
#endif

enum eKeyCode
{
	eKey_Power,
	eKey_Src,
	eKey_Play,
	eKey_Pause,
	eKey_Stop,
	eKey_FF,
	eKey_FB,
	eKey_Next,
	eKey_Pre,
	eKey_Mute,
	eKey_Vol_Up,
	eKey_Vol_Down,
	eKey_Repeat,
	eKey_Shuffle,
	ekey_Select,
	eKey_Play2,
	eKey_SURROUND_3D,
	eKey_EQ,
	eKey_Up,
	eKey_Down,
	ekey_Program,
	ekey_0,
	ekey_1,
	ekey_2,
	ekey_3,
	ekey_4,
	ekey_5,
	ekey_6,
	ekey_7,
	ekey_8,
	ekey_9,
	ekey_10,
	ekey_SubP,
	ekey_language,
	ekey_source_disconnect,
	ekey_goto,
	ekey_pn,
	ekey_prog,
	ekey_preview,
	ekey_tv_radio,
	ekey_mixer,
	ekey_ripping,
	ekey_rec,
	ekey_hd,
	ekey_BT,
	ekey_USB,
	ekey_woofer_vol_up,
	ekey_woofer_vol_down,

    	ekey_MUTE_ON,
   	ekey_MUTE_OFF,
    	ekey_POWER_OFF,
    	ekey_POWER_ON,
	ekey_dolby,
	ekey_return,
	ekey_menu,

	ekey_opt,
	ekey_aux,
	ekey_arc,
	ekey_hdmi,
	ekey_bass_up,
	ekey_bass_dn,
	ekey_treble_up,
	ekey_treble_dn,
	
	eKey_Num,


};

//ir key we support
const int gKeyCodeNum = eKey_Num;
char aIRKeyCodeTable[eKey_Num] =
{
	IRC_POWER,
	IRC_SRC,
	IRC_PLAY_PAUSE,
	IRC_PAUSE,
	IRC_STOP,
	IRC_FORWARD,
	IRC_BACKWARD,
	IRC_NEXT,
	IRC_PREV,
	IRC_MUTE,
	IRC_VOLUME_UP,
	IRC_VOLUME_DN,
	IRC_REPEAT,
	IRC_SLOW,
	IRC_SELECT,
	IRC_PLAY,
	IRC_3D,
	IRC_EFFECT,
	IRC_UP,
	IRC_DOWN,
	IRC_PROGRAM,
	IRC_0,
	IRC_1,
	IRC_2,
	IRC_3,
	IRC_4,
	IRC_5,
	IRC_6,
	IRC_7,
	IRC_8,
	IRC_9,
	IRC_10,
	IRC_SUBP,
	IRC_AUD_LANGUAGE,
	IRC_BT_SOURCE_DISCONNECT,
	IRC_GOTO,
	IRC_PN,
	IRC_PROG,
	IRC_PREVIEW,
	IRC_TV_RADIO,
	IRC_MIXER,
	IRC_RIPPING,
	IRC_REC,
	IRC_HD,
	IRC_BT,
	IRC_USB,
	IRC_WOOFER_VOL_UP,
	IRC_WOOFER_VOL_DOWN,

	IRC_MUTE_ON,
   	IRC_MUTE_OFF,
    	IRC_POWER_OFF,
    	IRC_POWER_ON,
	IRC_DOLBY,
	IRC_RETURN,
	IRC_MENU,

	IRC_OPTICAL,
	IRC_AUX,
	IRC_ARC,
	IRC_HDMI,

	IRC_BASS_UP,
	IRC_BASS_DN,
	IRC_TREBLE_UP,
	IRC_TREBLE_DN,
};

//ir key code convert to QP signal
int aKeyCode2SigTable[eKey_Num] =
{
	POWER_SIG,//0
	SOURCE_SW_SIG,// 1
	PLAY_SIG, // 2
	PAUSE_SIG, // 3
	STOP_SIG, // 4
	FF_SIG, //5
	FB_SIG, // 6
	NEXT_SIG, //7
	PRE_SIG, // 8
	MUTE_SIG, // 9
	VOL_UP_SIG, // A
	VOL_DOWN_SIG, // B
	REPEAT_SIG,
	SHUFFLE_SIG,
	SELECT_SIG,
	PLAY_SIG,
	SURROUND_3D_SIG,
	EQ_SIG,
	UP_SIG,
	DOWN_SIG,
	PROGRAM_SIG,
	ZERO_SIG,
	ONE_SIG,
	TWO_SIG,
	THREE_SIG,
	FOUR_SIG,
	FIVE_SIG,
	SIX_SIG,
	SEVEN_SIG,   //KOK_ECHO_UP_SIG,
	EIGHT_SIG,
	NINE_SIG,    //KOK_KEY_SHIFT_UP_SIG,
	KOK_ECHO_DN_SIG,
	BT_START_SEARCH_SIG,
	BT_TWS_MODE_SWITCH_SIG,
	BT_SOURCE_DISCONNECT_SIG,
	KOK_KEY_SHIFT_DN_SIG,
	KOK_BGM_VOL_UP_SIG,
	KOK_BGM_VOL_DN_SIG,
	KOK_MIC1_VOL_UP_SIG,
	KOK_MIC1_VOL_DN_SIG,
	KOK_MIC2_VOL_UP_SIG,
	KOK_MIC2_VOL_DN_SIG,
	KOK_VC_LVL_SIG,
	KOK_NR_SIG,
	BT_SIG,
	USB_SIG,
	WOOFER_VOL_UP_SIG,
	WOOFER_VOL_DOWN_SIG,

	CEC_MUTE_ON_SIG,
   	CEC_MUTE_OFF_SIG,
    	CEC_POWER_OFF_SIG,
    	CEC_POWER_ON_SIG,
	DOLBY_SIG,
	RETURN_SIG,
	MENU_SIG,

	OPT_SIG,
	AUX_SIG,
	ARC_SIG,
	HDMI_SIG,
	BASS_UP_SIG,
	BASS_DN_SIG,
	TREBLE_UP_SIG,
	TREBLE_DN_SIG,
};

const UINT8 IR_Learning_Key_Tbl[IR_L_KEY_NUM] = {IRC_POWER, IRC_VOLUME_UP, IRC_VOLUME_DN, IRC_MUTE}; //IR_L_KEY_0 is only for power key

//VFD key
#if defined (SUPPORT_SUNPLUS_PT6315_PAD)

#define VFD_KEY_NUM (16)
_IR_KEY_LIST VFD_KeyTab[VFD_KEY_NUM] =
{
	IRC_SRC, IRC_BT, IRC_USB, IRC_VOLUME_UP, IRC_PLAY_PAUSE, IRC_VOLUME_DN, IRC_PREV, IRC_NEXT,
	IRC_NULL, IRC_NULL, IRC_NULL, IRC_NULL, IRC_NULL, IRC_NULL, IRC_NULL, IRC_NULL
};

#else
#define VFD_KEY_NUM (16)
_IR_KEY_LIST VFD_KeyTab[VFD_KEY_NUM] =
{
	IRC_SRC, IRC_PROGRAM, IRC_NULL, IRC_NULL, IRC_VOLUME_UP, IRC_NULL, IRC_NULL, IRC_NULL, IRC_PLAY_PAUSE, IRC_VOLUME_DN,
	IRC_NULL, IRC_NULL, IRC_NULL, IRC_NULL, IRC_NULL, IRC_NULL
};
#endif

//SAR ADC key
#define ADC_KEY_NUM 	(3)
const BYTE ADC_keycode[ADC_KEY_NUM] =
{
	IRC_SRC, IRC_VOLUME_DN, IRC_VOLUME_UP
};

//gpio
#define GPIO_KEY_NUM 	(1)
const BYTE GPIO_KeyCode[GPIO_KEY_NUM] =
{
	IRC_POWER
};

//CEC cmd we support
//CEC_CmdCode and CEC_Cmd2KeyCode are couple
#define CEC_CMD_NUM 	(8)
const int gCECCmdNum = CEC_CMD_NUM;
const BYTE CEC_CmdCode[CEC_CMD_NUM] =
{
	USER_KEY_CMD_VOLUME_UP,
	USER_KEY_CMD_VOLUME_DN,
	USER_KEY_CMD_MUTE,
	USER_KEY_CMD_POWER_ON,
	USER_KEY_CMD_MUTE_ON,
	USER_KEY_CMD_MUTE_OFF,
	USER_KEY_CMD_POWER_OFF,
	USER_KEY_CMD_POWER,
};

const BYTE CEC_Cmd2KeyCode[CEC_CMD_NUM] =
{
    IRC_VOLUME_UP,
    IRC_VOLUME_DN,
    IRC_MUTE,
    IRC_MUTE_ON,
    IRC_MUTE_OFF,
    IRC_POWER_OFF,
    IRC_POWER_ON,
    IRC_POWER,
};


#endif

