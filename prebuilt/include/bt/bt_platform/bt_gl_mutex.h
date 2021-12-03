#ifndef __BT_GL_MUTEX_H__
#define __BT_GL_MUTEX_H__

/**
 * @file bt_gl_mutex.h
 *
 * A header file for bt_gl_mutex.c
 *
 * Copyright (c) 2013 Sunplus Technology Co., Ltd
 *
 * <b>
 @verbatim
   FILE NAME : bt_gl_mutex.h
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

#include "bt_gl_config.h"
#include "bt_gl_types.h"

#ifdef __BT_GL_MUTEX_C__
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
EXTERN BTGL_Status_t BT_GL_MutexCreate(char *szName, BT_GL_HandleId_t *pMutexId);
EXTERN BTGL_Status_t BT_GL_MutexDelete(BT_GL_HandleId_t mutexId);
EXTERN BTGL_Status_t BT_GL_MutexLock(BT_GL_HandleId_t mutexId);
EXTERN BTGL_Status_t BT_GL_MutexUnlock(BT_GL_HandleId_t mutexId);



#undef EXTERN
#endif /* ifndef  __GL_MUTEX_H__*/
