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
#include "VFD_TM1640.h"


#define VFD_MEM_LEN 16
#define TOTAL_SEG	5

#define I2C_CLK 22
#define I2C_DAT 23


static BYTE vfd_mem[VFD_MEM_LEN] = {0};
static BYTE vfd_dot_mem;

#define VFD_WRITE_DELAY	5
#define EIGHT_BITS_PER_BYTE 8

static void SetGpioClk(UINT8 bGpioLvl)
{

	GPIO_Output_Write(I2C_CLK, bGpioLvl);
}

static void SetGpioDIO(UINT8 bGpioLvl)
{

	GPIO_Output_Write(I2C_DAT, bGpioLvl);
}

static void SetStart(void)
{
	SetGpioClk(1);
	SetGpioDIO(1);
//	delay_1us(VFD_WRITE_DELAY);
	SetGpioDIO(0);
//	delay_1us(VFD_WRITE_DELAY);
	SetGpioClk(0);
}


static void SetStop(void)
{
	SetGpioClk(0);
	SetGpioDIO(0);
//	delay_1us(VFD_WRITE_DELAY);
	SetGpioClk(1);
//	delay_1us(VFD_WRITE_DELAY);
	SetGpioDIO(1);
}


static void SendAByte(const UINT8 bData)
{
	UINT32 i = 0;
	
	for(i=0; i<EIGHT_BITS_PER_BYTE; i++)
	{
		SetGpioClk(0);
	
		SetGpioDIO((bData >> i) & 0x01);
	//	delay_1us(VFD_WRITE_DELAY);
		
		SetGpioClk(1);
	//	delay_1us(VFD_WRITE_DELAY);
	}
}


static void TM1640Write(UINT32 bRegAddr, BYTE *data, UINT32 len)
{
	BYTE i=0;
	
	SetStart();

	SendAByte(bRegAddr);

	for(i=0; i<len; i++)	
		SendAByte(data[i]);
	

	SetStop();
}


static int tm1640_Show(void)
{
	BYTE i = 0, j = 0;
	BYTE tm1640_mem[TM1640_MEM_CACHE];
	UINT16 vfd_data = 0;

	memset(tm1640_mem, 0, TM1640_MEM_CACHE);

	for(i=0; i<TOTAL_SEG; i++)   
	{
		vfd_data = vfd_mem[i*2+1];    //high 8 bit
		vfd_data = vfd_data<<8 | vfd_mem[i*2];    //high 8 bit + low 8 bit
	
		for(j=0; j<TM1640_MEM_CACHE; j++)     //14¶Î
		{		
			if( vfd_data & 1<<j )   
			{
				tm1640_mem[j] |= 1<<i;    
			}
		}
	}

	vfd_data = vfd_dot_mem; 
	for(j=0; j<TM1640_MEM_CACHE; j++)    
	{		
		if( vfd_data & 1<<j )   
		{
			tm1640_mem[j] |= 1<<i;    
		}
	}


	TM1640Write(eAddressCmdSetMode | eAddr0, tm1640_mem, TM1640_MEM_CACHE);
	
	
	return 0;
}

static int tm1640_SetStr(const char  *s)
{
	BYTE    i,bStrLen;
	UINT16  c = 0;	
	VFD_SEG_MAP *seg_map = vfd_get_seg_table();
	if(seg_map == NULL)
		return -1;
		
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
			else if(s[i] == '-')
				c= VFD_7SEG_SG | VFD_7SEG_SK;
			else if(s[i] == '_')
				c= VFD_7SEG_SD;
			else if(s[i] == '+')
				c = VFD_7SEG_SG | VFD_7SEG_SK | VFD_7SEG_SI | VFD_7SEG_SM;
			else 
				c = 0;			
		}
		else 
			c = 0;
		
		vfd_mem[i*2] =  c &0xff;
		vfd_mem[i*2+1] = (c >> 8)&0xff;

	//	vfd_printf1("%s : %c %x-%x\n ", __func__, s[i], vfd_mem[i*2], vfd_mem[i*2+1] );
	}

	return 0;
}


BYTE vfd_init_flag = 1;

static int tm1640_Init(void)
{ 
	BYTE tm1640_mem[TM1640_MEM_CACHE]; 
	memset(tm1640_mem, 0, TM1640_MEM_CACHE);

	vfd_init_flag = 0;
	
	if((Pinmux_GPIO_CheckStatus(I2C_CLK) == 0) && (Pinmux_GPIO_CheckStatus(I2C_DAT) == 0))
	{
		if(Pinmux_GPIO_Alloc(I2C_CLK,IO_GPIO,IO_RISC)==0 && Pinmux_GPIO_Alloc(I2C_DAT,IO_GPIO,IO_RISC)==0)
		{
			GPIO_OE_Set(I2C_CLK, IO_OUTPUT);
			GPIO_OE_Set(I2C_DAT, IO_OUTPUT);
		}	
	}

	TM1640Write(eDataCmdMode | eAddressAutoIncrease, NULL, 0);
	TM1640Write(eAddressCmdSetMode | eAddr0, tm1640_mem, TM1640_MEM_CACHE);
	TM1640Write(eDisPlayControlMode | eDisplayOn | ePulseWidth14, NULL, 0);

//	tm1640_SetStr(" ON ");
	tm1640_SetStr("     ");
	tm1640_Show();
	
	LOGDT(" %s \n", __func__ );

	return (0);
}

static int tm1640_Standby_Init(void)
{

	return (0);
}



static int tm1640_dimmer(BYTE mode)
{
	if(mode >= 4)     // {{Added by Heaven
		mode = 0;

	BYTE cmd[] = 
	{
		eDisplayOn | ePulseWidth14,
		eDisplayOn | ePulseWidth4,	
		eDisplayOn | ePulseWidth1,	
		eDisplayOff,
	};

	TM1640Write(eDisPlayControlMode | cmd[mode], NULL, 0);

	return 0;
}


int tm1640_Set(const char *s)
{
	tm1640_SetStr(s);
	return tm1640_Show();
}

int tm1640_dot(UINT32 p, UINT32 method)
{
	if(p == ICON_SET)
	{
		vfd_dot_mem |= method;
	}
	else if(p == ICON_CLEAN)
	{
		vfd_dot_mem &= ~method;
	}
	
	return 0;
}

UINT32 tm1640_ReadVFDKeys(void)
{
	return 0;
}


/*
int tm1640_test(void)
{
	
	BYTE v_data = 0;
	static BYTE v_inc = 0;
	BYTE tm1640_mem[TM1640_MEM_CACHE] = {0};

	v_data |= 1<<v_inc;
	if(v_inc < 7)
		v_inc++;
	else
		v_inc = 0;
	
	memset(tm1640_mem, v_data, TM1640_MEM_CACHE);

	vfd_printf1(" %s : seg v_inc %d \n", __func__,v_inc );

	TM1640Write(eAddressCmdSetMode | eAddr0, tm1640_mem, TM1640_MEM_CACHE);
	
	return 0;
}
*/

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
**	
*/
static UINT16 tm1640_seg_bit_map[] = 
{

	BIT_N(0),  //A	
	BIT_N(1) , //B
	BIT_N(2) , //C
	BIT_N(3),  //D
	BIT_N(4) , //E
	BIT_N(5) , //F
	BIT_N(6) ,  //G
	
	BIT_N(8) ,  //H	
	BIT_N(9) ,  //I
	BIT_N(10) ,   //J	
	BIT_N(7),   // K
	BIT_N(11),   // L   	
     	BIT_N(12) ,  //M
	BIT_N(13) ,  //N	  	
};


VFD_FUNC vfd_tm1640 = 
{
	tm1640_seg_bit_map,
	sizeof(tm1640_seg_bit_map),
	tm1640_Init,
	tm1640_Standby_Init,
	tm1640_Show,
	tm1640_Set,
	tm1640_dimmer,
	tm1640_dot,
	tm1640_ReadVFDKeys,
	NULL,
	
};




/* 
 * the routine was embedded in  boot flow.
 */
//int drv_vfd_init(void) __attribute__((weak, alias("aip650_Init")));



