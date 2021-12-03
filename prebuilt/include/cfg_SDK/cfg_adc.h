/******************************************************************************/
/**
*
* \file	    cfg_adc.h
*
* \brief	SDK configuration for ADC.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   tingwei.lin@sunplus.com.cn
* \version	 v0.01
* \date 	 2016/08/25
******************************************************************************/
#ifndef CFG_ADC_H
#define CFG_ADC_H

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include "qp_pub.h"
/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/

#define ADC_CH_NUM_MAX		(4)

typedef enum
{
	EN_ADC_USER_FUNC = 0,
	EN_ADC_KEY_FUNC,
	EN_ADC_MAX_FUNC,
} EN_ADC_FUNC;

typedef struct
{
	unsigned int	Enable;
	EN_ADC_FUNC		Func;
	unsigned int	LevelNum;
	const unsigned int *pLevelTab;
} CFG_ADC_T;

#endif

