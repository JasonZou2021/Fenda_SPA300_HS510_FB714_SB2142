/******************************************************************************/
/**
*
* \file	    amp_cfg.c
*
* \brief	configuration for amp.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   daniel.yeh@sunplus.com
* \version	 v0.01
* \date 	 2016/11/1
******************************************************************************/

#include "amp_cfg.h"
#include "amp.h"
#include "initcall.h"
#include "user_def.h"


//#define DEC_CERT

#define AMP_MAIN_GAIN_STEP (41) // for master gain
#define AMP_CH1_GAIN_STEP (41) // for ch1 gain
#define AMP_CH2_GAIN_STEP (41) // for ch2 gain
#define AMP_CH3_GAIN_STEP (41) // for ch3 gain
#define AMP_CH4_GAIN_STEP (41) // for ch4 gain
#define AMP_CH5_GAIN_STEP (41) // for ch5 gain
#define AMP_CH6_GAIN_STEP (41) // for ch6 gain
#define AMP_CH7_GAIN_STEP (41) // for ch7 gain
#define AMP_CH8_GAIN_STEP (41) // for ch8 gain

#ifndef DEC_CERT
BYTE bAmpMainGainTable[AMP_MAIN_GAIN_STEP] = {0xFF, 0x78, 0x73, 0x6e, 0x6a,
											  0x66, 0x63, 0x60, 0x5d, 0x5a, 0x58, 0x56, 0x54, 0x52, 0x50, 0x4e, 0x4c, 0x4a, 0x48,
											  0x46, 0x44, 0x43, 0x42, 0x41, 0x40, 0x3f, 0x3e, 0x3d, 0x3c, 0x3b, 0x3a, 0x39, 0x38, 0x37, 0x36,
											  0x35, 0x34, 0x33, 0x32, 0x31, 0x30,
											 };

#else
BYTE bAmpMainGainTable[AMP_MAIN_GAIN_STEP] = {0xFF, 0x35, 0x34, 0x33, 0x32,
											  0x31, 0x30, 0x2f, 0x2e, 0x2d, 0x2c, 0x2b, 0x2a, 0x29, 0x28, 0x27, 0x26, 0x25, 0x24,
											  0x23, 0x22, 0x21, 0x20, 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x17, 0x16, 0x15, 0x14,
											  0x13, 0x12, 0x11, 0x10, 0x0f, 0x0e,
											 };
#endif


BYTE bAmpCh1GainTable[AMP_CH1_GAIN_STEP] = {0xFF,
											0x66, 0x63, 0x60, 0x5d, 0x5a, 0x58, 0x56, 0x54, 0x52, 0x50, 0x4e, 0x4c, 0x4a, 0x48,
											0x46, 0x44, 0x43, 0x42, 0x41, 0x40, 0x3f, 0x3e, 0x3d, 0x3c, 0x3b, 0x3a, 0x39, 0x38, 0x37, 0x36,
											0x35, 0x34, 0x33, 0x32, 0x31, 0x30, 0x2f, 0x2e, 0x2d, 0x2c,
										   };

BYTE bAmpCh2GainTable[AMP_CH2_GAIN_STEP] = {0xFF,
											0x66, 0x63, 0x60, 0x5d, 0x5a, 0x58, 0x56, 0x54, 0x52, 0x50, 0x4e, 0x4c, 0x4a, 0x48,
											0x46, 0x44, 0x43, 0x42, 0x41, 0x40, 0x3f, 0x3e, 0x3d, 0x3c, 0x3b, 0x3a, 0x39, 0x38, 0x37, 0x36,
											0x35, 0x34, 0x33, 0x32, 0x31, 0x30, 0x2f, 0x2e, 0x2d, 0x2c,
										   };

BYTE bAmpCh3GainTable[AMP_CH3_GAIN_STEP] = {0xFF,
											0x66, 0x63, 0x60, 0x5d, 0x5a, 0x58, 0x56, 0x54, 0x52, 0x50, 0x4e, 0x4c, 0x4a, 0x48,
											0x46, 0x44, 0x43, 0x42, 0x41, 0x40, 0x3f, 0x3e, 0x3d, 0x3c, 0x3b, 0x3a, 0x39, 0x38, 0x37, 0x36,
											0x35, 0x34, 0x33, 0x32, 0x31, 0x30, 0x2f, 0x2e, 0x2d, 0x2c,
										   };

#ifdef ATMOS_CERT
///used for AD85050

BYTE  bAmpMainGainTable_AD85050[AMP_MAIN_GAIN_STEP] =
{
	0xFF, 0x60, 0x5e, 0x5c, 0x5a, 0x58, 0x56, 0x54, 0x52, 0x50,
	0x4e, 0x4c, 0x4a, 0x48, 0x46, 0x44, 0x42, 0x40, 0x3e, 0x3c,
	0x3a, 0x38, 0x36, 0x34, 0x32, 0x30, 0x2e, 0x2c, 0x2a, 0x28,
	0x26, 0x24, 0x22, 0x20, 0x1e, 0x1c, 0x1a, 0x18, 0x16, 0x15,
	0x14,
};
#elif defined(SPA100_DEC_CERT)
///used for AD85050
BYTE  bAmpMainGainTable_AD85050[AMP_MAIN_GAIN_STEP] =
{
	0xFF, 0x35, 0x34, 0x33, 0x32, 0x31, 0x30, 0x2f, 0x2e, 0x2d,
	0x2c, 0x2b, 0x2a, 0x29, 0x28, 0x27, 0x26, 0x25, 0x24, 0x23,
	0x22, 0x21, 0x20, 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19,
	0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 0x0f,
	0x0e,
};
#else
///used for AD85050
BYTE  bAmpMainGainTable_AD85050[AMP_MAIN_GAIN_STEP] =
{
	0xFF, 0x82, 0x7c, 0x76, 0x71, 0x6c, 0x68, 0x64, 0x60, 0x5d,
	0x5a, 0x57, 0x54, 0x51, 0x4e, 0x4b, 0x48, 0x46, 0x44, 0x42,
	0x40, 0x3e, 0x3c, 0x3a, 0x38, 0x36, 0x34, 0x32, 0x30, 0x2e,
	0x2c, 0x2a, 0x28, 0x26, 0x24, 0x22, 0x20, 0x1e, 0x1c, 0x1a,
	0x18,
};
#endif

BYTE  bAmpCh1GainTable_AD85050[AMP_CH1_GAIN_STEP] =
{
	0xFF, 0x82, 0x7c, 0x76, 0x71, 0x6c, 0x68, 0x64, 0x60, 0x5d,
	0x5a, 0x57, 0x54, 0x51, 0x4e, 0x4b, 0x48, 0x46, 0x44, 0x42,
	0x40, 0x3e, 0x3c, 0x3a, 0x38, 0x36, 0x34, 0x32, 0x30, 0x2e,
	0x2c, 0x2a, 0x28, 0x26, 0x24, 0x22, 0x20, 0x1e, 0x1c, 0x1a,
	0x18,
};
BYTE  bAmpCh2GainTable_AD85050[AMP_CH2_GAIN_STEP] =
{
	0xFF, 0x82, 0x7c, 0x76, 0x71, 0x6c, 0x68, 0x64, 0x60, 0x5d,
	0x5a, 0x57, 0x54, 0x51, 0x4e, 0x4b, 0x48, 0x46, 0x44, 0x42,
	0x40, 0x3e, 0x3c, 0x3a, 0x38, 0x36, 0x34, 0x32, 0x30, 0x2e,
	0x2c, 0x2a, 0x28, 0x26, 0x24, 0x22, 0x20, 0x1e, 0x1c, 0x1a,
	0x18,
};
BYTE  bAmpCh3GainTable_AD85050[AMP_CH3_GAIN_STEP] =
{
	0xFF, 0x82, 0x7c, 0x76, 0x71, 0x6c, 0x68, 0x64, 0x60, 0x5d,
	0x5a, 0x57, 0x54, 0x51, 0x4e, 0x4b, 0x48, 0x46, 0x44, 0x42,
	0x40, 0x3e, 0x3c, 0x3a, 0x38, 0x36, 0x34, 0x32, 0x30, 0x2e,
	0x2c, 0x2a, 0x28, 0x26, 0x24, 0x22, 0x20, 0x1e, 0x1c, 0x1a,
	0x18,
};
BYTE  bAmpCh4GainTable_AD85050[AMP_CH4_GAIN_STEP] =
{
	0xFF, 0x82, 0x7c, 0x76, 0x71, 0x6c, 0x68, 0x64, 0x60, 0x5d,
	0x5a, 0x57, 0x54, 0x51, 0x4e, 0x4b, 0x48, 0x46, 0x44, 0x42,
	0x40, 0x3e, 0x3c, 0x3a, 0x38, 0x36, 0x34, 0x32, 0x30, 0x2e,
	0x2c, 0x2a, 0x28, 0x26, 0x24, 0x22, 0x20, 0x1e, 0x1c, 0x1a,
	0x18,
};
BYTE  bAmpCh5GainTable_AD85050[AMP_CH5_GAIN_STEP] =
{
	0xFF, 0x82, 0x7c, 0x76, 0x71, 0x6c, 0x68, 0x64, 0x60, 0x5d,
	0x5a, 0x57, 0x54, 0x51, 0x4e, 0x4b, 0x48, 0x46, 0x44, 0x42,
	0x40, 0x3e, 0x3c, 0x3a, 0x38, 0x36, 0x34, 0x32, 0x30, 0x2e,
	0x2c, 0x2a, 0x28, 0x26, 0x24, 0x22, 0x20, 0x1e, 0x1c, 0x1a,
	0x18,
};
BYTE  bAmpCh6GainTable_AD85050[AMP_CH6_GAIN_STEP] =
{
	0xFF, 0x82, 0x7c, 0x76, 0x71, 0x6c, 0x68, 0x64, 0x60, 0x5d,
	0x5a, 0x57, 0x54, 0x51, 0x4e, 0x4b, 0x48, 0x46, 0x44, 0x42,
	0x40, 0x3e, 0x3c, 0x3a, 0x38, 0x36, 0x34, 0x32, 0x30, 0x2e,
	0x2c, 0x2a, 0x28, 0x26, 0x24, 0x22, 0x20, 0x1e, 0x1c, 0x1a,
	0x18,
};
BYTE  bAmpCh7GainTable_AD85050[AMP_CH7_GAIN_STEP] =
{
	0xFF, 0x82, 0x7c, 0x76, 0x71, 0x6c, 0x68, 0x64, 0x60, 0x5d,
	0x5a, 0x57, 0x54, 0x51, 0x4e, 0x4b, 0x48, 0x46, 0x44, 0x42,
	0x40, 0x3e, 0x3c, 0x3a, 0x38, 0x36, 0x34, 0x32, 0x30, 0x2e,
	0x2c, 0x2a, 0x28, 0x26, 0x24, 0x22, 0x20, 0x1e, 0x1c, 0x1a,
	0x18,
};
BYTE  bAmpCh8GainTable_AD85050[AMP_CH8_GAIN_STEP] =
{
	0xFF, 0x82, 0x7c, 0x76, 0x71, 0x6c, 0x68, 0x64, 0x60, 0x5d,
	0x5a, 0x57, 0x54, 0x51, 0x4e, 0x4b, 0x48, 0x46, 0x44, 0x42,
	0x40, 0x3e, 0x3c, 0x3a, 0x38, 0x36, 0x34, 0x32, 0x30, 0x2e,
	0x2c, 0x2a, 0x28, 0x26, 0x24, 0x22, 0x20, 0x1e, 0x1c, 0x1a,
	0x18,
};

/*---------------------------------------------------------------
These bits control both left and right channel digital volume. The
digital volume is 24 dB to -103 dB in -0.5 dB step.
00000000: +24.0 dB
00000001: +23.5 dB
........
and 00101111: +0.5 dB
00110000: 0.0 dB
00110001: -0.5 dB
.......
11111110: -103 dB
11111111: Mute
--------------------------------------------------------------*/
#ifdef Q70_DEMO
static BYTE bAmpMainGainTable_Tas5825M[AMP_MAIN_GAIN_STEP] = {

	0XFF, 0xb0, 0xa8, 0xa0,  0x9c, 0x98, 0x94, 0x90, 0x8c, 0x88,
	0x84, 0x80, 0x7c, 0x78,  0x74, 0x70, 0x6c, 0x68, 0x64, 0x60,
	0x5c, 0x58, 0x54, 0x52,  0x50, 0x4e, 0x4c, 0x4a, 0x48, 0x46,
	0x44, 0x42, 0x40, 0x3e,  0x3c, 0x3a, 0x38, 0x36, 0x34, 0x32,
	0x30,
};
#else
static BYTE bAmpMainGainTable_Tas5825M[AMP_MAIN_GAIN_STEP] = {

	0XFF, 0xCC, 0xC8, 0xC4,  0xB8, 0xB4, 0xA8, 0xA4, 0xA0, 0x9C,
	0x94, 0x92, 0x8C, 0x8A,  0x88, 0x82, 0x7C, 0x76, 0x74, 0x72,
	0x71, 0x6F, 0x6E, 0x6B,  0x67, 0x65, 0x62, 0x5F, 0x5E, 0x5C,
	0x5A, 0x58, 0x56, 0x50,  0x4E, 0x42, 0x3B, 0x38, 0x36, 0x32,
	0x30,
};
#endif

#define AMP_NUM_TAS5825M (4) // how many AMPs are using in IIS mode. only IIS amp can be controlled


/*----------Tas5825M I2C Addr---------------------------------*
ADR PIN Configuration       MSBs         User Define   LSB
0 ��   to  GND           1 0 0 1 1       0 0           R/W
1k��   to  GND           1 0 0 1 1       0 1           R/W
4.7k�� to  GND           1 0 0 1 1       1 0           R/W
15k��  to  GND           1 0 0 1 1       1 1           R/W

*----------------------------------------------*/


AmpModuleConf stAmp_Tas5825M[AMP_NUM_TAS5825M] =
{
	{eAmp1, "i2cm_hw", 0x98, -1},
	{eAmp2, "i2cm_hw", 0x9a, -1},
	{eAmp3, "i2cm_hw", 0x9c, -1},
	{eAmp4, "i2cm_hw", 0x9e, -1},
	//{eAmp5, "i2cm_hw", 0x98, -1},
};

CFG_AMP_T stAmpCfg_Tas5825M =
{
	.AmpModule = eTas5825M, // to select amp driver. check the amp driver has been inserted before selecting it. "DRIVER_INIT"
	.AmpNum = AMP_NUM_TAS5825M,
	.SignalModeAmp1 = eAmpSignal_I2S,
	.SignalModeAmp2 = eAmpSignal_I2S,
	.SignalModeAmp3 = eAmpSignal_I2S,
	.SignalModeAmp4 = eAmpSignal_I2S,
	.SignalModeAmp5 = eAmpSignal_I2S,
	.MainGainStepNum = AMP_MAIN_GAIN_STEP,
	.MainGainTbl = bAmpMainGainTable_Tas5825M,

};



#define AMP_NUM (2) // how many AMPs are using in IIS mode. only IIS amp can be controlled

//an amp group with the same amp driver
AmpModuleConf stAmp[AMP_NUM] =
{
	{eAmp1, "i2cm_hw", 0x36, -1}, // LR
	{eAmp2, "i2cm_hw", 0x34, -1},
//	{eAmp3, "i2c2_sw", 0x34, -1},
};

CFG_AMP_T stAmpCfg =
{
	.AmpModule = eTas5711, // to select amp driver. check the amp driver has been inserted before selecting it. "DRIVER_INIT"
	.AmpNum = AMP_NUM,
	.SignalModeAmp1 = eAmpSignal_ANALOG,
	.SignalModeAmp2 = eAmpSignal_ANALOG,
	.SignalModeAmp3 = eAmpSignal_ANALOG,
	.SpeakerCh = 0, // reserve: enum for each speaker cfg setting, like 2.0 ch, 2.1 ch, 6 ch...
	.MainGainStepNum = AMP_MAIN_GAIN_STEP,
	.MainGainTbl = bAmpMainGainTable,
	.Ch1GainStepNum = AMP_CH1_GAIN_STEP,
	.Ch1_GainTbl = bAmpCh1GainTable,
	.Ch2GainStepNum = AMP_CH2_GAIN_STEP,
	.Ch2_GainTbl = bAmpCh2GainTable,
	.Ch3GainStepNum = AMP_CH3_GAIN_STEP,
	.Ch3_GainTbl = bAmpCh3GainTable,
};

#define AMP_NUM_AD85050_8CH (2)
AmpModuleConf stAmp_AD85050_8CH[AMP_NUM_AD85050_8CH] =
{
	{eAmp1, "i2cm_hw", 0x62, -1},
	{eAmp2, "i2cm_hw", 0x6A, -1},
	//{eAmp3, "i2cm_hw", 0x60, -1},
	//{eAmp4, "i2cm_hw", 0xD0, -1}
};
CFG_AMP_T stAmpCfg_AD85050_8CH =
{
	.AmpModule = eAD85050, // to select amp driver. check the amp driver has been inserted before selecting it. "DRIVER_INIT"
	.AmpNum = AMP_NUM_AD85050_8CH,
	.SignalModeAmp1 = eAmpSignal_ANALOG,
	.SignalModeAmp2 = eAmpSignal_ANALOG,
	.SignalModeAmp3 = eAmpSignal_ANALOG,
	.SignalModeAmp4 = eAmpSignal_ANALOG,
	.SpeakerCh = 0, // reserve: enum for each speaker cfg setting, like 2.0 ch, 2.1 ch, 6 ch...
	.MainGainStepNum = AMP_MAIN_GAIN_STEP,
	.MainGainTbl = bAmpMainGainTable_AD85050,
	.Ch1GainStepNum = AMP_CH1_GAIN_STEP,
	.Ch1_GainTbl = bAmpCh1GainTable_AD85050,
	.Ch2GainStepNum = AMP_CH2_GAIN_STEP,
	.Ch2_GainTbl = bAmpCh2GainTable_AD85050,
	.Ch3GainStepNum = AMP_CH3_GAIN_STEP,
	.Ch3_GainTbl = bAmpCh3GainTable_AD85050,
	.Ch4GainStepNum = AMP_CH4_GAIN_STEP,
	.Ch4_GainTbl = bAmpCh4GainTable_AD85050,
};

#define AMP_NUM_AD85050_12CH (6)
AmpModuleConf stAmp_AD85050_12CH[AMP_NUM_AD85050_12CH] =
{
	{eAmp1, "i2cm_hw", 0x60, -1},
	{eAmp2, "i2cm_hw", 0x62, -1},
	{eAmp3, "i2cm_hw", 0x68, -1},
	{eAmp4, "i2c2_sw", 0x60, -1},
	{eAmp5, "i2c2_sw", 0x62, -1},
	{eAmp6, "i2c2_sw", 0x68, -1},
};
CFG_AMP_T stAmpCfg_AD85050_12CH =
{
	.AmpModule = eAD85050, // to select amp driver. check the amp driver has been inserted before selecting it. "DRIVER_INIT"
	.AmpNum = AMP_NUM_AD85050_12CH,
	.SignalModeAmp1 = eAmpSignal_I2S,
	.SignalModeAmp2 = eAmpSignal_I2S,
	.SignalModeAmp3 = eAmpSignal_I2S,
	.SignalModeAmp4 = eAmpSignal_I2S,
	.SignalModeAmp5 = eAmpSignal_I2S,
	.SignalModeAmp6 = eAmpSignal_I2S,
	.SpeakerCh = 0, // reserve: enum for each speaker cfg setting, like 2.0 ch, 2.1 ch, 6 ch...
	.MainGainStepNum = AMP_MAIN_GAIN_STEP,
	.MainGainTbl = bAmpMainGainTable_AD85050,
	.Ch1GainStepNum = AMP_CH1_GAIN_STEP,
	.Ch1_GainTbl = bAmpCh1GainTable_AD85050,
	.Ch2GainStepNum = AMP_CH2_GAIN_STEP,
	.Ch2_GainTbl = bAmpCh2GainTable_AD85050,
	.Ch3GainStepNum = AMP_CH3_GAIN_STEP,
	.Ch3_GainTbl = bAmpCh3GainTable_AD85050,
	.Ch4GainStepNum = AMP_CH4_GAIN_STEP,
	.Ch4_GainTbl = bAmpCh4GainTable_AD85050,
	.Ch5GainStepNum = AMP_CH5_GAIN_STEP,
	.Ch5_GainTbl = bAmpCh5GainTable_AD85050,
	.Ch6GainStepNum = AMP_CH6_GAIN_STEP,
	.Ch6_GainTbl = bAmpCh6GainTable_AD85050,
};

#define AMP_NUM_AD85050_16CH (8)
AmpModuleConf stAmp_AD85050_16CH[AMP_NUM_AD85050_16CH] =
{
	{eAmp1, "i2cm_hw", 0x60, -1},
	{eAmp2, "i2cm_hw", 0x62, -1},
	{eAmp3, "i2cm_hw", 0x68, -1},
	{eAmp4, "i2cm_hw", 0x6a, -1},
	{eAmp5, "i2c2_sw", 0x60, -1},
	{eAmp6, "i2c2_sw", 0x62, -1},
	{eAmp7, "i2c2_sw", 0x68, -1},
	{eAmp8, "i2c2_sw", 0x6a, -1},
};
CFG_AMP_T stAmpCfg_AD85050_16CH =
{
	.AmpModule = eAD85050, // to select amp driver. check the amp driver has been inserted before selecting it. "DRIVER_INIT"
	.AmpNum = AMP_NUM_AD85050_16CH,
	.SignalModeAmp1 = eAmpSignal_I2S,
	.SignalModeAmp2 = eAmpSignal_I2S,
	.SignalModeAmp3 = eAmpSignal_I2S,
	.SignalModeAmp4 = eAmpSignal_I2S,
	.SignalModeAmp5 = eAmpSignal_I2S,
	.SignalModeAmp6 = eAmpSignal_I2S,
	.SignalModeAmp7 = eAmpSignal_I2S,
	.SignalModeAmp8 = eAmpSignal_I2S,
	.SpeakerCh = 0, // reserve: enum for each speaker cfg setting, like 2.0 ch, 2.1 ch, 6 ch...
	.MainGainStepNum = AMP_MAIN_GAIN_STEP,
	.MainGainTbl = bAmpMainGainTable_AD85050,
	.Ch1GainStepNum = AMP_CH1_GAIN_STEP,
	.Ch1_GainTbl = bAmpCh1GainTable_AD85050,
	.Ch2GainStepNum = AMP_CH2_GAIN_STEP,
	.Ch2_GainTbl = bAmpCh2GainTable_AD85050,
	.Ch3GainStepNum = AMP_CH3_GAIN_STEP,
	.Ch3_GainTbl = bAmpCh3GainTable_AD85050,
	.Ch4GainStepNum = AMP_CH4_GAIN_STEP,
	.Ch4_GainTbl = bAmpCh4GainTable_AD85050,
	.Ch4GainStepNum = AMP_CH5_GAIN_STEP,
	.Ch4_GainTbl = bAmpCh5GainTable_AD85050,
	.Ch4GainStepNum = AMP_CH6_GAIN_STEP,
	.Ch4_GainTbl = bAmpCh6GainTable_AD85050,
	.Ch4GainStepNum = AMP_CH7_GAIN_STEP,
	.Ch4_GainTbl = bAmpCh7GainTable_AD85050,
	.Ch4GainStepNum = AMP_CH8_GAIN_STEP,
	.Ch4_GainTbl = bAmpCh8GainTable_AD85050,
};

#define AMP_NUM_PCM5242 (6) // how many AMPs are using in IIS mode. only IIS amp can be controlled

//an amp group with the same amp driver
AmpModuleConf stAmp_PCM5242[AMP_NUM_PCM5242]=
{
    {eAmp1, "i2cm_hw", 0x98, -1},
    {eAmp2, "i2cm_hw", 0x9A, -1},
    {eAmp3, "i2cm_hw", 0x9C, -1},
    {eAmp4, "i2cm_hw", 0x9E, -1},
    {eAmp5, "i2c2_sw", 0x98, -1},
    {eAmp6, "i2c2_sw", 0x9A, -1},
};

CFG_AMP_T stAmpCfg_PCM5242 =
{
    .AmpModule = ePCM5242, // to select amp driver. check the amp driver has been inserted before selecting it. "DRIVER_INIT"
    .AmpNum = AMP_NUM_PCM5242,
    .SignalModeAmp1 = eAmpSignal_I2S,
    .SignalModeAmp2 = eAmpSignal_I2S,
    .SignalModeAmp3 = eAmpSignal_I2S,
    .SignalModeAmp4 = eAmpSignal_I2S,
    .SignalModeAmp5 = eAmpSignal_I2S,
    .SignalModeAmp6 = eAmpSignal_I2S,
    .SpeakerCh = 0, // reserve: enum for each speaker cfg setting, like 2.0 ch, 2.1 ch, 6 ch...
    .MainGainStepNum = AMP_MAIN_GAIN_STEP,
    .MainGainTbl = bAmpMainGainTable,
    .Ch1GainStepNum = AMP_CH1_GAIN_STEP,
    .Ch1_GainTbl = bAmpCh1GainTable,
    .Ch2GainStepNum = AMP_CH2_GAIN_STEP,
    .Ch2_GainTbl = bAmpCh2GainTable,
    .Ch3GainStepNum = AMP_CH3_GAIN_STEP,
    .Ch3_GainTbl = bAmpCh3GainTable,
};

#define AMP_NUM_PCM5242_8CH (4) // how many AMPs are using in IIS mode. only IIS amp can be controlled

//an amp group with the same amp driver
AmpModuleConf stAmp_PCM5242_8CH[AMP_NUM_PCM5242_8CH]=
{
    {eAmp1, "i2cm_hw", 0x98, -1},
    {eAmp2, "i2cm_hw", 0x9A, -1},
    {eAmp3, "i2cm_hw", 0x9C, -1},
    {eAmp4, "i2cm_hw", 0x9E, -1},
};

CFG_AMP_T stAmpCfg_PCM5242_8CH =
{
    .AmpModule = ePCM5242, // to select amp driver. check the amp driver has been inserted before selecting it. "DRIVER_INIT"
    .AmpNum = AMP_NUM_PCM5242_8CH,
    .SignalModeAmp1 = eAmpSignal_I2S,
    .SignalModeAmp2 = eAmpSignal_I2S,
    .SignalModeAmp3 = eAmpSignal_I2S,
    .SignalModeAmp4 = eAmpSignal_I2S,
    .SpeakerCh = 0, // reserve: enum for each speaker cfg setting, like 2.0 ch, 2.1 ch, 6 ch...
    .MainGainStepNum = AMP_MAIN_GAIN_STEP,
    .MainGainTbl = bAmpMainGainTable,
    .Ch1GainStepNum = AMP_CH1_GAIN_STEP,
    .Ch1_GainTbl = bAmpCh1GainTable,
    .Ch2GainStepNum = AMP_CH2_GAIN_STEP,
    .Ch2_GainTbl = bAmpCh2GainTable,
    .Ch3GainStepNum = AMP_CH3_GAIN_STEP,
    .Ch3_GainTbl = bAmpCh3GainTable,
};
// this structure is a handler of an AMP group (with the same amp driver)
// if you want to add some AMPs with different driver, you should add another "stAmpList" and initialize it

AmpList stAmpList =
{
	.peList = stAmp_AD85050_8CH,/*指向的是AD85050功放列表*/
	.pstAmpCfg = &stAmpCfg_AD85050_8CH,/*指向的是AD85050功放的属�? */
};

// to insert amp driver
DRIVER_INIT(Insert_AD85050drv);


