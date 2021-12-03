#ifndef __BT_NVM_H__
#define __BT_NVM_H__

/**
 * @file bt_nvm.h
 *
 *
 *
 * Copyright (c) 2013 Sunplus Technology Co., Ltd
 *
 * <b>
 @verbatim
   FILE NAME : bt_nvm.h
   PURPOSE   : Platform dependent storages will be implemented here.
   AUTHOR    :
   PHASE     :
   Document  :
   History   :
       Date             Author     Version   Description
       ----------  --------------  ------  ---------------------------------
       2015/10/08   Titan Ho        v0.00      first creation

 @endverbatim
 * </b>\n
 *
 * @note N/A
 * @bug N/A
 */

#include "settingmanager/btsetting.h"


#ifdef __BT_NVM_C__
    #define EXTERN
#else
    #define EXTERN extern
#endif
/************ ********* ********* ********* ********* *********** ****
**			Definitions
********* ********* ********* ********* ********* ********* **********/

// define section sizes.
#define BT_DEV_INFO BT_DEVICE_RECORD_SIZE

/************ ********* ********* ********* ********* *********** ****
**			enum
********* ********* ********* ********* ********* ********* **********/

/************ ********* ********* ********* ********* *********** ****
**			struct
********* ********* ********* ********* ********* ********* **********/


/************ ********* ********* ********* ********* *********** ****
**			Macros
********* ********* ********* ********* ********* ********* **********/


/*******************************************************************************
**	Functions
********************************************************************************/
EXTERN UINT32 BT_NVM_crc(void *pbBuf, UINT32 dLen);
EXTERN INT32 BT_NVM_save_data(void *pbBuf, UINT32 dLen, UINT32 dOffset);
EXTERN INT32 BT_NVM_load_data(void *pbBuf, UINT32 dLen);

#endif /* ifndef __BT_NVM_H__ */
