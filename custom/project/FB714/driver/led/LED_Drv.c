/******************************************************************************/
/**
*
* \file	 LED_Drv.c
*
* \brief	This file provides the LED driver layer function \n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*\author       email@sunplus.com
*\version	 v0.01
*\date 	 2016/08/9
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log_utils.h"
#include "pinmux.h"
#include "LED_Drv.h"
#include "Display_cfg.h"

/******************************************************************************************/
/**
 * \fn			void Set_LED_ON(int LED_GPIO_NUM)
 *
 * \brief		LED ON
 *
 * \param
 *
 *
 * \return
 *
 *
 ******************************************************************************************/
void Set_LED_ON(int LED_GPIO_NUM)
{
	GPIO_Output_Write(LED_GPIO_NUM, IO_HIGH);
}

/******************************************************************************************/
/**
 * \fn			void Set_LED_OFF(int LED_GPIO_NUM)
 *
 * \brief		LED OFF
 *
 * \param
 *
 *
 * \return
 *
 *
 ******************************************************************************************/
void Set_LED_OFF(int LED_GPIO_NUM)
{
	GPIO_Output_Write(LED_GPIO_NUM, IO_LOW);
}

/******************************************************************************************/
/**
 * \fn			void Set_AllLED_ON(void)
 *
 * \brief		ALL LED ON
 *
 * \param
 *
 *
 * \return
 *
 *
 ******************************************************************************************/
void Set_AllLED_ON(void)
{
	LED_CFG_T *pstLED_CFG_T;

	pstLED_CFG_T = GET_DISPLAY_CFG(LED_CFG_T);

	int i = 0;
	for (i = 0 ; i < pstLED_CFG_T->LED_Number ; i++)
	{
		Set_LED_ON(pstLED_CFG_T->pLedGpio_Tab[i]);
	}
}

/******************************************************************************************/
/**
 * \fn			void Set_AllLED_OFF(void)
 *
 * \brief		ALL LED OFF
 *
 * \param
 *
 *
 * \return
 *
 *
 ******************************************************************************************/
void Set_AllLED_OFF(void)
{
	LED_CFG_T *pstLED_CFG_T;

	pstLED_CFG_T = GET_DISPLAY_CFG(LED_CFG_T);

	int i = 0;
	for (i = 0 ; i < pstLED_CFG_T->LED_Number ; i++)
	{
		Set_LED_OFF(pstLED_CFG_T->pLedGpio_Tab[i]);
	}
}

/******************************************************************************************/
/**
 * \fn			void Set_Led_Light(int led)
 *
 * \brief		set led on
 *
 * \param
 *
 *
 * \return
 *
 *
 ******************************************************************************************/

void Set_Led_Light(UINT16 led)
{
	LED_CFG_T *pstLED_CFG_T;

	pstLED_CFG_T = GET_DISPLAY_CFG(LED_CFG_T);

	int led_light_num_check;

	int i = 0;

	leddrv_printf2("led = 0x%x\n", led);

	for (i = 0 ; i < pstLED_CFG_T->LED_Number ; i++)
	{
		led_light_num_check = (led >> i) & 0x1;

		if ( led_light_num_check != 0 )
		{
			Set_LED_ON(pstLED_CFG_T->pLedGpio_Tab[i]);
		}
	}
}



/******************************************************************************************/
/**
 * \fn			void Set_Led_Off(int led)
 *
 * \brief		set led off
 *
 * \param
 *
 *
 * \return
 *
 *
 ******************************************************************************************/

void Set_Led_Off(UINT16 led)
{
	LED_CFG_T *pstLED_CFG_T;

	pstLED_CFG_T = GET_DISPLAY_CFG(LED_CFG_T);

	int led_dark_num_check;

	int i = 0;

	leddrv_printf2("led = 0x%x\n", led);

	for (i = 0 ; i < pstLED_CFG_T->LED_Number ; i++)
	{
		led_dark_num_check = (led >> i) & 0x1;

		if ( led_dark_num_check != 0 )
		{
			Set_LED_OFF(pstLED_CFG_T->pLedGpio_Tab[i]);
		}
	}
}


/******************************************************************************************/
/**
 * \fn			void InitLed(void)
 *
 * \brief		LED GPIO init
 *
 * \param
 *
 *
 * \return
 *
 *
 ******************************************************************************************/
void InitLed(void)
{
	leddrv_printf1("!!!!! InitLed in !!!!!\n");

	LED_CFG_T *pstLED_CFG_T;

	pstLED_CFG_T = GET_DISPLAY_CFG(LED_CFG_T);

	int iRes = 0;

	int i = 0;

	for (i = 0 ; i < pstLED_CFG_T->LED_Number ; i++)
	{
		iRes |= Pinmux_GPIO_CheckStatus(pstLED_CFG_T->pLedGpio_Tab[i]);

		if (iRes != 0)
		{
			leddrv_printf_E("driver: CHECK LED GPIO STATUS FAIL!!\n");
		}
		else
		{
			iRes |= Pinmux_GPIO_Alloc(pstLED_CFG_T->pLedGpio_Tab[i], IO_GPIO, IO_RISC);

			if (iRes != 0)
			{
				leddrv_printf_E("driver: ALLOC LED GPIO Fail!!\n");
			}


			iRes |= GPIO_OE_Set(pstLED_CFG_T->pLedGpio_Tab[i], IO_OUTPUT);

			if (iRes != 0)
			{
				leddrv_printf_E("driver: SET LED GPIO FAIL!!\n");
			}
		}
		if (iRes != 0)
		{
			leddrv_printf_E("driver: LED GPIO %d init Fail\n", pstLED_CFG_T->pLedGpio_Tab[i]);
			iRes = 0;
		}
		else
		{
			leddrv_printf1("driver: LED GPIO %d init OK!!!\n", pstLED_CFG_T->pLedGpio_Tab[i]);
		}
	}

	Set_AllLED_OFF();
}

