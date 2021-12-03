#ifndef __BT_GL_TASK_H__
#define __BT_GL_TASK_H__

/**
 * @file bt_gl_task.h
 *
 * A header file for bt_gl_task.c
 *
 * Copyright (c) 2013 Sunplus Technology Co., Ltd
 *
 * <b>
 @verbatim
   FILE NAME : bt_gl_task.h
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

#ifdef __BT_GL_TASK_C__
    #define EXTERN
#else
    #define EXTERN extern
#endif



/************ ********* ********* ********* ********* *********** ****
**			Definitions
********* ********* ********* ********* ********* ********* **********/
#define PTHREAD_STACK_MIN     (1024)

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
EXTERN BTGL_Status_t BT_GL_TaskCreate(char *szName, void (*pEntryPoint)(void *pArg),
                          void *pTaskArg, BT_GL_UINT32 dPriority,
                          BT_GL_UINT32 dStackSize, BT_GL_BOOL fTaskRun, BT_GL_HandleId_t *pTaskId);

EXTERN BTGL_Status_t BT_GL_TaskSelfDelete(void);
EXTERN BTGL_Status_t BT_GL_TaskDestroy(BT_GL_HandleId_t taskId);

EXTERN void BT_GL_TaskSleep(BT_GL_UINT32 timeout_ms);
EXTERN BT_GL_HandleId_t BT_GL_CurrentTask(void);
EXTERN BTGL_Status_t BT_GL_Thread_Join(BT_GL_HandleId_t taskId);

EXTERN BT_GL_UINT32 BT_GL_getPid(void);
EXTERN BT_GL_UINT32 BT_GL_getTid(void);

/*[causion] this get name API is used for debug only!!*/
EXTERN void BT_GL_getTaskName(BT_GL_HandleId_t taskId, char *szName, BT_GL_UINT8 len);

#undef EXTERN
#endif /* ifndef __BT_TRACE_H__ */
