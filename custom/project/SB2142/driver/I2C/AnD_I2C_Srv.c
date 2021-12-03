/******************************************************************************/
/**
*
* \file	    AnD_I2C_Srv.c
*
* \brief	configuration for i2c to adjust all non-standard I2C
*
* \note  	Copyright (c) 2017 A&D Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   Jasonzou_and@126.com
* \version	 v0.01 
* \date 	 2017/7/28
******************************************************************************/


/*

	注意使用自定义 i2c ，不能多个器件共用，只能挂载一个i2c器件


*/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define LOG_MODULE_ID  ID_APUSR
#define LOG_EXTRA_STR  "[I2C]"
#include "log_utils.h"
#include "types.h"
#include "pinmux.h"
#include "busy_delay.h"
#include "AnD_I2C_Srv.h"




#define I2C_SRV_MAX		8


static I2C_CFG i2c_srv[I2C_SRV_MAX] =
{
	{.clk = 0, .dat = 0,}, // set to 0, because sometime is not 0
	{.clk = 0, .dat = 0,},
	{.clk = 0, .dat = 0,},
	{.clk = 0, .dat = 0,},
	{.clk = 0, .dat = 0,}, // set to 0, because sometime is not 0
	{.clk = 0, .dat = 0,},
	{.clk = 0, .dat = 0,},
	{.clk = 0, .dat = 0,},
};



int I2c_IO_Check_Used(I2C_CFG *cfg)
{
	BYTE i=0;

	for(i=0; i<I2C_SRV_MAX; i++)
	{
		if(i2c_srv[i].clk == cfg->clk)
		{
			return i;
		}
	}

	return -1;
}

int I2c_IO_Get_Cfg(int fd, I2C_CFG *cfg)
{
	if(fd >= I2C_SRV_MAX || cfg == NULL)
		return -1;

	memcpy(cfg, &i2c_srv[fd], sizeof(I2C_CFG));

	return 0;
}


int I2c_IO_Init(I2C_CFG *cfg)
{
	BYTE i=0;

	for(i=0; i<I2C_SRV_MAX; i++)
	{
		//I2C_printf(" srv[%d] clk %d , dat %d,  cfg clk %d, dat %d\n", i, i2c_srv[i].clk, i2c_srv[i].dat, cfg->clk, cfg->dat);
	
		if(i2c_srv[i].clk == 0)
		{
			memcpy(&i2c_srv[i], cfg, sizeof(I2C_CFG));

			Pinmux_GPIO_Free(cfg->clk);
			Pinmux_GPIO_Alloc(cfg->clk,IO_GPIO, IO_RISC);
			GPIO_OE_Set(cfg->clk, IO_OUTPUT);
			GPIO_Output_Write(cfg->clk, IO_HIGH);

			Pinmux_GPIO_Free(cfg->dat);
			Pinmux_GPIO_Alloc(cfg->dat,IO_GPIO, IO_RISC);
			GPIO_OE_Set(cfg->dat, IO_OUTPUT);
			GPIO_Output_Write(cfg->dat, IO_HIGH);
			
			return i;
		}
		else if(i2c_srv[i].clk == cfg->clk)
			return i;
	}

	LOGD(" I2C bus is full\n ");
	
	return -1;
}


int I2c_IO_ReInit(I2C_CFG *cfg)
{
	BYTE i=0;

	for(i=0; i<I2C_SRV_MAX; i++)
	{
		if(i2c_srv[i].clk == cfg->clk)
		{
			memcpy(&i2c_srv[i], cfg, sizeof(I2C_CFG));
			return i;
		}
	}
	
	return -1;
}

I2C_CFG *I2C_GetSrv(int i)
{
	if(i<0 || i > I2C_SRV_MAX-1)
		return NULL;

	return &i2c_srv[i];
}

static inline void I2CSWDelay(const I2C_CFG * pI2C)
{
	int delay = pI2C->delay;
	delay = delay;

	do
	{
		int j = 30;
		do
		{
			asm volatile ("nop");
		}
		while (--j >= 0);
	}
	while (--delay > 0);

}


static inline void I2CSWAckDelay(const I2C_CFG * pI2C)
{
	int delay = pI2C->ack_delay;
	delay = delay;

	do
	{
		int j = 30;
		do
		{
			asm volatile ("nop");
		}
		while (--j >= 0);
	}
	while (--delay > 0);

}


static inline int I2CSWReadSDA(const I2C_CFG* pI2C)
{
	return GPIO_Intput_Read(pI2C->dat);
}

static inline int I2CSWReadSCL(const I2C_CFG* pI2C)
{
	return GPIO_Intput_Read(pI2C->clk);
}

static inline void SetSDAOutput(const I2C_CFG* pI2C)
{
	GPIO_OE_Set(pI2C->dat, IO_OUTPUT);
}

static inline void SetSDAInput(const I2C_CFG* pI2C)
{
	GPIO_OE_Set(pI2C->dat, IO_INPUT);
}

static inline void SetSCLOutput(const I2C_CFG* pI2C)
{
	GPIO_OE_Set(pI2C->clk, IO_OUTPUT);
}

static inline void SetSCLInput(const I2C_CFG* pI2C)
{
	GPIO_OE_Set(pI2C->clk, IO_INPUT);
}

static inline void I2CSWSDALow(const I2C_CFG* pI2C)
{
	GPIO_Output_Write(pI2C->dat, IO_LOW);
	if(pI2C->high_voltage == I2C_HIGH_V_INPUT)
		GPIO_OE_Set(pI2C->dat, IO_OUTPUT);
	
}

static inline void I2CSWSCLLow(const I2C_CFG* pI2C)
{
	GPIO_Output_Write(pI2C->clk, IO_LOW);
	if(pI2C->high_voltage == I2C_HIGH_V_INPUT)
		GPIO_OE_Set(pI2C->clk, IO_OUTPUT);

}


static inline void I2CSWSDAHigh(const I2C_CFG* pI2C)
{
	if(pI2C->high_voltage == I2C_HIGH_V_INPUT)
		GPIO_OE_Set(pI2C->dat, IO_INPUT);
	else
	{
		GPIO_Output_Write(pI2C->dat, IO_HIGH);
		GPIO_OE_Set(pI2C->dat, IO_OUTPUT);
	}
}

static inline void I2CSWSCLHigh(const I2C_CFG* pI2C)
{
	if(pI2C->high_voltage == I2C_HIGH_V_INPUT)
		GPIO_OE_Set(pI2C->clk, IO_INPUT);
	else
	{
		GPIO_Output_Write(pI2C->clk, IO_HIGH);
		GPIO_OE_Set(pI2C->clk, IO_OUTPUT);
	}
}

static void I2CSWInitIO(const I2C_CFG* pI2C)
{

	SetSDAOutput(pI2C);
	SetSCLOutput(pI2C);
	I2CSWSDAHigh(pI2C);
	I2CSWSCLHigh(pI2C);
}

static void I2CSWStartSig(const I2C_CFG* pI2C)
{
    I2CSWInitIO(pI2C);
    I2CSWDelay(pI2C);
    I2CSWSDALow(pI2C);
    I2CSWDelay(pI2C);
}


static eOpStat I2CSWWriteBytes(const I2C_CFG* pI2C, const UINT8* data, int len)
{
	int i = 0;
	int j = 0;
	UINT8 tempdata;

	if(len<1)
	{
		return eI2COK;
	}
	
	I2CSWSCLLow(pI2C);

	for(j=0; j<len; j++)
	{
		tempdata = data[j];

		for(i = 8; i; i--)
		{
			if(pI2C->byte_type == I2C_BYTE_HIGH_TO_LOW)
			{
				if (tempdata & 0x80)
				{
					I2CSWSDAHigh(pI2C);
				}
				else
				{
					I2CSWSDALow(pI2C);
				}

				tempdata <<= 1;
			}
			else
			{
				if (tempdata & 0x01)
				{
					I2CSWSDAHigh(pI2C);
				}
				else
				{
					I2CSWSDALow(pI2C);
				}

				tempdata >>= 1;
			}
			
			I2CSWDelay(pI2C);     // scl -- low
			I2CSWSCLHigh(pI2C);
			I2CSWDelay(pI2C); //hawk	// scl -> high
			
			I2CSWSCLLow(pI2C);    // scl -> low
		}

		// Get ACK
		SetSDAInput(pI2C);
		I2CSWAckDelay(pI2C);  //I2CSWDelay(pI2C);           // scl -- low
		I2CSWSCLHigh(pI2C);
		I2CSWDelay(pI2C);  //I2CSWAckDelay(pI2C);           // scl -> high
		i = I2CSWReadSDA(pI2C);
		I2CSWSCLLow(pI2C);
		//I2CSWDelay(pI2C);           // scl -> low
		//SetSDAOutput(pI2C);

		I2CSWAckDelay(pI2C);

		if(pI2C->wr_method == I2C_WR_BYTE_WAIT)
			delay_1ms(1);
		
		SetSDAOutput(pI2C);

	}

	return ((i) ? eI2CFail: eI2COK);  
}

static void I2CSWStopSig(const I2C_CFG* pI2C)
{
    	//SetSDAOutput(pI2C);
	//SetSCLOutput(pI2C);
	I2CSWSDALow(pI2C);
	I2CSWSCLLow(pI2C);
	I2CSWDelay(pI2C);
	I2CSWSCLHigh(pI2C);
	I2CSWDelay(pI2C);
	I2CSWSDAHigh(pI2C);
}

static void I2CSWStop(const I2C_CFG* pI2C)
{
	I2CSWSCLLow(pI2C);
	I2CSWSDALow(pI2C);
	I2CSWDelay(pI2C);
	I2CSWSCLHigh(pI2C);
	I2CSWDelay(pI2C);
	I2CSWSDAHigh(pI2C);
	I2CSWDelay(pI2C);
	I2CSWSCLLow(pI2C);
	I2CSWDelay(pI2C);
	I2CSWSCLHigh(pI2C);
	I2CSWSDAHigh(pI2C);

	//GPIO_OE_Set(pI2C->clk, IO_INPUT);
	//GPIO_OE_Set(pI2C->dat, IO_INPUT);
}


static void I2CSWReInit(const I2C_CFG* pI2C)
{
    	//I2CSWInitIO(pI2C);
	//I2CSWDelay(pI2C);
	I2CSWStop(pI2C);
}


/****************************************************
Function:I2SSWWrite
Description:
Input:const I2CSW* pI2C  
         int iDeviceAddr    
         int iAddr          
         UINT8 *bData        
         int len            
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2014/11/12           Created             chaofa.yan
Others:

*****************************************************/
eOpStat AnD_I2CSWWrite(int fd, int iDeviceAddr, int iAddr, UINT8 *bData, int len)
{
    UINT16 loop;
    UINT8 bTmp = 0;
	UINT8 ret = 0, read_addr;
	I2C_CFG* pI2C = I2C_GetSrv(fd);

	if( pI2C == NULL)
		return eI2CFail;
    
    if (iAddr > 0xff) 
    {
        iDeviceAddr &= 0xf0;
        iDeviceAddr |= ((iAddr & 0x700) >> 7);
    }
	
    for(loop=0; loop<3; loop++) 
    {
        	I2CSWStartSig(pI2C);
        	bTmp = iDeviceAddr & 0xfe;
        
		if(iDeviceAddr)		// some IC no i2c address
			read_addr = I2CSWWriteBytes(pI2C, &bTmp, sizeof(bTmp));
		else
			read_addr = eI2COK;
		
        	if(read_addr == eI2COK)
        	{
        		bTmp = iAddr;
        	
            		if(eI2COK == I2CSWWriteBytes(pI2C, &bTmp, sizeof(bTmp)))
           		{

				if(len == 0)
					read_addr = eI2COK;
				else
					read_addr = I2CSWWriteBytes(pI2C, bData, len);
		
				if(read_addr == eI2COK)
				{
					I2CSWStopSig(pI2C);
					break;
				}
				else
					ret = 3;
            		}
			else
				ret = 2;
	 	}
	 	else
			ret = 1;
		
		I2CSWReInit(pI2C);

		delay_1ms(3);

		//LOGD(" I2C write error %d\n ", loop);
		
    }
    	
    if (loop >= 3)
    {
        return eI2CFail+ret;
    }

    return eI2COK;
}


static eOpStat I2CSWReadBytes(const I2C_CFG* pI2C, UINT8* data, int len) 
{
	int i, j;
	UINT8 bTemp = 0;
	
	I2CSWSCLLow(pI2C);
	SetSDAInput(pI2C);

	for(j=0; j<len-1 ; j++)
	{
		for(bTemp=0,i=8; i; i--)
		{
			I2CSWDelay(pI2C);
			I2CSWSCLHigh(pI2C);
			I2CSWDelay(pI2C);
			bTemp = (bTemp<<1) | I2CSWReadSDA(pI2C);
			I2CSWSCLLow(pI2C);
		}

		data[j] = bTemp;

		// Set ACK !!
		SetSDAOutput(pI2C);
		I2CSWSDALow(pI2C);
		I2CSWDelay(pI2C);
		I2CSWSCLHigh(pI2C);
		I2CSWDelay(pI2C);
		I2CSWSCLLow(pI2C);
		SetSDAInput(pI2C);

		I2CSWAckDelay(pI2C);

		if(pI2C->wr_method == I2C_WR_BYTE_WAIT)
		{
			I2CSWSCLHigh(pI2C);
			delay_1ms(1);
		}
	}
		
	for (bTemp=0, i=8; i; i--)
	{
		I2CSWDelay(pI2C);
		I2CSWSCLHigh(pI2C);
		I2CSWDelay(pI2C);
		bTemp = (bTemp<<1) | I2CSWReadSDA(pI2C);
		I2CSWSCLLow(pI2C);
	}

	data[len-1] = bTemp;
		
	// Set NACK !!
	I2CSWDelay(pI2C);
	//SetSDAOutput(pI2C);
	I2CSWSDAHigh(pI2C);
	I2CSWDelay(pI2C);   // scl -- low
	I2CSWSCLHigh(pI2C);
	I2CSWDelay(pI2C);   // scl -> high
	I2CSWSCLLow(pI2C);
	I2CSWDelay(pI2C);   // scl -> low
    
	//printf("read:");
	for(j=0; j<len; j++)
	{
	//	printf("0x%x ",data[j]);
	}

	//printf("over\n");
	
	return 0;
}


/****************************************************
Function:I2CSWRead
Description:
Input:const I2CSW* pI2C  
         int iDeviceAddr    
         int iAddr          
         BYTE *bBuffer      
         int len            
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2014/11/12           Created             chaofa.yan
Others:

*****************************************************/

eOpStat AnD_I2CSWRead(int fd, int iDeviceAddr, int iAddr, BYTE *bBuffer, int len)
{
    UINT32 loop;
    UINT8 bTmp = 0;
    UINT8 ret = 0;
	I2C_CFG* pI2C = I2C_GetSrv(fd);


 	if( pI2C == NULL)
		return eI2CFail;

	if(iAddr > 0xff)
	{
    		iDeviceAddr &= 0xf0;
    		iDeviceAddr |= ((iAddr & 0x700) >> 7);
	}

	for (loop=0; loop<3; loop++)
	{
	       I2CSWStartSig(pI2C);
		bTmp = iDeviceAddr & 0xfe;

		if(pI2C->read_method == I2C_READ_NO_REG)
		{
			bTmp = iDeviceAddr | 1;
		                
			if(eI2COK == I2CSWWriteBytes(pI2C, &bTmp, sizeof(bTmp)))
			{
			    	I2CSWReadBytes(pI2C, bBuffer, len);
			   	I2CSWStopSig(pI2C);
			   	break;
			}
		}
		else
		{
		        if(eI2COK == I2CSWWriteBytes(pI2C, &bTmp, sizeof(bTmp)))
		        {
		        	bTmp = iAddr;
		        	
		            if(eI2COK == I2CSWWriteBytes(pI2C, &bTmp, sizeof(bTmp)))
		            {

					if(pI2C->continue_start == I2C_START_SIG_CONTINUE_UNSUPPORT)
					{
						I2CSWStopSig(pI2C);
						I2CSWAckDelay(pI2C);
					}
					
		                	I2CSWStartSig(pI2C);
		                	bTmp = iDeviceAddr | 1;
		                
		                if(eI2COK == I2CSWWriteBytes(pI2C, &bTmp, sizeof(bTmp)))
		                {
		                    I2CSWReadBytes(pI2C, bBuffer, len);
		                    I2CSWStopSig(pI2C);
		                    break;
		                }
		            }
		        }
		 }
	        I2CSWReInit(pI2C);
	}
	
	
    	if (loop >= 3)
   	{
       		return eI2CFail + ret;
   	}

    	return eI2COK;
}




