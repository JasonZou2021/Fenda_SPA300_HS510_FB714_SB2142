/******************************************************************************/
/**
*
* \file	    cfg_player.h
*
* \brief	player SDK configuration sample code .\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   yangf.wang@sunplus.com.cn
* \version	 v0.01
* \date 	 2016/07/22
******************************************************************************/

#ifndef CFG_PLAYER_H
#define CFG_PLAYER_H

/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/

#define SUPPORT_MAX_BITRATE	(9*1024*1024)
#define SUPPORT_MAX_FILE_CNT	999
#define SUPPORT_MAX_DIR_CNT	 (500)
#define SUPPORT_RESUME 1
#define NOT_SUPPORT_RESUME 0

#define CFG_FILTER_MP3	0x1
#define CFG_FILTER_WAV	0x2
#define CFG_FILTER_FLAC	0x4
#define CFG_FILTER_AAC	0x8
#define CFG_FILTER_WMA	0x10
#define CFG_FILTER_AC3	0x20
#define CFG_FILTER_DTS	0x40
#define CFG_FILTER_APE	0x80
#define CFG_FILTER_MKA	0x100
#define CFG_FILTER_M4A	0x200
#define CFG_FILTER_OGG	0x400
#define CFG_FILTER_EC3  0x800
#define CFG_FILTER_TS   0x1000
#define CFG_FILTER_ASF  0x2000
#define CFG_FILTER_EAC3 0x4000
#define CFG_FILTER_MLP  0x8000
#define CFG_FILTER_AIF  0x10000
#define CFG_FILTER_AIFF 0x20000
#define CFG_FILTER_AIFC 0x40000
#define CFG_FILTER_DSD  0x80000




#define CFG_AUTOPLAY_DEFAULT		0
#define CFG_AUTOPLAY_USB_DISABLE	(1 << 0)
#define CFG_AUTOPLAY_USB_ENABLE		(1 << 1)
#define CFG_AUTOPLAY_CARD_DISABLE	(1 << 2)
#define CFG_AUTOPLAY_CARD_ENABLE	(1 << 3)


typedef struct
{
	unsigned int dStartMode;
	unsigned int dFmtFilter;
	unsigned int dMaxBitrate;
	unsigned int dMaxFileCnt;
	unsigned int dMaxDirCnt;
	unsigned char nIsResume;
	unsigned char nAutoPlay;
	unsigned int dUSBResumeTime;

} CFG_PLAYER_T;

#endif
