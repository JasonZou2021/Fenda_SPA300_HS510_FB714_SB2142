#include <string.h>
#include <stdio.h>
#include "log_utils.h"
#include "busy_delay.h"
#include "vfd_seg_map.h"
#include "pinmux.h"
#include "initcall.h"
#include "busy_delay.h"
#include "exstd_interfaces.h"
#include "AnD_I2C_Srv.h"
#include "vfd.h"
#include "datarammap.h"

#define STANDBY_LED 45

#define	VFD_MEM_LEN			20
#define SEG_MAX	8

#define I2C_ARR	0XE0

#define I2C_CLK 22
#define I2C_DAT 23
static int vfd_fd = -1;

static BYTE vfd_mem[VFD_MEM_LEN] = {0};


#define SYS_ON					(1<<0)

#define ROW_OUTPUT				(0<<0)
#define ROW_INPUT_ACT_LOW		(1<<0)
#define ROW_INPUT_ACT_HIGH	(3<<0)

#define DISPLAY_ON				(1<<0)
#define DISPLAY_BLINK_2HZ		(1<<1)
#define DISPLAY_BLINK_1HZ		(2<<1)
#define DISPLAY_BLINK_0p5HZ		(3<<1)

enum
{
	REG_DATA_SET = 0X00,
	REG_SYS_SET = 0X20,
	REG_DISPLAY_SET = 0X80,
	REG_ROW_SET = 0XA0,
	REG_DIMMER_SET = 0XE0,
};

static int ET6238_dimmer(BYTE mode);
static BOOL ET6238Write(UINT32 bRegAddr, BYTE *bBuf, BYTE len)
{

 	BOOL bRet = FALSE;
	BYTE ret = 1;
	
	if(vfd_fd >= 0) 
	{
		ret = AnD_I2CSWWrite(vfd_fd, I2C_ARR, bRegAddr, bBuf, len);
		if(ret != eI2COK)
		{
			LOGD(" >> write()  dev reg %x data %x NG(%d)\n", bRegAddr, *((BYTE *)bBuf), ret);
		}else
		{
			bRet = TRUE;
			//vfd_printf1(" >> write()  dev reg %x data %x OK\n",bRegAddr, *((BYTE *)bBuf));	
		}
	}
	else
	{
		LOGD(" >> i2c pin fail \n");
	}

	return bRet;
}


static int ET6238_Show(void)
{
	BYTE *data = vfd_mem;

	ET6238Write(REG_DATA_SET, data, SEG_MAX*2);
	//printf(" %s : %x\n", __func__, data[i*2]);
	
	return 0;
}

static int ET6238_SetStr(const char  *s)
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
			else if(s[i] == '-')
				c= VFD_7SEG_SG|VFD_7SEG_SK;
			else if(s[i] == '+')
				c=VFD_7SEG_SG|VFD_7SEG_SK|VFD_7SEG_SI|VFD_7SEG_SM;
			else
				c = 0;			
		}
		else 
			c = 0;
		
		vfd_mem[i*2] =  c &0xff;
		vfd_mem[i*2+1] = (c >> 8)&0xff;

		//printf("%s : %c %x-%x\n ", __func__, s[i], vfd_mem[i*2], vfd_mem[i*2+1] );
	}

	return 0;
}

static int ET6238_Init(void)
{
	//BYTE dim = 2;
	Pinmux_GPIO_Free(STANDBY_LED);
	Pinmux_GPIO_Alloc(STANDBY_LED,IO_GPIO,IO_RISC);
	GPIO_OE_Set(STANDBY_LED, IO_OUTPUT);
	
	if(vfd_fd < 0)
	{

		if((Pinmux_GPIO_CheckStatus(I2C_CLK) == 0) && (Pinmux_GPIO_CheckStatus(I2C_DAT) == 0))
		{
			if(Pinmux_GPIO_Alloc(I2C_CLK,IO_GPIO,IO_RISC)==0 && Pinmux_GPIO_Alloc(I2C_DAT,IO_GPIO,IO_RISC)==0)
			{
				GPIO_OE_Set(I2C_CLK, IO_OUTPUT);
				GPIO_OE_Set(I2C_DAT, IO_OUTPUT);
			}	
			else
			{
				LOGD(" pin has use error 1111 \n ");
				return -1;
			}
		}
		else
		{
			LOGD(" pin has use error 2222 \n ");
			return -1;
		}

		I2C_CFG cfg;
		cfg.clk = I2C_CLK;
		cfg.dat = I2C_DAT;
		cfg.delay = 10;
		cfg.ack_delay = 10;
		cfg.high_voltage = I2C_HIGH_V_OUTPUT;
		cfg.byte_type = I2C_BYTE_HIGH_TO_LOW;
		cfg.continue_start = I2C_START_SIG_CONTINUE_SUPPORT;
	       vfd_fd = I2c_IO_Init(&cfg);

		
		
	}
	else
	{
		//GPIO_Output_Write(STANDBY_LED, IO_HIGH);
	}

	
		ET6238Write(REG_SYS_SET|SYS_ON, NULL, 0);
		
		volatile int *pRsm_Magic = (int * )SRAM_ADDR_POWER_MAGIC;
		//volatile int *pRsm_LVL = (int * )SRAM_ADDR_DIM_LVL;
	//	if(*pRsm_LVL >= 0x123320 && *pRsm_LVL <= 0x123322)
	//		dim = *pRsm_LVL & 0xf;
		//if(dim == 1)
		//	ET6238_dimmer(1);
		//else
		//	ET6238_dimmer(2);
		ET6238Write(REG_DIMMER_SET|6, NULL, 0);
	
		if(*pRsm_Magic == 0x123321)	
		{
			ET6238_SetStr("WELCOME ");
			GPIO_Output_Write(STANDBY_LED, IO_LOW);
		}
		else
		{
			ET6238_SetStr("        ");
			GPIO_Output_Write(STANDBY_LED, IO_HIGH);
		}

		ET6238_Show();
		ET6238Write(REG_DISPLAY_SET|DISPLAY_ON, NULL, 0);
	
	LOGD(" %s : %x\n", __func__ , vfd_fd);

	return (0);
}

static int ET6238_Standby_Init(void)
{
	
	if(vfd_fd < 0)
	{
		if((Pinmux_GPIO_CheckStatus(I2C_CLK) == 0) && (Pinmux_GPIO_CheckStatus(I2C_DAT) == 0))
		{
			if(Pinmux_GPIO_Alloc(I2C_CLK,IO_GPIO,IO_RISC)==0 && Pinmux_GPIO_Alloc(I2C_DAT,IO_GPIO,IO_RISC)==0)
			{
				GPIO_OE_Set(I2C_CLK, IO_OUTPUT);
				GPIO_OE_Set(I2C_DAT, IO_OUTPUT);
			}	
			else
			{
				LOGD(" pin has use error 1111 \n ");
				//return -1;
			}
		}
		else
		{
			LOGD(" pin has use error 2222 \n ");
			//return -1;
		}

		I2C_CFG cfg;
		cfg.clk = I2C_CLK;
		cfg.dat = I2C_DAT;
		cfg.delay = 10;
		cfg.ack_delay = 10;
		cfg.high_voltage = I2C_HIGH_V_OUTPUT;
		cfg.byte_type = I2C_BYTE_HIGH_TO_LOW;
		cfg.continue_start = I2C_START_SIG_CONTINUE_SUPPORT;
	       vfd_fd = I2c_IO_Init(&cfg);

		
	}

	{
		ET6238Write(REG_SYS_SET|SYS_ON, NULL, 0);
		ET6238_SetStr("        ");
		ET6238_Show();
		ET6238Write(REG_DISPLAY_SET|DISPLAY_ON, NULL, 0);
	}
	
	LOGD(" %s : %x\n", __func__ , vfd_fd);

	return (0);
}



static int ET6238_dimmer(BYTE mode)
{
	return -1;

	BYTE dim[3] = 
	{
		0, 3, 15,
	};

	if(mode >= sizeof(dim))
		mode = 2;

	if(mode == 0)
		ET6238Write(REG_DISPLAY_SET, NULL, 0);
	else
	{
		ET6238Write(REG_DIMMER_SET|dim[mode], NULL, 0);
		ET6238Write(REG_DISPLAY_SET|DISPLAY_ON, NULL, 0);
	}
	return 0;
}


int ET6238_Set(const char *s)
{
	ET6238_SetStr(s);
	return ET6238_Show();
}

int ET6238_dot(UINT32 p, UINT32 method)
{
	p = p;
	method = method;
	return 0;
}

UINT32 ET6238_ReadVFDKeys(void)
{
	return 0;
}


int ET6238_test(void)
{

	BYTE data[16];
	BYTE v_data = 0;
	static BYTE v_inc = 0;

	LOGD(" %s : %x\n", __func__, v_inc);

	v_data |= 1<<v_inc;
	if(v_inc < 7)
		v_inc++;
	else
		v_inc = 0;
	
	memset(data, v_data, 16);

	ET6238Write(REG_DATA_SET, data, SEG_MAX*2);
	
	
	return 0;
}

void ET6238_full_test(void)
{
	BYTE data[16];
	memset(data, 0xff, 16);

	ET6238Write(REG_DATA_SET, data, SEG_MAX*2);
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
**	
*/
static UINT16 ET6238_seg_bit_map[] = 
{

/*A*/	BIT_N(0),
/*B*/	BIT_N(1),
/*C*/	BIT_N(2),
/*D*/	BIT_N(3),
/*E*/	BIT_N(4),
/*F*/	BIT_N(5),
/*G*/	BIT_N(6),

/*H*/	BIT_N(8),
/*I*/	BIT_N(9),
/*J*/	BIT_N(10),
/*K*/	BIT_N(7),
/*L*/	BIT_N(11),
/*M*/	BIT_N(12),
/*N*/	BIT_N(13),

};


VFD_FUNC vfd_ET6238 = 
{
	ET6238_seg_bit_map,
	sizeof(ET6238_seg_bit_map),
	ET6238_Init,
	ET6238_Standby_Init,
	ET6238_Show,
	ET6238_Set,
	ET6238_dimmer,
	ET6238_dot,
	ET6238_ReadVFDKeys,
	NULL,
	
};





/* 
 * the routine was embedded in  boot flow.
 */
//int drv_vfd_init(void) __attribute__((weak, alias("aip650_Init")));
