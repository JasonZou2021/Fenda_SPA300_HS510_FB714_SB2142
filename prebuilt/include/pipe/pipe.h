/******************************************************************************/
/**
*
* \file	 pipe.h
*
* \brief	streaming pipe operation function.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author       bin.chen@sunplus.com
*\version	 v0.01
*\date 	 2016/04/29
******************************************************************************/
#ifndef _PIPE_H
#define _PIPE_H

#include <string.h>
#include <semaphore.h>
#include <qpc.h>
#include "mutex.h"


/*! Pipe  Notify mode */

enum PipeNotifyMode
{
	WRITE_NOTIFY_SELF = 0,
	WRITE_NOTIFY_ENABLE,
	READ_NOTIFY_SELF,
	READ_NOTIFY_ENABLE,
};


/*! Pipe  frame struct */

typedef struct _PIPEFRAME_S
{
	UINT8 *pbFrmData;		/*!< Pipe id*/
	UINT32 dRealsize;		/*!< frame full size */
	struct _PIPEFRAME_S *next;
} PIPEFRAME_T;

typedef struct _PIPE_S PIPE_T;

typedef void (*pfNotifyFunc)(PIPE_T *pstPipe, int sdMode);

/*! Pipe  struct */
struct _PIPE_S
{
	UINT8 bFrmcnt;			/*!< streaming frame total count */
	UINT32 dFrmsize;		/*!< frame size */

	UINT8 bWriteNumFrames;			/*!< Free frame count*/
	UINT8 bReadNumFrames;			/*!< occupy frame count*/

	PIPEFRAME_T *pdHead;	/*!< point of Pipe  frame struct*/
	PIPEFRAME_T *pdWriteCurDesc;	/*!< point of write  frame */
	PIPEFRAME_T *pdReadCurDesc;	/*!< point of write  frame */

	pfNotifyFunc pfNotifyReader;	/*!<Put Callback function */
	pfNotifyFunc pfNotifyWriter;	/*!< Free Callback function */
};

/*! Pipe event struct */

typedef struct _PIPEEVT_S
{
	QEvt super;				/*!< qp evt parent */
	PIPE_T *pipe;			/*!< pipe evt */
} PIPEEVT_T;

/******************************************************************************************/
/**
 * \fn		PIPE_T *Pipe_create(UINT32 dFrmsize, UINT8 bFrmcnt, pfNotifyFunc pfNotifyWriter, pfNotifyFunc pfNotifyReader)
 *
 * \brief		set framesize and framecount return a pipe struct .
 *
 * \param		dFrmsize : frame data size.
 *			bFrmcnt £ºpipe data count
 *			pfNotifyWriter: User set writer notify function
 *			pfNotifyReader: User set reader notify function, the almost need reader to set, you can set NULL here
 *
 * \return		\e success return point of pipe struct . \n
 *            	\e fail return 0 \n
 *
 * \note		none.
 *
 ******************************************************************************************/
PIPE_T *Pipe_create(UINT32 dFrmsize, UINT8 bFrmcnt, pfNotifyFunc pfNotifyWriter, pfNotifyFunc pfNotifyReader);

/******************************************************************************************/
/**
 * \fn		void Pipe_setNotifyReaderFunc(PIPE_T *pstPipe, pfNotifyFunc pfNotifyReader)
 *
 * \brief		set reader notify function .
 *
 * \param  	pstPipe : (Input) point of pipe struct
 * \param  	pfNotifyReader : (Input) the reader nitify function
 *
 * \return		none.
 *
 * \note		reader use this API to regist notify function
 *
 ******************************************************************************************/
void Pipe_setNotifyReaderFunc(PIPE_T *pstPipe, pfNotifyFunc pfNotifyReader);

/******************************************************************************************/
/**
 * \fn		void Pipe_start(PIPE_T *pstPipe)
 *
 * \brief		after handshake, writer notify itself  to start to write
 *
 * \param  	pstPipe : (Input) point of pipe struct
 *
 * \return		none.
 *
 * \note		none.
 *
 ******************************************************************************************/
void Pipe_start(PIPE_T *pstPipe);

/******************************************************************************************/
/**
 * \fn		UINT8 *Pipe_alloc(PIPE_T *pstPipe, UINT32 *dWriteableSize)
 *
 * \brief		get free framebuffer address from pipe struct .
 *
 * \param  	pstPipe : (Input) point of pipe struct
 * \param  	dWriteableSize : (Output) return writeable size
 *
 * \return		address of write data.
 *
 * \note		none.
 *
 ******************************************************************************************/
UINT8 *Pipe_alloc(PIPE_T *pstPipe, UINT32 *dWriteableSize);

/******************************************************************************************/
/**
 * \fn		void Pipe_put(PIPE_T *pstPipe, UINT32 dPutsize)
 *
 * \brief		put a useable data buffer upata data size .
 *
 * \param		pstPipe : (Input) point of pipe struct
 * \param		dPutsize : (Input) put data size
 *
 * \return		none.
 *
 * \note		none.
 *
 ******************************************************************************************/
void Pipe_put(PIPE_T *pstPipe, UINT32 dPutsize);

/******************************************************************************************/
/**
 * \fn		UINT8 *Pipe_get(PIPE_T *pstPipe, UINT32 *dReadsize)
 *
 * \brief		get a useable data buffer .
 *
 * \param		pstPipe : (Input) point of pipe struct
 * \param		dPutsize : (Output) buffer real size
 *
 * \return		address of read data.
 *
 * \note		none.
 *
 ******************************************************************************************/
UINT8 *Pipe_get(PIPE_T *pstPipe, UINT32 *dReadsize);

/******************************************************************************************/
/**
 * \fn		void Pipe_free(PIPE_T *pstPipe)
 *
 * \brief		free framebuffer .
 *
 * \param		pstPipe : (Input) point of pipe struct
 *
 * \return		none.
 *
 * \note		none.
 *
 ******************************************************************************************/
void Pipe_free(PIPE_T *pstPipe);

/******************************************************************************************/
/**
 * \fn		void Pipe_delete(PIPE_T *pstPipe)
 *
 * \brief		delete  pipe struct .
 *
 * \param		pstPipe : (Input) point of pipe struct
 *
 * \return		none.
 *
 * \note		none.
 *
 ******************************************************************************************/
void Pipe_delete(PIPE_T *pstPipe);

/******************************************************************************************/
/**
 * \fn		UINT8 Pipe_getWriteNumFrames(PIPE_T *pstPipe)
 *
 * \brief		get write frames numbers
 *
 * \param		pstPipe : (Input) point of pipe struct
 *
 * \return		pipe write frames numbers
 *
 * \note		none.
 *
 ******************************************************************************************/
UINT8 Pipe_getWriteNumFrames(PIPE_T *pstPipe);

/******************************************************************************************/
/**
 * \fn		UINT8 Pipe_getReadNumFrames(PIPE_T *pstPipe)
 *
 * \brief		get read frames numbers
 *
 * \param		pstPipe : (Input) point of pipe struct
 *
 * \return		pipe read frames numbers
 *
 * \note		none.
 *
 ******************************************************************************************/
UINT8 Pipe_getReadNumFrames(PIPE_T *pstPipe);

/******************************************************************************************/
/**
 * \fn		void Pipe_reset(PIPE_T *pstPipe)
 *
 * \brief		reset pipe
 *
 * \param		pstPipe : (Input) point of pipe struct
 *
 * \return		none.
 *
 * \note		none.
 *
 ******************************************************************************************/
void Pipe_reset(PIPE_T *pstPipe);

#endif



