/**************************************************************************/
/**
*
* \file AudLogService.h
*
* \brief audio log serivce provide APIs which read data frome pipe  and
*        writes the data to USB
* \note Copyright (c) 2016 Sunplus Technology Co., Ltd. \n
*        All rights reserved.
* \note This code has been modified according to Sunplus Coding Regulation
*       Check Sunplus Coding Regulation v0.0.3 for the naming rules.
*
* \author
* \version v0.01
* \date 2016/09/29
***************************************************************************/
#ifndef AUD_LOG_SERVICE_h
#define AUD_LOG_SERVICE_h

/*---------------------------------------------------------------------------*
* INCLUDE DECLARATIONS
*---------------------------------------------------------------------------*/
//#include <stdio.h>
#include "types.h"
#include "qpc.h"
#include "qp_pub.h"
#include "pipe.h"
#include "AudDspService.h"
/*---------------------------------------------------------------------------*
* MACRO DECLARATIONS
*---------------------------------------------------------------------------*/
typedef enum
{
	OLOG_STATUS_OK,
	OLOG_STATUS_OPEN_FILE_FAIL,
	OLOG_STATUS_OPEN_PIPE_FAIL,
	OLOG_STATUS_GET_WFRM_FAIL,
	OLOG_STATUS_GET_RFRM_FAIL,
	OLOG_STATUS_CLOSE_FILE_FAIL,
	OLOG_STATUS_CLOSE_PIPE_FAIL,
	OLOG_STATUS_WRITE_TIMEOUT,
} eOLOG_STATUS;

typedef enum
{
	OLOG_TYPE_DECODE,
	OLOG_TYPE_ENCODE,
	OLOG_TYPE_OUTPUT,
	OLOG_TYPE_REPORT,
	OLOG_TYPE_OTHERS,
	MAX_OLOG_SESSION
} eOLOG_TYPE;

typedef enum
{
	OLOG_MESSAGE_TYPE_PREPARE_END,
	OLOG_MESSAGE_TYPE_CLOSE_END,
	OLOG_MESSAGE_TYPE_WRITE_ONE_FRAME,
	OLOG_MESSAGE_TYPE_ERRORS,
	OLOG_MESSAGE_TYPE_OTHERS,
} eOLOG_MESSAGE_TYPE;

typedef struct spOLOGSession_tag
{
	UINT8 bNumFrame;

	eOLOG_TYPE eType;
	UINT32  dParam;
	void (*pfCallback)(QActive *pSender, eOLOG_MESSAGE_TYPE eMsgType, PIPE_T *pstPipe, eOLOG_STATUS eStatus);
	eOLOG_STATUS  eStatus;
	PIPE_T    *pstPipe;
	INT32  fid;
	UINT32 dFileIndex;
} spOLOGSession;

//event for signal OLOG_PREPARE_SIG:
typedef struct
{
	QEvt stEvt;
	UINT8 bNumFrame;
	UINT32 dSize ;
	eOLOG_TYPE eType;
	union
	{
		UINT8 *pbName;
		UINT32  dParam;
	};
	void (*pfCallback)(QActive *pSender, eOLOG_MESSAGE_TYPE eMsgType,  PIPE_T *pstPipe, eOLOG_STATUS eStatus);
	void (*pfWriterNotify)(PIPE_T *pstPipe, int sdMode);
} OlogPrepareEvt;

//event for signal  OLOG_FEEDBACK_SIG:
typedef struct
{
	QEvt stEvt;
	eOLOG_TYPE eType;
	eOLOG_MESSAGE_TYPE eMessageType;
	eOLOG_STATUS eStatus;
	PIPE_T *pstPipe;
} OlogFeedbackEvt;

//event for signal  OLOG_FRAME_READY_SIG:
typedef struct
{
	QEvt stEvt;
	PIPE_T *pstPipe;
} OlogFrameReadyEvt;

//event for signal  for OLOG_CLOSE_SIG:
typedef struct
{
	QEvt stEvt;
	PIPE_T *pstPipe;
} OlogCloseEvt;

typedef struct
{
	PIPE_T *pstPipe;
	UINT8 *pbAdd;
	UINT32 dRes ;
	UINT32 dSize;
	eOLOG_STATUS eStatus;
} OlogArchive;

/*---------------------------------------------------------------------------*
 * GLOBAL DATA
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * EXTERNAL DECLARATIONS
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * FUNCTION DECLARATIONS
 *---------------------------------------------------------------------------*/
QActive *SpOlogGet(void);
void AudLogService_Prepare(QActive *pSender, UINT8 bNumFrame, UINT32 dSize,
						   eOLOG_TYPE eType,	UINT32  dParam,
						   void (*pfCallback)(QActive *pSender, eOLOG_MESSAGE_TYPE eMsgType, PIPE_T *pstPipe, eOLOG_STATUS eStatus),
						   void (*pfWriterNotify)(PIPE_T *pstPipe, int sdMode));
void AudLogService_FrameReady(PIPE_T *pstPipe, int sdMode ) ;
void AudLogService_Close (QActive *pSender, PIPE_T *pstPipe) ;

#endif                        /* AUD_LOG_SERVICE_h */
