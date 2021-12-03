/******************************************************************************

  Copyright (c) 2018 Sunplus technology co. Ltd.All right reserved!

 ******************************************************************************
 File Name: datarammap.h
 Author: Daniel Yeh
 Created : 2018/02/07
 Last Modified:
 Description: The memory map for bootloader.
 Function List:

 History Information Description:
  Date                          Author                          Modification
  2018/02/07                    Daniel                          Created function

  Date                          Author                          Modification

******************************************************************************/

#ifndef __DATARAMMAP_H__
#define __DATARAMMAP_H__

#define DATA_RAM_BASE						(0xbfff0000)
#define DATA_RAM_SIZE						(0x200)			//from booting table
															//noused area	0x230~78f

#define SRAM_SIZE_4B						(4)
#define SRAM_SIZE_8B						(8)

#define SRAM_ADDR_ST						(DATA_RAM_BASE)

//burn test
#define SRAM_ADDR_BURN_CHECK				SRAM_ADDR_ST
#define SRAM_SIZE_BURN_CHECK				(sizeof(int))

#define SRAM_ADDR_BURN_SW_SRC_PERIOD		(SRAM_ADDR_BURN_CHECK + SRAM_SIZE_BURN_CHECK)
#define SRAM_SIZE_BURN_SW_SRC_PERIOD		(sizeof(int))

#define SRAM_ADDR_BURN_PWR_OFF_TIME			(SRAM_ADDR_BURN_SW_SRC_PERIOD + SRAM_SIZE_BURN_SW_SRC_PERIOD)
#define SRAM_SIZE_BURN_PWR_OFF_TIME			(sizeof(int))

#define SRAM_ADDR_BURN_PWR_ON_TIME			(SRAM_ADDR_BURN_PWR_OFF_TIME + SRAM_SIZE_BURN_PWR_OFF_TIME)
#define SRAM_SIZE_BURN_PWR_ON_TIME			(sizeof(int))

#define SRAM_ADDR_BURN_PWR_CNT				(SRAM_ADDR_BURN_PWR_ON_TIME + SRAM_SIZE_BURN_PWR_ON_TIME)
#define SRAM_SIZE_BURN_PWR_CNT				(sizeof(int))
//=======================================================================================================

// isp upgrade
#define SRAM_ADDR_ISP_UPGRADE				(SRAM_ADDR_BURN_PWR_CNT + SRAM_SIZE_BURN_PWR_CNT)
#define SRAM_SIZE_ISP_UPGRADE				(sizeof(int))
//=======================================================================================================

// uart baudrate
#define SRAM_ADDR_UART1_BR					(SRAM_ADDR_ISP_UPGRADE + SRAM_SIZE_ISP_UPGRADE)
#define SRAM_SIZE_UART1_BR					(sizeof(int))
//=======================================================================================================

// system wakeup info
#define SRAM_ADDR_WAKEUP_INFO				(SRAM_ADDR_UART1_BR + SRAM_SIZE_UART1_BR)
#define SRAM_SIZE_WAKEUP_INFO				(sizeof(int))
//=======================================================================================================

//repeater saved source
#define SRAM_ADDR_REPEATER_SAVED_SRC			(SRAM_ADDR_WAKEUP_INFO + SRAM_SIZE_WAKEUP_INFO)
#define SRAM_SIZE_REPEATER_SAVED_SRC			(sizeof(int))
//=======================================================================================================

// EP wake up flag
#define SRAM_ADDR_EP_WAKE_UP_FLAG			(SRAM_ADDR_REPEATER_SAVED_SRC + SRAM_SIZE_REPEATER_SAVED_SRC)
#define SRAM_SIZE_EP_WAKE_UP_FLAG			(sizeof(int))
//=======================================================================================================

// log module
#define SRAM_ADDR_LOG_STAT					(SRAM_ADDR_EP_WAKE_UP_FLAG + SRAM_SIZE_EP_WAKE_UP_FLAG)
#define SRAM_SIZE_LOG_STAT					(sizeof(int))

#define SRAM_ADDR_LOG_LEVEL					(SRAM_ADDR_LOG_STAT + SRAM_SIZE_LOG_STAT)
#define SRAM_SIZE_LOG_LEVEL					(((int)MODULE_MAX + 3) & (~0x03))
//=======================================================================================================
#define SRAM_ADDR_END						(SRAM_ADDR_LOG_LEVEL + SRAM_SIZE_LOG_LEVEL) // 0xbfff063C

// exemple // SRAM_ADDR_XXX = previous data address + previous data size

//#if ((SRAM_ADDR_END) > (DATA_RAM_BASE |0x700 ))
//	#error plz check SRAM data address
//#endif

#endif /*__MEMMAP_H__*/

