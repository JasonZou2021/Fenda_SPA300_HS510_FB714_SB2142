/**************************************************************************/
/**
*
* \file	mbase.h
*
* \brief	 for mcode make for load code addr
*
* \note  	Copyright (c) 2016 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   email@sunplus.com
* \version	 v0.01
* \date 	 2016/04/15
***************************************************************************/
#ifndef _MBASE_H
#define _MBASE_H
/*---------------------------------------------------------------------------*
* INCLUDE DECLARATIONS
*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
* MACRO DECLARATIONS
*---------------------------------------------------------------------------*/

/****************************************************
 base memory: intr, stack, data, code,
****************************************************/
#define BT_HEAP_SIZE		(120)

/**
*\stack for before enter FreeRTOS
*/
#define BOOTLOADER_STK_SIZE	(6)

//ISP size
#if defined(SYS_ZEBU_ZMEM) && (SYS_ZEBU_ZMEM == 1)
	#define ISP_BOOT_START		0x80800000  // 8 MB
	#define ISP_CODE_ST			(12 * 1024) // 12 MB
#else
	#define ISP_BOOT_START		0x88000000  // nor start
	#define ISP_CODE_ST			(0)
#endif
#define ISP_CODE_SIZE			(48)

#define ISP_DATA_SECTION_ST		(ISP_CODE_ST + (ISP_CODE_SIZE << 1))
#define ISP_DATA_SECTION_SIZE	(96)

#include "pinmux.i"

#define APP_HEADER_SIZE		(0x80)

//system required partiation name
#define APP_BIN_NAME		"app.bin"
#define APPB_BIN_NAME		"app_b.bin"
#define ALL_BIN_NAME		"all.bin"
#define VD_BIN_NAME			"vendor_data.bin"
#define XML_BIN_NAME		"xml_pack.bin"			//for DSP H
#define XML_CONFIG_NAME		"dsptool_config.xml"

#endif

