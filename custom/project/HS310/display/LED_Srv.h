/******************************************************************************/
/**
*
* \file	 Led.h
*
* \brief	8107 Class-D led led .\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author       email@sunplus.com
*\version	 v0.01
*\date 	 2016/10/28
******************************************************************************/
#ifndef _LED_srv_H_
#define _LED_srv_H_

#include "qp_pub.h"
#include "Display_Srv.h"

enum ledshowtype
{
	eLED_null = 0,
	eLED_constant = 1,
	eLED_blink = 3,
	eLED_max,
};


typedef struct
{
	int led_id;
	uint16_t bright_time;
	uint16_t dark_time;
} stledinfo_t;


/*LED Display..........................................................................*/
typedef struct       /* the LED structure */
{
	QHsm super;
	stledinfo_t *stLedShowInfo; //determine led id and blink mode type information,if show type = constant,blink mode = 0
	int show_type;/* show type:blink , constant */
	QStateHandler LED_history; //use to get last led state
	uint8_t count; /* show counter */
} LEDDisplay;


typedef struct
{
	QEvt stEvt;
} LEDEvt;

typedef struct
{
	LEDEvt stType;
	LED_SHOW_ID show_id;
} LEDDisplayEvt;

typedef struct
{
	LEDEvt stType;

} LEDConstantEvt;

typedef struct
{
	LEDEvt stType;

} LEDDarkEvt;


typedef struct
{
	LEDEvt stType;
	uint16_t bright_time;
	uint16_t dark_time;
} LEDBlinkEvt;



void LEDDisplay_ctor(LEDDisplay *const me);

#endif // _LED_srv_H_

