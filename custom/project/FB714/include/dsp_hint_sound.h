/**************************************************************************/
/**
*
* \file	dsp_hint_sound.h
*
* \brief
*
* \note  	Copyright (c) 2016 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author
* \version	 v0.01
* \date 	 2016/11/10
***************************************************************************/
/*---------------------------------------------------------------------------*
*                            INCLUDE   DECLARATIONS                         *
*---------------------------------------------------------------------------*/
#ifndef DSP_HINT_SOUND_H
#define DSP_HINT_SOUND_H

#include <stdio.h>
#include "types.h"
#include "spXeffect_api.h"

#define DEFAULT_DUCK_VOL -90
#define DEFAULT_XEF_VOL -15
#define DEFAULT_L_OUT_CH XEFFECT_L_CH
#define DEFAULT_R_OUT_CH XEFFECT_R_CH
#define L_ALL_CH XEFFECT_L_CH | XEFFECT_LS_CH | XEFFECT_LRS_CH | XEFFECT_C_CH
#define R_ALL_CH XEFFECT_R_CH | XEFFECT_RS_CH | XEFFECT_RRS_CH | XEFFECT_SUB_CH

#define DEFAULT_INTEVAL_TIME 0	//unit is 50ms

#define SUPPORT_SPEAKER_TEST 1
#define SUPPORT_FW_VERSION 1
#define SUPPORT_SRC_NAME 1
#define SUPPORT_POWER_HINT 1
#define SUPPORT_INFO 1
#define SUPPORT_BT_SEARCH 1
#define SUPPORT_DETECT_SEARCH 1
#define SUPPORT_EQ_TYPE 1

#define POWER_ON_SRC_HINT 0

typedef struct
{
	const BYTE *HINT_DATA;
	const UINT32 SIZE; // bytes
	const int duck_vol;
	const int xef_vol;
	const unsigned short L_to_out_ch_num;
	const unsigned short R_to_out_ch_num;
} DSP_HINT_SOUND_P;

typedef struct Comb_Hint
{
	DSP_HINT_SOUND_P *pCurHint;
	int interval_time;	//The interval time between each hint
	struct Comb_Hint *next;
}Comb_Hint_t;

typedef enum
{
	eHintData_BtConnect,

	eHintData_BtDisconnect,

	eHintData_BtSearching,

	eHintData_DetectSearching,

	eHintData_SrcUSB,

	eHintData_SrcCard,

	eHintData_SrcAux,

	eHintData_SrcLine,

	eHintData_SrcI2S,

	eHintData_SrcOpt,

	eHintData_SrcCoax,

	eHintData_SrcARC,

	eHintData_SrceARC,

	eHintData_SrcHDMI0,

	eHintData_SrcHDMI1,

	eHintData_SrcHDMI2,

	eHintData_SrcBT,

	eHintData_Left,

	eHintData_Right,

	eHintData_Dot,

	eHintData_Zero,

	eHintData_One,

	eHintData_Two,

	eHintData_Three,

	eHintData_Four,

	eHintData_Five,

	eHintData_Six,

	eHintData_Seven,

	eHintData_Eight,

	eHintData_Nine,

	eHintData_DolbyDigital,

	eHintData_DolbyAtmos,

	eHintData_DolbyAudio,

	eHintData_DolbyDigitalPlus,

	eHintData_DolbyMAT,

	eHintData_DolbyTrueHD,

	eHintData_DTSDigitalSurr,

	eHintData_Stereo,

	eHintData_PCM,

	eHintData_EQMovie,

	eHintData_EQMusic,

	eHintData_EQGame,

	eHintData_PowerOn,

	eHintData_PowerOff,

	eHintData_VolMax,

	eHintData_MAX,
}eHint_Data_ID;

typedef enum
{
	eHint_None,

	/* important!!! below is single hint data hint type */

	eHint_BtReady,

	eHint_BtConnect,

	eHint_BtDisconnect,

	eHint_BtSearching,

	eHint_DetectSearching,

	eHint_SrcUSB,

	eHint_SrcCard,

	eHint_SrcAux,

	eHint_SrcLine,

	eHint_SrcI2S,

	eHint_SrcOpt,

	eHint_SrcCoax,

	eHint_SrcARC,

	eHint_SrceARC,

	eHint_SrcHDMI0,

	eHint_SrcHDMI1,

	eHint_SrcHDMI2,

	eHint_SrcBT,

	eHint_EQ_Min,	//EQ Type start

	eHint_EQMovie,

	eHint_EQMusic,

	eHint_EQGame,

	eHint_PowerOn,

	eHint_PowerOff,

	eHint_VolMax,

	eHint_EQ_Max,	//EQ Type end

	eHint_Single_Max,	//single hint type max

	/* important!!! below is combination hint data hint type */

	eHint_Speaker,

	eHint_FwVersion,

	eHint_Info,

	eHint_Demo1,

	eHint_Demo2,

	eHint_Demo3,

	eHint_Comb_Max,		//combination hint type max
} eHint_Type;

//extern DSP_HINT_SOUND_P HINT_TONE;
//extern DSP_HINT_SOUND_P HINT_BT_CONNECT_OK;
//extern DSP_HINT_SOUND_P HINT_BT_READY_OK;
//extern DSP_HINT_SOUND_P HINT_Dobly;

extern DSP_HINT_SOUND_P HINT_BT_CONNECT;
extern DSP_HINT_SOUND_P HINT_BT_DISCONNECT;
extern DSP_HINT_SOUND_P HINT_BT_SEARCHING;
extern DSP_HINT_SOUND_P HINT_DETECT_SEARCHING;
extern DSP_HINT_SOUND_P HINT_SRC_USB;
extern DSP_HINT_SOUND_P HINT_SRC_CARD;
extern DSP_HINT_SOUND_P HINT_SRC_I2S;
extern DSP_HINT_SOUND_P HINT_SRC_AUX;
extern DSP_HINT_SOUND_P HINT_SRC_OPT;
extern DSP_HINT_SOUND_P HINT_SRC_ARC;
extern DSP_HINT_SOUND_P HINT_LEFT;
extern DSP_HINT_SOUND_P HINT_RIGHT;
extern DSP_HINT_SOUND_P HINT_DOT;
extern DSP_HINT_SOUND_P HINT_NUM_ZERO;
extern DSP_HINT_SOUND_P HINT_NUM_ONE;
extern DSP_HINT_SOUND_P HINT_NUM_TWO;
extern DSP_HINT_SOUND_P HINT_NUM_THREE;
extern DSP_HINT_SOUND_P HINT_NUM_FOUR;
extern DSP_HINT_SOUND_P HINT_NUM_FIVE;
extern DSP_HINT_SOUND_P HINT_NUM_SIX;
extern DSP_HINT_SOUND_P HINT_NUM_SEVEN;
extern DSP_HINT_SOUND_P HINT_NUM_EIGHT;
extern DSP_HINT_SOUND_P HINT_NUM_NINE;
extern DSP_HINT_SOUND_P HINT_DOLYBY;
extern DSP_HINT_SOUND_P HINT_DTS;
extern DSP_HINT_SOUND_P HINT_STEREO;
extern DSP_HINT_SOUND_P *hint_data_map[eHintData_MAX];

int hintDataAdd(eHint_Type Hint_Type);
void hintEndProcess();
int judgeNextSrcData();
DSP_HINT_SOUND_P * getNextSrcData();
char *getFwVersion();
void HintSrcAdd(Comb_Hint_t *add_data);
int Hint_SingleDataAdd(DSP_HINT_SOUND_P *single_data, int interval_time);
int FwVersionProc();
int getCurHintInterval();
int InfoProc();
int getCurUserSrc();
UINT32 getCurCodecType();
UINT16 getAudioFormat();

#endif

