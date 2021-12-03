#ifndef __BT_DATA_TYPES_H__
#define __BT_DATA_TYPES_H__

/**
 * @file bt_data_types.h
 *
 * basic data types
 *
 * Copyright (c) 2013 Sunplus Technology Co., Ltd
 *
 * <b>
 @verbatim
   FILE NAME : bt_data_types.h
   PURPOSE   :
   AUTHOR    :
   PHASE     :
   Document  :
   History   :
       Date             Author     Version   Description
       ----------  --------------  ------  ---------------------------------
       2013/12/11   Titan Ho        v0.00      first creation

 @endverbatim
 * </b>\n
 *
 * @note N/A
 * @bug N/A
 */


#if 1 //linux 8700
#include <stdio.h>
#include <stdlib.h> //free
#include <string.h>  //malloc
#else
#include "types.h" //to include types in  8388 eCos
#endif
#ifdef __cplusplus
	extern "C" {
#endif

/************ ********* ********* ********* ********* *********** ****
**			Definitions
********* ********* ********* ********* ********* ********* **********/
//data type for APIs
#define		BT_NOT_FOUND			2
#define		BT_NOT_READY			1
#define		BT_SUCCESS				0
#define		BT_ERROR				(-1)
#define		BT_INVALID_PARAM		(-2)
#define		BT_INVALID_FD			(-3)

#ifndef NULL
#define NULL     0
#endif

#ifndef FALSE
#define FALSE  0
#endif

#ifndef TRUE
#define TRUE   (!FALSE)
#endif


#define W64     long long
#define W32     int
#define W16     short
#define W8      char

typedef W64                     INT64;
typedef W32                     INT32;
typedef W16                     INT16;
typedef signed  W8              INT8;
typedef unsigned W64            UINT64;
typedef unsigned W32            UINT32;
typedef unsigned W16            UINT16;
typedef unsigned W8             UINT8;
typedef char			        Boolean;
typedef int                     bool;
typedef	signed char		        int8_t;
typedef	unsigned char		    u_int8_t;
typedef	unsigned char		    uint8_t;
typedef	short			        int16_t;
typedef	unsigned short		    u_int16_t;
typedef	unsigned short		    uint16_t;
typedef	int			            int32_t;
typedef	unsigned int		    u_int32_t;
typedef	unsigned int		    uint32_t;
typedef	long long		        int64_t;
typedef	unsigned long long	    u_int64_t;
typedef	unsigned long long	    uint64_t;
typedef unsigned char           BYTE;

#define BD_ADDR_LEN		(6)					/* don't modify this length */
#define LINK_KEY_LEN    (16)
#define DEV_CLASS_LEN   (3)

typedef UINT8 LINK_KEY[LINK_KEY_LEN];       /* Link Key */
typedef UINT8 DEV_CLASS[DEV_CLASS_LEN];     /* Device class */
typedef UINT8 *DEV_CLASS_PTR;               /* Pointer to Device class */

/************ ********* ********* ********* ********* *********** ****
**			enum
********* ********* ********* ********* ********* ********* **********/


/************ ********* ********* ********* ********* *********** ****
**			struct
********* ********* ********* ********* ********* ********* **********/


/************ ********* ********* ********* ********* *********** ****
**			Macros
********* ********* ********* ********* ********* ********* **********/


#ifdef __cplusplus
}
#endif

#endif /* ifndef __BT_DATA_TYPES_H__ */
