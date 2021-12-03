/******************************************************************************/
/**
*
* \file	    cfg_sdk.c
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

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include "Display_cfg.h"
#include "VFD_KeyTab.inc"
#include "user_def.h"
#include "hw_gpio_list.h"

/*---------------------------------------------------------------------------*
 *                             parameter                                     *
 *---------------------------------------------------------------------------*/
#ifdef Q70_DEMO
	#define Led0_CTRL_GPIO   		eHW_IO_47 			//USB	LED_W
	#define Led1_CTRL_GPIO			eHW_IO_44 			//OPT	LED_B
	#define Led2_CTRL_GPIO			eHW_IO_43 			//ARC	LED_R
	#define Led3_CTRL_GPIO			eHW_IO_45 			//HDMI1	LED_Y
	#define Led4_CTRL_GPIO			eHW_IO_46 			//BT	LED_G
#else
	#define Led0_CTRL_GPIO   		eHW_IO_31 			//LINE
	//#define Led1_CTRL_GPIO			eHW_IO_67 			//USB
	//#define Led2_CTRL_GPIO			eHW_IO_10 			//COAX
	//#define Led3_CTRL_GPIO			eHW_IO_46 			//OPT
	//#define Led4_CTRL_GPIO			eHW_IO_22 			//WIFI
	//#define Led5_CTRL_GPIO			eHW_IO_44 			//BT
	//#define Led6_CTRL_GPIO			eHW_IO_63 			//sunplus sub WSS board BT
#endif

#ifdef Q70_DEMO
int LED_GPIO_Tab[] =
{
	Led0_CTRL_GPIO,
	Led1_CTRL_GPIO,
	Led2_CTRL_GPIO,
	Led3_CTRL_GPIO,
	Led4_CTRL_GPIO,
};
#else
int LED_GPIO_Tab[] =
{
	Led0_CTRL_GPIO,
	//	Led1_CTRL_GPIO,
	//	Led2_CTRL_GPIO,
	//	Led3_CTRL_GPIO,
	//	Led4_CTRL_GPIO,
	//	Led5_CTRL_GPIO,
	//	Led6_CTRL_GPIO
};
#endif
#define LED_NUM (sizeof(LED_GPIO_Tab) / sizeof(LED_GPIO_Tab[0]))


int KeyGpioNum_tbl[] =
{
	eHW_IO_21,
};
#define KEY_GPIO_NUM (sizeof(KeyGpioNum_tbl) / sizeof(KeyGpioNum_tbl[0]))


/*---------------------------------------------------------------------------*
    cfg structure
*---------------------------------------------------------------------------*/

static const DISPLAY_CFG_T gstDISPLAYConfig =
{
	// configure option for VFD module
	.VFD_CFG_TO =
	{
		.Cfg_Enable = 1,
		.VFD_KeyNum = DISPLAY_VFD_VAL_NUM,
		.pVFD_val = DISPLAY_VFD_ValTab,
	},

	.KEY_GPIO_CFG_TO =
	{
#if defined(SUNPLUS_WSS_DEMO_BOARD_LINEOUT_ANALOG_AMP)
		.KeyGpio_Enable = 1,
#else
		.KeyGpio_Enable = 0,
#endif
		.KeyGpio_Num = KEY_GPIO_NUM,
		.pKeyGpio_Tbl = KeyGpioNum_tbl,
		.KeyGpio_ActiveLv = 0,
	},
	.LED_CFG_TO =
	{
#ifdef Q70_DEMO
		.Cfg_Enable = 1,
#else
		.Cfg_Enable = 0,
#endif
		.pLedGpio_Tab = LED_GPIO_Tab,
		.LED_Number = LED_NUM,
	},

};

/*---------------------------------------------------------------------------*
 *                          FUNCTION   DECLARATIONS                          *
 *---------------------------------------------------------------------------*/

DISPLAY_CFG_T const *GetDISPLAYConfig(void)
{
	return &gstDISPLAYConfig;
}

