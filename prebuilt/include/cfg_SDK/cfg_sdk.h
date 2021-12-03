/******************************************************************************/
/**
*
* \file	    cfg_sdk.h
*
* \brief	SDK configuration for customer.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   yangf.wang@sunplus.com.cn
* \version	 v0.01
* \date 	 2016/07/22
******************************************************************************/
#ifndef CFG_SDK_H
#define CFG_SDK_H

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include "qp_pub.h"
#include "cfg_player.h"
#include "cfg_adc.h"
#include "cfg_ir.h"
#include "cfg_key.h"
#include "cfg_pmc.h"
#include "cfg_cec.h"
#include "cfg_hdmi.h"
#include "cfg_power_key.h"
#include "cfg_audtool.h"
#include "cfg_mcuif.h"
#include "cfg_upgrade.h"
#include "cfg_bt.h"
#include "cfg_log.h"
#include "cfg_i2c.h"
#include "cfg_setting.h"
#include "cfg_btheap.h"
#include "cfg_va.h"
#include "cfg_usb.h"
#include "cfg_hint.h"
#include "cfg_rotary.h"

/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/

#define SDK_ADC_CH_NUM  ADC_CH_NUM_MAX
#define AO_GPIO_NUM     6

typedef struct {
    CFG_LOG_T         CFG_LOG_TO;
    CFG_PLAYER_T      CFG_PLAYER_TO;
    CFG_ADC_T         CFG_ADC_TO[SDK_ADC_CH_NUM];
    CFG_IR_T          CFG_IR_TO;
    CFG_KEY_T         CFG_KEY_TO;
    CFG_CEC_T         CFG_CEC_TO;
    CFG_HDMI_T        CFG_HDMI_TO;
    CFG_PMC_T         CFG_PMC_TO;
    CFG_STDBY_GPIO_T  CFG_STDBY_GPIO_TO[AO_GPIO_NUM];
    CFG_POWER_KEY_T   CFG_POWER_KEY_TO;
    CFG_AUD_T         CFG_AUD_TO;
    CFG_MCUIF_T       CFG_MCUIF_TO;
    CFG_UPGRADE_T     CFG_UPGRADE_TO;
    CFG_BT_T          CFG_BT_TO;
    CFG_I2C_T         CFG_I2C_TO;
    CFG_SETTING_T     CFG_SETTING_TO;
    CFG_BTHEAP_T      CFG_BTHEAP_TO;
    CFG_VA_T      	  CFG_VA_TO;
    CFG_USB_T         CFG_USB_TO;
	CFG_HINT_T        CFG_HINT_TO;
    CFG_ROTARY_T      CFG_ROTARY_TO;
} CFG_SDK_T;

/*---------------------------------------------------------------------------*
 *                          FUNCTION   DECLARATIONS                          *
 *---------------------------------------------------------------------------*/
CFG_SDK_T const *GetSDKConfig(void);

#define GET_SDK_CFG(type_) ((type_ *)(&(GetSDKConfig()->type_##O)))

CFGPRIVATE_BT_T const *GetBTSDKPrivateConfig(void);


#endif

