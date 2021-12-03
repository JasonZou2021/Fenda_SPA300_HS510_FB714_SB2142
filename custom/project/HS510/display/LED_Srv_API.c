/******************************************************************************/
/**
*
* \file	 LED_Srv_API.c
*
* \brief	This file provides the vfd user api \n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*\author       email@sunplus.com
*\version	 v0.01
*\date 	 2016/08/9
******************************************************************************/
#include <string.h>
#include "qpc.h"
#define LOG_MODULE_ID ID_VFD
#include "log_utils.h"
#include "custom_sig.h"
#include "LED_Srv.h"

Q_DEFINE_THIS_FILE

/******************************************************************************************/
/**
 * \fn			int LED_Service_Display(void *  sender , char * string )
 *
 * \brief		api to get led id which user set
 *
 * \param		void *  sender , LED_SHOW_ID led_id
 *
 * \return		return 0:SUCCESS ; -1 :ERROR
 *
 *
 ******************************************************************************************/
int LED_Service_Display(void   *sender, LED_SHOW_ID led_id )
{
	sender = sender ;

	LOGD("****************LED_Service_Display****************\n");

	LEDDisplayEvt *e = Q_NEW(LEDDisplayEvt, DISPLAY_LED_DISPLAY_SIG);

	e->show_id = led_id;

	LOGD("LED ID number : %d....\n", e->show_id);

	QACTIVE_POST(DisplaySrv_get(),  (QEvt *)e, sender);

	return SUCCESS;

}


/******************************************************************************************/
/**
 * \fn			void LED_Service_Show_Dark(void *  sender)
 *
 * \brief		api for user to show led in dark mode
 *
 * \param		void *  sender , char * string(INPUT)
 *
 * \return
 *
 *
 ******************************************************************************************/
void LED_Service_Show_Dark(void   *sender)
{
	sender = sender ;

	LOGD("****************LED_Service_Dark****************\n");

	LEDDarkEvt *e = Q_NEW(LEDDarkEvt, DISPLAY_LED_SHOWDARK_SIG);

	QACTIVE_POST(DisplaySrv_get(),  (QEvt *)e, sender);

}


/******************************************************************************************/
/**
 * \fn			void LED_Service_Constant(void *  sender)
 *
 * \brief		api for user to show led in constant mode
 *
 * \param		void *  sender\n
 *
 * \return
 *
 *
 ******************************************************************************************/
void LED_Service_Constant(void   *sender)
{
	sender = sender ;
	LOGD("****************LED_Service_Constant****************\n");

	LEDConstantEvt *e = Q_NEW(LEDConstantEvt, DISPLAY_LED_CONST_SIG);

	QACTIVE_POST(DisplaySrv_get(),  (QEvt *)e, sender);

}



/******************************************************************************************/
/**
 * \fn			void LED_Service_Blink (void *  sender , uint16_t bright_time ,uint16_t dark_time)
 *
 * \brief		api for user to show led in blink mode
 *
 * \param		void *  sender,\n
 *              uint16_t bright_time(INPUT)(unit 100ms) ; 1 = 100ms ; 10 = 1s ,
 *              uint16_t dark_time(INPUT)(unit 100ms)  ; 1 = 100ms ; 10 = 1s,
 * \return
 *
 *
 ******************************************************************************************/
void LED_Service_Blink (void   *sender, uint16_t bright_time, uint16_t dark_time)
{
	sender = sender ;
	LOGD("****************LED_Service_Blink****************\n");

	LEDBlinkEvt *e = Q_NEW(LEDBlinkEvt, DISPLAY_LED_BLINK_SIG);

	bright_time = (bright_time * 10);
	dark_time   = (dark_time * 10);

	e->bright_time = bright_time;
	e->dark_time = dark_time;

	if ((e->bright_time == 0) || (e->dark_time == 0))
	{
		LOGE("Blink parameter set error , cannot be zero!!!!\n");
		return ;
	}

	QACTIVE_POST(DisplaySrv_get(),  (QEvt *)e, sender);

}


