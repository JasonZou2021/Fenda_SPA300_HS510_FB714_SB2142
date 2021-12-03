#ifndef __BT_GL_QUEUE_H__
#define __BT_GL_QUEUE_H__

/**
 * @file bt_gl_queue.h
 *
 * A header file for bt_gl_queue.c
 *
 * Copyright (c) 2013 Sunplus Technology Co., Ltd
 *
 * <b>
 @verbatim
   FILE NAME : bt_gl_queue.h
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

#ifdef __BT_GL_QUEUE_C__
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
EXTERN BTGL_Status_t BT_GL_QueueCreate(char *szName, BT_GL_UINT32 dElementSize, BT_GL_UINT32 dMaxElements, BT_GL_UINT32 dMaxUrgentElements, BT_GL_HandleId_t *pQueueId);
EXTERN BTGL_Status_t BT_GL_QueueDelete(BT_GL_HandleId_t queueId);
EXTERN BTGL_Status_t BT_GL_QueueSend(BT_GL_HandleId_t queueId, void *pBuffer, BT_GL_UINT32 dSize);
EXTERN BTGL_Status_t BT_GL_QueueSendUrgent(BT_GL_HandleId_t queueId, void *pBuffer, BT_GL_UINT32 dSize);
EXTERN BTGL_Status_t BT_GL_QueueReceive(BT_GL_HandleId_t queueId, void *pBuffer, BT_GL_UINT32 dSize, BT_GL_INT32 sdTimeout);
EXTERN BTGL_Status_t BT_GL_QueueFlush(BT_GL_HandleId_t queueId);

EXTERN void* BT_GL_QueueGetUsedList(BT_GL_HandleId_t queueId,void **ppStartPos);
EXTERN void BT_GL_Queue_Lock(BT_GL_HandleId_t queueId);
EXTERN void BT_GL_Queue_Unlock(BT_GL_HandleId_t queueId);



#undef EXTERN
#endif /* ifndef __BT_TRACE_H__ */
