/******************************************************************************/
/**
*
* \file	    TvRemoteReady_cfg.h
*
* \brief	configuration for TvRemoteReady.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   Daniel.yeh@sunplus.com
* \version	 v0.01
* \date 	 2017/06/22
******************************************************************************/
#ifndef IR_MULTI_H
#define IR_MULTI_H

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include"types.h"

/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/
#define IR_Cus_ID_TBL_END (0xFFFFFFFF)
#define IR_KEY_CODE_TBL_END (0xFFFF)

#define IR_FMT_MONKEY_TEST (0xff)
enum
{
	IR_FMT_TC9012 = 0,
	IR_FMT_UPD6121,
	IR_FMT_M50560,
	IR_FMT_MATC6D6,
	IR_FMT_SONYD7C5,
	IR_FMT_SONYD7C8,
	IR_FMT_AHEA48,
	IR_FMT_MATC5D6,
	IR_FMT_RC6,
	IR_FMT_SHARP,
	IR_FMT_SAMSUNG,
	IR_FMT_MAX,
};


typedef struct
{
	const UINT32 *IR_CusId;
	const UINT16 *pIR_KeyCode[15];
	const UINT8 *pIR_KeyCmd;
} stIR_KeyTbl_T;

/*---------------------------------------------------------------------------*
 *                          FUNCTION   DECLARATIONS                          *
 *---------------------------------------------------------------------------*/

stIR_KeyTbl_T const *GetIR_Tbl(int IR_Config);


#endif

