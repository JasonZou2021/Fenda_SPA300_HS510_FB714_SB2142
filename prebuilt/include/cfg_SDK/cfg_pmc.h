/******************************************************************************/
/**
*
* \file		cfg_pmc.h
*
* \brief	power manager control configure \n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author    none@sunplus.com.cn
* \version	 v0.01
* \date 	 2016/07/22
******************************************************************************/

#ifndef __CFG_PMC_H__
#define __CFG_PMC_H__
/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/
typedef struct {
    uint8_t  bFstPwrOnFlag;        // 1: system need into standby mode ,when system first power on. 0: system running ,when system first power on
    uint8_t  bStandbyMode;         // 0: PMC 1: IOP 2: RISC
    uint16_t wIRwakeupMode    : 2, // 1: Enable IR wake up mode 0: Disable
             wRtcwakeupMode   : 2, // 1: Enable RTC wake up mode 0: Disable
             wCECwakeupMode   : 2, // 1: Enable CEC wake up mode 0: Disable
             wSPDIFwakeupMode : 2;
    uint16_t wRtcIntervalTime;     // RTC interval time () for wakeup
    uint16_t riscSleepTime;
} CFG_PMC_T;

typedef struct {
    uint8_t gpioChkEnable;
    uint8_t gpioTrigLvl;
    uint8_t functionEnable;
} CFG_STDBY_GPIO_T;

#endif // __CFG_PMC_H__
