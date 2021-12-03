#ifndef __BT_GL_SEMA_H__
#define __BT_GL_SEMA_H__

/**
 * @file bt_gl_sema.h
 *
 * A header file for bt_gl_sema.c
 *
 * Copyright (c) 2013 Sunplus Technology Co., Ltd
 *
 * <b>
 @verbatim
   FILE NAME : bt_gl_sema.h
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

#ifdef __BT_GL_SEMA_C__
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
EXTERN BTGL_Status_t BT_GL_SemaphoreCreate(char *szName, BT_GL_UINT32 dMaxCount, BT_GL_UINT32 dInitialValue, BT_GL_HandleId_t *pSemId);
EXTERN BTGL_Status_t BT_GL_SemaphoreDelete(BT_GL_HandleId_t semId);
EXTERN BTGL_Status_t BT_GL_SemaphoreGive(BT_GL_HandleId_t semId);
EXTERN BTGL_Status_t BT_GL_SemaphoreTake(BT_GL_HandleId_t semId, BT_GL_INT32 sdTimeout);



#undef EXTERN
#endif /* ifndef  __GL_SEMA_H__*/
