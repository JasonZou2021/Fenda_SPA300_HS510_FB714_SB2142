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
#ifndef KEY_GPIO_H
#define KEY_GPIO_H

#include "Display_Srv.h"

int InitKeyGpio(int PinNum);
int ReadKey(int PinNum);
int InitKeyGpioTbl(int *pGpioNumTbl, int TblSize);
int ReadKeyGpioTbl(int *pGpioNumTbl, int TblSize, int ActiveLv);

#endif


