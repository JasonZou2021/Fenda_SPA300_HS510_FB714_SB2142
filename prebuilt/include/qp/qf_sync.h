/******************************************************************************/
/**
*
* \file	qf_sync.h
*
* \brief	middleware sync mode function.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   yangf.wang@sunplus.com.cn
* \version	 v0.01
* \date 	 2016/05/05
******************************************************************************/

#ifndef QF_SYNC_H
#define QF_SYNC_H


/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/

#include "qpc.h"

/*---------------------------------------------------------------------------*
*                            MACRO   DECLARATIONS                            *
*----------------------------------------------------------------------------*/

/**
*\brief define
*/
#define SUCCESS (0)
#define ERROR (-1)
#define MAXMSG (1)
#define NAME_SIZE (16)

/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/

/**
*\brief sync event struct
*/
typedef struct
{
	QEvt super;						//inherit base event class
	volatile void *privateHandle;	//private member

	char *pbBufRet;					//buffer pointer from caller to bring data back
	int sdLen;						//length of provided buffer
} SYNCEVT;

/*---------------------------------------------------------------------------*
 *                          FUNCTION   DECLARATIONS                          *
 *---------------------------------------------------------------------------*/

/******************************************************************************************/
/**
 * \fn		 int QF_SyncRequest(QMActive * const pstReceiver,
 *				SYNCEVT * const pstEvt,
 *				void const * const pstSender,
 *				char *pbBuf,
 *				int sdLen,
 *				uint32_t dMs)
 *
 * \brief		send a event, and wait for result.
 *
 * \param		pstReceiver : (Input) the event receiver, if it'NULL, publish pstEvt
 * \param		pstEvt : (Input) sync event to send
 * \param		pstSender : (Input) the sender for QS
 * \param		pbBuf : (Output) get return value
 * \param		sdLen : (Input) pbBuf's size
 * \param		dMs : (Input) timeout with millisecond
 *
 * \return		\e ERROR on fail. \n
 *				\e read size on success \n
 *
 * \note		none.
 *
 * \sa			QF_SyncReturn()
 *
 ******************************************************************************************/
int QF_SyncRequest(QMActive * const pstReceiver,
				SYNCEVT * const pstEvt,
				void const * const pstSender,
				char *pbBuf,
				int sdLen,
				uint32_t dMs);

/******************************************************************************************/
/**
 * \fn		int QF_SyncReturn(SYNCEVT const * const pstEvt, uint32_t dMs)
 *
 * \brief		return a result to sender.
 *
 * \param		pstEvt : (Input) event with return data buffer and private info
 * \param		dMs : (Input) timeout with millisecond
 *
 * \return		\e -1 on fail. \n
 *				\e 0 on success \n
 *
 * \note		none.
 *
 * \sa          QF_SyncRequest()
 *
 ******************************************************************************************/
int QF_SyncReturn(SYNCEVT const * const pstEvt, uint32_t dMs);

#endif
