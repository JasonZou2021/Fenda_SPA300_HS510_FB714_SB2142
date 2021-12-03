/******************************************************************************/
/**
*
* \file		btsetting.h
*
* \brief	8107 bt module save partition api.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
* \author       email@sunplus.com
*\version	 v0.01
*\date 	 2016/04/15
******************************************************************************/
#ifndef __BTSETTING_H__
#define __BTSETTING_H__

#define BT_DEVICE_RECORD_SIZE          (1024)
#define BT_SETTING_ITEM_SIZE            (20)

/**
*\brief bluetooch partition struct
*/
typedef struct _BTSETTING_S
{
	BYTE bBTDevice[BT_DEVICE_RECORD_SIZE];   /*!< the device data space*/
	BYTE bBTSetup[BT_SETTING_ITEM_SIZE];     /*!< the setup data space*/
} BTSETTING_T;

/******************************************************************************************/
/**
 * \fn      UINT16 BTSettingGetBTDeviceStart()
 *
 * \brief   the item offset, which in the BTSETTING_T struct
 *
 * \param   void
 *
 * \return  other: item start address, which in the BTSETTING_T struct.
 *
******************************************************************************************/
UINT16 BTSettingGetBTDeviceStart();

/******************************************************************************************/
/**
 * \fn      UINT16 BTSettingGetBTSettingStart()
 *
 * \brief   the item offset, which in the BTSETTING_T struct
 *
 * \param   void
 *
 * \return  other: item start address, which in the BTSETTING_T struct.
 *
******************************************************************************************/
UINT16 BTSettingGetBTSettingStart();

/******************************************************************************************/
/**
 * \fn      INT32 BTSettingReadInfo(INT32 addr, UINT8 *pBuf, INT32 sdLen)
 *
 * \brief   read the save data
 *
 * \param   addr :(input) the offset value.
 * \param   pBuf :(input) the read data buffer point.
 * \param   sdLen :(input) buffer length.
 *
 * \return  0: success , other: fail.
 *
******************************************************************************************/
INT32 BTSettingReadInfo(INT32 addr, UINT8 *pBuf, INT32 sdLen);

/******************************************************************************************/
/**
 * \fn      INT32 BTSettingSaveInfo(INT32 addr, UINT8 *pBuf, INT32 sdLen, INT32 sdWriteMode)
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
INT32 BTSettingSaveInfo(INT32 addr, UINT8 *pBuf, INT32 sdLen, INT32 sdWriteMode);

#endif //__BTSETTING_H__
