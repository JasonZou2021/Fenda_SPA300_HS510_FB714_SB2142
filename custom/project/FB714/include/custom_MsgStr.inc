/******************************************************************************/
/**
*
* \file	 custom_sig.h
*
* \brief	 signals for customer.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author       Daniel.yeh@sunplus.com
*\version	 v0.01
*\date 	 2016/11/07
******************************************************************************/
#ifndef __CUSTOM_STR_H__
#define __CUSTOM_STR_H__

#include "ir_key.h"

char *KeySigTable[] =
{
	"POWER_SIG",
	"POWER_RELEASE_SIG",
	"POWER_LONG_PRESS_SIG",
	"POWER_LONG_RELEASE_SIG",
	"POWER_TICK_SIG",


	"PLAY_SIG",                             //eKeyPress
	"PLAY_RELEASE_SIG",             //eKeyRelease
	"PLAY_LONG_PRESS_SIG",      //eKeyLongPress
	"PLAY_LONG_RELEASE_SIG",  //eKeyLongRelease
	"PLAY_TICK_SIG",                   //eKeyLongTick


	"STOP_SIG",
	"STOP_RELEASE_SIG",
	"STOP_LONG_PRESS_SIG",
	"STOP_LONG_RELEASE_SIG",
	"STOP_TICK_SIG",

	"PAUSE_SIG",
	"PAUSE_RELEASE_SIG",
	"PAUSE_LONG_PRESS_SIG",
	"PAUSE_LONG_RELEASE_SIG",
	"PAUSE_TICK_SIG",

	"SOURCE_SW_SIG",
	"SOURCE_SW_RELEASE_SIG",
	"SOURCE_SW_LONG_PRESS_SIG",
	"SOURCE_SW_LONG_RELEASE_SIG",
	"SOURCE_SW_TICK_SIG",

	"VOL_UP_SIG",
	"VOL_UP_RELEASE_SIG",
	"VOL_UP_LONG_PRESS_SIG",
	"VOL_UP_LONG_RELEASE_SIG",
	"VOL_UP_TICK_SIG",

	"VOL_DOWN_SIG",
	"VOL_DOWN_RELEASE_SIG",
	"VOL_DOWN_LONG_PRESS_SIG",
	"VOL_DOWN_LONG_RELEASE_SIG",
	"VOL_DOWN_TICK_SIG",

	"MUTE_SIG",//204
	"MUTE_RELEASE_SIG",//205
	"MUTE_LONG_PRESS_SIG",
	"MUTE_LONG_RELEASE_SIG",
	"MUTE_TICK_SIG",

	"FF_SIG",
	"FF_RELEASE_SIG",
	"FF_LONG_PRESS_SIG",
	"FF_LONG_RELEASE_SIG",
	"FF_TICK_SIG",

	"FB_SIG",
	"FB_RELEASE_SIG",
	"FB_LONG_PRESS_SIG",
	"FB_LONG_RELEASE_SIG",
	"FB_TICK_SIG",

	"NEXT_SIG",
	"NEXT_RELEASE_SIG",
	"NEXT_LONG_PRESS_SIG",
	"NEXT_LONG_RELEASE_SIG",
	"NEXT_TICK_SIG",

	"PRE_SIG",
	"PRE_RELEASE_SIG",
	"PRE_LONG_PRESS_SIG",
	"PRE_LONG_RELEASE_SIG",
	"PRE_TICK_SIG",

	"REPEAT_SIG",
	"REPEAT_RELEASE_SIG",
	"REPEAT_LONG_PRESS_SIG",
	"REPEAT_LONG_RELEASE_SIG",
	"REPEAT_TICK_SIG",

	"SHUFFLE_SIG",
	"SHUFFLE_RELEASE_SIG",
	"SHUFFLE_LONG_PRESS_SIG",
	"SHUFFLE_LONG_RELEASE_SIG",
	"SHUFFLE_TICK_SIG",

	"SELECT_SIG",
	"SELECT_RELEASE_SIG",
	"SELECT_LONG_PRESS_SIG",
	"SELECT_LONG_RELEASE_SIG",
	"SELECT_TICK_SIG",

	"SURROUND_3D_SIG",
	"SURROUND_3D_RELEASE_SIG",
	"SURROUND_3D_LONG_PRESS_SIG",
	"SURROUND_3D_LONG_RELEASE_SIG",
	"SURROUND_3D_TICK_SIG",

	"EQ_SIG",
	"EQ_RELEASE_SIG",
	"EQ_LONG_PRESS_SIG",
	"EQ_LONG_RELEASE_SIG",
	"EQ_TICK_SIG",

	"UP_SIG",
	"UP_RELEASE_SIG",
	"UP_LONG_PRESS_SIG",
	"UP_LONG_RELEASE_SIG",
	"UP_TICK_SIG",

	"DOWN_SIG",
	"DOWN_RELEASE_SIG",
	"DOWN_LONG_PRESS_SIG",
	"DOWN_LONG_RELEASE_SIG",
	"DOWN_TICK_SIG",

	"PROGRAM_SIG",
	"PROGRAM_RELEASE_SIG",
	"PROGRAM_LONG_PRESS_SIG",
	"PROGRAM_LONG_RELEASE_SIG",
	"PROGRAM_TICK_SIG",

	"ZERO_SIG",
	"ZERO_RELEASE_SIG",
	"ZERO_LONG_PRESS_SIG",
	"ZERO_LONG_RELEASE_SIG",
	"ZERO_TICK_SIG",

	"ONE_SIG",
	"ONE_RELEASE_SIG",
	"ONE_LONG_PRESS_SIG",
	"ONE_LONG_RELEASE_SIG",
	"ONE_TICK_SIG",

	"TWO_SIG",
	"TWO_RELEASE_SIG",
	"TWO_LONG_PRESS_SIG",
	"TWO_LONG_RELEASE_SIG",
	"TWO_TICK_SIG",

	"THREE_SIG",
	"THREE_RELEASE_SIG",
	"THREE_LONG_PRESS_SIG",
	"THREE_LONG_RELEASE_SIG",
	"THREE_TICK_SIG",

	"FOUR_SIG",
	"FOUR_RELEASE_SIG",
	"FOUR_LONG_PRESS_SIG",
	"FOUR_LONG_RELEASE_SIG",
	"FOUR_TICK_SIG",

	"FIVE_SIG",
	"FIVE_RELEASE_SIG",
	"FIVE_LONG_PRESS_SIG",
	"FIVE_LONG_RELEASE_SIG",
	"FIVE_TICK_SIG",

	"SIX_SIG",
	"SIX_RELEASE_SIG",
	"SIX_LONG_PRESS_SIG",
	"SIX_LONG_RELEASE_SIG",
	"SIX_TICK_SIG",

	"SEVEN_SIG",		//"KOK_ECHO_UP_SIG",
	"SEVEN_RELEASE_SIG",	//"KOK_ECHO_UP_RELEASE_SIG",
	"SEVEN_LONG_PRESS_SIG",	//"KOK_ECHO_UP_LONG_PRESS_SIG",
	"SEVEN_LONG_RELEASE_SIG",	//"KOK_ECHO_UP_LONG_RELEASE_SIG",
	"SEVEN_TICK_SIG",		//"KOK_ECHO_UP_TICK_SIG",

	"EIGHT_SIG",
	"EIGHT_RELEASE_SIG",
	"EIGHT_LONG_PRESS_SIG",
	"EIGHT_LONG_RELEASE_SIG",
	"EIGHT_TICK_SIG",

	"NINE_SIG", 		//"KOK_KEY_SHIFT_UP_SIG",
	"NINE_RELEASE_SIG",		//"KOK_KEY_SHIFT_UP_RELEASE_SIG",
	"NINE_LONG_PRESS_SIG",	//"KOK_KEY_SHIFT_UP_LONG_PRESS_SIG",
	"NINE_LONG_RELEASE_SIG",	//"KOK_KEY_SHIFT_UP_LONG_RELEASE_SIG",
	"NINE_TICK_SIG",    	//"KOK_KEY_SHIFT_UP_TICK_SIG",

	"KOK_ECHO_DN_SIG",
	"KOK_ECHO_DN_RELEASE_SIG",
	"KOK_ECHO_DN_LONG_PRESS_SIG",
	"KOK_ECHO_DN_LONG_RELEASE_SIG",
	"KOK_ECHO_DN_TICK_SIG",

	"WSS_BT_PAIR_SIG",
	"WSS_BT_PAIR_RELEASE_SIG",
	"WSS_BT_PAIR_LONG_PRESS_SIG",
	"WSS_BT_PAIR_LONG_RELEASE_SIG",
	"WSS_BT_PAIR_TICK_SIG",


	"BT_START_SEARCH_SIG",
	"BT_START_SEARCH_RELEASE_SIG",
	"BT_START_SEARCH_LONG_PRESS_SIG",
	"BT_START_SEARCH_LONG_RELEASE_SIG",
	"BT_START_SEARCH_TICK_SIG",

	"BT_TWS_MODE_SWITCH_SIG",
	"BT_TWS_MODE_SWITCH_RELEASE_SIG",
	"BT_TWS_MODE_SWITCH_LONG_PRESS_SIG",
	"BT_TWS_MODE_SWITCH_LONG_RELEASE_SIG",
	"BT_TWS_MODE_SWITCH_TICK_SIG",

	"BT_SOURCE_DISCONNECT_SIG",
	"BT_SOURCE_DISCONNECT_RELEASE_SIG",
	"BT_SOURCE_DISCONNECT_LONG_PRESS_SIG",
	"BT_SOURCE_DISCONNECT_LONG_RELEASE_SIG",
	"BT_SOURCE_DISCONNECT_TICK_SIG",

	"WOOFER_VOL_UP_SIG",
	"WOOFER_VOL_UP_RELEASE_SIG",
	"WOOFER_VOL_UP_LONG_PRESS_SIG",
	"WOOFER_VOL_UP_LONG_RELEASE_SIG",
	"WOOFER_VOL_UP_TICK_SIG",

	"WOOFER_VOL_DOWN_SIG",
	"WOOFER_VOL_DOWN_RELEASE_SIG",
	"WOOFER_VOL_DOWN_LONG_PRESS_SIG",
	"WOOFER_VOL_DOWN_LONG_RELEASE_SIG",
	"WOOFER_VOL_DOWN_TICK_SIG",

	"KOK_BGM_VOL_UP_SIG",
	"KOK_BGM_VOL_UP_RELEASE_SIG",
	"KOK_BGM_VOL_UP_LONG_PRESS_SIG",
	"KOK_BGM_VOL_UP_LONG_RELEASE_SIG",
	"KOK_BGM_VOL_UP_TICK_SIG",

	"KOK_BGM_VOL_DN_SIG",
	"KOK_BGM_VOL_DN_RELEASE_SIG",
	"KOK_BGM_VOL_DN_LONG_PRESS_SIG",
	"KOK_BGM_VOL_DN_LONG_RELEASE_SIG",
	"KOK_BGM_VOL_DN_TICK_SIG",

	"KOK_MIC1_VOL_UP_SIG",
	"KOK_MIC1_VOL_UP_RELEASE_SIG",
	"KOK_MIC1_VOL_UP_LONG_PRESS_SIG",
	"KOK_MIC1_VOL_UP_LONG_RELEASE_SIG",
	"KOK_MIC1_VOL_UP_TICK_SIG",

	"KOK_MIC1_VOL_DN_SIG",
	"KOK_MIC1_VOL_DN_RELEASE_SIG",
	"KOK_MIC1_VOL_DN_LONG_PRESS_SIG",
	"KOK_MIC1_VOL_DN_LONG_RELEASE_SIG",
	"KOK_MIC1_VOL_DN_TICK_SIG",

	"KOK_MIC2_VOL_UP_SIG",
	"KOK_MIC2_VOL_UP_RELEASE_SIG",
	"KOK_MIC2_VOL_UP_LONG_PRESS_SIG",
	"KOK_MIC2_VOL_UP_LONG_RELEASE_SIG",
	"KOK_MIC2_VOL_UP_TICK_SIG",

	"KOK_MIC2_VOL_DN_SIG",
	"KOK_MIC2_VOL_DN_RELEASE_SIG",
	"KOK_MIC2_VOL_DN_LONG_PRESS_SIG",
	"KOK_MIC2_VOL_DN_LONG_RELEASE_SIG",
	"KOK_MIC2_VOL_DN_TICK_SIG",

	"KOK_KEY_SHIFT_DN_SIG",
	"KOK_KEY_SHIFT_DN_RELEASE_SIG",
	"KOK_KEY_SHIFT_DN_LONG_PRESS_SIG",
	"KOK_KEY_SHIFT_DN_LONG_RELEASE_SIG",
	"KOK_KEY_SHIFT_DN_TICK_SIG",

	"KOK_VC_LVL_SIG",
	"KOK_VC_LVL_RELEASE_SIG",
	"KOK_VC_LVL_LONG_PRESS_SIG",
	"KOK_VC_LVL_LONG_RELEASE_SIG",
	"KOK_VC_LVL_TICK_SIG",

	"KOK_NR_SIG",
	"KOK_NR_RELEASE_SIG",
	"KOK_NR_LONG_PRESS_SIG",
	"KOK_NR_LONG_RELEASE_SIG",
	"KOK_NR_TICK_SIG",

	"BT_SIG",
	"BT_RELEASE_SIG",
	"BT_LONG_PRESS_SIG",
	"BT_LONG_RELEASE_SIG",
	"BT_TICK_SIG",

	"USB_SIG",
	"USB_RELEASE_SIG",
	"USB_LONG_PRESS_SIG",
	"USB_LONG_RELEASE_SIG",
	"USB_TICK_SIG",

	"UPGRADE_SHOW_PERCENTAGE_SIG",

	"CUSTOM_TEST_AUTOSWITCHSRC_SIG",
	"CUSTOM_TEST_POWEROFF_SIG",

	"LED_CONST_SIG,"
	"LED_BLINK_SIG",

	"CUSTOM_HINT_START_SIG",
	"CUSTOM_HINT_END_SIG",


	"CUSTOMER_PRIVATE_SIG_MAX",
};

char *KeySrc[eKeySrc_Num] =
{
	"IR",
	"VFD Key",
	"ADC CH0 Key",
	"ADC CH1 Key",
	"ADC CH2 Key",
	"ADC CH3 Key",
	"CEC Cmd",
	"PWR Key",
	"GPIO Key",
	"IR learning",
};

char *KeyType[eKeyStateNum] =
{
	"Press",
	"Release",
	"LongPress",
	"LongRlease",
	"LongTick",
};


char *IRKeyTable[IR_CODE_MAX_NUM] =
{
	//00-0F
	"key_0",                              //IRC_0,                //0x00
	"key_1",                              //IRC_1,                //0x01
	"key_2",                              //IRC_2,                //0x02
	"key_3",                              //IRC_3,                //0x03
	"key_4",                              //IRC_4,                //0x04
	"key_5",                              //IRC_5,                //0x05
	"key_6",                              //IRC_6,                //0x06
	"key_7",                              //IRC_7,                //0x07
	"key_8",                              //IRC_8,                //0x08
	"key_9",                              //IRC_9,                //0x09
	"key_10",                             //IRC_10,               //0x0a
	"key_p10",                            //IRC_P10,              //0x0b
	"key_eject_toggle",                   //IRC_EJECT,            //0x0c
	"key_mute",                           //IRC_MUTE,             //0x0d
	"key_key_up",                         //IRC_KEY_UP,           //0x0e
	"key_key_reset",                      //IRC_KEY_RESET,        //0x0f

	//10-1F
	"key_key_down",                       //IRC_KEY_DN,           //0x10
	"key_volume_up",                      //IRC_VOLUME_UP,        //0x11
	"key_volume_down",                    //IRC_VOLUME_DN,        //0x12
	"key_slowmotion",                     //IRC_SLOW,             //0x13
	"key_step",                           //IRC_STEP,             //0x14
	"key_pause",                          //IRC_PAUSE,            //0x15
	"key_play",                           //IRC_PLAY,             //0x16
	"key_play_resume",                    //IRC_PLAY_RESUME       //0x17
	"key_return",                         //IRC_RETURN,           //0x18
	"key_stop",                           //IRC_STOP,             //0x19
	"key_null",                           //IRC_LANGUAGE,         //0x1a,(not use)
	"key_backward",                       //IRC_BACKWARD,         //0x1b
	"key_forward",                        //IRC_FORWARD,          //0x1c
	"key_prev",                           //IRC_PREV,             //0x1d
	"key_next",                           //IRC_NEXT,             //0x1e
	"key_display",                        //IRC_DISPLAY,          //0x1f

	//20-2F
	"key_repeat_ab",                      //IRC_AB,               //0x20
	"key_repeat_all",                     //IRC_REPEAT,           //0x21
	"key_pbc",                			//IRC_PBC,              //0x22
	"key_goto",                           //IRC_GOTO,             //0x23
	"key_random",                        	//IRC_RANDOM,           //0x24
	"key_resume",                         //IRC_RESUME,           //0x25
	"key_time",                     		//IRC_TIME,             //0x26
	"key_power",                          //IRC_POWER,            //0x27
	"key_zoom",                           //IRC_ZOOM,             //0x28
	"key_up",                             //IRC_UP,               //0x29
	"key_down",                           //IRC_DOWN,             //0x2a
	"key_left",                           //IRC_LEFT,             //0x2b
	"key_right",                          //IRC_RIGHT,            //0x2c
	"key_source",                         //IRC_SRC,              //0x2d,
	"key_null",                         	//IRC_FREEZE,                           //0x2e,no use
	"key_channel",                        //IRC_CHANNEL,          //0x2f

	//30-3F
	"key_channel_dn",                     //IRC_CHANNEL_DN        //0x30
	"key_effect",                        	//IRC_EFFECT,           //0x31,(not use)
	"key_preview",                        //IRC_PREVIEW,          //0x32
	"key_9frames",                        //IRC_9FRAMES,          //0x33
	"key_prog",                           //IRC_PROGRAM,          //0x34
	"key_format",          				//IRC_FORMAT,           //0x35
	"key_intro",                          //IRC_INTRO,            //0x36
	"key_null",                           //IRC_GAME_L,           //0x37
	"key_null",                           //IRC_GAME_R,           //0x38
	"key_null",                           //IRC_GAME_UP,          //0x39
	"key_null",                           //IRC_GAME_DN,          //0x3a
	"key_null",                           //IRC_GAME_TN,          //0x3b
	"key_null",                           //IRC_GAME_UL,          //0x3c
	"key_null",                           //IRC_GAME,             //0x3d,(not use)
	"key_null",                           //IRC_FOCUS,            //0x3e,(not use)
	"key_null",                           //IRC_DISC1,            //0x3f

	//40-4F
	"key_disk2",                          //IRC_DISK2,            //0x40
	"key_disk3",                          //IRC_DISK3,            //0x41
	"key_play_pause",                     //IRC_PAUSEPLAY,        //0x42
	"key_null",                           //IRC_QUALIFY           //0x43,(not use)
	"key_null",                      //IRC_RESET_DSP         //0x44
	"key_sub_picture",                    //IRC_SUBP              //0x45                                     //0x45
	"key_angle",                          //IRC_ANGLE             //0x46
	"key_audio_language",                 //IRC_AUD_LANGUAGE      //0x47
	"key_select",                         //IRC_SELECT            //0x48
	"key_title_menu",                     //IRC_TITLEMENU         //0x49
	"key_clear",                          //IRC_CLEAR             //0x4a
	"key_setup",                          //IRC_SETUP             //0x4b
	"key_karaoke",                        //IRC_KARAOKE           //0x4c
	"key_3dsound",                        //IRC_3D                //0x4d
	"key_bk_slow",                        //IRC_BK_SLOW           //0x4e
	"key_reading",                        //IRC_READING           //0x4f

	//50-5F.
	"key_menu",                      		//IRC_ROOT_MENU         //0x50
	"key_volume",                    		//IRC_VOLUME       //0x51
	"key_AUDIO",                       	//IRC_AUDIO      //0x52
	"key_play_pause",                     //IRC_PLAY_PAUSE     //0x53,(not use)
	"key_home",                           //IRC_HOME              //0x54
	"key_play_select",                          //IRC_SCORE,            //0x55
	"key_mix_func",                   //IRC_TITLE_RETURN      //0x56
	"key_slow_bak_ford",                        //IRC_ECHO_UP           //0x57
	"key_book",                      //IRC_ECHO_DOWN         //0x58
	"key_score",                         //IRC_MIC_UP            //0x59
	"key_title_return",                       //IRC_MIC_DOWN          //0x5a
	"key_echo_up",                         //IRC_VIDEO_OUT,        //0x5b
	"key_echo_down",                             //IRC_NULL,             //0x5c
	"key_mic_up",                             //IRC_NULL,             //0x5d
	"key_mic_down",                     //IRC_NULL,             //0x5e
	"key_null",                             //IRC_NULL,             //0x5f

	// 60-IR_CODE_MAX_NUM
	"key_null",                         //IRC_NULL,             	//0x60,no use
	"key_null",                         //IRC_NULL,             	//0x61,no use
	"key_null",                         //IRC_NULL,             	//0x62,no use
	"key_null",                         //IRC_NULL,             	//0x63,no use
	"key_null",                         //IRC_NULL,             	//0x64,no use
	"key_null",                         //IRC_NULL,             	//0x65,no use
	"key_null",                         //IRC_NULL,             	//0x66,no use
	"key_option",                         //IRC_NULL,             	//0x67,no use
	"key_subtitle",                         //IRC_NULL,             	//0x68,no use
	"key_kok_rec",                         //IRC_NULL,             	//0x69,no use
	"key_null",                         //IRC_NULL,             	//0x6a,no use
	"key_null",                         //IRC_NULL,             	//0x6b,no use
	"key_null",                         //IRC_NULL,             	//0x6c,no use
	"key_null",                         //IRC_NULL,             	//0x6d,no use
	"key_null",                         //IRC_NULL,             	//0x6e,no use
	"key_delete",                         //IRC_DELETE,             	//0x6f
	"key_encode",                         //IRC_ENCODE,             	//0x70

	"key_pn",                           //IRC_PN,                    //0x71
	"key_prog",                         //IRC_PROG,                  //0x72
	"key_preview",                      //IRC_PREVIEW,               //0x73
	"key_radio",                        //IRC_TV_RADIO,              //0x74
	"key_mixer",                        //IRC_MIXER,                 //0x75
	"key_ripping",                      //IRC_RIPPING,               //0x76
	"key_rec",                          //IRC_REC,                   //0x77
	"key_hd",                           //IRC_HD,                    //0x78
	"key_BT_SOURCE_DISCONNECT",			//IRC_BT_SOURCE_DISCONNECT,	 //0x79
	"key_WSS_BT_PAIR",					//IRC_WSS_BT_PAIR,	 		 //0x7a
	"key_BT",
	"key_USB",
	"key_woofer_vol_up",
	"key_woofer_vol_down",
};

#endif // __CUSTOM_SIG_H__

