/******************************************************************************/
/**
*
* \file	    cfg_cec.h
*
* \brief	SDK configuration for cec.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   daniel.yeh@sunplus.com.tw
* \version	 v0.01
* \date 	 2016/10/4
******************************************************************************/
#ifndef CFG_CEC_H
#define CFG_CEC_H

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include "qp_pub.h"

/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/

//#define CEC_CTS_TEST

#define	PWR_ON_CMD_SYS_AUDIO_MODE_REQUEST		0x70
#define	PWR_ON_CMD_USR_CTL_PRESSED				0x44
#define	PWR_ON_CMD_SET_STREAM_PATH				0x86
#define	PWR_ON_CMD_ACTIVE_SOURCE				0x82
#define	PWR_ON_CMD_ROUTING_CHANGE				0x80

// Short Audio Descriptor  //2020 0426 mingjin.he

#define CEC_RESERVED 0
#define	CEC_LPCM     1
#define	CEC_AC3      2
#define	CEC_MPEG1    3
#define	CEC_MP3      4
#define	CEC_MPEG2    5
#define	CEC_AAC      6
#define	CEC_DTS      7
#define	CEC_DDP      10
#define	CEC_UNUSE    15
enum
{
	eHPD_By_ADC = 0,
	eHPD_By_GPIO,
};

typedef struct {
    unsigned int  Cfg_Enable;
    unsigned int  HPD_DetectFunc;
    unsigned int  HPD_VolThreshold;
    unsigned int  CEC_VendorId;
    char *pbCEC_DevOsdName;
    char *pbI2C_DevStr;
    unsigned char repeaterAddr;
    unsigned char repeaterIntrGpio;
    unsigned char Cfg_CecAlwaysRun;
    unsigned char CEC_HPD_WakeUp;
    unsigned char CEC_PwrOnOpcodeTbl[8];
    unsigned char CEC_PwrOnOpcodeNum;
    unsigned char CEC_ShortAudDescriptor[15];
    unsigned char CEC_ShortAudDescriptorNum;
    unsigned char CEC_ShortAudDescriptorData[15][3];
} CFG_CEC_T;

#endif
