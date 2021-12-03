#ifndef __SERIAL_DRIVER_H__
#define __SERIAL_DRIVER_H__

/**
 * @file serial_driver.h
 *
 * uart API for bluetooth
 *
 * Copyright (c) 2015 Sunplus Technology Co., Ltd
 *
 * <b>
 @verbatim
   FILE NAME : serial_driver.h
   PURPOSE   : Contains bluetooth uart API
   AUTHOR    :
   PHASE     :
   Document  :
   History   :
       Date             Author     Version   Description
       ----------  --------------  ------  ---------------------------------
       2015/08/21   ds.ming      v0.00      first creation

 @endverbatim
 * </b>\n
 *
 * @note N/A
 * @bug N/A
 */

/*#ifdef PLATFORM_8388
#include "bt_data_types.h"
#endif*/

#ifdef __SERIAL_DRIVER_C__
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
typedef enum eUART_IOCTL_CMD
{
	SET_BAUNDRATE,
	BT_UARTINIT,
	BT_UARTDEL,
	BT_UARTSTATE,
}UART_IOCTL_CMD;

typedef enum eBT_UART_BAUNDRATE
{
	BT_UART_BAUDRATE_115200,
	BT_UART_BAUDRATE_921600,
	BT_UART_BAUDRATE_3M
} BT_UART_BAUNDRATE;

/************ ********* ********* ********* ********* *********** ****
**			struct
********* ********* ********* ********* ********* ********* **********/

/************ ********* ********* ********* ********* *********** ****
**			Macros
********* ********* ********* ********* ********* ********* **********/

/************************************************************************************
**		Function Prototypes
************************************************************************************/

/*EXTERN INT32 BT_UART_init(void);
EXTERN INT32 BT_UART_enable(void);
EXTERN INT32 BT_UART_read(INT32 sdFd, void *pbBuf, UINT32 dLen);
EXTERN INT32 BT_UART_write(INT32 sdFd, void *pbBuf, UINT32 dLen);
EXTERN void  BT_UART_loop(void);
EXTERN INT32 BT_UART_setbaudrate(BT_UART_BAUNDRATE eBaudrate);
EXTERN INT32 BT_UART_SET_PARITY(UINT32 polarity);
EXTERN INT32 BT_UART_CLR_PARITY(void);
EXTERN INT32 BT_UART_putbytes(INT32 sdFd, void *pbBuf, UINT32 dLen);
EXTERN INT32 BT_UART_deinit(void);*/

#undef EXTERN
#endif /* ifndef __SERIAL_DRIVER_H__ */

