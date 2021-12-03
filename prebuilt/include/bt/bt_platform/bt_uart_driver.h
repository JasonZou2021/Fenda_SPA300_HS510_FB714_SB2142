#ifndef __BT_RDA_DRIVER_H__
#define __BT_RDA_DRIVER_H__

/**
 * @file bt_rda_driver.h
 *
 * rda uart API for bluetooth
 *
 * Copyright (c) 2015 Sunplus Technology Co., Ltd
 *
 * <b>
 @verbatim
   FILE NAME : bt_rda_driver.h
   PURPOSE   : Contains rda bluetooth uart API
   AUTHOR    :
   PHASE     :
   Document  :
   History   :
       Date             Author     Version   Description
       ----------  --------------  ------  ---------------------------------
       2015/09/10   ds.ming      v0.00      first creation

 @endverbatim
 * </b>\n
 *
 * @note N/A
 * @bug N/A
 */
#include "serial_driver.h"

#ifdef __BT_RDA_DRIVER_C__
    #define EXTERN
#else
    #define EXTERN extern
#endif

/************ ********* ********* ********* ********* *********** ****
**			Definitions
********* ********* ********* ********* ********* ********* **********/

/************ ********* ********* ********* ********* *********** ****
**			enum
********* ********* ********* ********* ********* ********* **********/

/************ ********* ********* ********* ********* *********** ****
**			struct
********* ********* ********* ********* ********* ********* **********/

/************ ********* ********* ********* ********* *********** ****
**			Macros
********* ********* ********* ********* ********* ********* **********/

/************************************************************************************
**		Function Prototypes
************************************************************************************/
/**
* @brief	open uart
* @param	n/a
* @return	n/a
* @note 	Date (y/m/d)  Editor	  comments
*			2015-09-10	  ds.ming	  1st created.
**/
int BT_UartOpen(void);
/**
* @brief	set uart baud rate
* @param	n/a
* @return	n/a
* @note 	Date (y/m/d)  Editor	  comments
*			2015-09-10	  ds.ming	  1st created.
**/
int BT_UartBaudRate(int fd,BT_UART_BAUNDRATE baundrate);
/**
* @brief	read data from uart
* @param	n/a
* @return	n/a
* @note 	Date (y/m/d)  Editor	  comments
*			2015-09-10	  ds.ming	  1st created.
**/
INT32 BT_UartRead(INT32 sdFd,void *pbBuf, UINT32 dLen);

/*
* @brief	 send host data to uart
* @param	n/a
* @return	n/a
* @note 	Date (y/m/d)  Editor	  comments
*			2015-09-10	  ds.ming	  1st created.
**/
INT32 BT_UartWrite(INT32 sdFd, void *pbBuf, UINT32 dLen);
/*
* @brief	 close uart
* @param	n/a
* @return	n/a
* @note 	Date (y/m/d)  Editor	  comments
*			2015-09-10	  ds.ming	  1st created.
**/
int BT_UartColse(int fd);

#endif /* ifndef __SERIAL_DRIVER_H__ */

