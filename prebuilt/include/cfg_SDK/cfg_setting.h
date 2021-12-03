/******************************************************************************/
/**
*
* \file		cfg_setting.h
*
* \brief	setting manager control configure \n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author    none@sunplus.com.cn
* \version	 v0.01
* \date 	 2016/07/22
******************************************************************************/

#ifndef __CFG_SETTING_H__
#define __CFG_SETTING_H__
/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/
#define CFG_SETTING_SAVE_RANGE  (16) // maximum 16 sector
#define CFG_SETTING_USER_RANGE  (4)   //maximum (total range / 2)  1 sector = 4K Byte


typedef struct
{
	unsigned short wSaveDataRange; 	//total flash sectoc.
	unsigned short wCustomerUseRange;  //sectoc number.
} CFG_SETTING_T;

#endif // __CFG_SETTING_H__
