/******************************************************************************/
/**
*
* \file	settingmanager.h
*
* \brief	8107 save global value function.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author       email@sunplus.com
*\version	 v0.01
*\date 	 2016/04/15
******************************************************************************/
#ifndef __SETTINGMANAGER_H__
#define __SETTINGMANAGER_H__
#include "types.h"

#define FACTORYCONTENT			(0)
#define BTCONTENT				(1)
#define SYSTEMCONTENT			(2)
#define USERCONTENT			(3)
#define FLASHPATITIONSIZE		(4)

#define WRITETHROUGH                  (0)
#define WRITEBACK                         (1)
#define SYNCDATA                           (2)

#define CUSTOMERUSE                      (0)
#define SYSUSE                                      (1)

#define BT_DATA_BLOCKSIZE	(512)		//In the last 512bytes area of VD partition
#define BT_RECORDFLAG_LEN	(4)
#define BT_LICENSE_LEN		(48)
#define BT_NAME_LEN			(16)
#define BT_ADDR_LEN			(6)

typedef struct _SETTINGCUSTOMIZE_INFO_S
{
	UINT32 dBytePerSector;
	UINT32 dTotalSector;
	UINT32 dTotalSize;
} CUSTINFO_T;

//error code
#define SETTING_Default_SUCCESS 0
#define SETTING_Default_FAIL -1
#define SETTING_INIT_SUCCESS 1
#define SETTING_INIT_FAIL -1
#define SETTING_READ_INFO_SUCCESS 1
#define SETTING_READ_INFO_FAIL -1
#define SETTING_SAVE_INFO_SUCCESS 1
#define SETTING_SAVE_INFO_FAIL -1
#define SETTING_OUT_RANGE

/******************************************************************************************/
/**
 * \fn		INT32 SettingManagerEarlyInit(void)
 *
 * \brief	Setting Manager Init function
 *
 * \return	0: Succes, other: fail.
 *
 ******************************************************************************************/
INT32 SettingManagerEarlyInit(void);

/******************************************************************************************/
/**
 * \fn		INT32 SettingManagerInit(void)
 *
 * \brief	Setting Manager Init function
 *
 * \return	0: Succes, other: fail.
 *
 ******************************************************************************************/
INT32 SettingManagerInit(void);

/******************************************************************************************/
/**
 * \fn		INT32 RecycleDirtySector(INT32 sdNum)
 *
 * \brief	recycle dirty sector.
 *
 * \param	sdNum : (input)  check sector time
 *
 * \return	0: Succes, other: fail.
 *
 ******************************************************************************************/
INT32 RecycleDirtySector(INT32 sdNum);

/******************************************************************************************/
/**
 * \fn		INT32 SavePartitionAlloc(INT16 wContentType, void *pBuf, UINT32 dLen)
 *
 * \brief	alloc the save partition.
 *
 * \param	wContentType : (input)  save partition type
 * \param	pBuf : (input)  Save data buf point
 * \param	dLen : (input)  Save date buf length
 *
 * \return	0: Succes, other: fail.
 *
 ******************************************************************************************/
INT32 SavePartitionAlloc(INT16 wContentType, void *pBuf, UINT32 dLen);

/******************************************************************************************/
/**
 * \fn		INT32 SavePartitionUpdate(INT16 wMode,INT32 sdOffset, void *pBuf, UINT32 dLen)
 *
 * \brief	update the save partition data.
 *
 * \param	wMode : (input) writemode & partition type
 * \param	sdOffset : (input) the save data offset,which in the partition struct
 * \param	pBuf : (input) the save date buf point
 * \param	dLen : (input) the save date buf length
 *
 * \return	0: Succes, other: fail.
 *
 ******************************************************************************************/
INT32 SavePartitionUpdate(INT16 wMode, INT32 sdOffset, void *pBuf, UINT32 dLen);

/******************************************************************************************/
/**
 * \fn		INT32 SavePartitionDataGet(INT16 wContentType,INT32 sdOffset, void *pBuf, UINT32 dLen)
 *
 * \brief	read the save partition data.
 *
 * \param	wContentType : (input) partition type
 * \param	sdOffset : (input) the read data offset,which in the partition struct
 * \param	pBuf : (input) the read date buf point
 * \param	dLen : (input) the read date buf length
 *
 * \return	0: Succes, other: fail.
 *
 ******************************************************************************************/
INT32 SavePartitionDataGet(INT16 wContentType, INT32 sdOffset, void *pBuf, UINT32 dLen);

/******************************************************************************************/
/**
 * \fn		void *SaveContentGet(INT16 wContentType)
 *
 * \brief	get the save partition data buf.
 *
 * \param	wContentType : (input) partition type
 *
 * \return	NULL: fail, other: the save struct point.
 *
 ******************************************************************************************/
void *SaveContentGet(INT16 wContentType);

/******************************************************************************************/
/**
 * \fn		INT32 SavePartitionDataSync(INT16 wContentType)
 *
 * \brief	sync the save date.
 *
 * \param	wContentType : (input) partition type
 *
 * \return	0: success , other: fail.
 *
 ******************************************************************************************/
INT32 SavePartitionDataSync(INT16 wContentType);

/******************************************************************************************/
/**
 * \fn		INT32 SettingManagerCustomerRead(INT32 addr, void *buf, UINT32 len)
 *
 * \brief	Setting Manager Customization Read function.
 *
 * \param	void
 *
 * \return	read data length.
 *
 ******************************************************************************************/
INT32 SettingManagerCustomerRead(INT32 addr, void *buf, UINT32 len);

/******************************************************************************************/
/**
 * \fn		INT32 SettingManagerSysRead(INT32 addr, void *buf, UINT32 len)
 *
 * \brief	Setting Manager Customization Read function.
 *
 * \param	void
 *
 * \return	read data length.
 *
 ******************************************************************************************/
INT32 SettingManagerSysRead(INT32 addr, void *buf, UINT32 len);

/******************************************************************************************/
/**
 * \fn		INT32 SettingManagerCustomerWrite(INT32 addr, void *buf, UINT32 len)
 *
 * \brief	Setting Manager Customization Write function.
 *
 * \param	void
 *
 * \return	Write data length.
 *
 ******************************************************************************************/
INT32 SettingManagerCustomerWrite(INT32 addr, void *buf, UINT32 len);

/******************************************************************************************/
/**
 * \fn		INT32 SettingManagerSysWrite(INT32 addr, void *buf, UINT32 len)
 *
 * \brief	Setting Manager Customization Write function.
 *
 * \param	void
 *
 * \return	Write data length.
 *
 ******************************************************************************************/
INT32 SettingManagerSysWrite(INT32 addr, void *buf, UINT32 len);

/******************************************************************************************/
/**
 * \fn		INT32 SettingManagerCustomerErase(INT32 sdSectorNum)
 *
 * \brief	Setting Manager Customization erase function.
 *
 * \param	void
 *
 * \return
 *
 ******************************************************************************************/
INT32 SettingManagerCustomerErase(INT32 sector);

/******************************************************************************************/
/**
 * \fn		INT32 SettingManagerSysErase(INT32 sdSectorNum)
 *
 * \brief	Setting Manager Customization erase function.
 *
 * \param	void
 *
 * \return
 *
 ******************************************************************************************/
INT32 SettingManagerSysErase(INT32 sector);

/******************************************************************************************/
/**
 * \fn		INT32 SettingManagerCustomerInfoGet(CUSTINFO_T *info)
 *
 * \brief	Setting Manager Customization erase function.
 *
 * \param	void
 *
 * \return	true 1 , false 0
 *
 ******************************************************************************************/
INT32 SettingManagerCustomerInfoGet(CUSTINFO_T *info);

/******************************************************************************************/
/**
 * \fn		INT32 SettingManagerSysInfoGet(CUSTINFO_T *info)
 *
 * \brief	Setting Manager Customization erase function.
 *
 * \param	void
 *
 * \return	true 1 , false 0
 *
 ******************************************************************************************/
INT32 SettingManagerSysInfoGet(CUSTINFO_T *info);

INT32 SettingManagerBT_RecordFlag(void *ptr, int len);
INT32 SettingManagerBT_License(void *ptr, int len);
INT32 SettingManagerBT_Name(void *ptr, int len);
INT32 SettingManagerBT_Address(void *ptr, int len);
INT32 SettingManagerBT_Info(void *ptr, int len);

extern int SystemSetting_Init(void);
extern INT32 BTSettingInit(void);

extern INT32 SettingSizeIsEqual(INT16 wContentType, UINT32 dLen);

extern INT32 ResavePartition(INT16 wMode,void *pBuf, UINT32 dLen);

#endif //__SETTINGMANAGER_H__
