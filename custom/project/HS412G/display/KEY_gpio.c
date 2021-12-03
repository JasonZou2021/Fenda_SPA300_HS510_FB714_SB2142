/******************************************************************************/
/**
*
* \file	 VFD_Drv.c
*
* \brief	This file provides the vfd driver layer function \n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*\author       email@sunplus.com
*\version	 v0.01
*\date 	 2016/08/9
******************************************************************************/
#include <string.h>
#define LOG_MODULE_ID ID_VFD
#include "log_utils.h"
#include "pinmux.h"
#include "KEY_gpio.h"

int ReadKey(int PinNum)
{
	int key = -1;

	key = GPIO_Intput_Read(PinNum);

	LOGD("key:  %d\n", key);
	return key;
}

int InitKeyGpio(int PinNum)
{
	int iRes = 0;

	LOGD("InitKeyGpio: %d\n", PinNum);

	iRes = Pinmux_GPIO_CheckStatus(PinNum);

	if (iRes == 0)
	{
		iRes |= Pinmux_GPIO_Alloc(PinNum, IO_GPIO, IO_RISC);
		LOGD("Pinmux_GPIO_Alloc: %d\n", iRes);

		iRes |= GPIO_OE_Set(PinNum, IO_INPUT);
		LOGD("GPIO_OE_Set: %d\n", iRes);
	}

	if (iRes != 0)
	{
		LOGE(" key init Fail: %d!!!\n", PinNum);
	}
	else
	{
		LOGD("gpio key init OK!!!\n");
	}

	return iRes;
}

//return 0 => success
int InitKeyGpioTbl(int *pGpioNumTbl, int TblSize)
{
	int cnt, res = 0;

	if ((pGpioNumTbl == NULL) || (TblSize <= 0))
	{
		LOGE("InitKeyGpioTbl Fail: %d!!!\n");
		return -1;
	}

	for (cnt = 0; cnt < TblSize; cnt++)
	{
		res |= InitKeyGpio(pGpioNumTbl[cnt]);
	}

	return res;
}

//return which gpio is active in pGpioNumTbl. return -1 if there is no key
//ActiveLv: 0: gpio is low active, 1: gpio is high active
int ReadKeyGpioTbl(int *pGpioNumTbl, int TblSize, int ActiveLv)
{
	int cnt, res = 0;

	for (cnt = 0; cnt < TblSize; cnt++)
	{
		res = GPIO_Intput_Read(pGpioNumTbl[cnt]);

		if (res == ActiveLv)
		{
			return cnt;
		}
	}

	return -1;
}


