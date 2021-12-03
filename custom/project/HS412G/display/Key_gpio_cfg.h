/******************************************************************************/
/**
*
* \file	    cfg_sdk.h
*
* \brief	SDK configuration for customer.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   yangf.wang@sunplus.com.cn
* \version	 v0.01
* \date 	 2016/07/22
******************************************************************************/
#ifndef Key_gpio_cfg_H
#define Key_gpio_cfg_H

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include "qp_pub.h"
/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/

typedef struct
{
	unsigned int KeyGpio_Enable;
	int KeyGpio_Num;
	int *pKeyGpio_Tbl;
	int KeyGpio_ActiveLv;
} KEY_GPIO_CFG_T;


#endif

