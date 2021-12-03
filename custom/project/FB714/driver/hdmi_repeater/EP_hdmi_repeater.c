/******************************************************************************/
/**
*
* \file	 hdmi_repeater.c
*
* \brief	This file provides the hdmi_repeater driver \n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*\author       Daniel.yeh@sunplus.com
*\version	 v0.01
*\date 	 2019/01/31
******************************************************************************/
#include "fsi_api.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "exstd_interfaces.h"
#define LOG_MODULE_ID ID_CEC
#define LOG_EXTRA_STR "[RPTR]"
#include "log_utils.h"
#include "i2c_api.h"
#include "user_def.h"
#if defined(USERAPP_SUPPORT_REPEATER) && (USERAPP_SUPPORT_REPEATER == 1)
#include "ITE_hdmi_repeater.h"
#else
#include "EP_hdmi_repeater.h"
#endif

static I2C_Chk_CB_t I2C_CB = NULL;

int HDMI_Repeat_Get_info(stHdmiRepeatHandler_t* pstRepeatHandler, UINT32 RegAddr, void *bBuf, UINT32 dLen)
{
    int res = FALSE;
    UINT32 cnt = 0;
    UINT8* pDataBuf;
    I2cData_t i2cData;
	int i;

    LOGD("R:\n");
    if ((pstRepeatHandler == 0) || (bBuf == 0))
    {
        return -1;
    }

    i2cData.buf = bBuf;
    i2cData.cmd = RegAddr;
    i2cData.cmdLen = 1;
    i2cData.dataLen = dLen;
    i2cData.slaveAddr = pstRepeatHandler->SlaveAddr;
    i2cData.showMsg = 0;
    res = I2C_Read(pstRepeatHandler->pCtlStr , &i2cData);

    if((res !=TRUE)&&(I2C_check_err()==0))
    {
       for(i=1;i<3;i++)
       {
        res = I2C_Read(pstRepeatHandler->pCtlStr , &i2cData);
		LOGD("EP I2C err,Re Read,count:--%d--\n",i);
		if(res == TRUE)
			break;
	   }
	}

    if (res == TRUE)
    {
        pDataBuf = (UINT8*)bBuf;
        for (cnt = 0; cnt < dLen; cnt++) // update pRegTbl
        {
            pstRepeatHandler->pRegTbl[RegAddr + cnt] = pDataBuf[cnt];
			LOGD("reg[0x%x]: 0x%x\n", RegAddr + cnt, pDataBuf[cnt]);
        }
    }

    return res;
}


int HDMI_Repeat_Set(stHdmiRepeatHandler_t* pstRepeatHandler, UINT32 RegAddr, void *bBuf, UINT32 dLen)
{
    int res = FALSE;
    UINT32 cnt = 0;
    UINT8* pDataBuf;
    I2cData_t i2cData;
    int i;
    LOGD("W:\n");
    if ((pstRepeatHandler == 0) || (bBuf == 0))
    {
        return -1;
    }

    i2cData.buf = bBuf;
    i2cData.cmd = RegAddr;
    i2cData.cmdLen = 1;
    i2cData.dataLen = dLen;
    i2cData.slaveAddr = pstRepeatHandler->SlaveAddr;
    i2cData.showMsg = 0;
    res = I2C_Write(pstRepeatHandler->pCtlStr , &i2cData);

    if((res !=TRUE)&&(I2C_check_err()==0))
    {
       for(i=1;i<3;i++)
       {
        res = I2C_Write(pstRepeatHandler->pCtlStr , &i2cData);
		LOGD("EP I2C err,Re Write,count:--%d--\n",i);
		if(res == TRUE)
			break;
	   }
	}
    if (res == TRUE)
    {
        pDataBuf = (UINT8*)bBuf;
        for (cnt = 0; cnt < dLen; cnt++) // update pRegTbl
        {
            pstRepeatHandler->pRegTbl[RegAddr + cnt] = pDataBuf[cnt];
			LOGD("reg[0x%x]: 0x%x\n", RegAddr + cnt, pDataBuf[cnt]);
        }
    }
    return res;
}

int HDMI_Repeat_init(stHdmiRepeatHandler_t* pstRepeatHandler)
{
    LOGD("HDMI_Repeat_init\n");

    if (pstRepeatHandler == 0)
    {

        LOGE(" fail\n");
        return -1;
    }

    HDMI_Repeat_Get_info(pstRepeatHandler, 0, pstRepeatHandler->pRegTbl, REPEATER_ADDR_MAX); // initial register table
#if 0
    int cnt;
    for (cnt = 0; cnt < EP92_REPEATER_ADDR_MAX+1; cnt++)
    {
        LOGD("addr[0x%x]: 0x%x\n", cnt, pstRepeatHandler->pRegTbl[cnt]);
    }
#endif
    return 0;
}

int HDMI_I2C_errcheck_RegCB(I2C_Chk_CB_t cb_t)
{
	if (cb_t == NULL)
	{
		LOGD("[%s] fail \n", __FUNCTION__);
		return -1;
	}
	LOGD("[%s] success \n", __FUNCTION__);
	I2C_CB = cb_t;
	return 0;
}

int I2C_check_err(void)
{
   int res = 1;
   	if (I2C_CB != NULL)
	{
       res =I2C_CB();
   	}
   return res;
}
