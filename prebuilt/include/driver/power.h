/******************************************************************************/
/**
*
* \file	 power.h
*
* \brief	This file provides the 8107 power manager control\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*\author       email@sunplus.com
*\version	 v0.01
*\date 	 2016/06/21
******************************************************************************/

#ifndef _POWER_STANDBY_H
#define _POWER_STANDBY_H

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/


#define CEC_RET_ENTER_STANDBY     0
#define CEC_RET_WAKEUP            1
#define CEC_RET_NO_ENTER_STANDBY  2



/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/

typedef int32_t (*PwrChk_CB_t)(uint8_t src);

typedef int (*Pwr_DisableDevicePwr_CB_t)(void);
typedef int (*power_setFunc)(void);

typedef enum
{
	Power_EP_INIT = 0,
	Power_EP_WakeChk_Handler,
	Power_EP_WakeChkFlash,
	Power_HDMI_Standby_init,
	Power_HDMI_GetPowerStatet,
	Power_Func_END,
} setFuncNum;

// system reset
enum
{
	HWIF_RESET_NONE,
	HWIF_RESET_GLOBAL,			// hardware reset ALL
	HWIF_RESET_RISC,
	HWIF_RESET_BR,
	HWIF_RESET_FLASHIF,			// Flash/Rom interface
	HWIF_RESET_RI,				// RISC interface
	HWIF_RESET_SDCTRL,			// SDRAM controller /SDRAM arbitrator
	HWIF_RESET_AGDC,
	HWIF_RESET_GRFX,			// Graphics hardware
	HWIF_RESET_PARM,			// Video decoding parameter
	HWIF_RESET_IDCT,
	HWIF_RESET_INVQ,
	HWIF_RESET_VLD,
	HWIF_RESET_MC,
	HWIF_RESET_TSIF,
	HWIF_RESET_RESERVE_BIT_14,
	HWIF_RESET_RESERVE_BIT_15,
	//
	HWIF_RESET_OGT,				// Subpicture / overlay-graphics hardware
	HWIF_RESET_VPP,
	HWIF_RESET_OSD,
	HWIF_RESET_TV,
	HWIF_RESET_IOP,
	HWIF_RESET_DSP,
	HWIF_RESET_STC,
	HWIF_RESET_CD,
	HWIF_RESET_CSS,
	HWIF_RESET_HOST,
	HWIF_RESET_LPT,				// printer port
	HWIF_RESET_UA0,
	HWIF_RESET_UA1,
	HWIF_RESET_TDM,
	HWIF_RESET_BT,				// Bootstrap
	HWIF_RESET_SRV,
};

typedef enum
{
	STANDBY_PMC_MODE = 0,
	STANDBY_IOP_MODE,
	STANDBY_RISC_MODE,
	STANDBY_RETENTION_MODE,
} standbymode;

typedef enum
{
	eWakeupInfo_IR = 0x55AA0001,
	eWakeupInfo_PWRKEY,
	eWakeupInfo_CEC,
	eWakeupInfo_GPIO,
	eWakeupInfo_RTC,
	eWakeupInfo_SPDIF,
	eWakeupInfo_REPEATER,
	eWakeupInfo_User = 0x12340000,

	eWakeupInfo_Clear = 0xFFFFFFFF,
} eWakeupInfo_t;

typedef struct
{
	int  switch_src_time;
	int  power_off_time;
	int  power_on_time;
} BurnTest_SwitchSrcPowerOnOff;


//return TRUE will reset system

/*---------------------------------------------------------------------------*
 *                          FUNCTION   DECLARATIONS                          *
 *---------------------------------------------------------------------------*/

/******************************************************************************************/
/**
 * \fn		void Power_system_stdy(int standbyMode)
 *
 * \brief		power system stanby
 *
 * \param 		standbyMode:system Power stdby Mode<STANDBY_PMC_MODE,STANDBY_IOP_MODE,STANDBY_RISC_MODE>
 *
 * \return		void
 *
 * \note
 *
 * \sa          Power_system_stdy()
 *
 ******************************************************************************************/

void Power_system_stdy(int standbyMode);

/******************************************************************************************/
/**
 * \fn		void Power_HWEnable(void)
 *
 * \brief		set HW power keywhen power on.
 *
 * \param 		void
 *
 * \return		void
 *
 * \note
 *
 * \sa          Power_HWEnable()
 *
 ******************************************************************************************/

void Power_HWEnable(void);
void BT_Reset_Enable(void);
void BT_Reset_Disable(void);


/******************************************************************************************/
/**
 * \fn		void Power_FirstPowerUPEntryStandby(void)
 *
 * \brief		check system first Power UP whether or not standby, this API just for provide to customer
 *
 * \param 		void
 *
 * \return		void
 *
 * \note
 *
 * \sa          Power_FirstPowerUPEntryStandby()
 *
 ******************************************************************************************/

void Power_FirstPowerUPEntryStandby(void);

void EnableSystem5VPower(void);

/******************************************************************************************/
/**
 * \fn		void Power_WriteBurnTestParameter(int SwitchSrcTime ,int PowerOffTime , int PowerOnTime)
 *
 * \brief		write Burn Test Parameter to system
 *
 * \param
 *
 * \return
 *
 * \note
 *
 * \sa          Power_WriteBurnTestParameter(int SwitchSrcTime ,int PowerOffTime , int PowerOnTime)
 *
 ******************************************************************************************/
void Power_WriteBurnTestParameter(int SwitchSrcTime, int PowerOffTime, int PowerOnTime);
/******************************************************************************************/
/**
 * \fn		int Power_ReadBurnTestParameter(BurnTest_SwitchSrcPowerOnOff* pBurnTest_SwitchSrcPowerOnOff)
 *
 * \brief		read Burn Test Parameter after system power on
 *
 * \param
 *
 * \return
 *
 * \note
 *
 * \sa          Power_ReadBurnTestParameter()
 *
 ******************************************************************************************/
int Power_ReadBurnTestParameter(BurnTest_SwitchSrcPowerOnOff *pBurnTest_SwitchSrcPowerOnOff);

int Power_PwrUpChkFun(PwrChk_CB_t CB_fun);

int Power_PwrResetFlag(int param);

int Power_RegDisableDevicePwrFun(Pwr_DisableDevicePwr_CB_t CB_fun);

int Power_DisableDevicePwr(void);

int HDMI_RegCB(UINT8 FuncNum,power_setFunc CB_fun);

void Power_ClrPowerMagic(void);

uint32_t Power_IsFirstPowerup(void);

#endif/*_POWER_STANDBY_H*/
