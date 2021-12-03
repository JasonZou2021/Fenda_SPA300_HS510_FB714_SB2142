/******************************************************************************/
/**
*
* \file	    cfg_key.h
*
* \brief	SDK configuration for io ctrl.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   daniel.yeh@sunplus.com.cn
* \version	 v0.01
* \date 	 2016/09/19
******************************************************************************/
#ifndef CFG_KEY_H
#define CFG_KEY_H

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
	int iPollingTime;// unit: 100ms, ex: 1->1 time/100ms, 2->1 times/200ms
	int iKeyTickFreq; // it is a multiple of iPollingTime
	int iLongPressTickNum; // when a key has pressed, it should wait how many ticks to be a long press
} CFG_KEY_T;


#endif

