/******************************************************************************/
/**
*
* \file	user_def.h
*
* \brief	user define.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   daniel.yeh@sunplus.com
* \version	 v0.01
* \date 	 2017/08/22
******************************************************************************/

#ifndef user_def_H
#define user_def_H


#define CUST_CUSTOMER_CODE   (0x4fb)   //(0x007F) //for other sunplus IR controller
#define CUST_POWER_KEY_CODE  0x20// (0x46)

//#define PRJ_HS510  // 5.1


/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include "project_cfg.h"

/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/

// the macro TAG here  like "USER_PROJECT_TAG" are from project_cfg.h
// project_cfg.h is generate from project_XXX.cfg and board_XXX.cfg automatically


//#define USER_BT_DUT

/*---------------------------------------------------------------------------*/
// USER_PROJECT_TAG Description
// use USER_PROJECT_TAG to make custom macros

#define USER_PROJECT_SUNPLUS_WITH_WSS	(0)
#define USER_PROJECT_SUNPLUS_NO_WSS		(1)
#define USER_PROJECT_EXBT				(2)
#define USER_PROJECT_KARAOKE			(3)
#define USER_PROJECT_Q70_DEMO			(4)

#define USER_PROJECT_DISABLE_HDMI	(0)
#define USER_PROJECT_HDMI_MODE		(1)
#define USER_PROJECT_REPEATER_MODE	(2)
#define REPEATER_EP		(0)
#define REPEATER_ITE	(1)

#define USER_PROJECT_SUPPORT_BT_MODE		(1)

#if (USER_PROJECT_TAG == USER_PROJECT_SUNPLUS_WITH_WSS)
	//#define USERAPP_SUPPORT_BT
	#define USERAPP_SUPPORT_WSS
	//#define SUPPORT_SUNPLUS_PT6315_PAD
	//====================================================================

#elif (USER_PROJECT_TAG == USER_PROJECT_KARAOKE)
	#define USERAPP_SUPPORT_KARAOKE
	//#define USERAPP_SUPPORT_BT
	//#define SUPPORT_SUNPLUS_PT6315_PAD
	//====================================================================

#elif (USER_PROJECT_TAG == USER_PROJECT_EXBT)
	//nothing
	//#define SUPPORT_SUNPLUS_PT6315_PAD
	//#define USERAPP_SUPPORT_BT
	//====================================================================

#elif (USER_PROJECT_TAG == USER_PROJECT_SUNPLUS_NO_WSS)
	// pochou: SPA100 disable BT
	//#define USERAPP_SUPPORT_BT

	//#define SUPPORT_SUNPLUS_PT6315_PAD
	#define AMP_AD85050_8CH
	//#define AMP_PCM5242


	// For SPA100 Dec Cert
	//#define SPA100_DEC_CERT

	#if (DSP_H0_SIZE_CFG > 0)
		#define AUD_CHLMAP_5_1_2
		#define CERTTEST_IF
		// Audio Flow Control
		#define SUPPORT_HE_FLOW 1
		// For SPA300 Atmos Cert
		//#define ATMOS_CERT
	#else
		#define AUD_CHLMAP_7_1_0
    #endif

	// MCU interface enable
	//#define MCU_IF_SP
	#ifndef AMP_AD85050_12CH
	//#define SYNCCOM_WIRELESS
	#endif
	//====================================================================
#elif (USER_PROJECT_TAG == USER_PROJECT_Q70_DEMO)
	// pochou: SPA100 disable BT
	//#define USERAPP_SUPPORT_BT
	#define AMP_TAS5825M
	#define Q70_DEMO
	#ifndef AMP_AD85050_12CH
	//#define SYNCCOM_WIRELESS
	#endif
	//#define SUPPORT_SUNPLUS_PT6315_PAD
	//====================================================================


#else
	#error : incorrect  USER_PROJECT_TAG
#endif

#if (CFG_HDMI_TAG == USER_PROJECT_HDMI_MODE)
	//#define USERAPP_SUPPORT_HDMI_TX
	#define USERAPP_SUPPORT_HDMI_CEC
#elif (CFG_HDMI_TAG == USER_PROJECT_REPEATER_MODE)
	#if (CFG_REPEATER_TYPE == REPEATER_EP)
		//#define USERAPP_SUPPORT_REPEATER (REPEATER_EP)
	#elif (CFG_REPEATER_TYPE == REPEATER_ITE)
		#define USERAPP_SUPPORT_REPEATER (REPEATER_ITE)
	#endif
#elif (CFG_HDMI_TAG == USER_PROJECT_DISABLE_HDMI)
#endif

#if (CFG_BT_TAG == USER_PROJECT_SUPPORT_BT_MODE)
    #define USERAPP_SUPPORT_BT
#endif

// if USERAPP_SUPPORT_WSS is enable, USERAPP_SUPPORT_BT is necessary
#ifdef USERAPP_SUPPORT_WSS
	#ifndef USERAPP_SUPPORT_BT
		#error : incorrect  BT config
	#endif
#endif

#endif

