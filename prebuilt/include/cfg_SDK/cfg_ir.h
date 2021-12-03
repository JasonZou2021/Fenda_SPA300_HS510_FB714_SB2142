/******************************************************************************/
/**
*
* \file	    cfg_ir.h
*
* \brief	SDK configuration for ir.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   daniel.yeh@sunplus.com.cn
* \version	 v0.01
* \date 	 2016/09/19
******************************************************************************/
#ifndef CFG_IR_H
#define CFG_IR_H

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
	unsigned int IopWakeUp;// which one wake up system from standby mode by checking IR iPowerKeycode // 1: IOP(recommand), 0: Risc
	int TV_Remote_Ready_Enable;
	int IR_Learning_Enable;
	int MainIrFmt; //which ir fmt is active in  standby mode (check its power key)//0 for UPD6121 (NEC), 1 for TC9012, 2 for RC6 (philip)
	UINT16 lCustomerID; // main ir customer ID for NEC protocol
	int iPowerKeycode; // ir power key code
} CFG_IR_T;


#endif

