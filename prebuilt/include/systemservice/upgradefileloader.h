/******************************************************************************/
/**
*
* \file	upgradefileloader.h
*
* \brief	read upgrade file from device and send it to related service\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   sean.lin@sunplus.com
* \version	 v0.01
* \date 	 2021/05/06
******************************************************************************/

#ifndef UPGRADEFILELOADER_H
#define UPGRADEFILELOADER_H

#include "qp_pub.h"

#define MAX_FILE_NUM    20

typedef struct
{
	QHsm super;
    QActive *superAO;
	QTimeEvt stUpgradeTimeEvt;
    char *psbDevName;
    char *psbDiskName;
    char *psbFilePath;
    INT32 sdfd;
    UINT8 *dataBuffer;
    INT32 lastOffset;
    INT32 readed;
    char *upFileList[MAX_FILE_NUM];
    INT32 upFileCount;
    INT32 currFileIdx;
} UPGRDFILELOADER;

typedef struct
{
	QEvt super;
	int offset;
	UINT32 size;
} UPGRADEREADEVT_T;

/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/
void UpgradeFileLoader_Ctor(UPGRDFILELOADER *const me, QActive *superAO);
int UpgradeFileLoader_Scan(void);
int UpgradeFileLoader_Select(char *pbUpgFileName);
int UpgradeFileLoader_GetFileList(void);
char *UpgradeFileLoader_PreFile(void);
char *UpgradeFileLoader_NextFile(void);

#endif
