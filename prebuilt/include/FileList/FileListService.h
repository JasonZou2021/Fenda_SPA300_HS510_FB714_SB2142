/**************************************************************************/
/**
*
* \file FileListService.h
*
* \brief FileListService is an active object which moniters FileList status and
*        manipulates requests to FileList from other active objects
* \note Copyright (c) 2016 Sunplus Technology Co., Ltd. \n
*        All rights reserved.
* \note This code has been modified according to Sunplus Coding Regulation
*       Check Sunplus Coding Regulation v0.0.3 for the naming rules.
*
* \author mm.li@sunmedia.com.cn
* \version v0.01
* \date 2016/08/08
***************************************************************************/

#ifndef FILE_LIST_SERVICE_H

#define FILE_LIST_SERVICE_H

/*---------------------------------------------------------------------------*
* INCLUDE DECLARATIONS
*---------------------------------------------------------------------------*/

#include "qp_pub.h"
#include "FileInfo.h"
#include "id3.h"

#define UINT32_ERR (0x7FFFFFFFU)

/*---------------------------------------------------------------------------*
* DATE TYPES
*---------------------------------------------------------------------------*/

typedef struct FileListServiceEvtTag
{
	QEvent super;
	QActive *who;
	UINT32 param;
	UINT32 param1;
	UINT32 param2;
	UINT32 param3;
	UINT32 param4;
	UINT32 param5;
} FLServiceEvt;

/*---------------------------------------------------------------------*
 *                                              GLOBAL DATA                                                  *
 *---------------------------------------------------------------------*/

extern QActive *const AO_FLService;

/*---------------------------------------------------------------------*
 *                                         FUNCTION DECLARATIONS                                      *
 *---------------------------------------------------------------------*/

/***********************************************************************
  *
  * \fn  int32_t fileScan(QActive* me, const char* path, uint32_t scanNum, void* resume, scanUpdate updater)
  *
  * \brief	 update filelist
  *
  * \param [in] me: the handler of caller
  * \param [in] path: path to scan
  * \param [in] scanNum: max files to scan
  * \param [in] updater: callback after scan finish should be called
  *
  * \return 0 for succes, other value for failed
  *
  * \note
  *
  ***********************************************************************/
int32_t fileScan(QActive *me, const char *path, uint32_t scanNum, void *resume, scanUpdate updater);

/***********************************************************************
  *
  * \fn  int32_t fileStopScan(QActive* me)
  *
  * \brief	 stop update filelist
  *
  * \param [in] me: the handler of caller
  *
  * \return 0 for succes, other value for failed
  *
  * \note
  *
  ***********************************************************************/
int32_t fileStopScan(QActive *me);

/***********************************************************************
  *
  * \fn  int32_t fileSrcOut(QActive* me)
  *
  * \brief	 send signal to free filelist
  *
  * \param [in] me: the handler of caller
  *
  * \return 0 for succes, other value for failed
  *
  * \note
  *
  ***********************************************************************/
int32_t fileSrcOut(QActive *me);

/***********************************************************************
  *
  * \fn  void fileUmountStopScan()
  *
  * \brief	 before umount stop scan filelist
  *
  * \param
  *
  * \return
  *
  * \note
  *
  ***********************************************************************/
void fileUmountStopScan(void);

/***********************************************************************
  *
  * \fn  int32_t PLCountGet(QActive* me, scanUpdate updater)
  *
  * \brief	 get count of filelist
  *
  * \param [in] me: the handler of caller
  * \param [in] updater: callback after scan finish should be called
  *
  * \return 0 for succes, other value for failed
  *
  * \note
  *
  ***********************************************************************/
int32_t fileCountGet(QActive *me, scanUpdate updater);

/***********************************************************************
  *
  * \fn  int32_t fileModeSet(QActive* me, uint8_t mode)
  *
  * \brief	 set mode of getting file
  *
  * \param [in] me: the handler of caller
  * \param [int] mode: mode to set
  * \param [in] updater: callback after scan finish should be called
  *
  * \return 0 for succes, other value for failed
  *
  * \note
  *
  ***********************************************************************/
int32_t fileModeSet(QActive *me, uint8_t mode, scanUpdate updater);

/***********************************************************************
  *
  * \fn  int32_t fileModeGet(uint8_t* mode)
  *
  * \brief	 set mode of getting file
  *
  * \param [in] me: the handler of caller
  * \param [in] updater: callback after scan finish should be called
  *
  * \return 0 for succes, other value for failed
  *
  * \note
  *
  ***********************************************************************/
int32_t fileModeGet(QActive *me, scanUpdate updater);

/***********************************************************************
  *
  * \fn  int32_t fileNext(QActive* me, fileUpdate updater)
  *
  * \brief	 get next file node
  *
  * \param [in] me: the handler of caller
  * \param [in] updater: callback after scan finish should be called
  *
  * \return 0 for succes, other value for failed
  *
  * \note
  *
  ***********************************************************************/
int32_t fileNext(QActive *me, fileUpdate updater);

/***********************************************************************
  *
  * \fn  int32_t filePrev(QActive* me, fileUpdate updater)
  *
  * \brief	 get prev file node
  *
  * \param [in] me: the handler of caller
  * \param [in] updater: callback after scan finish should be called
  *
  * \return 0 for succes, other value for failed
  *
  * \note
  *
  ***********************************************************************/
int32_t filePrev(QActive *me, fileUpdate updater);
/***********************************************************************
  *
  * \fn  int32_t fileGet(QActive* me, fileUpdate updater)
  *
  * \brief	 get current file node
  *
  * \param [in] me: the handler of caller
  * \param [in] updater: callback after scan finish should be called
  *
  * \return 0 for succes, other value for failed
  *
  * \note
  *
  ***********************************************************************/
int32_t fileGet(QActive *me, fileUpdate updater);

#if (SUPPORT_FLODER)
	/***********************************************************************
	*
	* \fn  int32_t folderNext(QActive* me, fileUpdate updater)
	*
	* \brief	 get next folder node
	*
	* \param [in] me: the handler of caller
	* \param [in] updater: callback after scan finish should be called
	*
	* \return 0 for succes, other value for failed
	*
	* \note
	*
	***********************************************************************/
	int32_t folderNext(QActive *me, fileUpdate updater);

	/***********************************************************************
	*
	* \fn  int32_t folderPrev(QActive* me, fileUpdate updater)
	*
	* \brief	 get prev folder node
	*
	* \param [in] me: the handler of caller
	* \param [in] updater: callback after scan finish should be called
	*
	* \return 0 for succes, other value for failed
	*
	* \note
	*
	***********************************************************************/
	int32_t folderPrev(QActive *me, fileUpdate updater);

	/***********************************************************************
	*
	* \fn  int32_t fileNext(QActive* me, fileUpdate updater)
	*
	* \brief	 get next file node
	*
	* \param [in] me: the handler of caller
	* \param [in] updater: callback after scan finish should be called
	*
	* \return 0 for succes, other value for failed
	*
	* \note
	*
	***********************************************************************/
	int32_t fileAllNext(QActive *me, fileUpdate updater);
	/***********************************************************************
	*
	* \fn  pFileInfo fileJumpNext(void)
	*
	* \brief	Jump next file node
	*
	* \param [in] me: the handler of caller
	* \param [in] updater: callback after scan finish should be called
	*
	* \return none
	*
	* \note
	*
	***********************************************************************/
	int32_t fileJumpNext(QActive *me, fileUpdate updater);

	/***********************************************************************
	*
	* \fn  int32_t filePrev(QActive* me, fileUpdate updater)
	*
	* \brief	 get prev file node
	*
	* \param [in] me: the handler of caller
	* \param [in] updater: callback after scan finish should be called
	*
	* \return 0 for succes, other value for failed
	*
	* \note
	*
	***********************************************************************/
	int32_t fileAllPrev(QActive *me, fileUpdate updater);

#endif

/***********************************************************************
  *
  * \fn  int32_t pageNext(QActive* me, fileUpdate updater)
  *
  * \brief	 get next page node
  *
  * \param [in] me: the handler of caller
  * \param [in] updater: callback after scan finish should be called
  *
  * \return 0 for succes, other value for failed
  *
  * \note
  *
  ***********************************************************************/
int32_t pageNext(QActive *me, fileUpdate updater);

/***********************************************************************
  *
  * \fn  int32_t pagePrev(QActive* me, fileUpdate updater)
  *
  * \brief	 get prev page node
  *
  * \param [in] me: the handler of caller
  * \param [in] updater: callback after scan finish should be called
  *
  * \return 0 for succes, other value for failed
  *
  * \note
  *
  ***********************************************************************/
int32_t pagePrev(QActive *me, fileUpdate updater);

#if (SUPPORT_ID3_GET)
	/***********************************************************************
	*
	* \fn  int32_t getId3Info(pFileInfo fileInfo, id3Update id3infoUpdate)
	*
	* \brief	 get id3info
	*
	* \param [int] fileInfo: file node of getting id3
	*
	* \param [int] id3infoUpdate: callback after getting id3
	*
	* \return 0 for succes, other value for failed
	*
	* \note
	*
	***********************************************************************/
	int32_t fileId3InfoGet(QActive *me, pFileInfo fileInfo, id3Update updater);
#endif

/***********************************************************************
*
* \fn  int32_t fileSetCur(QActive* me, void* resume)
*
* \brief	 update filelist
*
* \param [in] me: the handler of caller
* \param [in] path: path to scan
* \param [in] scanNum: max files to scan
* \param [in] updater: callback after scan finish should be called
*
* \return 0 for succes, other value for failed
*
* \note
*
***********************************************************************/
int32_t fileSetCur(QActive *me, void *resume);


#endif
