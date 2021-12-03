#ifndef __BT_GL_MALLOC_H__
#define __BT_GL_MALLOC_H__

/**
 * @file bt_gl_malloc.h
 *
 * General System Layer
 *
 * Copyright (c) 2013 Sunplus Technology Co., Ltd
 *
 * <b>
 @verbatim
   FILE NAME : bt_gl_malloc.h
   PURPOSE   : (1) provide standard interface for memory usage
               (2) provide memory debug message here as running on debug mode.
   AUTHOR    :
   PHASE     :
   Document  :
   History   :
       Date             Author     Version   Description
       ----------  --------------  ------  ---------------------------------
       2016/04/06   Titan Ho        v0.00      first creation.

 @endverbatim
 * </b>\n
 *
 * @note N/A
 * @bug N/A
 */


#include "bt_gl_config.h"
#include "bt_gl_types.h"

#ifdef __BT_GL_MALLOC_C__
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
* @brief	the standard malloc API
* @param	size: the size want to allocate
* @return	n/a
* @note		Date (y/m/d) Editor     comments
*           2016/04/06   Titan.ho   1st created
**/
EXTERN void* BT_GL_Malloc(BT_GL_UINT32 size);
/**
* @brief	the standard realloc API
* @param	pPtr: pointer to a memory block previously alocated with malloc or others..
                 Alternatively, this can be a null pointer, in which case a new block is allocated.
*           size: New size for the memory block
* @return	void*: A pointer to the reallocted memory block.
               A NULL pointer indicates either that size was zero(an thus pPtr was deallocated),
               or that the function did not allocate storage (and thus pPtr was not modified).
* @note		Date (y/m/d) Editor     comments
*           2016/04/06   Titan.ho   1st created
**/
EXTERN void* BT_GL_Realloc(void* pPtr, BT_GL_UINT32 size);
/**
* @brief	malloc memory and bzero memory data API
* @param	size: the size want to allocate
* @return	void*: A pointer to the allocted memory block.
               A NULL pointer indicates either that size was zero(an thus pPtr was deallocated),
               or that the function did not allocate storage (and thus pPtr was not modified).
* @note		Date (y/m/d) Editor     comments
*           2016/04/06   Titan.ho   1st created
**/
EXTERN void* BT_GL_BzeroMalloc(BT_GL_UINT32 size);
/**
* @brief	the standard free API
* @param	ptr: the pointer that own by user
* @return	n/a
* @note		Date (y/m/d) Editor     comments
*           2016/04/06   Titan.ho   1st created
**/
EXTERN BTGL_Status_t BT_GL_Free(void *pPtr);

#undef EXTERN
#endif /* ifndef __BT_TRACE_H__ */
