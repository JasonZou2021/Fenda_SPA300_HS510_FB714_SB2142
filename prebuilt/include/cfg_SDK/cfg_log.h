/******************************************************************************/
/**
*
* \file	    cfg_log.h
*
* \brief	SDK configuration for log system.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   xh.huang@sunplus.com.cn
* \version	 v0.01
* \date 	 2017/03/28
******************************************************************************/
#ifndef __CFG_LOG_H__
#define __CFG_LOG_H__

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include "log_imp.h"

/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/

typedef struct
{
	STATE_e eLogState;
    // LEVEL_e eLogLevel;
	char *pcPassword;
} CFG_LOG_T;

#endif
