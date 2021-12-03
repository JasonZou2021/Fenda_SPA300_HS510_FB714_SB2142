/******************************************************************************/
/**
*
* \file	 mcu_if.h
*
* \brief	8107 Class-D led led .\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author       email@sunplus.com
*\version	 v0.01
*\date 	 2016/10/28
******************************************************************************/
#ifndef _CERTTEST_IF_H_
#define _CERTTEST_IF_H_

#include "qp_pub.h"

#include "AudDspService.h"



typedef struct
{
	QHsm super;
	QActive *superAO;
	unsigned char dspmode;
	sp_atmos_option_s atmos_opt;
	sp_ddp_option_s ddp_opt;
	sp_dts_option_s dts_opt;
	sp_dtsx_option_s dtsx_opt;

} stCertTest_IF_t;




//****Evt****//





void CertTest_IF_ctor(stCertTest_IF_t *const me);


#endif // _MCU_IF_H_
