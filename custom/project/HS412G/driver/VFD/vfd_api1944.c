#include <string.h>
#include <stdio.h>
#include "log_utils.h"
#include "busy_delay.h"
#include "pinmux.h"
#include "vfd_seg_map.h"
#include "initcall.h"
#include "busy_delay.h"
#include "exstd_interfaces.h"
#include "AnD_I2C_Srv.h"
#include "vfd.h"
#include "datarammap.h"

#define SEG_MAX	5
#define VFD_MEM_LEN 10
static BYTE vfd_mem[VFD_MEM_LEN] = {0};

//static BYTE vfd_dimer;

int aip1944_Set(const char *s);
void aip1944_Full(void);

#define VFD_WRITE_DELAY	2
#define EIGHT_BITS_PER_BYTE 8


#define SPI_CLK  	23
#define SPI_STB	24
#define SPI_DAT  	22

#define STANDBY_LED  	42
#define VFD_POWER_PIN	42

static void SetGpioSTB(UINT8 bGpioLvl)
{
	GPIO_Output_Write(SPI_STB, bGpioLvl);
}

static void SetGpioClk(UINT8 bGpioLvl)
{
	GPIO_Output_Write(SPI_CLK, bGpioLvl);
}

static void SetGpioDIO(UINT8 bGpioLvl)
{
	GPIO_Output_Write(SPI_DAT, bGpioLvl);
}

static void SetStart(void)
{
	SetGpioSTB(0);
	//delay_1us(VFD_WRITE_DELAY);
	
	SetGpioClk(1);
	SetGpioDIO(1);
	//delay_1us(VFD_WRITE_DELAY);
	SetGpioDIO(0);
	//delay_1us(VFD_WRITE_DELAY);
	SetGpioClk(0);
}

static void SetStop(void)
{
	SetGpioClk(0);
	SetGpioDIO(0);
	//delay_1us(VFD_WRITE_DELAY);
	SetGpioClk(1);
	//delay_1us(VFD_WRITE_DELAY);
	SetGpioDIO(1);

	//delay_1us(VFD_WRITE_DELAY);
	SetGpioSTB(1);
}


static void SendAByte(const UINT8 bData)
{
	UINT32 i = 0;
	
	for(i=0; i<EIGHT_BITS_PER_BYTE; i++)
	{
		SetGpioClk(0);
	
		SetGpioDIO((bData >> i) & 0x01);
		//delay_1us(VFD_WRITE_DELAY);
		
		SetGpioClk(1);
		//delay_1us(VFD_WRITE_DELAY);
	}
}


static void aip1944Write(UINT32 bRegAddr, BYTE *data, BYTE len)
{
	BYTE i=0;
	
	SetStart();

	SendAByte(bRegAddr);

	for(i=0; i<len; i++)	
		SendAByte(data[i]);
	

	SetStop();

}


static int aip1944_Show(void)
{
	BYTE *data = vfd_mem;
	
	aip1944Write(0xc0, &data[0], 2);
	aip1944Write(0xc4, &data[2], 2);
	aip1944Write(0xc8, &data[4], 2);
	aip1944Write(0xcc, &data[6], 2);
	aip1944Write(0xd0, &data[8], 2);
	//printf(" %s : %x\n", __func__, data[i*2]);

	//if(vfd_dimer == 0)
		//aip1944Write(0x8f, NULL, 0);
	
	return 0;
}


static int aip1944_SetStr(const char  *s)
{
	BYTE    i,bStrLen;
	UINT16  c = 0;	
	VFD_SEG_MAP *seg_map = vfd_get_seg_table();
	if(seg_map == NULL)
		return -1;
		
	bStrLen = strlen(s);
	for(i=0; i<SEG_MAX; i++)
	{
		if(i<bStrLen)
		{
			if((s[i]>='a') && (s[i]<='z'))
				c= seg_map->char_lowercase_tbl[s[i]-'a'];
			else if( (s[i]>='A')&&(s[i]<='Z') )
				c= seg_map->char_tbl[s[i]-'A'];
			else if( (s[i]>='0')&&(s[i]<='9') )
				c= seg_map->digit_tbl[s[i]-'0'];
			else if(s[i]==':')
				c= VFD_7SEG_SG|VFD_7SEG_SK;
			else if(s[i] == '-')
				c= VFD_7SEG_SG|VFD_7SEG_SK;
			else if(s[i] == '_')
				c= VFD_7SEG_SD;
			else if(s[i] == '+')
				c= VFD_7SEG_SG|VFD_7SEG_SK|VFD_7SEG_SI|VFD_7SEG_SM;
			else
				c = 0;			
		}
		else 
			c = 0;
		
		vfd_mem[i*2] =  (c &0xff);
		vfd_mem[i*2+1] = (c >> 8)&0xff;

		//printf("%s : %c %x-%x\n ", __func__, s[i], vfd_mem[i*2], vfd_mem[i*2+1] );
	}

	

	return 0;
}

int vfd_init_flag = -1;
static int aip1944_Init(void)
{

	volatile int *pRsm_Magic = (int * )SRAM_ADDR_UART1_BR;
	//volatile int *pRsm_LVL = (int * )SRAM_ADDR_DIM_LVL;

	//if(vfd_init_flag < 0)
	{
		vfd_init_flag = 0;

		Pinmux_GPIO_Free(33);
		Pinmux_GPIO_Alloc(33,IO_GPIO,IO_RISC);
		GPIO_OE_Set(33, IO_OUTPUT);

		if(*pRsm_Magic == 0x123321)	
		{
			GPIO_Output_Write(33, IO_HIGH);
		}
		else
		{
			GPIO_Output_Write(33, IO_LOW);
		}

		Pinmux_GPIO_Free(SPI_STB);
		Pinmux_GPIO_Alloc(SPI_STB,IO_GPIO,IO_RISC);
		Pinmux_GPIO_Free(SPI_CLK);
		Pinmux_GPIO_Alloc(SPI_CLK,IO_GPIO,IO_RISC);
		Pinmux_GPIO_Free(SPI_DAT);
		Pinmux_GPIO_Alloc(SPI_DAT,IO_GPIO,IO_RISC);
		GPIO_OE_Set(SPI_STB, IO_OUTPUT);
		GPIO_OE_Set(SPI_CLK, IO_OUTPUT);
		GPIO_OE_Set(SPI_DAT, IO_OUTPUT);	
		GPIO_Output_Write(SPI_STB, IO_HIGH);
		GPIO_Output_Write(SPI_CLK, IO_HIGH);
		GPIO_Output_Write(SPI_DAT, IO_HIGH);


		Pinmux_GPIO_Free(STANDBY_LED);
		Pinmux_GPIO_Alloc(STANDBY_LED,IO_GPIO,IO_RISC);
		GPIO_OE_Set(STANDBY_LED, IO_OUTPUT);
		//if(*pRsm_Magic == 0x123321)	
		{
			GPIO_Output_Write(STANDBY_LED, IO_HIGH);
		}
		//else
		{
		//	GPIO_Output_Write(STANDBY_LED, IO_LOW);
		}

		delay_1ms(50);



		
	
	}

	aip1944Write(0x00, NULL, 0);  // display mode
	aip1944Write(0x40, NULL, 0); // address auto increase

	if(*pRsm_Magic == 0x123321)	
	{
		aip1944_Set("HELLO");	
	}
	else
	{
		aip1944_Set("     ");
	}

	*pRsm_Magic = 0x123321;

/*
	if(*pRsm_LVL == 0x123320 )
		aip1944Write(0x8f, NULL, 0); // display on & dimmer max
	else if(*pRsm_LVL == 0x123321)
		aip1944Write(0x8A, NULL, 0); 
	else
*/		aip1944Write(0x8f, NULL, 0); // display on & dimmer max

	//vfd_dimer = *pRsm_LVL&0xf;
	
	LOGD(" %s : %x\n", __func__ , vfd_init_flag);

	return (0);
}

static int aip1944_Standby_Init(void)
{
	//if(vfd_init_flag < 0)
	{
		vfd_init_flag = 0;
		
		Pinmux_GPIO_Alloc(SPI_STB,IO_GPIO,IO_RISC);
		Pinmux_GPIO_Alloc(SPI_CLK,IO_GPIO,IO_RISC);
		Pinmux_GPIO_Alloc(SPI_DAT,IO_GPIO,IO_RISC);
		GPIO_OE_Set(SPI_STB, IO_OUTPUT);
		GPIO_OE_Set(SPI_CLK, IO_OUTPUT);
		GPIO_OE_Set(SPI_DAT, IO_OUTPUT);	
		GPIO_Output_Write(SPI_STB, IO_HIGH);
		GPIO_Output_Write(SPI_CLK, IO_HIGH);
		GPIO_Output_Write(SPI_DAT, IO_HIGH);
		

		aip1944Write(0x00, NULL, 0);  // display mode
		aip1944Write(0x40, NULL, 0); // address auto increase
		aip1944_SetStr("     ");
		
	}

	aip1944Write(0x8f, NULL, 0); // display on & dimmer max
	LOGD(" %s : %x\n", __func__ , vfd_init_flag);

	return (0);
}



static int aip1944_dimmer(BYTE mode)
{
	mode = mode;

	BYTE cmd[3]= 
	{
		0x8f, 0x8A, 0x80, 
	};

	if(mode > 2)
		mode = 2;

//	vfd_dimer = mode;

	aip1944Write(cmd[mode], NULL, 0);
	
	return 0;
}


int aip1944_Set(const char *s)
{
	aip1944_SetStr(s);
	return aip1944_Show();
}

int aip1944_dot(UINT32 p, UINT32 method)
{
	p = p;
	method = method;
	
	return 0;
}

int aip1944_off(void)
{
	GPIO_Output_Write(STANDBY_LED, IO_LOW);
	return 0;
}


UINT32 aip1944_ReadVFDKeys(void)
{
	return 0;
}

void aip1944_heng(void)
{
	BYTE i=0;
	UINT16  c = 0;

	for(i=0; i<SEG_MAX; i++)
	{
		c= VFD_7SEG_SG|VFD_7SEG_SK|VFD_7SEG_SA|VFD_7SEG_SD;

		vfd_mem[i*2] =  (c &0xff);
		vfd_mem[i*2+1] = (c >> 8)&0xff;
	}

	aip1944_Show();
}

void aip1944_shu(void)
{
	BYTE i=0;
	UINT16  c = 0;

	for(i=0; i<SEG_MAX; i++)
	{
		c= VFD_7SEG_SF|VFD_7SEG_SI|VFD_7SEG_SB|VFD_7SEG_SE|VFD_7SEG_SM|VFD_7SEG_SC;

		vfd_mem[i*2] =  (c &0xff);
		vfd_mem[i*2+1] = (c >> 8)&0xff;
	}

	aip1944_Show();
}


void aip1944_xie(void)
{
	BYTE i=0;
	UINT16  c = 0;

	for(i=0; i<SEG_MAX; i++)
	{
		c= VFD_7SEG_SH|VFD_7SEG_SJ|VFD_7SEG_SN|VFD_7SEG_SL;

		vfd_mem[i*2] =  (c &0xff);
		vfd_mem[i*2+1] = (c >> 8)&0xff;
	}

	aip1944_Show();
}

void aip1944_null(void)
{
	BYTE i=0;
	UINT16  c = 0;

	for(i=0; i<SEG_MAX; i++)
	{
		c= 0;

		vfd_mem[i*2] =  (c &0xff);
		vfd_mem[i*2+1] = (c >> 8)&0xff;
	}

	aip1944_Show();
}

void aip1944_Full(void)
{
	BYTE i=0;
	UINT16  c = 0;

	for(i=0; i<SEG_MAX; i++)
	{
		c= 0xffff;

		vfd_mem[i*2] =  (c &0xff);
		vfd_mem[i*2+1] = (c >> 8)&0xff;
	}

	aip1944_Show();
}


		/*
** Definition for 14-seg display
**
** we assume 7-seg are defined as traditional 14-segment LED.
**
**	   AAAAAAAAA			
**	F  H     I     J    B				 
**	F    H   I  J       B				 
**	   GGG    KKKK			 
**	E     N  M  L     C				 
**	E  N     M     L  C				 
**     DDDDDDDDD				 
**					 
*/	  

static UINT16 aip1944_seg_bit_map[] = 
{

	BIT_N(0),  //A
	BIT_N(1),  //B
	BIT_N(2),  //C
	BIT_N(3),  //D
	BIT_N(4),  //E
	BIT_N(5),  //F
	BIT_N(6),   // G
	BIT_N(8),   //H
	BIT_N(9),   //I
	BIT_N(10),    //J
	BIT_N(7),   // K
	BIT_N(13),   // L
	BIT_N(12),   //M
	BIT_N(11),   //N

};


VFD_FUNC vfd_aip1944 = 
{
	aip1944_seg_bit_map,
	sizeof(aip1944_seg_bit_map),
	aip1944_Init,
	aip1944_Standby_Init,
	aip1944_Show,
	aip1944_Set,
	aip1944_dimmer,
	aip1944_dot,
	aip1944_ReadVFDKeys,
	aip1944_off,
	
};




/* 
 * the routine was embedded in  boot flow.
 */
//int drv_vfd_init(void) __attribute__((weak, alias("aip1944_Init")));


