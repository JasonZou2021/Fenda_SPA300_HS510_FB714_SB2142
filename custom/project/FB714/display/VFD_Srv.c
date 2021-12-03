/******************************************************************************/
/**
*
* \file	 VFD_Srv.c
*
* \brief	This file provides the vfd middleware state machine framework \n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*\author       email@sunplus.com
*\version	 v0.01
*\date 	 2016/08/8
******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "qpc.h"
#include "VFD_Srv.h"
#define LOG_MODULE_ID ID_VFD
#include "log_utils.h"
#include "custom_sig.h"
#include "user_def.h"
#include "vfd_seg_map.h"

Q_DEFINE_THIS_FILE

#if 1
stinfo_t showinfo ;
stinfo_t popshowinfo ;


/* hierarchical state machine ... */
QState VfdDisplay_initial(VfdDisplay *const me, QEvt const *const e);
QState VfdDisplay_active(VfdDisplay *const me, QEvt const *const e);
QState VfdDisplay_const(VfdDisplay *const me, QEvt const *const e);
QState VfdDisplay_scroll(VfdDisplay *const me, QEvt const *const e);
QState VfdDisplay_scrollone_const(VfdDisplay * const me, QEvt const * const e); 
QState VfdDisplay_blinkone_const(VfdDisplay * const me, QEvt const * const e); 
QState VfdDisplay_blink(VfdDisplay *const me, QEvt const *const e);
QState VfdDisplay_pop(VfdDisplay *const me, QEvt const *const e);
QState VfdDisplay_PlayTime(VfdDisplay *const me, QEvt const *const e);
QActive *UserApp_get(void);

extern void *memcpy(void *pDst, const void *pSrc, UINT32 uiLen);
extern void *memset(void *pDst, int iFillValue, UINT32 uiLen);

#define Space_Char   0x20

char *PlayTime_str[] =
{
	"00",
	"01",
	"02",
	"03",
	"04",
	"05",
	"06",
	"07",
	"08",
	"09",
	"10",
	"11",
	"12",
	"13",
	"14",
	"15",
	"16",
	"17",
	"18",
	"19",
	"20",
	"21",
	"22",
	"23",
	"24",
	"25",
	"26",
	"27",
	"28",
	"29",
	"30",
	"31",
	"32",
	"33",
	"34",
	"35",
	"36",
	"37",
	"38",
	"39",
	"40",
	"41",
	"42",
	"43",
	"44",
	"45",
	"46",
	"47",
	"48",
	"49",
	"50",
	"51",
	"52",
	"53",
	"54",
	"55",
	"56",
	"57",
	"58",
	"59",
};


/******************************************************************************************/
/**
 * \fn			void VfdDisplay_ctor(VfdDisplay * const me)
 *
 * \brief		initial the vfd AO paramater
 *
 * \param		VfdDisplay * const me
 *
 * \return
 *
 *
 ******************************************************************************************/

void VfdDisplay_ctor(VfdDisplay *const me)
{
	LOGD("VfdDisplay_ctor ENTRY; \n");
	QHsm_ctor(&me->super, Q_STATE_CAST(&VfdDisplay_initial));
}



/******************************************************************************************/
/**
 * \fn			QState VfdDisplay_initial(VfdDisplay * const me, QEvt const * const e)
 *
 * \brief		initial the vfd AO paramater
 *
 * \param		VfdDisplay * const me , QEvt const * const e
 *
 * \return		\e transform to VfdDisplay_active \n
 *
 *
 ******************************************************************************************/

QState VfdDisplay_initial(VfdDisplay *const me, QEvt const *const e)
{
	(void)e; /* avoid compiler warning about unused parameter */
	LOGD("VfdDisplay_initial Entry; \n");
	me->showinfo = &showinfo;
	me->popshowinfo = &popshowinfo;
	me->count = 0;
	me->show_type = eVfd_null;
	return Q_TRAN(&VfdDisplay_active);
}

/******************************************************************************************/
/**
 * \fn			QState VfdDisplay_active(VfdDisplay * const me, QEvt const * const e)
 *
 * \brief		the state which transform to other sub_status by different SIG
 *
 * \param		VfdDisplay * const me:(Input) the vfd handler here,\n
 *              QEvt const * const e :event structure of signal \n
 *
 * \return		\e status : Q_HANDLED() in active state \n
 *			    \e status : Q_TRAN() trans to other sub_state \n
 *              \e status : Q_SUPER() trans to father state \n
 ******************************************************************************************/

QState VfdDisplay_active(VfdDisplay *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGD("VfdDisplay_active ENTRY; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGD("VfdDisplay_active EXIT; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			LOGD("VfdDisplay_active INIT; \n");
			status = Q_HANDLED();
			break;
		}
		case DISPLAY_VFD_SHOWDARK_SIG:
		{
			LOGDT("VVfdDisplay_active: to show dark \n");
			VFD_SHOW_DARK();
			status = Q_HANDLED();
			break;
		}
		case DISPLAY_VFD_DISPLAY_SIG:
		{
			LOGDT("VfdDisplay_active:to Vfd_Display\n");
			VfdDisplayEvt *pVFDDisplayEvt = ((VfdDisplayEvt *)e) ;
			VFD_Display(me, pVFDDisplayEvt->Svfd_src, pVFDDisplayEvt->Ssrc_len);
			status = Q_HANDLED();
			break;
		}
		case DISPLAY_VFD_ALIGN_SIG:
		{
			LOGDT("VfdDisplay_active:to Vfd_Align\n");
			int Align_Check;
			VfdAlignEvt *pVFDAlignEvt = ((VfdAlignEvt *)e) ;
			Align_Check = VFD_Align(me, pVFDAlignEvt->SAlign_Type);
			if (Align_Check != 0)
			{
				LOGE("VfdDisplay_active: Align Fail...\n");
				status = Q_HANDLED();
			}
			else
			{
				status = Q_HANDLED();
			}
			break;
		}
		case DISPLAY_VFD_CONST_SIG:
		{
			LOGDT("VfdDisplay_active:to VfdDisplay_const\n");
			status = Q_TRAN(&VfdDisplay_const);
			break;
		}
		case DISPLAY_VFD_SCOLL_SIG:
		{
			LOGDT("VfdDisplay_active: to VfdDisplay_scroll\n");
			int Scroll_Check;
			VfdScrollEvt *pVFDScrollEvt = ((VfdScrollEvt *)e);
			Scroll_Check = VFD_Scroll(me, pVFDScrollEvt->Sscroll_mode, pVFDScrollEvt->Sstr_offset);
			if (Scroll_Check != 0)
			{
				LOGE("VfdDisplay_active: Scroll Fail...\n");
				status = Q_HANDLED();
			}
			else
			{
				status = Q_TRAN(&VfdDisplay_scroll);
			}
			break;
		}
	case DISPLAY_VFD_BLINKONE_CONST_SIG:
		{
		 

			VfdBlinkEvt *pVFDBlinkEvt = ((VfdBlinkEvt *)e);

			LOGDT("VfdDisplay_active: to VfdDisplay_scroll One Const  %d- %d- %d\n", pVFDBlinkEvt->blinkover_back, pVFDBlinkEvt->Sbright_time,pVFDBlinkEvt->Sdark_time);
			
			VFD_Blink_One_Const(me, pVFDBlinkEvt->const_vfd, pVFDBlinkEvt->blinkover_back, pVFDBlinkEvt->blink_time,pVFDBlinkEvt->once_bright_time, pVFDBlinkEvt->once_dark_time,pVFDBlinkEvt->Sbright_time,pVFDBlinkEvt->Sdark_time);
     
                status = Q_TRAN(&VfdDisplay_blinkone_const);
            
		break;
	}
		
	case DISPLAY_VFD_SCOLLONE_CONST_SIG:{
		 
		 int Scroll_Check;
			VfdScrollEvt *pVFDScrollEvt = ((VfdScrollEvt *)e);

			LOGDT("VfdDisplay_active: to VfdDisplay_scroll One Const  %d- %d- %d\n", pVFDScrollEvt->scrollover_back, pVFDScrollEvt->bright_time,pVFDScrollEvt->dark_time);
			
			Scroll_Check = VFD_Scroll_One_Const(me, pVFDScrollEvt->Sscroll_mode, pVFDScrollEvt->Sstr_offset, pVFDScrollEvt->const_vfd, pVFDScrollEvt->scrollover_back, pVFDScrollEvt->bright_time,pVFDScrollEvt->dark_time);
            if(Scroll_Check != 0)
            {
                //disp_printf_E("VfdDisplay_active: Scroll Fail...\n");
                status = Q_HANDLED();
            }
            else
            {
                status = Q_TRAN(&VfdDisplay_scrollone_const);
            }
		break;
	}
		case DISPLAY_VFD_BLINK_SIG:
		{
			LOGDT("VfdDisplay_active: to VfdDisplay_blink\n");
			VfdBlinkEvt *pVFDBlinkEvt = ((VfdBlinkEvt *)e);
			VFD_Blink(me, pVFDBlinkEvt->Sbright_time, pVFDBlinkEvt->Sdark_time);
			status = Q_TRAN(&VfdDisplay_blink);
			break;
		}
		case DISPLAY_VFD_Play_Time_SIG:
		{
			LOGDT("VfdDisplay_active : show Play Time\n");
			VfdPlayTimeEvt *pVFDPlayTimeEvt = ((VfdPlayTimeEvt *)e);
			VFD_PlayTime(me, pVFDPlayTimeEvt->Sminute, pVFDPlayTimeEvt->Ssecond, pVFDPlayTimeEvt->Shour);
			status = Q_TRAN(&VfdDisplay_PlayTime);
			break;
		}
		case DISPLAY_VFD_POP_SIG:
		{
			LOGDT("VfdDisplay_active:to VfdDisplay_POP\n");
			int Pop_Check;
			VfdPopEvt *pVFDPopEvt = ((VfdPopEvt *)e);
			Pop_Check = VFD_POP(me, pVFDPopEvt->Svfd_popsrc, pVFDPopEvt->Stime, pVFDPopEvt->SAlign_Type);
			if (Pop_Check != 0)
			{
				LOGE("VfdDisplay_active: Pop Fail...\n");
				status = Q_HANDLED();
			}
			else
			{
				status = Q_TRAN(&VfdDisplay_pop);
			}
			break;
		}
		case DISPLAY_VFD_DOT_SIG:
		{
			VfdDotCtrlEvt_t *pstVfdDotCtrlEvt = (VfdDotCtrlEvt_t *)e;
			LOGD("DISPLAY_VFD_DOT_SIG: %d, %d\n", pstVfdDotCtrlEvt->DotType, pstVfdDotCtrlEvt->Op);
			Vfd_Set_Dot(pstVfdDotCtrlEvt->DotType, pstVfdDotCtrlEvt->Op);//clear show time dot2
			status = Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&QHsm_top);
			break;
		}
	}
	return status;
}


/******************************************************************************************/
/**
 * \fn			QState VfdDisplay_const(VfdDisplay * const me, QEvt const * const e)
 *
 * \brief		the state which vfd segment display show constant style
 *
 * \param		VfdDisplay * const me:(Input) the vfd handler here,\n
 *              QEvt const * const e :event structure of signal \n
 *
 * \return		\e status : Q_HANDLED() in const state \n
 *			    \e status : Q_SUPER() trans to active state \n
 *
 ******************************************************************************************/
QState VfdDisplay_const(VfdDisplay *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGDT("VfdDisplay_const ENTRY; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGDT("VfdDisplay_const EXIT; \n");
			me->vfd_history = Q_STATE_CAST(&VfdDisplay_const);
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			LOGDT("VfdDisplay_const INIT; \n");
			me->show_type = eVfd_constant;
			VFD_SHOW_STRING(me->showinfo->vfd_align_src);
			status = Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&VfdDisplay_active);
			break;
		}
	}
	return status;
}

/******************************************************************************************/
/**
 * \fn			QState VfdDisplay_PlayTime(VfdDisplay * const me, QEvt const * const e)
 *
 * \brief		the state which vfd segment display show file play time
 *
 * \param		VfdDisplay * const me:(Input) the vfd handler here,\n
 *              QEvt const * const e :event structure of signal \n
 *
 * \return		\e status : Q_HANDLED() in PlayTime state \n
 *			    \e status : Q_SUPER() trans to active state \n
 *
 ******************************************************************************************/

QState VfdDisplay_PlayTime(VfdDisplay *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGDT("VfdDisplay_PlayTime ENTRY; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGDT("VfdDisplay_PlayTime EXIT; \n");
			Vfd_Set_Dot(VFD_DOT_COLON1, VFD_DOT_OP_CLR);//clear show time dot1
			Vfd_Set_Dot(VFD_DOT_COLON2, VFD_DOT_OP_CLR);//clear show time dot2
			me->vfd_history = Q_STATE_CAST(&VfdDisplay_PlayTime);
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			LOGDT("VfdDisplay_PlayTime INIT; \n");
			me->show_type = eVfd_playtime;
			VFD_SHOW_STRING(me->PlayTime);
			Vfd_Set_Dot(VFD_DOT_COLON1, VFD_DOT_OP_SET);//set show time dot1
			Vfd_Set_Dot(VFD_DOT_COLON2, VFD_DOT_OP_SET);//set show time dot2
			status = Q_HANDLED();
			break;
		}
		case DISPLAY_VFD_Play_Time_SIG:
		{
			LOGDT("VfdDisplay_PlayTime : show Play Time\n");
			VfdPlayTimeEvt *pVFDPlayTimeEvt = ((VfdPlayTimeEvt *)e);
			VFD_PlayTime(me, pVFDPlayTimeEvt->Sminute, pVFDPlayTimeEvt->Ssecond, pVFDPlayTimeEvt->Shour);
			VFD_SHOW_STRING(me->PlayTime);
			status = Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&VfdDisplay_active);
			break;
		}
	}
	return status;
}

/******************************************************************************************/
/**
 * \fn			QState VfdDisplay_scroll(VfdDisplay * const me, QEvt const * const e)
 *
 * \brief		the state which vfd segment display show scroll style
 *
 * \param		VfdDisplay * const me:(Input) the vfd handler here,\n
 *              QEvt const * const e :event structure of signal \n
 *
 * \return		\e status : Q_HANDLED() in scroll state \n
 *			    \e status : Q_SUPER() trans to active state \n
 *
 ******************************************************************************************/

QState VfdDisplay_scroll(VfdDisplay *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGDT("VfdDisplay_scroll ENTRY; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGDT("VfdDisplay_scroll EXIT; \n");
			me->vfd_history = Q_STATE_CAST(&VfdDisplay_scroll);
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			LOGDT("VfdDisplay_scroll INIT; \n");
			me->show_type = eVfd_scroll;
			status = Q_HANDLED();
			break;
		}
		case DISPLAY_VFD_COUNTER_SIG:
		{
			LOGDT("VfdDisplay scrolling.....\n");

			me->count++;
			if (me->count % 5 == 0)
			{
				VFD_SCROLLING(me);

				me->showinfo->str_offset++;

				LOGDT("TTTme->showinfo->str_offset = %d \n", me->showinfo->str_offset);
				LOGDT("TTTme->showinfo->Align_Len = %d \n", me->showinfo->Align_Len);

				if (me->showinfo->str_offset >= me->showinfo->Align_Len)
				{
					me->showinfo->str_offset = 0;
				}
				me->count = 0;
			}

			status = Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&VfdDisplay_active);
			break;
		}
	}
	return status;
}


QState VfdDisplay_scrollone_const(VfdDisplay * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            LOGDT("VfdDisplay_scroll ENTRY; \n");
            status = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG: {
            LOGDT("VfdDisplay_scroll EXIT; \n");
            me->vfd_history = Q_STATE_CAST(&VfdDisplay_scrollone_const);
            status = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            LOGDT("VfdDisplay_scroll INIT; \n");
			me->show_type = eVfd_scrollone_const;

			VFD_SHOW_STRING(me->showinfo->vfd_align_src);
			
            status = Q_HANDLED();
            break;
        }		
        case DISPLAY_VFD_COUNTER_SIG: {			
            LOGDT("VfdDisplay scrolling once const.....\n");

			me->count++;
			if(me->count % 3 == 0)
			{             
				me->count = 0;

				if(me->showinfo->str_offset >= me->showinfo->Align_Len)  
				{
					me->showinfo->str_offset = 0;

					if(me->showinfo->once_back_const_vfd != NULL)
					{
						memset(me->showinfo->vfd_align_src, 0, sizeof(me->showinfo->vfd_align_src));
						if(strlen(me->showinfo->once_back_const_vfd) > sizeof(me->showinfo->vfd_align_src))
							memcpy(me->showinfo->vfd_align_src, me->showinfo->once_back_const_vfd, sizeof(me->showinfo->vfd_align_src));	
						else		
							memcpy(me->showinfo->vfd_align_src, me->showinfo->once_back_const_vfd, strlen(me->showinfo->once_back_const_vfd));
					}
					
					LOGDT("scroll once back -%d.....\n", me->showinfo->once_back_mode);


					if(me->showinfo->once_back_mode ==  BACK_CONST)
						status = Q_TRAN(&VfdDisplay_const);
					else	 if(me->showinfo->once_back_mode == BACK_BLINK)	
						status = Q_TRAN(&VfdDisplay_blink);
					else if(me->showinfo->once_back_mode == BACK_SCROLL)	
						status = Q_TRAN(&VfdDisplay_scroll);
					else 
					{
						LOGDT("usb scroll over\n");
						me->show_type = eVfd_constant;
						QEvt *evt = Q_NEW(QEvt, SCROLL_BLINK_ONE_EXIT_SIG);
						QACTIVE_POST(UserApp_get(),  evt, (void *)0);
						status = Q_HANDLED();
					}
					
					break;
					
				}

				if(me->show_type != eVfd_constant)
				{
					VFD_SCROLLING_ONCE(me);
					me->showinfo->str_offset++;
				}
				
                		LOGDT("TTTme->showinfo->str_offset = %d \n",me->showinfo->str_offset);
                		LOGDT("TTTme->showinfo->Align_Len = %d \n",me->showinfo->Align_Len);                
                
				
			}
			
            status = Q_HANDLED();
            break;
        }             
        default: {            
            status = Q_SUPER(&VfdDisplay_active);
            break;
        }
    }
    return status;
}

QState VfdDisplay_blinkone_const(VfdDisplay * const me, QEvt const * const e)
{
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            LOGDT("VfdDisplay_blinklone_const ENTRY; \n");
            status = Q_HANDLED();
            break;
        }
        case Q_EXIT_SIG: {
            LOGDT("VfdDisplay_blinklone_const EXIT; \n");
            me->vfd_history = Q_STATE_CAST(&VfdDisplay_blinkone_const);
            status = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            LOGDT("VfdDisplay_blinklone_const INIT; \n");
			me->show_type = eVfd_blinkone_const;
			 me->count = 0;
            status = Q_HANDLED();
            break;
        }		
        case DISPLAY_VFD_COUNTER_SIG: {
            LOGDT("VfdDisplay blinking......\n");

			if(me->showinfo->blink_time > 0)
			{

		            if((me->count) == 0)
		            {
		                LOGDT("bright start\n");
		                  VFD_SHOW_DARK();
		            }
		            else if(((me->count) == (me->showinfo->blink_once_bright_time / TICKS_PER_100MS)))
		            {
		                LOGDT("dark start\n");
		                 VFD_SHOW_STRING(me->showinfo->vfd_align_src);
		            }
					me->count++;

		            if(((me->count) > ((me->showinfo->blink_once_bright_time + me->showinfo->blink_once_dark_time) / TICKS_PER_100MS)))
		            {
		                LOGDT("me->count:%d\n",me->count);
		                me->count = 0;

					me->showinfo->blink_time--;
					if(me->showinfo->blink_time == 0)
					{
						memset(me->showinfo->vfd_align_src, 0, sizeof(me->showinfo->vfd_align_src));
						memcpy(me->showinfo->vfd_align_src, me->showinfo->once_back_const_vfd, strlen(me->showinfo->once_back_const_vfd));
					
						if(me->showinfo->once_back_mode == BACK_CONST)
							status = Q_TRAN(&VfdDisplay_const);
						else	 if(me->showinfo->once_back_mode == BACK_BLINK)	
							status = Q_TRAN(&VfdDisplay_blink);
						else if(me->showinfo->once_back_mode == BACK_SCROLL)	
							status = Q_TRAN(&VfdDisplay_scroll);
						else 
						{
							LOGDT("usb blink over\n");
							me->show_type = eVfd_constant;
							QEvt *evt = Q_NEW(QEvt, SCROLL_BLINK_ONE_EXIT_SIG);
							QACTIVE_POST(UserApp_get(),  evt, (void *)0);
							status = Q_HANDLED();
						}

						break;
					}
		            }

					

			}

            status = Q_HANDLED();
            break;
        }             
        default: {            
			status = Q_SUPER(&VfdDisplay_active);
			break;
		}
	}
	return status;
}

/******************************************************************************************/
/**
 * \fn			QState VfdDisplay_blink(VfdDisplay * const me, QEvt const * const e)
 *
 * \brief		the state which vfd segment display show blink style
 *
 * \param		VfdDisplay * const me:(Input) the vfd handler here,\n
 *              QEvt const * const e :event structure of signal \n
 *
 * \return		\e status : Q_HANDLED() in blink state \n
 *			    \e status : Q_SUPER() trans to active state \n
 *
 ******************************************************************************************/

QState VfdDisplay_blink(VfdDisplay *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGDT("VfdDisplay_blink ENTRY; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGDT("VfdDisplay_blink EXIT; \n");
			me->vfd_history = Q_STATE_CAST(&VfdDisplay_blink);
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			LOGDT("VfdDisplay_blink INIT; \n");
			me->show_type = eVfd_blink;
			me->count = 0;
			status = Q_HANDLED();
			break;
		}
		case DISPLAY_VFD_COUNTER_SIG:
		{
			LOGDT("VfdDisplay blinking......\n");

			if ((me->count) == 0)
			{
				LOGDT("bright start\n");
				VFD_SHOW_STRING(me->showinfo->vfd_align_src);
			}
			else if (((me->count) == (me->showinfo->bright_time / TICKS_PER_100MS)))
			{
				LOGDT("dark start\n");
				VFD_SHOW_DARK();
			}
			me->count++;

			if (((me->count) > ((me->showinfo->bright_time + me->showinfo->dark_time) / TICKS_PER_100MS)))
			{
				LOGDT("me->count:%d\n", me->count);
				me->count = 0;
			}


			status = Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&VfdDisplay_active);
			break;
		}
	}
	return status;
}

/******************************************************************************************/
/**
 * \fn			QState VfdDisplay_pop(VfdDisplay * const me, QEvt const * const e)
 *
 * \brief		the state which vfd segment display show pop style
 *
 * \param		VfdDisplay * const me:(Input) the vfd handler here,\n
 *              QEvt const * const e :event structure of signal \n
 *
 * \return		\e status : Q_HANDLED() in blink state \n
 *			    \e status : Q_SUPER() trans to active state \n
 *              \e status : Q_TRAN_HIST() trans to previous state \n
 ******************************************************************************************/

QState VfdDisplay_pop(VfdDisplay *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGDT("VfdDisplay_pop ENTRY; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGDT("VfdDisplay_pop EXIT; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			LOGDT("VfdDisplay_pop INIT; \n");
			me->show_type = eVfd_pop ;
			me->count = 0;
			status = Q_HANDLED();
			break;
		}
		case DISPLAY_VFD_COUNTER_SIG:
		{
			//LOGDT("VfdDisplay POPing...\n");

			me->count++;

			VFD_SHOW_STRING(me->popshowinfo->vfd_align_src);

			if ((me->count) >= ((me->popshowinfo->time) * 10))
			{
				me->count = 0;
				if (me->vfd_history == NULL)
				{
					LOGE("me->vfd_history is NULL ,plz check...\n");
					status = Q_HANDLED();
				}
				else
				{
					status = Q_TRAN_HIST(me->vfd_history);
				}
			}
			else
			{
				status = Q_HANDLED();
			}
			break;
		}
		case DISPLAY_VFD_Play_Time_SIG:
		{
			LOGDT("VfdDisplay_pop : DISPLAY_VFD_Play_Time_SIG \n");
			status = Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&VfdDisplay_active);
			break;
		}
	}
	return status;
}


/******************************************************************************************/
/**
 * \fn			void VFD_SHOW_STRING(char * string)
 *
 * \brief		show vfd string
 *
 * \param		string : (INPUT) : source name after align
 *
 * \return
 *
 *
 ******************************************************************************************/

void VFD_SHOW_STRING(char *string)
{
	LOGDT("VFD_SHOW_STRING in ..\n");
	int istrchk;
	istrchk = Vfd_Set_Str(string);
	if (istrchk != 0)
		LOGE("Show String Fail...in show string\n");
	return ;
}

/******************************************************************************************/
/**
 * \fn			void VFD_SHOW_DARK(void)
 *
 * \brief		show dark style in vfd display
 *
 * \param		null
 *
 * \return
 *
 *
 ******************************************************************************************/

void VFD_SHOW_DARK(void)
{
	LOGDT("VFD_SHOW_DARK in ..\n");

	int Vfd_Srv_ShowDark_7_Seq_Len = -1;

	Vfd_Srv_ShowDark_7_Seq_Len = Vfd_Get_Seven_Seq_Len();

	char Null_String[Vfd_Srv_ShowDark_7_Seq_Len];
	int j;
	int istrchk;
	for (j = 0 ; j < Vfd_Srv_ShowDark_7_Seq_Len ; j++) //clean array Null_String value
		Null_String[j] = '\0';

	for (j = 0 ; j < Vfd_Srv_ShowDark_7_Seq_Len ; j++)
		Null_String[j] = Space_Char;

	istrchk = Vfd_Set_Str(Null_String);
	if (istrchk != 0)
		LOGE("Show String Fail...in show dark\n");
	return ;
}

/******************************************************************************************/
/**
 * \fn			int VFD_SCROLLING(VfdDisplay * const me)
 *
 * \brief		handle the string for scroll style
 *
 * \param		VfdDisplay * const me
 *
 * \return		\e 0 on success. \n
 *			    \e -1 on scrolling mode fail. \n
 *
 ******************************************************************************************/
int VFD_SCROLLING(VfdDisplay *const me)
{
	/************************************************************************************/

	//every 0.5 sec ,this func will run malloc , easy to make heap broken ,so use fixed memory
	/*
	    char* StrBuf = NULL;
	    StrBuf = (char *) malloc((me->showinfo->Align_Len + 1)*(sizeof(char)));

	    if(StrBuf == NULL)
	    {
	       dislpay_printf("malloc fail..\n");
	       return -1;
	    }
	*/
	/************************************************************************************/
	LOGDT("VFD_SCROLLING in \n");
	int i, j;

	LOGDT("me->showinfo->scroll_len = %d \n", me->showinfo->Align_Len);

	char StrBuf[VFD_STR_MAX] = {0};

	for (j = 0 ; j <= me->showinfo->Align_Len ; j++) //clean array StrBuf value
		StrBuf[j] = '\0';

	for (j = 0 ; j < me->showinfo->Align_Len ; j++)
		StrBuf[j] = Space_Char;


	if (me->showinfo->scrolling_mode == R2L)
	{
		for (j = 0 ; j < me->showinfo->Align_Len ; j++ )
		{
			i = (me->showinfo->str_offset + j) % (me->showinfo->Align_Len) ;

			StrBuf[j] = me->showinfo->vfd_align_src[i];
		}
		LOGDT("StrBuf = %s \n", StrBuf);
		VFD_SHOW_STRING(StrBuf);
	}
	else if (me->showinfo->scrolling_mode == L2R)
	{
		for (i = 0 ; i < me->showinfo->Align_Len ; i++ )
		{
			j = (me->showinfo->str_offset + i) % (me->showinfo->Align_Len) ;

			StrBuf[j] = me->showinfo->vfd_align_src[i];
		}

		VFD_SHOW_STRING(StrBuf);
	}
	else
	{
		LOGE("scrolling mode error \n");
		return -1;
	}

	//free(StrBuf);

	return 0;
}


int VFD_SCROLLING_ONCE(VfdDisplay * const me)
{

   //disp_printf1("VFD_SCROLLING in \n");
    int i,j;

    //disp_printf1("me->showinfo->scroll_len = %d \n",me->showinfo->Align_Len);

    char StrBuf[VFD_STR_MAX] = {0};

    for(j=0 ; j <= me->showinfo->Align_Len ; j++) //clean array StrBuf value
    	StrBuf[j] = '\0';

    for(j=0 ; j < me->showinfo->Align_Len ; j++)
    	StrBuf[j] = Space_Char;


    if(me->showinfo->scrolling_mode == R2L)
    {
    		for(j = 0 ; j < me->showinfo->Align_Len ; j++ )
			StrBuf[j] = me->showinfo->vfd_align_src[me->showinfo->str_offset+j];

		//disp_printf1("StrBuf = %s \n",StrBuf);
		VFD_SHOW_STRING(StrBuf);
			
    }
    else if (me->showinfo->scrolling_mode == L2R)
    {
		for(i = 0 ; i < me->showinfo->Align_Len ; i++ )
		{
			j = (me->showinfo->str_offset + i) % (me->showinfo->Align_Len) ;	
			
			StrBuf[j] = me->showinfo->vfd_align_src[i];
		}
    	
    		VFD_SHOW_STRING(StrBuf);
    }
    else
    {
    		LOGE("scrolling once mode error \n");
    		return -1;
    }

    //free(StrBuf);
    
	return 0;
}




/******************************************************************************************/
/**
 * \fn			void VFD_Display(VfdDisplay * const me, char *vfd_src , int src_len)
 *
 * \brief		get the string pass from VFD_Service_Display
 *
 * \param		VfdDisplay * const me , vfd_src : the source name , src_len : the string length
 *
 * \return
 *
 *
 ******************************************************************************************/

void VFD_Display(VfdDisplay *const me, char *vfd_src, int src_len)
{
	LOGDT("VFD_Display in\n");

	LOGDT("sizeof(me->showinfo->vfd_src) : %d....\n", sizeof(me->showinfo->vfd_src));

	memset(me->showinfo->vfd_src, 0, sizeof(me->showinfo->vfd_src));

	memcpy(me->showinfo->vfd_src, vfd_src, strlen(vfd_src));

	LOGDT("Src String name : %s....\n", me->showinfo->vfd_src);

	me->showinfo->src_len = src_len;

	LOGDT("Src String length : %d\n", me->showinfo->src_len);

	return;
}

/******************************************************************************************/
/**
 * \fn			int VFD_Align(VfdDisplay * const me , int Align_Type)
 *
 * \brief		align the string
 *
 * \param		VfdDisplay * const me , Align_Type(INPUT):0(left) 1(right) 2(center) 3(NONE)
 *
 * \return		\e 0 on success. \n
 *			    \e -1 on Align_Type error. \n
 *
 ******************************************************************************************/

int VFD_Align(VfdDisplay *const me, int Align_Type)
{

	char tmpBuf [VFD_STR_MAX] = {0};
	int cnt;
	int align_offset ;
	int Vfd_Srv_Align_7_Seq_Len = -1;

	Vfd_Srv_Align_7_Seq_Len = Vfd_Get_Seven_Seq_Len();


	if ((Align_Type > NONE) || (Align_Type < LEFT))
	{
		LOGE("VFD_Align : Align_Type is error !!! \n");
		return -1 ;
	}

	me->showinfo->Align_type = Align_Type ;

	memset(me->showinfo->vfd_align_src, 0, sizeof(me->showinfo->vfd_align_src));

	if (me->showinfo->src_len < Vfd_Srv_Align_7_Seq_Len)
	{
		if (me->showinfo->Align_type == LEFT)
		{
			LOGDT("Align_Type is left\n");

			for (cnt = 0 ; cnt < me->showinfo->src_len ; cnt++ )
			{
				tmpBuf[cnt] = me->showinfo->vfd_src[cnt];
			}

			for (cnt = (Vfd_Srv_Align_7_Seq_Len - 1) ; cnt >= me->showinfo->src_len ; cnt-- )
			{
				tmpBuf[cnt] = Space_Char;
			}

			memcpy(me->showinfo->vfd_align_src, tmpBuf, strlen(tmpBuf));

			me->showinfo->vfd_align_src[strlen(tmpBuf)] = Space_Char;

			me->showinfo->Align_Len = strlen(me->showinfo->vfd_align_src);


		}
		else if (me->showinfo->Align_type == RIGHT)
		{
			LOGDT("Align_Type is right\n");

			for (cnt = 0 ; cnt < (Vfd_Srv_Align_7_Seq_Len - (me->showinfo->src_len)) ; cnt++ )
			{
				tmpBuf[cnt] = Space_Char;
			}

			for (cnt = 0 ; cnt < me->showinfo->src_len ; cnt++ )
			{
				tmpBuf[Vfd_Srv_Align_7_Seq_Len - (me->showinfo->src_len) + cnt] = me->showinfo->vfd_src[cnt];
			}

			memcpy(me->showinfo->vfd_align_src, tmpBuf, strlen(tmpBuf));

			me->showinfo->vfd_align_src[strlen(tmpBuf)] = Space_Char;

			me->showinfo->Align_Len = strlen(me->showinfo->vfd_align_src);

		}
		else if (me->showinfo->Align_type == CENTER)
		{
			LOGDT("Align_Type is center\n");

			if ((me->showinfo->src_len) <= 3)
			{
				align_offset = ((Vfd_Srv_Align_7_Seq_Len / 2) - 1 );
			}
			else
			{
				align_offset = 1;
			}

			for (cnt = 0; cnt < align_offset; cnt++)
			{
				tmpBuf[cnt] = Space_Char;
			}

			for (cnt = 0; cnt < me->showinfo->src_len; cnt++)
			{
				tmpBuf[align_offset + cnt] = me->showinfo->vfd_src[cnt];
			}

			for (cnt = (align_offset + me->showinfo->src_len) ; cnt <= Vfd_Srv_Align_7_Seq_Len ; cnt++)
			{
				tmpBuf[cnt] = Space_Char;
			}

			memcpy(me->showinfo->vfd_align_src, tmpBuf, strlen(tmpBuf));

			me->showinfo->Align_Len = strlen(me->showinfo->vfd_align_src);

		}
	}
	else
	{
		LOGDT("String length longer than 7 sequence length \n");

		me->showinfo->vfd_src[me->showinfo->src_len] = Space_Char;

		me->showinfo->vfd_src[(me->showinfo->src_len) + 1] = Space_Char;

		memcpy(me->showinfo->vfd_align_src, me->showinfo->vfd_src, strlen(me->showinfo->vfd_src));

		me->showinfo->Align_Len = strlen(me->showinfo->vfd_src);

		LOGDT("me->showinfo->Align_Len = %d\n", me->showinfo->Align_Len);
	}

	return 0;
}

/******************************************************************************************/
/**
 * \fn			int VFD_POP_Align(VfdDisplay * const me , int Align_Type)
 *
 * \brief		align the POP string
 *
 * \param		VfdDisplay * const me , Align_Type(INPUT):0(left) 1(right) 2(center) 3(NONE)
 *
 * \return		\e 0 on success. \n
 *			    \e -1 on Align_Type error. \n
 *
 ******************************************************************************************/

int VFD_POP_Align(VfdDisplay *const me, int Align_Type)
{

	char tmpBuf [VFD_STR_MAX] = {0};
	int cnt;
	int align_offset ;
	int Vfd_Srv_POPAlign_7_Seq_Len = -1;

	Vfd_Srv_POPAlign_7_Seq_Len = Vfd_Get_Seven_Seq_Len();

	if ((Align_Type > NONE) || (Align_Type < LEFT))
	{
		LOGE("VFD_POP_Align : Align_Type is error !!! \n");
		return -1 ;
	}

	me->popshowinfo->Align_type = Align_Type ;

	memset(me->popshowinfo->vfd_align_src, 0, sizeof(me->popshowinfo->vfd_align_src));

	if (me->popshowinfo->src_len < Vfd_Srv_POPAlign_7_Seq_Len)
	{
		if (me->popshowinfo->Align_type == LEFT)
		{
			LOGDT("Align_Type is left\n");

			for (cnt = 0 ; cnt < me->popshowinfo->src_len ; cnt++ )
			{
				tmpBuf[cnt] = me->popshowinfo->vfd_src[cnt];
			}

			for (cnt = (Vfd_Srv_POPAlign_7_Seq_Len - 1) ; cnt >= me->popshowinfo->src_len ; cnt-- )
			{
				tmpBuf[cnt] = Space_Char;
			}

			memcpy(me->popshowinfo->vfd_align_src, tmpBuf, strlen(tmpBuf));

			me->popshowinfo->Align_Len = strlen(me->popshowinfo->vfd_align_src);


		}
		else if (me->popshowinfo->Align_type == RIGHT)
		{
			LOGDT("Align_Type is right\n");

			for (cnt = 0 ; cnt < (Vfd_Srv_POPAlign_7_Seq_Len - (me->popshowinfo->src_len)) ; cnt++ )
			{
				tmpBuf[cnt] = Space_Char;
			}

			for (cnt = 0 ; cnt < me->popshowinfo->src_len ; cnt++ )
			{
				tmpBuf[Vfd_Srv_POPAlign_7_Seq_Len - (me->popshowinfo->src_len) + cnt] = me->popshowinfo->vfd_src[cnt];
			}

			memcpy(me->popshowinfo->vfd_align_src, tmpBuf, strlen(tmpBuf));

			me->popshowinfo->Align_Len = strlen(me->popshowinfo->vfd_align_src);

		}
		else if (me->popshowinfo->Align_type == CENTER)
		{
			LOGDT("Align_Type is center\n");

			if ((me->popshowinfo->src_len) <= 3)
			{
				align_offset = ((Vfd_Srv_POPAlign_7_Seq_Len / 2) - 1 );
			}
			else
			{
				align_offset = 1;
			}

			for (cnt = 0; cnt < align_offset; cnt++)
			{
				tmpBuf[cnt] = Space_Char;
			}

			for (cnt = 0; cnt < me->popshowinfo->src_len; cnt++)
			{
				tmpBuf[align_offset + cnt] = me->popshowinfo->vfd_src[cnt];
			}

			for (cnt = (align_offset + me->popshowinfo->src_len) ; cnt < Vfd_Srv_POPAlign_7_Seq_Len ; cnt++)
			{
				tmpBuf[cnt] = Space_Char;
			}

			memcpy(me->popshowinfo->vfd_align_src, tmpBuf, strlen(tmpBuf));

			me->popshowinfo->Align_Len = strlen(me->popshowinfo->vfd_align_src);

		}
	}
	else
	{
		LOGDT("String length longer than 7 sequence length \n");

		me->popshowinfo->vfd_src[me->popshowinfo->src_len] = Space_Char;

		me->popshowinfo->vfd_src[(me->popshowinfo->src_len) + 1] = Space_Char;

		memcpy(me->popshowinfo->vfd_align_src, me->popshowinfo->vfd_src, strlen(me->popshowinfo->vfd_src));

		me->popshowinfo->Align_Len = strlen(me->popshowinfo->vfd_src);

	}

	return 0;
}

/******************************************************************************************/
/**
 * \fn			int VFD_Scroll (VfdDisplay * const me  , int scroll_mode , int str_offset)
 *
 * \brief		accept the scroll_mode and str_offset
 *
 * \param		VfdDisplay * const me,scroll_mode(INPUT):  0 (R2L) 1(L2R),str_offset(INPUT): 0
 *
 *
 * \return		\e 0 on success. \n
 *			    \e -1 on scroll_mode error. \n
 *
 ******************************************************************************************/

int VFD_Scroll (VfdDisplay *const me, int scroll_mode, int str_offset)
{
	LOGDT("VFD_Scroll in \n");

	if ((scroll_mode > L2R) || (scroll_mode < R2L))
	{
		LOGE("VFD_Scroll : scroll_mode is error !!! \n");
		return -1 ;
	}

	me->showinfo->scrolling_mode = scroll_mode;

	me->showinfo->str_offset = str_offset;

	return 0;
}



int VFD_Scroll_One_Const (VfdDisplay * const me  , int scroll_mode , int str_offset, char *const_vfd, eShowOncemode_t back_mode, uint16_t bright_time ,uint16_t dark_time)
{
	LOGDT("VFD_Scroll in \n");

    if((scroll_mode > L2R) || (scroll_mode < R2L))
    {
        LOGE("VFD_Scroll : scroll_mode is error !!! \n");
        return -1 ;
    }    
	
	me->showinfo->scrolling_mode= scroll_mode;
	me->showinfo->str_offset = str_offset;

	me->showinfo->once_back_const_vfd = const_vfd;
	me->showinfo->once_back_mode = back_mode;

	if(me->showinfo->once_back_mode == BACK_BLINK)
	{
		me->showinfo->bright_time = bright_time;
		me->showinfo->dark_time = dark_time;
	}	
	
	return 0;
}


int VFD_Blink_One_Const (VfdDisplay * const me  ,char *const_vfd, eShowOncemode_t back_mode,BYTE blink_time, uint16_t once_bright_time ,uint16_t once_dark_time, uint16_t bright_time ,uint16_t dark_time)
{
	LOGDT("VFD_Blink once in \n");

	me->showinfo->blink_once_bright_time = once_bright_time;
	me->showinfo->blink_once_dark_time = once_dark_time;

	me->showinfo->once_back_const_vfd = const_vfd;
	me->showinfo->once_back_mode = back_mode;
	me->showinfo->blink_time = blink_time;
	
	if(me->showinfo->once_back_mode == BACK_BLINK)
	{
		me->showinfo->bright_time = bright_time;
		me->showinfo->dark_time = dark_time;
	}	
	
	return 0;
}



/******************************************************************************************/
/**
 * \fn			void VFD_Blink (VfdDisplay * const me , uint16_t bright_time ,uint16_t dark_time)
 *
 * \brief		accept the show_freq pass from VFD_Service_Blink
 *
 * \param	    VfdDisplay * const me, bright_time(INPUT),dark_time(INPUT)
 *
 *
 *
 * \return
 *
 *
 ******************************************************************************************/

void VFD_Blink (VfdDisplay *const me, uint16_t bright_time, uint16_t dark_time)
{
	LOGDT("VFD_Blink in \n");

	me->showinfo->bright_time = bright_time;

	me->showinfo->dark_time = dark_time;

	return ;
}

/******************************************************************************************/
/**
 * \fn			int VFD_POP (VfdDisplay * const me  , char *vfd_src  , int time , int Align_Type)
 *
 * \brief		get pop string from VFD_Service_POP & align pop string
 *
 * \param		VfdDisplay * const me , \n
 *              vfd_src(INPUT) : pop string \n
 *              time(INPUT) : pop time \n
 *              Align_Type(INPUT) : 0(left) 1(right) 2(center) 3(NONE) \n
 *
 * \return		\e 0 on success. \n
 *			    \e -1 on POP Align Fail. \n
 *
 ******************************************************************************************/

int VFD_POP (VfdDisplay *const me, char *vfd_src, int time, int Align_Type)
{
	LOGDT("VFD_POP in \n");

	int POP_Align_check;

	if (strcasecmp(me->popshowinfo->vfd_align_src, vfd_src) == 0)
	{
		return 0;
	}
	LOGDT("sizeof(me->popshowinfo->vfd_src) : %d....\n", sizeof(me->popshowinfo->vfd_src));

	memset(me->popshowinfo->vfd_src, 0, sizeof(me->popshowinfo->vfd_src));

	memcpy(me->popshowinfo->vfd_src, vfd_src, strlen(vfd_src));

	LOGDT("POP source = %s\n", me->popshowinfo->vfd_src);

	me->popshowinfo->src_len =  strlen(me->popshowinfo->vfd_src);

	me->popshowinfo->time = time;

	LOGDT("POP time = %d\n", me->popshowinfo->time);

	POP_Align_check = VFD_POP_Align(me, Align_Type);

	if (POP_Align_check != 0)
	{
		LOGE("VFD_POP :POP Align Fail !!! \n");
		return -1 ;
	}

	return 0 ;
}

/******************************************************************************************/
/**
 * \fn			void VFD_PlayTime(VfdDisplay * const me, int min, int sec)
 *
 * \brief		get file play time
 *
 * \param		VfdDisplay * const me ,\n
 *              min(INPUT) : minute \n
 *              sec(INPUT) : second \n
 *
 *
 * \return
 *
 *
 ******************************************************************************************/

void VFD_PlayTime(VfdDisplay *const me, int min, int sec, int hr)
{
	LOGDT("VFD_PlayTime in \n");

	   char timeBuf [15] = {0};

	hr = hr;
   sprintf(timeBuf, "%02d-%02d", min, sec);	
   
	LOGDT("sizeof(me->PlayTime): %d....\n", sizeof(me->PlayTime));

	memset(me->PlayTime, 0, sizeof(me->PlayTime));

	memcpy(me->PlayTime, timeBuf, strlen(timeBuf));

	LOGDT("me->PlayTime = %s.. \n", me->PlayTime);

	return ;
}

#endif




