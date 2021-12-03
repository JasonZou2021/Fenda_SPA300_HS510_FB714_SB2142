/******************************************************************************/
/**
*
* \file	    LED_cfg.h
*
* \brief	SDK configuration for VFD.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   tingwei.lin@sunplus.com.cn
* \version	 v0.01
* \date 	 2016/08/25
******************************************************************************/
#ifndef LED_cfg_H
#define LED_cfg_H

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include "qp_pub.h"
/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/

typedef struct
{
	unsigned int Cfg_Enable;
	int *pLedGpio_Tab;
	int LED_Number;
} LED_CFG_T;

#endif

