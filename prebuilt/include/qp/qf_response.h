/******************************************************************************/
/**
*
* \file	qf_response.h
*
* \brief	middleware listener & callback mode function.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   yangf.wang@sunplus.com.cn
* \version	 v0.01 
* \date 	 2016/05/05
******************************************************************************/

#ifndef QF_RESPONSE_H
#define QF_RESPONSE_H


/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include "qpc.h"

/*---------------------------------------------------------------------------*
*                            MACRO   DECLARATIONS                            *
*----------------------------------------------------------------------------*/
 
#define SUCCESS (0)
#define ERROR (-1)
#define HEAD_SIG (920350134)


#define Q_NEW_RESPONSE(e_, sig_, param_, func_) do { \
	(e_) = (AUTORETURNEVT *)QF_newX_((uint_fast16_t)sizeof(AUTORETURNEVT), (uint_fast16_t)0, (sig_)); \
	(e_)->pParam = (param_); \
	(e_)->pfFreeFunc = (func_); \
	(e_)->super.resMode_ = RESPONSE_MODE_LSRCBK; \
} while (0)

/* not use yet */
#if 0
#define Q_NEW_LISTENER(e_, sig_, param_, func_) do { \
	(e_) = (AUTORETURNEVT *)QF_newX_((uint_fast16_t)sizeof(AUTORETURNEVT), (uint_fast16_t)0, (sig_)); \
	(e_)->pParam = (param_); \
	(e_)->pfFreeFunc = (func_); \
	(e_)->super.resMode_ = RESPONSE_MODE_LISTENER; \
} while (0)

#define Q_NEW_CALLBACK(e_, sig_, param_, func_) do { \
	(e_) = (AUTORETURNEVT *)QF_newX_((uint_fast16_t)sizeof(AUTORETURNEVT), (uint_fast16_t)0, (sig_)); \
	(e_)->pParam = (param_); \
	(e_)->pfFreeFunc = (func_); \
	(e_)->super.resMode_ = RESPONSE_MODE_CALLBACK; \
} while (0)
#endif

/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/

typedef void (*ARFunc_t)(QActive * const pstMe, void *pParam);

/**  
*\brief callback event to regist, listener event is not necessary to use this
*/
typedef struct 
{
    QEvt super;
	QActive *pstSender;
} CBKREGISTEVT; /*!< callback regist event  */

/**  
*\brief callback & listener event to return
*/
typedef struct 
{
	QEvt super;
	void (*pfFreeFunc)(void *pParam); /*!< function to free pParam  */
	void *pParam;
} AUTORETURNEVT; /*!< auto return event  */

/**  
*\brief to mark what kind of event
*/
enum RESMODE 
{
	RESPONSE_MODE_NORMAL,		/*!< for normal event */
	RESPONSE_MODE_LISTENER,		/*!< for listener event */
	RESPONSE_MODE_CALLBACK,		/*!< for callback event */
	RESPONSE_MODE_LSRCBK,		/*!< for listener or callback event both */
};


/*---------------------------------------------------------------------------*
 *                          FUNCTION   DECLARATIONS                          *
 *---------------------------------------------------------------------------*/

/******************************************************************************************/
/**
 * \fn		int QF_ListenerInit(QActive * const pstMe, int sdSig, ARFunc_t pfFunc)
 *
 * \brief		listener register, AO can listen sig and do the func if receive the right event
 *
 * \param  	pstMe : (Input) the AO which need listen event
 * \param		sdSig : (Input) listen signal
 * \param		pfFunc : (Input) listen funtion with param QEvt, when get the registed signal, run this function
 *
 * \return		\e ERROR on regist fail. \n
 *            	\e SUCCESS on regist success \n
 *
 * \note		none.
 *
 * \sa          ListenerDelete(), ListenerRun()
 *
 ******************************************************************************************/
int QF_ListenerInit(QActive * const pstMe, int sdSig, ARFunc_t pfFunc);

/******************************************************************************************/
/**
 * \fn		int QF_ListenerDelete(QActive * const pstMe, int sdSig)
 *
 * \brief		unregist listener from AO
 *
 * \param  	pstMe : (Input) the AO which need unregist
 * \param		sdSig : (Input) unregist function with this signal
 *
 * \return		\e ERROR on unregist fail. \n
 *            	\e SUCCESS on unregist success \n
 *
 * \note		none.
 *
 * \sa          ListenerInit(), ListenerRun()
 *
 ******************************************************************************************/
int QF_ListenerDelete(QActive * const pstMe, int sdSig);

/******************************************************************************************/
/**
 * \fn		void QF_ListenerRun(QActive * const pstMe, QEvt const * const pstEvt)
 *
 * \brief		HSM top run the funtion if AO have registed listener
 *
 * \param  	pstMe : (Input) the AO which regist function
 * \param		pstEvt : (Input) param for function
 *
 * \return		none.
 *
 * \note		none.
 *
 * \sa          ListenerInit(), ListenerDelete()
 *
 ******************************************************************************************/
void QF_ListenerRun(QActive * const pstMe, QEvt const * const pstEvt);

/******************************************************************************************/
/**
 * \fn		int QF_CallbackInit(QActive * const pstMe, int sdSig, ARFunc_t pfFunc)
 *
 * \brief		callback register, AO can receive the sig and do the callback function
 *
 * \param  	pstMe : (Input) the AO which need regist callback
 * \param		sdSig : (Input) callback signal
 * \param		pfFunc : (Input) callback funtion with param QEvt, when get the registed signal, run this function
 *
 * \return		\e ERROR on regist fail. \n
 *            	\e SUCCESS on regist success \n
 *
 * \note		if pstMe==NULL, AO should subscribe signal itself, and if used a global header
 *			if pstMe!=NULL, AO can post event directly to another
 *
 * \sa          CallbackDelete(), CallbackRun()
 *
 ******************************************************************************************/
int QF_CallbackInit(QActive * const pstMe, int sdSig, ARFunc_t pfFunc);

/******************************************************************************************/
/**
 * \fn		int QF_CallbackDelete(QActive * const pstMe, int sdSig)
 *
 * \brief		unregist callback from AO
 *
 * \param  	pstMe : (Input) the AO which need unregist
 * \param		sdSig : (Input) unregist function with this signal
 *
 * \return		\e ERROR on unregist fail. \n
 *            	\e SUCCESS on unregist success \n
 *
 * \note		none.
 *
 * \sa          CallbackInit(), CallbackRun()
 *
 ******************************************************************************************/
int QF_CallbackDelete(QActive * const pstMe, int sdSig);

/******************************************************************************************/
/**
 * \fn		void QF_CallbackRun(QActive * const pstMe, QEvt const * const pstEvt)
 *
 * \brief		HSM top run the funtion if AO have registed callback
 *
 * \param  	pstMe : (Input) the AO which regist function
 * \param		pstEvt : (Input) param for function
 *
 * \return		none.
 *
 * \note		none.
 *
 * \sa          CallbackInit(), CallbackDelete()
 *
 ******************************************************************************************/
void QF_CallbackRun(QActive * const pstMe, QEvt const * const pstEvt);

#endif
