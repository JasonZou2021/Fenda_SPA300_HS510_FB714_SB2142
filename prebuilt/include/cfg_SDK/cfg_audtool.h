/******************************************************************************/
/**
*
* \file	    cfg_audtool.h
*
* \brief	SDK configuration for dsp tool ctrl.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author    chungyao.tsai@sunplus.com
* \version	 v0.01
* \date 	 2016/10/13
******************************************************************************/
#ifndef CFG_AUD_H
#define CFG_AUD_H

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include "types.h"

/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/
typedef struct
{
	UINT8 Repeater;
	UINT8 Optical;
	UINT8 Coaxial;
} CFG_SPDIF_T;

typedef struct
{
	UINT8 CLOCK_I2S_OUT;
	UINT8 MODE;
	UINT8 LRCK_CYCLE;
	UINT8 BCK_SYNC_EDGE;
	UINT8 DATA_WIDTH;
	UINT8 DATA_PARITY;
	UINT8 DATA_DELAY;
	UINT8 CLOCK_EXT_ADC;
	UINT8 MODE_EXT_ADC;
	UINT8 LRCK_CYCLE_EXT_ADC;
	UINT8 BCK_SYNC_EDGE_EXT_ADC;
	UINT8 DATA_WIDTH_EXT_ADC;
	UINT8 DATA_PARITY_EXT_ADC;
	UINT8 DATA_DELAY_EXT_ADC;
	UINT8 CLOCK_HDMI_IN;
	UINT8 MODE_HDMI_IN;
	UINT8 LRCK_CYCLE_HDMI_IN;
	UINT8 BCK_SYNC_EDGE_HDMI_IN;
	UINT8 DATA_WIDTH_HDMI_IN;
	UINT8 DATA_PARITY_HDMI_IN;
	UINT8 DATA_DELAY_HDMI_IN;
} CFG_I2S_T;

typedef struct
{
	UINT8 CH1[3];
	UINT8 CH2[3];
	UINT8 CH3[3];
} CFG_PWM_T;

typedef struct
{
	UINT8 OutMode;
	UINT8 OutLfe;
	UINT8 CompMode;
	UINT8 StereoMode;
} CFG_DDP_T;


typedef struct
{
	UINT8 OutMode;
	UINT8 StereoMode;
} CFG_DTS_T;

 typedef struct
 {
	UINT8 Select;
	UINT8 AppAutoCtrlEnable;
	UINT8 App_config[12][5];
} CFG_AUDIOFLOW_T;

typedef struct
{
    UINT32 DSP_SUPPORT;
	UINT8 AUDIO_FLOW;
	UINT8 Channel_Map[16];
	CFG_I2S_T I2S_IN;
	CFG_I2S_T I2S_OUT;
	CFG_PWM_T PWM_OUT;
	UINT8 DSP_Mode;
	UINT8 KOK_ADC;
	UINT8 DSP_Downmix;
	CFG_DDP_T DDPCfg;
	CFG_DTS_T DTSCfg;
} CFG_AUD_T;
#endif

