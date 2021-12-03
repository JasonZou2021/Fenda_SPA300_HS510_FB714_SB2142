/******************************************************************************/
/**
*
* \file	    cfg_hdmi.h
*
* \brief	SDK configuration for hdmi.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author    ribin.huang@Sunplus-prof.com
* \version	 v0.01
* \date 	 2018/3/7
******************************************************************************/
#ifndef CFG_HDMI_H
#define CFG_HDMI_H

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include "qp_pub.h"

/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/


enum
{
	eHDMI_480P = 0,
	eHDMI_576P,
	eHDMI_AUTO,
};

typedef struct
{
	unsigned int Cfg_HdmiOutputMode;
} CFG_HDMI_T;

#endif

