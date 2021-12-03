/******************************************************************************/
/**
*
* \file	    Display_cfg.h
*
* \brief	Display configuration for customer.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   yangf.wang@sunplus.com.cn
* \version	 v0.01
* \date 	 2016/07/22
******************************************************************************/
#ifndef Display_cfg_H
#define Display_cfg_H

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include "qp_pub.h"
#include "VFD_cfg.h"
#include "Key_gpio_cfg.h"
#include "LED_cfg.h"

/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/

typedef struct
{
	VFD_CFG_T VFD_CFG_TO;
	KEY_GPIO_CFG_T KEY_GPIO_CFG_TO;
	LED_CFG_T LED_CFG_TO;
} DISPLAY_CFG_T;


/*---------------------------------------------------------------------------*
 *                          FUNCTION   DECLARATIONS                          *
 *---------------------------------------------------------------------------*/
DISPLAY_CFG_T const *GetDISPLAYConfig(void);

#define GET_DISPLAY_CFG(type_) (type_ *)(&(GetDISPLAYConfig()->type_##O))

#endif

