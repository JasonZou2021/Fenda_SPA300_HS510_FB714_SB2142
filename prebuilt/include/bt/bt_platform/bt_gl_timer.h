#ifndef __BT_GL_TIMER_H__
#define __BT_GL_TIMER_H__

/**
 * @file bt_gl_timer.h
 *
 * A header file for bt_gl_timer.c
 *
 * Copyright (c) 2013 Sunplus Technology Co., Ltd
 *
 * <b>
 @verbatim
   FILE NAME : bt_gl_timer.h
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

#ifdef __BT_GL_TIMER_C__
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
EXTERN BTGL_Status_t BT_GL_TimerCreate(char *szName, void (*pTimerFunction)(void *pArg), void *pTimerArg,
                           BT_GL_BOOL fTimerStart, BT_GL_UINT32 dInterval, BT_GL_BOOL fPeriodic, BT_GL_HandleId_t *pTimerId);
EXTERN BTGL_Status_t BT_GL_TimerDelete(BT_GL_HandleId_t timerId);
EXTERN BTGL_Status_t BT_GL_TimerDisable(BT_GL_HandleId_t timerId);
EXTERN BTGL_Status_t BT_GL_TimerEnable(BT_GL_HandleId_t timerId);
EXTERN BTGL_Status_t BT_GL_TimerReschedule(BT_GL_HandleId_t timerId, BT_GL_UINT32 dNewInterval);


#undef EXTERN
#endif /* ifndef  __GL_TIMER_H__*/
