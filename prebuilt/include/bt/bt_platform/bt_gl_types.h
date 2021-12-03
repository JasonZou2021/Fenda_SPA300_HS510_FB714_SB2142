#ifndef __BT_GL_TYPE_H__
#define __BT_GL_TYPE_H__

/**
 * @file gl_type.h
 *
 * A header file for gl_type.c
 *
 * Copyright (c) 2013 Sunplus Technology Co., Ltd
 *
 * <b>
 @verbatim
   FILE NAME : bt_gl_types.h
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





/************ ********* ********* ********* ********* *********** ****
**			Definitions
********* ********* ********* ********* ********* ********* **********/
#define BTGL_NO_WAIT			0
#define BTGL_INFINITE_WAIT		-1

typedef char				BT_GL_BOOL;
typedef char				BT_GL_INT8;
typedef unsigned char		BT_GL_UINT8;
typedef short				BT_GL_INT16;
typedef unsigned short		BT_GL_UINT16;
typedef int					BT_GL_INT32;
typedef unsigned int		BT_GL_UINT32;
typedef long long			BT_GL_INT64;
typedef unsigned long long	BT_GL_UINT64;


typedef unsigned int BT_GL_HandleId_t;




/************ ********* ********* ********* ********* *********** ****
**			enum
********* ********* ********* ********* ********* ********* **********/
/*!
 * @enum BTGL_Status_t
 * Return value of GSL function and their meanings.
 */
typedef enum _BTGL_Status_t {
	/* generic error codes */
	BTGL_SUCCESS,				/**< operation successful */
	BTGL_FAILURE,				/**< operation failed */
	BTGL_TIMEOUT,				/**< operation timeout */

	BTGL_ERR_INVALID_PARAM,	/**< invalid parameters */
	BTGL_ERR_INVALID_OP,		/**< invalid operation */
	BTGL_ERR_MEMORY_ALLOC,	/**< error occurs when doing memory allocation */

	/* message queue-specific error codes */
	BTGL_QUEUE_EMPTY,			/**< the queue is empty */
	BTGL_QUEUE_FULL,			/**< the queue is full */

	/* semaphore specific error codes */
	BTGL_COUNT_EXCEEDED,		/**< the counter of semaphore is overflowed */

	BTGL_WARN_PARAM_CLIPPED,
} BTGL_Status_t;

/************ ********* ********* ********* ********* *********** ****
**			struct
********* ********* ********* ********* ********* ********* **********/


/************ ********* ********* ********* ********* *********** ****
**			Macros
********* ********* ********* ********* ********* ********* **********/


/************************************************************************************
**		Function Prototypes
************************************************************************************/


#endif /* ifndef  */
