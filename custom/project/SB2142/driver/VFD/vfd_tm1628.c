/*******************************************************************************

Copyright (c) 2015 A&D technology co. Ltd.All right reserved!

********************************************************************************
File Name        	:	vfd_drv_pt16556.c
Version          		:	release 0.1
Author           		:	Jasonzou	
Created          	:	2013/06/24
Last Modified    	:
Description      	:  
public functions 	:

History Information Description
Date				Author				Modification
2013/06/24		Jasonzou				Created
*******************************************************************************/

/*---------------------------------------------------------------------------*
*                            INCLUDE   DECLARATIONS                         *
*---------------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "log_utils.h"
#include "busy_delay.h"
#include "pinmux.h"
#include "initcall.h"
#include "busy_delay.h"
#include "exstd_interfaces.h"
#include "AnD_I2C_Srv.h"
#include "vfd_seg_map.h"
#include "Display_Srv.h"
#include "datarammap.h"
/*----------------------------------------------*
  * MACRO DECLARATIONS
  *----------------------------------------------*/
#define VFD_MEM_LEN 16
#define TOTAL_SEG	4
//================================================================================================
static int Tm1628_SetStr(const char *s);
static int Tm1628_Show(void);
/*----------------------------------------------*
  * FUNCTION DECLARATIONS
  *----------------------------------------------*/

#define VFD_7SEG_SL_POS0    BIT_N(8) 
#define VFD_7SEG_SJ_POS0    BIT_N(9) 

#define VFD_7SEG_SL_POS1    	BIT_N(4) 
#define VFD_7SEG_SJ_POS1    	BIT_N(11) 

#define VFD_7SEG_SL_POS2    	BIT_N(14) 
#define VFD_7SEG_SJ_POS2    	BIT_N(10) 

#define VFD_7SEG_SL_POS3    	BIT_N(13) 
#define VFD_7SEG_SJ_POS3    	BIT_N(15) 

#define VFD_7SEG_DOT1    	BIT_N(12) 	
#define VFD_7SEG_DOT2   	BIT_N(3) 	


#define EIGHT_BITS_PER_BYTE 8
static BYTE vfd_mem[VFD_MEM_LEN] = {0};
static int giVFD_STB_PinNum;
static int giVFD_CLK_PinNum;
static int giVFD_DIO_PinNum;
// ================================================================================================

/*----------------------------------------------*
  * DATA TYPE  DECLARATIONS
  *----------------------------------------------*/

typedef enum _eTM1623CmdMode
{
	// bit 7 & bit 6

	eDisPlayMode 			= 0,
	eDataCmdMode 			= (1<<6),
	eDisPlayControlMode 	= (1<<7),
	eAddressCmdSetMode 	= (1<<7) |(1<<6),
	
}_eCmdMode;

typedef enum _eTM1623DisplayMode
{
	// bit 0 & bit 1

	e4Grid14Seg = 0x00,
	e5Grid13Seg = 0x01,
	e6Grid12Seg = 0x02,
	e7Grid11Seg = 0x03,
}_eDisPlayMode;


typedef enum _eTM1623DataCmdMode
{
	// bit 2

	eAddressAutoIncrease 	= (0<<2),
	eAddressfixation 		= (1<<2),

}_eDataCmdMode;


typedef enum _eTM1623AddressCmdSetMode
{
	// bit 3 ~ bit 0
	eAddr0 = 0,
	eAddr1,
	eAddr2,
	eAddr3,
	eAddr4,
	eAddr5,
	eAddr6,
	eAddr7,
	eAddr8,
	eAddr9,
	eAddrA,
	eAddrB,
	eAddrC,
	eAddrD,

}_eAddressCmdSetMode;



typedef enum _eTM1623DisPlayControlMode
{
	// bit 2~0
	ePulseWidth1 	= 0,
	ePulseWidth2 	= 1,
	ePulseWidth4 	= 2,
	ePulseWidth10	= 3,
	ePulseWidth11 	= 4,
	ePulseWidth12 	= 5,
	ePulseWidth13 	= 6,
	ePulseWidth14 	= 7,

	// bit 3
	eDisplayOn 	= (1<<3),
	eDisplayOff  	= (0<<3),

}_eDisPlayControlMode;


static inline void SetGpio(UINT32 GpioIdx, UINT8 Data)
{
	int iRes = 0;      
    
   	iRes |= GPIO_OE_Set(GpioIdx, IO_OUTPUT);    
	iRes |= GPIO_Output_Write(GpioIdx, Data);
}


#define SetGpioSTB(bGpioLvl) 	SetGpio(giVFD_STB_PinNum, bGpioLvl)
#define SetGpioClk(bGpioLvl) 	SetGpio(giVFD_CLK_PinNum, bGpioLvl)
#define SetGpioDIO(bGpioLvl) 	SetGpio(giVFD_DIO_PinNum, bGpioLvl)
#define SetSPIStart() 			SetGpioSTB(0)
#define SetSPIStop() 			SetGpioSTB(1)


static void SendAByteToTm1628(const UINT8 bData)
{
	UINT32 i = 0;
	
	for(i=0; i<EIGHT_BITS_PER_BYTE; i++)
	{
		SetGpioClk(0);
		SetGpioDIO((bData >> i) & 0x01);
		//delay_1us(VFD_SPI_DELAY/2);
		
		SetGpioClk(1);
		//delay_1us(VFD_SPI_DELAY/2);
	}
}


// ===================================================================

static void Send_DisplayMode(_eDisPlayMode mode)
{
	SetSPIStart();

	SendAByteToTm1628(eDisPlayMode | mode);
	
	SetSPIStop();
}

static void Send_DataCmdMode(_eDataCmdMode mode)
{
	SetSPIStart();

	SendAByteToTm1628(eDataCmdMode | mode); 

	SetSPIStop();
}


static void Send_AddressCmdSetMode(const UINT8* const pBuf, const UINT32 dLen)
{
	BYTE i=0;
	
	SetSPIStart();

	SendAByteToTm1628(eAddressCmdSetMode | eAddr0);

	for(i=0; i<dLen; i++)
		SendAByteToTm1628(pBuf[i]);
	
	SetSPIStop();
	

}


static void Send_DisPlayControlMode(_eDisPlayControlMode mode)
{
	SetSPIStart();

	SendAByteToTm1628(eDisPlayControlMode | mode);

	SetSPIStop();
}


/****************************************************
Function:service_SRV_VFDDRV_INIT
Description:
Input:void  
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2015/05/29           Created             Jasonzou
Others:

*****************************************************/

static int Tm1628_GpioInit(void)
{
	
	LOGDT("InitTm1628Gpio in : vfd without IOP\n");	
	int iRes = 0;
	
	giVFD_STB_PinNum = 24;
	giVFD_CLK_PinNum = 23;
	giVFD_DIO_PinNum = 22;
	LOGDT("Pinmux_ModulePin_Find STB: %d\n", giVFD_STB_PinNum);    
	LOGDT("Pinmux_ModulePin_Find CLK: %d\n", giVFD_CLK_PinNum);    
	LOGDT("Pinmux_ModulePin_Find DIO: %d\n", giVFD_DIO_PinNum);    

	Pinmux_GPIO_Free(giVFD_STB_PinNum);
	Pinmux_GPIO_Free(giVFD_CLK_PinNum);
	Pinmux_GPIO_Free(giVFD_DIO_PinNum);

	iRes |= Pinmux_GPIO_Alloc(giVFD_STB_PinNum, IO_GPIO, IO_RISC);
	iRes |= Pinmux_GPIO_Alloc(giVFD_CLK_PinNum, IO_GPIO, IO_RISC);
	iRes |= Pinmux_GPIO_Alloc(giVFD_DIO_PinNum, IO_GPIO, IO_RISC);
	
	LOGDT("Pinmux_GPIO_Alloc: %d\n", iRes);       

	iRes |= GPIO_OE_Set(giVFD_STB_PinNum, IO_OUTPUT);
	iRes |= GPIO_OE_Set(giVFD_CLK_PinNum, IO_OUTPUT);    
	iRes |= GPIO_OE_Set(giVFD_DIO_PinNum, IO_OUTPUT);    
	LOGDT("GPIO_OE_Set: %d\n", iRes);          

	iRes |= GPIO_Output_Write(giVFD_STB_PinNum, IO_HIGH);
	iRes |= GPIO_Output_Write(giVFD_CLK_PinNum, IO_HIGH);    
	iRes |= GPIO_Output_Write(giVFD_DIO_PinNum, IO_HIGH);    
	LOGDT("GPIO_Output_Write: %d\n", iRes); 

	if (iRes != 0)
	{
	    LOGDT("driver: VFD init Fail!!!\n");
	}
	else
	{
	    LOGDT("driver: VFD  init OK!!!\n");
	}	

	return iRes;
}

extern int UserSetting_ReadSrc(UINT16 * pSrc);
extern int Hdmi_Repeater_Init(void);

static BYTE vfd_init_flag = 0;


static int Tm1628_Init(void)
{
	int iRes = 0;
		volatile int *psdiISP_BR_Magic = (int * )SRAM_ADDR_UART1_BR;

	
	//if(vfd_init_flag == 0)
	{
	
Pinmux_GPIO_Free(33);
Pinmux_GPIO_Alloc(33, IO_GPIO, IO_RISC);
GPIO_OE_Set(33, IO_OUTPUT);    
GPIO_Output_Write(33, IO_HIGH);

	
		delay_1ms(10);
		Tm1628_GpioInit();
	
		delay_1us(1);
		
		LOGD("@@@@@@ InitVfd @@@@@@, src  %d\n", vfd_init_flag);
		Send_DisPlayControlMode(eDisplayOff | ePulseWidth14);
		delay_1us(1);
		Send_DisplayMode(e5Grid13Seg);
		delay_1us(1);
		Send_DataCmdMode(eAddressAutoIncrease);
		delay_1us(1);
		if(*psdiISP_BR_Magic != 0x123456)
			vfd_set_str(" -- ");	
		else
			vfd_set_str(" ON ");
		delay_1us(1);
		Send_DisPlayControlMode(eDisplayOn | ePulseWidth14);

		*psdiISP_BR_Magic = 0x123456;

		LOGD("%s : VFD init OK \n", __func__);	

		vfd_init_flag = 1;

	}
	//else
	{
		LOGD("%s : VFD have init %d\n", __func__, vfd_init_flag);
	}
	
	return iRes;
}

static int Tm1628_dimmer(BYTE mode)
{
	if(mode)
		Send_DisPlayControlMode(eDisplayOn | ePulseWidth14);
	else
		Send_DisPlayControlMode(eDisplayOff);
	
	return 0;
}


static int Tm1628_PowerOff(void)
{
	vfd_set_str(" -- ");
	return 0;
}

static int Tm1628_Show(void)
{
	BYTE i=0;
	BYTE  *vfd_buf = vfd_mem;
	UINT16 vfd_data = 0;
	UINT16 vfd_temp_data = 0;

	UINT16 vfd_J[TOTAL_SEG] = {VFD_7SEG_SJ_POS0, VFD_7SEG_SJ_POS1, VFD_7SEG_SJ_POS2, VFD_7SEG_SJ_POS3};
	UINT16 vfd_L[TOTAL_SEG] = {VFD_7SEG_SL_POS0, VFD_7SEG_SL_POS1, VFD_7SEG_SL_POS2, VFD_7SEG_SL_POS3};

	for(i=0; i<TOTAL_SEG; i++)
	{
		vfd_data = vfd_buf[i*2];
		vfd_data = vfd_data<<8 | vfd_buf[i*2+1];
	
		if( vfd_data & VFD_7SEG_SL )
			vfd_temp_data |= vfd_L[i];

		if( vfd_data & VFD_7SEG_SJ )
			vfd_temp_data |= vfd_J[i];

		//vfd_printf1("%s, %x\n", __func__, vfd_temp_data);
	}

	vfd_buf[TOTAL_SEG*2] = (vfd_temp_data>>8)&0xff;
	vfd_buf[TOTAL_SEG*2+1] = (vfd_temp_data)&0xff;

	//vfd_printf1("%s\n", __func__);
	delay_1us(1);
	Send_DisplayMode(e5Grid13Seg);
	delay_1us(1);
	Send_DataCmdMode(eAddressAutoIncrease);
	delay_1us(1);
	Send_AddressCmdSetMode(vfd_buf, (TOTAL_SEG+1)*2);

	return 0;
}


void Tm1628_null(void)
{
	memset(vfd_mem, 0 , sizeof(vfd_mem));
	Tm1628_Show();
}


static int Tm1628_SetStr(const char *s)
{
	BYTE i,bStrLen;
	UINT16  c = 0;
	BYTE  *vfd_buf = vfd_mem;
	VFD_SEG_MAP *seg_map =  vfd_get_seg_table();
	
	bStrLen = strlen(s);

	for(i=0; i<TOTAL_SEG; i++)
	{
		if(i<bStrLen)
		{
			if((s[i]>='a') && (s[i]<='z'))
				c= seg_map->char_lowercase_tbl[s[i]-'a'];
			else if( (s[i]>='A')&&(s[i]<='Z') )
				c= seg_map->char_tbl[s[i]-'A'];
			else if( (s[i]>='0')&&(s[i]<='9') )
				c= seg_map->digit_tbl[s[i]-'0'];
			else if(s[i]=='.')
				c= VFD_7SEG_SE;
			else if(s[i] == '-')
				c= VFD_7SEG_SG|VFD_7SEG_SK;
			else if(s[i] == '+')
				c= VFD_7SEG_SG|VFD_7SEG_SK|VFD_7SEG_SI|VFD_7SEG_SM;
			else
				c = 0;			
		}
		else 
			c = 0;
		  
		vfd_buf[i*2]=(c>>8)&0xff;
    		vfd_buf[i*2+1]=c&0xff;
	}

	Tm1628_Show();

	return 0;

}




/*
static int Tm1628_Dimmer(BYTE level)
{
	BYTE level_tbl[] = {ePulseWidth4, ePulseWidth10, ePulseWidth14};


	Send_DisPlayControlMode(eDisplayOn | level_tbl[level]);

	vfd_printf1("%s : VFD init OK \n", __func__);

	return 0;
}
*/

// ==================================================


//=========== for test ======================

void vfd_test()
{
	BYTE i=0;
	UINT16 data = 0;
	static BYTE cnt = 0;
	BYTE  *vfd_buf = vfd_mem;
	
	data = 1<<cnt;

	LOGD("vfd_test %d!!! \n", cnt);

	if(cnt++ > 14)
		cnt = 0;
	
	for(i=0; i<TOTAL_SEG; i++)
	{
		vfd_buf[i*2] = (data>>8)&0xff;
		vfd_buf[i*2+1] = data&0xff;
	}

	
	Send_AddressCmdSetMode(vfd_buf, TOTAL_SEG*2);
}
/*
void vfd_test_LJ()
{
	BYTE i=0;
	UINT16 data = 0;
	static BYTE cnt = 0;
	BYTE  *vfd_buf = vfd_mem;
	
	data = 1<<cnt;

	vfd_printf1("vfd_test_LJ %d!!! \n", cnt);

	if(cnt++ > 14)
		cnt = 0;

	vfd_buf[0] = (data>>8)&0xff;
	vfd_buf[1] = data&0xff;
	
	for(i=0; i<2; i++)
	{
		SetSPIStart();

		SendAByteToTm1628(eAddressCmdSetMode | (eAddr0+(TOTAL_SEG*2+i)) );
		
		SendAByteToTm1628(vfd_buf[i]);
		
		SetSPIStop();
	}
}
*/


static UINT16 TM1628_seg_bit_map[] = 
{
     	BIT_N(9),  //A
	BIT_N(11),  //B
	BIT_N(10),  //C
	BIT_N(15),  //D
	BIT_N(8),  //E
	BIT_N(4),  //F
	BIT_N(14),   // G
	BIT_N(12),   //H
	BIT_N(3),   //I
	BIT_N(6),    //J
	BIT_N(13),   // K
	BIT_N(5),   // L
	BIT_N(0),   //M
	BIT_N(1),   //N
};


VFD_FUNC vfd_TM1628 = 
{
	TM1628_seg_bit_map,
	sizeof(TM1628_seg_bit_map),
	Tm1628_Init,
	NULL,
	Tm1628_Show,
	Tm1628_SetStr,
	Tm1628_dimmer,
	NULL,
	NULL,
	Tm1628_PowerOff,
	
};



