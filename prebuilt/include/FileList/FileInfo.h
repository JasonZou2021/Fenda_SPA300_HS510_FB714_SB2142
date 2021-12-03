/**********************************************************************/
/**
 *
 * \file 	fileinfo.h
 *
 * \brief fileinfo
 * 		detail standard header, fileinfo define
 *
 * \note Copyright (c) 2016 Sunplus Technology Co., Ltd. \n
 * 		All rights reserved.
 *
 * \author mm.li@sunmedia.com.cn
 *
 * \version v0.01
 *
 * \date 2016/08/011
 *
 ***********************************************************************/

#ifndef FILE_INFO_H_

#define FILE_INFO_H_

/*---------------------------------------------------------------------------*
 *                            MACRO DECLARATIONS                             *
 *---------------------------------------------------------------------------*/

/* 0 for store short name, 1 for stort long name*/
#define NOT_USE_SCANFILE_IN_FS (0)
#define STORE_LONG_NAME NOT_USE_SCANFILE_IN_FS

#define SHORT_NAME_LEN (12)
#if (STORE_LONG_NAME)
	#define EXT_NAME_LEN (4)
#else
	#define EXT_NAME_LEN (3)
#endif


/*---------------------------------------------------------------------------*
* DATE TYPES
*---------------------------------------------------------------------------*/

typedef struct fileInfo
{
#if	(!NOT_USE_SCANFILE_IN_FS)
	DWORD offset;
	DWORD FoClust;
#endif
#if (STORE_LONG_NAME)
	char *name;
#else
	char name[SHORT_NAME_LEN + 1];
#endif
} fileInfo, *pFileInfo;
#define FILE_INFO_SIZE (sizeof(fileInfo))

typedef void(*scanUpdate)(uint32_t);
typedef void(*fileUpdate)(pFileInfo);

#endif
