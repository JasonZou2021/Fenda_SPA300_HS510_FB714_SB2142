/******************************************************************************/
/**
*
* \file	factorydefault.h
*
* \brief	8107 factory default.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
* \author       daniel.yeh@sunplus.com
*\version	 v0.01
*\date 	 2016/04/15
******************************************************************************/
#ifndef __SYSTEMSETTING_H__
#define __SYSTEMSETTING_H__

#include "settingmanager.h"
#include "iop_scanner.h"

typedef struct resumeSetting
{
	UINT32 time;
	DWORD offset;
	DWORD FoClust;
	UINT16 index;
#if (SUPPORT_FLODER)
	DWORD offset_floder;
	DWORD FoClust_floder;
	UINT16 index_floder;
#endif
} resumeSetting_t;

/**
*\brief system partition struct
*/

typedef struct
{
	UINT8 SystemSetting_test1;
	UINT8 SystemSetting_test2;
	UINT16 SystemSetting_test3;
	resumeSetting_t	resume_usb;
	resumeSetting_t	resume_sd;
	stSaveIRInfo_t IR_SaveTable[IR_L_KEY_NUM];

	int L_Vol_UP_POSITION;
	int L_Vol_DN_POSITION;
	int L_Vol_MUTE_POSITION;

} stSystemSetting_T;

/******************************************************************************************/
/**
 * \fn      INT32 SystemSettingSaveInfo(INT32 addr, UINT8 *pBuf, INT32 sdLen, INT32 sdWriteMode)
 *
 * \brief   write the save data into partition space
 *
 * \param   addr :(input) the offset value.
 * \param   pBuf :(input) the write data buffer point.
 * \param   sdLen :(input) write length.
 * \param   sdWriteMode :(input) write mode WRITEBACK or WRITETHROUGH.
 *
 * \return  0: success , other: fail.
 *
******************************************************************************************/
INT32 SystemSettingSaveInfo(INT32 addr, UINT8 *pBuf, INT32 sdLen, INT32 sdWriteMode);

/******************************************************************************************/
/**
 * \fn			int SystemSetting_SaveTest1(UINT8 Data)
 *
 * \brief		save system setting value to save partition
 *
 *
 * \param		UINT8 Data
 *
 *
 * \return	    return : 1(SETTING_SAVE_INFO_SUCCESS) ; -1(SETTING_SAVE_INFO_FAIL)
 *
 *
 ******************************************************************************************/
int SystemSetting_SaveTest1(UINT8 Data);

/******************************************************************************************/
/**
 * \fn			int SystemSetting_SaveIRTable1(UINT8 *g_IR1)
 *
 * \brief		save system setting IRTable1 value to save partition
 *
 *
 * \param		UINT8 *g_IR1
 *
 *
 * \return	    return : 1(SETTING_SAVE_INFO_SUCCESS) ; -1(SETTING_SAVE_INFO_FAIL)
 *
 *
 ******************************************************************************************/
int SystemSetting_SaveIRTable(int Index, stSaveIRInfo_t *g_IR1);


/******************************************************************************************/
/**
 * \fn			int SystemSetting_ReadIRTable1(UINT8 *g_IR1)
 *
 * \brief		read system setting IRTable1 value from save partition
 *
 *
 * \param		UINT8 *g_IR1
 *
 *
 * \return	    return : 1(SETTING_READ_INFO_SUCCESS) ; -1(SETTING_READ_INFO_FAIL)
 *
 *
 ******************************************************************************************/
int SystemSetting_ReadIRTable(int Index, stSaveIRInfo_t *g_IR1);


/******************************************************************************************/
/**
* \fn			int SystemSetting_SaveResume(resumeSetting_t* Data)
*
* \brief		save system setting value to save partition
*
*
* \param		UINT8 Data
*
*
* \return	    return : 1(SETTING_SAVE_INFO_SUCCESS) ; -1(SETTING_SAVE_INFO_FAIL)
*
*
******************************************************************************************/
int SystemSetting_SaveResumeUsb(resumeSetting_t *Data);
int SystemSetting_CleanResumeUsb(resumeSetting_t *Data);

/******************************************************************************************/
/**
* \fn			int SystemSetting_SaveResumeSd(resumeSetting_t* Data)
*
* \brief		save system setting value to save partition
*
*
* \param		UINT8 Data
*
*
* \return	    return : 1(SETTING_SAVE_INFO_SUCCESS) ; -1(SETTING_SAVE_INFO_FAIL)
*
*
******************************************************************************************/
int SystemSetting_SaveResumeSd(resumeSetting_t *Data);


/******************************************************************************************/
/**
 * \fn			int SystemSetting_SaveTest2(UINT8 Data)
 *
 * \brief		save system setting value to save partition
 *
 *
 * \param		UINT8 Data
 *
 *
 * \return	    return : 1(SETTING_SAVE_INFO_SUCCESS) ; -1(SETTING_SAVE_INFO_FAIL)
 *
 *
 ******************************************************************************************/
int SystemSetting_SaveTest2(UINT8 Data);

/******************************************************************************************/
/**
 * \fn			int SystemSetting_SaveTest3(UINT8 Data)
 *
 * \brief		save system setting value to save partition
 *
 *
 * \param		UINT16 Data
 *
 *
 * \return	    return : 1(SETTING_SAVE_INFO_SUCCESS) ; -1(SETTING_SAVE_INFO_FAIL)
 *
 *
 ******************************************************************************************/
int SystemSetting_SaveTest3(UINT16 Data);

/******************************************************************************************/
/**
 * \fn			int SystemSetting_ReadTest1(UINT8 * Data)
 *
 * \brief		read system setting Test1 value from save partition
 *
 *
 * \param		UINT8 * Data
 *
 *
 * \return	    return : 1(SETTING_READ_INFO_SUCCESS) ; -1(SETTING_READ_INFO_FAIL)
 *
 *
 ******************************************************************************************/
int SystemSetting_ReadTest1(UINT8 *Data);

/******************************************************************************************/
/**
* \fn			int SystemSetting_ReadResumeUsb(resumeSetting_t* Data
*
* \brief		read system setting Test1 value from save partition
*
*
* \param		UINT8 * Data
*
*
* \return	    return : 1(SETTING_READ_INFO_SUCCESS) ; -1(SETTING_READ_INFO_FAIL)
*
*
******************************************************************************************/
int SystemSetting_ReadResumeUsb(resumeSetting_t *Data);

/******************************************************************************************/
/**
* \fn			int SystemSetting_ReadResumeSd(resumeSetting_t* Data
*
* \brief		read system setting Test1 value from save partition
*
*
* \param		UINT8 * Data
*
*
* \return	    return : 1(SETTING_READ_INFO_SUCCESS) ; -1(SETTING_READ_INFO_FAIL)
*
*
******************************************************************************************/
int SystemSetting_ReadResumeSd(resumeSetting_t *Data);

int SystemSetting_ClearIRTable_RawData(int Index);

/******************************************************************************************/
/**
 * \fn			int SystemSetting_ReadIRPosition(UINT8 *g_IR1)
 *
 * \brief		read system setting learning key position
 *
 *
 * \param
 *
 *
 * \return	    return : other(key position) ; -1(FAIL)
 *
 *
 ******************************************************************************************/

int SystemSetting_ReadIRPosition(int Index);

/******************************************************************************************/
/**
 * \fn			int SystemSetting_Init(stSystemSetting_T* pstSetting)
 *
 * \brief		system setting default set and init
 *
 *
 * \param		none
 *
 *
 * \return	    return : 1(SETTING_INIT_SUCCESS) ; -1(SETTING_INIT_FAIL)
 *
 *
 ******************************************************************************************/

int SystemSetting_Init(void);

/******************************************************************************************/
/**
 * \fn			int SystemSetting_ReadALL(stSystemSetting_T* pstSetting)
 *
 * \brief		read all system setting data from save partition
 *
 *
 * \param		stSystemSetting_T* pstSetting : system save structure pointer
 *
 *
 * \return	    return : 1(SETTING_READ_INFO_SUCCESS) ; -1(SETTING_READ_INFO_FAIL)
 *
 *
 ******************************************************************************************/
int SystemSetting_ReadALL(stSystemSetting_T *pstSetting);

#endif //__SYSTEMSETTING_H__
