/******************************************************************************/
/**
*
* \file	    cfg_upgrade.h
*
* \brief	SDK configuration for upgrade.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   Daniel.yeh@sunplus.com
* \version	 v0.01
* \date 	 2016/12/2
******************************************************************************/
#ifndef CFG_UPGRADE_H
#define CFG_UPGRADE_H

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
	char *psbDevName;
	char *psbSpUpdateFileName;
	unsigned int baudRate;
} CFG_UPGRADE_T;


#endif

