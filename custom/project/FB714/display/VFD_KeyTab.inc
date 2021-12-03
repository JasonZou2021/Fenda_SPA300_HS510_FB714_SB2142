/**************************************************************************/
/**
*
* \file	vfd_KeyTab.c
*
* \brief
*
* \note  	Copyright (c) 2016 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author
* \version	 v0.01
* \date 	 2016/08/23
***************************************************************************/
/*---------------------------------------------------------------------------*
*                            INCLUDE   DECLARATIONS                         *
*---------------------------------------------------------------------------*/
#ifndef VFD_KeyTab_INC
#define VFD_KeyTab_INC

#include "user_def.h"


#define DISPLAY_VFD_S1 (1 << 0)
#define DISPLAY_VFD_S2 (1 << 1)
#define DISPLAY_VFD_S3 (1 << 2)
#define DISPLAY_VFD_S4 (1 << 3)
#define DISPLAY_VFD_S5 (1 << 4)
#define DISPLAY_VFD_S6 (1 << 5)
#define DISPLAY_VFD_S7 (1 << 6)
#define DISPLAY_VFD_S8 (1 << 7)
#define DISPLAY_VFD_S9 (1 << 8)
#define DISPLAY_VFD_S10 (1 << 9)
#define DISPLAY_VFD_S11 (1 << 10)
#define DISPLAY_VFD_S12 (1 << 11)
#define DISPLAY_VFD_S13 (1 << 12)
#define DISPLAY_VFD_S14 (1 << 13)
#define DISPLAY_VFD_S15 (1 << 14)
#define DISPLAY_VFD_S16 (1 << 15)


#define DISPLAY_VFD_VAL_NUM (16)

UINT32 DISPLAY_VFD_ValTab[DISPLAY_VFD_VAL_NUM] =
{
	DISPLAY_VFD_S1, DISPLAY_VFD_S2, DISPLAY_VFD_S3, DISPLAY_VFD_S4, DISPLAY_VFD_S5, DISPLAY_VFD_S6,
	DISPLAY_VFD_S7, DISPLAY_VFD_S8, DISPLAY_VFD_S9, DISPLAY_VFD_S10, DISPLAY_VFD_S11, DISPLAY_VFD_S12,
	DISPLAY_VFD_S13, DISPLAY_VFD_S14, DISPLAY_VFD_S15, DISPLAY_VFD_S16
};

#endif

