/******************************************************************************/
/**
*
* \file	 VFD.c
*
* \brief	This file provides the vfd driver layer function pointer \n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*\author       email@sunplus.com
*\version	 v0.01
*\date 	 2018/11/27
******************************************************************************/
#include <string.h>
#include "initcall.h"
#include "log_utils.h"
#include "VFD.h"
#include "user_def.h"

extern VFDOP_T	 g_stNec16312;
extern VFDOP_T	 g_stPt6315;
extern VFDOP_T	 g_stVfdApp;
//vfd driver
static VFDOP_T *g_pstVfd;


/******************************************************************************************/
/**
 * \fn		int Vfd_ReadVFDKeys(VfdList* pstVfdList)
 *
 * \brief	read vfd key
 *
 * \return
 *
 * \note		none.
 *
 ******************************************************************************************/
int Vfd_ReadVFDKeys(void)
{
	VFDOP_T *pfnampop = NULL;
	UINT32 sdRet = 0;

	pfnampop = g_pstVfd;

	if (NULL != pfnampop)
	{
		sdRet = pfnampop->pfn_ReadVFDKeys();
		return sdRet;
	}
	return sdRet;
}


/******************************************************************************************/
/**
 * \fn		int Vfd_Get_Seven_Seq_Len(VfdList* pstVfdList)
 *
 * \brief	get 7 sequence length
 *
 * \return
 *
 * \note		none.
 *
 ******************************************************************************************/
int Vfd_Get_Seven_Seq_Len(void)
{
	VFDOP_T *pfnampop = NULL;
	int sdRet = -1;

	pfnampop = g_pstVfd;

	if (NULL != pfnampop)
	{
		sdRet = pfnampop->pfn_Get_Seven_Seq_Len();
		return sdRet;
	}
	return sdRet;
}


/******************************************************************************************/
/**
 * \fn		int Vfd_Set_Str(VfdList* pstVfdList,const char *s)
 *
 * \brief	VFD set str
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/
int Vfd_Set_Str(const char *s)
{
	VFDOP_T *pfnampop = NULL;
	int sdRet = -1;

	pfnampop = g_pstVfd;

	if (NULL != pfnampop)
	{
		pfnampop->pfn_set_str(s);
		sdRet = 0;
	}

	return sdRet;
}


/******************************************************************************************/
/**
 * \fn		int Vfd_Set_Dot(VfdList* pstVfdList,UINT32 p, UINT32 method)
 *
 * \brief	VFD set dot
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/
int Vfd_Set_Dot(UINT32 p, UINT32 method)
{
	VFDOP_T *pfnampop = NULL;
	int sdRet = -1;

	pfnampop = g_pstVfd;

	if (NULL != pfnampop)
	{
		pfnampop->pfn_set_dot(p, method);
		sdRet = 0;
	}

	return sdRet;
}



/******************************************************************************************/
/**
 * \fn		int Vfd_Drv_Init(VfdList* pstVfdList)
 *
 * \brief		Init VFD Driver
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/
int Vfd_Drv_Init(void)
{
	VFDOP_T *pfnampop = NULL;
	int sdRet = -1;

	pfnampop = g_pstVfd;

	if (NULL != pfnampop)
	{
		pfnampop->pfn_vfd_init();
		sdRet = 0;
	}

	return sdRet;
}

int Vfd_Drv_Deinit(void)
{
    VFDOP_T *pfnampop = NULL;
    int sdRet = -1;

    pfnampop = g_pstVfd;

    if (NULL != pfnampop) {
        pfnampop->pfn_deinit();
        sdRet = 0;
    }

    return sdRet;
}
void Set_device_id1(void);
extern void InitVfd_Nec16312(void);
extern int InitVfd_Pt6315(void);
extern int vfd_init(void);
void Insert_Nec16312drv(void)
{
    g_pstVfd = &g_stNec16312;
    InitVfd_Nec16312();
}

void Insert_Pt6315drv(void)
{
    g_pstVfd = &g_stPt6315;
    InitVfd_Pt6315();
}

void Insert_VfdAppdrv(void)
{
	g_pstVfd = &g_stVfdApp;
	//vfd_init();
		Set_device_id1();
}

/*
// to insert vfd driver
#ifndef SUPPORT_SUNPLUS_PT6315_PAD
	DRIVER_INIT(Insert_Nec16312drv);
#else
	DRIVER_INIT(Insert_Pt6315drv);
#endif
*/
DRIVER_INIT(Insert_VfdAppdrv);

