/******************************************************************************/
/**
*
* \file	    cfg_mcuif.h
*
* \brief	SDK configuration for upgrade.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   sean.lin@sunplus.com
* \version	 v0.01
* \date 	 2021/5/17
******************************************************************************/
#ifndef CFG_MCUIF_H
#define CFG_MCUIF_H

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include "qp_pub.h"
/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/

enum eBaudRate_ConfigType {
	eUART_BR_115200 = 0,
	eUART_BR_921600,
	eUART_BR_1843200,
	eUART_BR_3686400,
	eUART_BR_CUSTOM,
	eUART_BR_max,
};

typedef struct
{
	unsigned int baudRate;
} CFG_MCUIF_T;


#endif

