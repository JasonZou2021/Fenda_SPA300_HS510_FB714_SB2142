/**************************************************************************/
/**
 *
 * \file	log_utils.h
 *
 * \brief	This file declare log output by its level\n
 * \note	Copyright (c) 2016 Sunplus Technology Co., Ltd. \n
 *		All rights reserved.
 *
 *
 * \author	xh.huang@sunplus.com.cn
 * \version	v0.01
 * \date	2016/05/18
 ***************************************************************************/

#ifndef __LOG_UTILS_H__
#define __LOG_UTILS_H__

#include "log_imp.h"

#ifdef LOG_SYSTEM_DISABLED
#define LOGDT(FMT, ARGS ...)                     do{}while(0)
#define LOGD(FMT, ARGS ...)                      do{}while(0)
#define LOGI(FMT, ARGS ...)                      do{}while(0)
#define LOGW(FMT, ARGS ...)                      do{}while(0)
#define LOGE(FMT, ARGS ...)                      do{}while(0)
#define LOGX(FMT, ARGS ...)                      do{}while(0)
#define LOGXDT(FMT, ARGS ...)                    do{}while(0)
#define LOGT(FMT, ARGS ...)                      do{}while(0)
#else
#ifndef LOG_MODULE_ID
#define LOG_MODULE_ID                            ID_UKN
#endif
#ifndef LOG_EXTRA_STR
#define LOG_EXTRA_STR                            ""
#endif

#define GEN_LOG_MODULE_STR(mod)                  mod ## _STR

#define LOG_WITH_PREFIX(ID, LEV, FMT, ARGS...)  LogImplemention(ID, LV_ ## LEV, "["#LEV " "GEN_LOG_MODULE_STR(ID) "] "LOG_EXTRA_STR""FMT, ## ARGS)
#define LOG_NO_PREFIX(ID, LEV, FMT, ARGS...)    LogImplemention(ID, LV_ ## LEV, FMT, ## ARGS)

#define LOGDT(FMT, ARGS...)                     LOG_WITH_PREFIX(LOG_MODULE_ID, T, FMT, ## ARGS)
#define LOGD(FMT, ARGS...)                      LOG_WITH_PREFIX(LOG_MODULE_ID, D, FMT, ## ARGS)
#define LOGI(FMT, ARGS...)                      LOG_WITH_PREFIX(LOG_MODULE_ID, I, FMT, ## ARGS)
#define LOGW(FMT, ARGS...)                      LOG_WITH_PREFIX(LOG_MODULE_ID, W, FMT, ## ARGS)
#define LOGE(FMT, ARGS...)                      LOG_WITH_PREFIX(LOG_MODULE_ID, E, "[%s-%d]"FMT, __FUNCTION__, __LINE__, ## ARGS)
#define LOGX(FMT, ARGS...)                      LOG_NO_PREFIX(LOG_MODULE_ID, D, FMT, ## ARGS)
#define LOGXDT(FMT, ARGS...)                    LOG_NO_PREFIX(LOG_MODULE_ID, T, FMT, ## ARGS)
#define LOGT(FMT, ARGS...)                      LOG_WITH_PREFIX(LOG_MODULE_ID, D, "[%s-%d]"FMT, __FUNCTION__, __LINE__, ## ARGS)

#define BLOCKPRINTF(s, l)                      \
        do {                                   \
            int i = 0; char *nl = (char *)s;   \
            for(i = 0; i < l; i++) {           \
                LOGX("%02x ", *(nl + i));      \
                if(i%32 == 31) LOGX("\r\n");   \
                else if(i%16 == 15) LOGX(" "); \
            }   LOGX("\r\n");                  \
        } while (0)
#endif
#endif
