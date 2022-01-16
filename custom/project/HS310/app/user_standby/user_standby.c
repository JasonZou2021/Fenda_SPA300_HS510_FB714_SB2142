/******************************************************************************/
/**
*
* \file	 Karaoke.c
*
* \brief	This file provides the karaoke middleware state machine framework \n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*\author       email@sunplus.com
*\version	 v0.01
*\date 	 2017/08/21
******************************************************************************/
#include <string.h>
#include "user_standby.h"
#include "pinmux.h"
#include "initcall.h"
#include "powerservice.h"
#include "iop_scanner.h"
#include "hw_gpio_list.h"
#define LOG_MOD_ID  ID_APUSR
#include "log_utils.h"
#include "VFD.h"

static int pinnum = -1; // for power check

/******************************************************************************************/
/**
 * \fn			int User_Set_PIN_Power_Down(int ModuleID , int PinID)
 *
 * \brief		Api for user to set pin to gpio mode (output low) in standby mode
 *
 * \param
 *
 * \return	    \e  0 on success. \n
 *
 *
 ******************************************************************************************/
int User_Set_PIN_Power_Down(int ModuleID, int PinID)
{
	int Pin_Num = -1;
	int iResPin = 0;

	Pin_Num = Pinmux_ModulePin_Find(ModuleID, PinID);

	if (Pin_Num == -1)
	{
		LOGE("ModuleID %d : PinID %d  Find Fail !\n", ModuleID, PinID);
		return -1;
	}

	Pinmux_GPIO_Free(Pin_Num);
	iResPin |= Pinmux_GPIO_Alloc(Pin_Num, IO_GPIO, IO_RISC);
	iResPin |= GPIO_OE_Set(Pin_Num, IO_OUTPUT);
	iResPin |= GPIO_Output_Write(Pin_Num, IO_LOW);

	if (iResPin != 0)
	{
		LOGE("ModuleID %d : PinID %d disable Fail !\n", ModuleID, PinID);
		return -1;
	}
	return 0;
}


/******************************************************************************************/
/**
 * \fn			int User_PowerDown_Cb(void)
 *
 * \brief		Api for user to power down GPIO or module in standby mode
 *
 * \param
 *
 * \return	    \e  0 on success. \n
 *
 *
 ******************************************************************************************/
int User_PowerDown_Cb(void)
{
	LOGD("%s:user standby CB entered\n", __func__);

	User_Set_PIN_Power_Down(I2S_OUT_8CH, I2S_OUT_LRCK);
	User_Set_PIN_Power_Down(I2S_OUT_8CH, I2S_OUT_MCK);
	User_Set_PIN_Power_Down(I2S_OUT_8CH, I2S_OUT_BCK);
	User_Set_PIN_Power_Down(I2S_OUT_8CH, I2S_OUT_D0);
	User_Set_PIN_Power_Down(I2S_OUT_8CH, I2S_OUT_D1);
	User_Set_PIN_Power_Down(I2S_OUT_8CH, I2S_OUT_D2);
	User_Set_PIN_Power_Down(I2S_OUT_8CH, I2S_OUT_D3);

    User_Set_PIN_Power_Down(I2S_IN_2CH, I2S_IN_2CH_MCK);
    User_Set_PIN_Power_Down(I2S_IN_2CH, I2S_IN_2CH_BCK);
    User_Set_PIN_Power_Down(I2S_IN_2CH, I2S_IN_2CH_LRCK);
    User_Set_PIN_Power_Down(I2S_IN_2CH, I2S_IN_2CH_D0);

    User_Set_PIN_Power_Down(I2S_IN0_8CH, I2S_IN0_8CH_LRCK);
    User_Set_PIN_Power_Down(I2S_IN0_8CH, I2S_IN0_8CH_MCK);
    User_Set_PIN_Power_Down(I2S_IN0_8CH, I2S_IN0_8CH_BCK);
    User_Set_PIN_Power_Down(I2S_IN0_8CH, I2S_IN0_8CH_D0);
    User_Set_PIN_Power_Down(I2S_IN0_8CH, I2S_IN0_8CH_D1);
    User_Set_PIN_Power_Down(I2S_IN0_8CH, I2S_IN0_8CH_D2);
    User_Set_PIN_Power_Down(I2S_IN0_8CH, I2S_IN0_8CH_D3);

   // Vfd_Drv_Deinit();

	LOGD("%s:user standby CB exited\n", __func__);
	return 0;
}

/******************************************************************************************/
/**
 * \fn			void User_Disable_Device_Pwr(void)
 *
 * \brief		register a callback function to power down GPIO or module by user in standby mode
 *
 * \param		none\n
 *
 * \return	    none\n
 *
 * \note        \n
 *
 ******************************************************************************************/
void User_Disable_Device_Pwr(void)
{
	LOGD("%s: registering cb User_PowerDown_Cb\n", __func__);
	// register a callback function  to power down GPIO or modules
	PowerService_RegDisableDevicePwrFun(User_PowerDown_Cb);
}

/******************************************************************************************/
/**
 * \fn			int32_t User_PwrOnChk_Cb(uint8_t src)
 *
 * \brief		a callback function to check power
 *
 * \param		none\n
 *
 * \return	    if it returns a value > 0, system will be woken up
 *
 * \note        the content of this API is just an sample\n
 *
 ******************************************************************************************/
int32_t User_PwrOnChk_Cb(uint8_t src)
{
	int32_t res = 0;

    (void)src;
	if (GPIO_Intput_Read(pinnum) == 0)
		res = 1;

	return res;
}

/******************************************************************************************/
/**
 * \fn			void UserAppPwrUpChk(void)
 *
 * \brief		register a callback function to add a power on condition in standby mode
 *
 * \param		none\n
 *
 * \return	    none\n
 *
 * \note        the content of this API is just an sample\n
 *
 ******************************************************************************************/

int32_t UserApp_Standby_Polling(uint8_t src);

void User_PwrUpChk(void)
{
	LOGD("userapp PwrUpChk; \n");

	PowerService_RegPwrUpChkFun(UserApp_Standby_Polling);

	//pinnum = Pinmux_ModulePin_Find(POWERKEY, 0);
	//PowerService_RegPwrUpChkFun(User_PwrOnChk_Cb); // register a callback function  to be a power on condition  in standby mode
}

// open below POWERON_INIT to register a custom power check function
POWERON_INIT(User_PwrUpChk);

// open below POWERON_INIT to register a custom power down function
POWERON_INIT(User_Disable_Device_Pwr);

