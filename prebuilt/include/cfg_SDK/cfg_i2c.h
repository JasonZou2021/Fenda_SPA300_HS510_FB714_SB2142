/******************************************************************************/
/**
*
* \file	    cfg_i2c.h
*
* \brief	SDK configuration for i2c ctrl.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   daniel.yeh@sunplus.com
* \version	 v0.01
* \date 	 2017/05/26
******************************************************************************/
#ifndef CFG_I2C_H
#define CFG_I2C_H

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include "qp_pub.h"
/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/

#define CFG_I2C_SPEED_H (0) //around 350kHz
#define CFG_I2C_SPEED_L (1) //around 90kHz

#define CFG_I2C_OUTPUT (0)
#define CFG_I2C_OPEN_DRAIN (1) // for pull high 5V/3.3V circuit
#define CFG_I2C_OPEN_DRAIN_FORCE (2) // when I2C bus is going to be high level in open-drain mode, force the output to 3.3V first

typedef struct
{
    uint32_t Cfg_CLK_I2C0;
	uint8_t Cfg_CLK_I2C1;
	uint8_t Cfg_CLK_I2C2;
	uint8_t Cfg_CLK_I2C3;
	uint8_t Cfg_Mode_I2C1;
	uint8_t Cfg_Mode_I2C2;
	uint8_t Cfg_Mode_I2C3;
} CFG_I2C_T;
#endif
