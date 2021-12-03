/******************************************************************************/
/**
*
* \file	 VFD_Srv_API.c
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
#include "VFD_Srv.h"
#define LOG_MODULE_ID ID_VFD
#include "log_utils.h"
#include "custom_sig.h"

#if 1
extern void *memcpy(void *pDst, const void *pSrc, UINT32 uiLen);
extern void *memset(void *pDst, int iFillValue, UINT32 uiLen);

#define Time_Unit   60

Q_DEFINE_THIS_FILE


/******************************************************************************************/
/**
 * \fn			void VFD_Service_Show_Dark(void *  sender)
 *
 * \brief		api for user to show dark in 7-segment
 *
 * \param		void *  sender , char * string(INPUT)
 *
 * \return
 *
 *
 ******************************************************************************************/
void VFD_Service_Show_Dark(void   *sender)
{
	sender = sender ;

	LOGDT("****************VFD_Service_Show_Dark****************\n");

	VfdShowDarkEvt *e = Q_NEW(VfdShowDarkEvt, DISPLAY_VFD_SHOWDARK_SIG);

	QACTIVE_POST(DisplaySrv_get(),  (QEvt *)e, sender);
}

/******************************************************************************************/
/**
 * \fn			int VFD_Service_Display(void *  sender , char * string )
 *
 * \brief		api for user to get show string and send event
 *
 * \param		void *  sender , char * string(INPUT)
 *
 * \return		return 0:SUCCESS ; -1 :ERROR
 *
 *
 ******************************************************************************************/
int VFD_Service_Display(void   *sender, char *string )
{
	sender = sender ;

	LOGDT("****************Service_VFD_Display****************\n");

	VfdDisplayEvt *e = Q_NEW(VfdDisplayEvt, DISPLAY_VFD_DISPLAY_SIG);

	LOGDT("sizeof(e->Svfd_src) = %d...\n", sizeof(e->Svfd_src));
	LOGDT("strlen(string) = %d...\n", strlen(string));

	//if (strlen(string) > (sizeof(e->Svfd_src) - 1))
	//{
	//	return ERROR;
	//}

	//memset(e->Svfd_src, 0, sizeof(e->Svfd_src));
	//memcpy(e->Svfd_src, string, strlen(string));

	e->Svfd_src = string;

	LOGDT("Src name : %s....\n", e->Svfd_src);

	e->Ssrc_len = strlen(e->Svfd_src);

	LOGDT("String length : %d\n", e->Ssrc_len);

	QACTIVE_POST(DisplaySrv_get(),  (QEvt *)e, sender);

	return SUCCESS;
}

/******************************************************************************************/
/**
 * \fn			void VFD_Service_Align(void *  sender , eStringAlignType_t Align_Type)
 *
 * \brief		api for user to choose the string align type and send event
 *
 * \param		void *  sender , \n
 *              StringAlignType Align_Type(INPUT) : Align_Type(INPUT):0(left) 1(right) 2(center) 3(NONE)\n
 *
 * \return
 *
 *
 ******************************************************************************************/
void VFD_Service_Align(void   *sender, eStringAlignType_t Align_Type)
{
	sender = sender ;
	LOGDT("****************VFD_Service_Align****************\n");

	VfdAlignEvt *e = Q_NEW(VfdAlignEvt, DISPLAY_VFD_ALIGN_SIG);

	e->SAlign_Type = Align_Type;

	LOGDT("Align_Type : %d\n", e->SAlign_Type);

	QACTIVE_POST(DisplaySrv_get(),  (QEvt *)e, sender);
}

/******************************************************************************************/
/**
 * \fn			void VFD_Service_Constant(void *  sender)
 *
 * \brief		api for user to show string in constant type and send event
 *
 * \param		void *  sender
 *
 * \return
 *
 *
 ******************************************************************************************/
void VFD_Service_Constant(void   *sender)
{
	sender = sender ;
	LOGDT("****************VFD_Service_Constant****************\n");

	VfdConstantEvt *e = Q_NEW(VfdConstantEvt, DISPLAY_VFD_CONST_SIG);

	QACTIVE_POST(DisplaySrv_get(),  (QEvt *)e, sender);
}

/******************************************************************************************/
/**
 * \fn			void VFD_Service_Scroll (void *  sender  , eScrollmode_t scroll_mode)
 *
 * \brief		api for user to show string in scroll type and send event
 *
 * \param		void *  sender,\n
 *              scrollmode scroll_mode(INPUT) :0 (R2L) 1(L2R)
 * \return
 *
 *
 ******************************************************************************************/
void VFD_Service_Scroll (void   *sender, eScrollmode_t scroll_mode)
{
	sender = sender ;
	LOGDT("****************VFD_Service_Scroll****************\n");

	VfdScrollEvt *e = Q_NEW(VfdScrollEvt, DISPLAY_VFD_SCOLL_SIG);

	e->Sscroll_mode = scroll_mode;

	e->Sstr_offset = 0;

	QACTIVE_POST(DisplaySrv_get(),  (QEvt *)e, sender);
}

/******************************************************************************************/
/**
 * \fn			void VFD_Service_Blink (void *  sender , uint16_t bright_time ,uint16_t dark_time)
 *
 * \brief		api for user to show string in blink type and send event
 *
 * \param		void *  sender,\n
 *              uint16_t bright_time(INPUT)(unit 100ms) ; 1 = 100ms ; 10 = 1s ,
 *              uint16_t dark_time(INPUT)(unit 100ms)  ; 1 = 100ms ; 10 = 1s,
 * \return
 *
 *
 ******************************************************************************************/
void VFD_Service_Blink (void   *sender, uint16_t bright_time, uint16_t dark_time)
{
	sender = sender ;
	LOGDT("****************VFD_Service_Blink****************\n");

	VfdBlinkEvt *e = Q_NEW(VfdBlinkEvt, DISPLAY_VFD_BLINK_SIG);

	bright_time = (bright_time * 10);
	dark_time   = (dark_time * 10);

	e->Sbright_time = bright_time;
	e->Sdark_time = dark_time;

	if ((e->Sbright_time == 0) || (e->Sdark_time == 0))
	{
		LOGE("Blink parameter set error , cannot be zero!!!!\n");
		return ;
	}

	QACTIVE_POST(DisplaySrv_get(),  (QEvt *)e, sender);
}


/******************************************************************************************/
/**
 * \fn			int VFD_Service_POP (void *  sender ,  char * string , int time, eStringAlignType_t Align_Type)
 *
 * \brief		api for user to show string in pop type and send event
 *
 * \param		void *  sender,\n
 *              char * string(INPUT) : the show string,\n
 *              int time(INPUT) : the pop time(sec),\n
 *              StringAlignType Align_Type(INPUT):0(left), 1(right), 2(center), 3(NONE)\n
 * \return		return 0:SUCCESS ; -1 :ERROR
 *
 *
 ******************************************************************************************/
int VFD_Service_POP (void   *sender,  char *string, int time, eStringAlignType_t Align_Type)
{
	sender = sender ;
	LOGDT("****************VFD_Service_POP****************\n");

	VfdPopEvt *e = Q_NEW(VfdPopEvt, DISPLAY_VFD_POP_SIG);

	LOGDT("sizeof(e->Svfd_popsrc) = %d...\n", sizeof(e->Svfd_popsrc));

	LOGDT("strlen(string) = %d...\n", strlen(string));

	//if (strlen(string) > (sizeof(e->Svfd_popsrc) - 1))
	//{
	//	return ERROR;
	//}

	e->Stime = time ;

	e->SAlign_Type = Align_Type;

	//memset(e->Svfd_popsrc, 0, sizeof(e->Svfd_popsrc));

	//memcpy(e->Svfd_popsrc, string, strlen(string));

	e->Svfd_popsrc = string;

	LOGDT("POP str is %s...\n", e->Svfd_popsrc);

	LOGDT("POP Time : %d s...\n", e->Stime);

	QACTIVE_POST(DisplaySrv_get(),  (QEvt *)e, sender);

	return SUCCESS;
}

/******************************************************************************************/
/**
 * \fn			void VFD_Service_Play_Time(void *  sender , int sec)
 *
 * \brief		api for user to show file play time and send event
 *
 * \param		void *  sender,\n
 *              int sec(INPUT) : the play time,\n
 *
 *
 * \return
 *
 *
 ******************************************************************************************/
void VFD_Service_Play_Time(void   *sender, int sec)
{
	sender = sender ;
	LOGDT("****************VFD_Service_Play_Time****************\n");

	VfdPlayTimeEvt *e = Q_NEW(VfdPlayTimeEvt, DISPLAY_VFD_Play_Time_SIG);

	LOGDT("sec = %d...\n", sec);

	if (sec >= Time_Unit)
	{
		e->Ssecond = (sec % Time_Unit);
		e->Sminute = (sec / Time_Unit);
		e->Shour = 0;

		if (e->Sminute >= Time_Unit)
		{
			e->Shour = ((e->Sminute) / Time_Unit);
			e->Sminute = ((e->Sminute) % Time_Unit) ;
		}
	}
	else
	{
		e->Ssecond = sec;
		e->Sminute = 0;
		e->Shour = 0;
	}
	LOGDT("hour = %d...\n", e->Shour);

	LOGDT("minute = %d...\n", e->Sminute);

	LOGDT("second = %d...\n", e->Ssecond);

	QACTIVE_POST(DisplaySrv_get(),  (QEvt *)e, sender);
}


void VFD_Service_ScrollOne_Constant (void *  sender  , char *Const_vfd, eScrollmode_t scroll_mode, eShowOncemode_t back_mode, uint16_t bright_time, uint16_t dark_time)
{
	sender = sender ;
	LOGDT("****************VFD_Service_Scroll One****************\n");

	VfdScrollEvt *e = Q_NEW(VfdScrollEvt, DISPLAY_VFD_SCOLLONE_CONST_SIG);

	e->Sscroll_mode = scroll_mode;
	e->const_vfd = Const_vfd;
	e->Sstr_offset = 0;
	e->scrollover_back = back_mode;
	e->bright_time = bright_time*10;
	e->dark_time = dark_time*10;
		
	QACTIVE_POST(DisplaySrv_get(),  (QEvt *)e, sender);
	
}


void VFD_Service_BlinkOne_Constant(void *  sender  , char *Const_vfd, BYTE  blink_time, eShowOncemode_t back_mode,uint16_t once_bright_time ,uint16_t once_dark_time, uint16_t bright_time, uint16_t dark_time)
{
	sender = sender ;
	LOGDT("****************VFD_Service_blink One****************\n");

	VfdBlinkEvt *e = Q_NEW(VfdBlinkEvt, DISPLAY_VFD_BLINKONE_CONST_SIG);

	e->blink_time = blink_time;
	e->const_vfd = Const_vfd;
	e->blinkover_back = back_mode;
	e->Sbright_time = bright_time*10;
	e->Sdark_time = dark_time*10;
	e->once_bright_time = once_bright_time*10;
	e->once_dark_time = once_dark_time*10;	
	
	QACTIVE_POST(DisplaySrv_get(),  (QEvt *)e, sender);
	
}

int VFD_Service_Num2Str(char *strBuf, int num)
{
	if ((strBuf == NULL)
			|| (num > VFD_MAX_NUM)
	   )
	{
		return -1;
	}

	char NumBuf1[14] = {0};
	char NumBuf2[14] = {0};
	int i, j;
	int k = 0;
	int remainder;
	for (i = 0 ; i < 14 ; i++)
	{
		remainder = num % 10 ;
		NumBuf1[i] = '0' + (remainder - 0);
		num = num / 10 ;
		if (num == 0)
		{
			for (j = (i + 1) ; j < 14 ; j++)
			{
				NumBuf1[j] = 0x20;
			}
			break;
		}
	}

	for (i = 13 ; i >= 0 ; i--)
	{
		if (NumBuf1[i] != 0x20)
		{
			NumBuf2[k] = NumBuf1[i];
			k++;
		}
	}

	memcpy(strBuf, NumBuf2, strlen(NumBuf2) + 1);

	LOGD("strlen(strBuf):%d, %s\n ", strlen(strBuf), strBuf);
	return 0;
}

int VFD_DotCtrl(void *pSender, UINT8 Dot, UINT8 Op)
{
	VfdDotCtrlEvt_t *e = Q_NEW(VfdDotCtrlEvt_t, DISPLAY_VFD_DOT_SIG);

	e->DotType = Dot;
	e->Op = Op;

	QACTIVE_POST(DisplaySrv_get(), (QEvt *)e, pSender);

	return 0;
}

#endif


