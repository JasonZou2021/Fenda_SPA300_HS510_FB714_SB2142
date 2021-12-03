#ifndef __BT_TRACE_H__
#define __BT_TRACE_H__

/**
 * @file bt_trace.h
 *
 * A header file for debug message
 *
 * Copyright (c) 2013 Sunplus Technology Co., Ltd
 *
 * <b>
 @verbatim
   FILE NAME : bt_trace.h
   PURPOSE   : different levels of debug message will be defined here
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

//#define ANDROID_SYS //at 8700
//#define LINUX_SYS	//at 6700
#define FREERTOS_SYS //at 8107

#if (defined ANDROID_SYS)	//at 8700
 //#include <stdio.h>
#include "utils/Log.h"
#elif (defined LINUX_SYS)	//at 6700
#include <stdio.h>
#elif (defined FREERTOS_SYS)	//at 8107
#include <stdio.h>
#define LOG_MODULE_ID  ID_BT
#include "log_utils.h"
#else
#include "types.h" // for diag_printf
#endif

#ifdef __BT_TRACE_C__
    #define EXTERN
#else
    #define EXTERN extern
#endif



/************ ********* ********* ********* ********* *********** ****
**			Definitions
********* ********* ********* ********* ********* ********* **********/
#define BT_BUG_ON

#ifdef BT_BUG_ON
#define BT_BUG_NTC_ON 1//NOTICE
#define BT_BUG_INFO_ON 1
#define BT_BUG_WARN_ON 1
#define BT_BUG_ERR_ON 1
#else

#endif

#ifdef BT_BUG_ON
	#ifndef BT_NTC_DBG
	  #define BT_NTC_DBG  1
	#endif
#else
	#undef BT_NTC_DBG
#endif

#ifdef BT_BUG_ON
	#ifndef BT_INFO_DBG
	  #define BT_INFO_DBG  1
	#endif
#else
	#undef BT_INFO_DBG
#endif

#ifdef BT_BUG_ON
	#ifndef BT_WARN_DBG
 	#define BT_WARN_DBG   1
	#endif
#else
	#undef BT_WARN_DBG
#endif

#ifdef BT_BUG_ON
	#ifndef BT_ERR_DBG
 	#define BT_ERR_DBG   1
	#endif
#else
	#undef BT_ERR_DBG
#endif

#ifndef LOG_TAG
#define LOG_TAG "bt"
#endif

/* opened to disable all debug
#undef BT_INFO_DBG
#undef BT_WARN_DBG
#undef BT_ERR_DBG
*/
/************ ********* ********* ********* ********* *********** ****
**			enum
********* ********* ********* ********* ********* ********* **********/


/************ ********* ********* ********* ********* *********** ****
**			struct
********* ********* ********* ********* ********* ********* **********/


/************ ********* ********* ********* ********* *********** ****
**			Macros
********* ********* ********* ********* ********* ********* **********/
#if (defined BT_BUG_ON)&&(defined BT_BUG_NTC_ON)&&(defined BT_NTC_DBG)&&(BT_NTC_DBG==1)
	#if (defined ANDROID_SYS)
		#define bt_ntc_printf(fmt, arg...) ALOGI(fmt,##arg)
	#elif (defined LINUX_SYS)
		#define bt_ntc_printf(fmt, arg...) printf("["LOG_TAG"]"fmt"\n",##arg)
    #elif (defined FREERTOS_SYS)
        #define bt_ntc_printf(fmt, arg...) LOGD(fmt, ##arg)
    #else
		#define bt_ntc_printf(fmt, arg...) diag_printf("["LOG_TAG"]"fmt"\n",##arg)
	#endif
#else
	#define bt_ntc_printf(a,...) do{}while(0)
#endif

#if (defined BT_BUG_ON)&&(defined BT_BUG_INFO_ON)&&(defined BT_INFO_DBG)&&(BT_INFO_DBG==1)
	#if (defined ANDROID_SYS)
		#define bt_printf(fmt, arg...) ALOGI(fmt,##arg)
	#elif (defined LINUX_SYS)
		#define bt_printf(fmt, arg...) printf("["LOG_TAG"]"fmt"\n",##arg)
    #elif (defined FREERTOS_SYS)
        #define bt_printf(fmt, arg...) LOGDT(fmt, ##arg)
    #else
		#define bt_printf(fmt, arg...) diag_printf("["LOG_TAG"]"fmt"\n",##arg)
	#endif
#else
	#define bt_printf(a,...) do{}while(0)
#endif

#if (defined BT_BUG_ON)&&(defined BT_BUG_WARN_ON)&&(defined BT_WARN_DBG)&&(BT_WARN_DBG==1)
	#if (defined ANDROID_SYS)
		#define bt_warn_printf(fmt, arg...) ALOGW("(Warning)"fmt,##arg)
	#elif (defined LINUX_SYS)
		#define bt_warn_printf(fmt, arg...) printf("["LOG_TAG"]Warning: "fmt"\n",##arg)
    #elif (defined FREERTOS_SYS)
        #define bt_warn_printf(fmt, arg...) LOGW(fmt, ##arg)
    #else
		#define bt_warn_printf(fmt, arg...) diag_printf("["LOG_TAG"]Warning: "fmt"\n",##arg)
	#endif
#else
	#define bt_warn_printf(a,...) do{}while(0)
#endif

#if (defined BT_BUG_ON)&&(defined BT_BUG_ERR_ON)&&(defined BT_ERR_DBG)&&(BT_ERR_DBG==1)
	#if (defined ANDROID_SYS)
		#define bt_err_printf(fmt, arg...) ALOGE("Err-%s(),line=%d: "fmt,__FUNCTION__,__LINE__,##arg)
	#elif (defined LINUX_SYS)
		#define bt_err_printf(fmt, arg...) printf("["LOG_TAG"]Err-%s(),line=%d: "fmt"\n",__FUNCTION__,__LINE__,##arg)
    #elif (defined FREERTOS_SYS)
        #define bt_err_printf(fmt, arg...) LOGE(fmt, ##arg)
    #else
		#define bt_err_printf(fmt, arg...) diag_printf("["LOG_TAG"]Err-%s(),line=%d: "fmt"\n",__FUNCTION__,__LINE__,##arg)
	#endif
#else
	#define bt_err_printf(a,...) do{}while(0)
#endif

#if	0
#define bt_pram_err_printf(fmt, arg...) 	bt_err_printf(fmt, arg...)
#else
#define bt_pram_err_printf(fmt, arg...)		do{}while(0)
#endif


/************************************************************************************
**		Function Prototypes
************************************************************************************/

#undef EXTERN
#endif /* ifndef __BT_TRACE_H__ */
