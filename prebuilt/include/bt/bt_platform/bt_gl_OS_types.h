#ifndef __GL_OS_TYPE_H__
#define __GL_OS_TYPE_H__

/**
 * @file gl_type.h
 *
 * A header file for gl_type.c
 *
 * Copyright (c) 2013 Sunplus Technology Co., Ltd
 *
 * <b>
 @verbatim
   FILE NAME : gl_type.h
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

#if 1//OS at 8700 android
#include "posix/pthread.h"
#include "posix/mutex.h"
#include "posix/semaphore.h"
#include "posix/time.h"
#include "bt_heap_manager.h"
//#include <errno.h>
#else//OS at eCos
#include "gsl/gl_types.h"
#include "gsl/gl_timer.h"
#endif





/************ ********* ********* ********* ********* *********** ****
**			Definitions
********* ********* ********* ********* ********* ********* **********/

#if 1//OS at 8700 android
typedef pthread_t		GL_Task_t;
typedef pthread_mutex_t	GL_Mutex_t;
typedef pthread_cond_t	GL_Cond_t;
typedef sem_t			GL_SEM_t;
typedef timer_t			GL_Timer_t;

typedef unsigned int *GL_Queue_t;
typedef unsigned int *GL_MEMPOOL_t;

#else //OS at eCos

typedef	GL_Task_t	BT_GL_Task_t;
typedef	GL_Mutex_t	BT_GL_Mutex_t;
typedef	GL_Cond_t	BT_GL_Cond_t;
typedef	GL_Timer_t	BT_GL_Timer_t;

typedef GL_Queue_t *BT_GL_Queue_t;
typedef GL_MEMPOOL_t *BT_GL_MEMPOOL_t;

#endif




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


#endif /* ifndef  */
