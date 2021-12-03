/**************************************************************************/
/**
*
* \file	karaoke_vfd_str_tbl.inc
*
* \brief
*
* \note  	Copyright (c) 2016 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author
* \version	 v0.01
* \date 	 2017/08/21
***************************************************************************/

enum _STR_ID_
{
	STR_BGMV = 0,
	STR_MIC1V,
	STR_MIC2V,
	STR_KEY,
	STR_ECHO,
	STR_NR,
	STR_VC,
	STR_OFF,
	STR_ON,
	STR_NULL,
};

char *kok_vfd_str[] =  //pointer to string address
{
	"BG",//BGMV
	"M1",//MIC1V
	"M2",//MIC2V
	"K",//keyshift
	"E",//echo
	"NR",
	"VC",
	"OFF",
	"ON",
	" ",//null
};

