
#include "busy_delay.h"
#include "pinmux.h"
#include "exstd_interfaces.h"
#define LOG_MODULE_ID  ID_APUSR
#define LOG_EXTRA_STR  "[TOUCH]"
#include "log_utils.h"
#include "custom_sig.h"
#include "ir_key.h"
#include "AnD_I2C_Srv.h"
#include "i2c_api.h"
#include "datarammap.h"
#include "touch_app.h"

QActive *UserApp_get(void);

#define I2C_CLK	11
#define I2C_DAT	10
#define TOUCH_ISR	25

#define I2C_ADDR	0xAC

#define I2C_DELAY_NORMAL	1000
#define I2C_DELAY_STANDBY 	3


#define TOUCH_REG_KEY 0x34
/****************************************************

0：Press  1：Release  2：Long Press  3：Long Release  4：Tick

******************************************************/

// 0x23
#define TOUCH_0CH	(1<<0)
#define TOUCH_1CH	(1<<1)
#define TOUCH_2CH	(1<<2)
#define TOUCH_3CH	(1<<3)
#define TOUCH_4CH	(1<<4)

enum
{
	TOUCH_KEY_PRESS,
	TOUCH_KEY_RELEASE,
	TOUCH_KEY_LONG_PRESS,
	TOUCH_KEY_LONG_RELEASE,
	TOUCH_KEY_TICK,
};


enum
{
	TOUCH_POWER,
	TOUCH_SRC,
	TOUCH_BT,
	TOUCH_VOL_UP,
	TOUCH_VOL_DN,
};

typedef struct _touch_key
{
	BYTE key_val;
	BYTE key_pre_val;
	BYTE key_type;
	UINT16 key_tick;
	
}TOUCH_KEY;

typedef struct _touch_handle
{

	int fd;
	BYTE status;
	I2C_CFG cfg;
	char *i2c_str;
	TOUCH_KEY key;
	UINT16 *key_tbl;
	UINT32 *sig_tbl;

	
}TOUCH_HANDLE;

static TOUCH_HANDLE touch_handle ={.fd = -1, .i2c_str = "i2c1_sw"};

#define	TOUCK_KEY_MAX		6

//--{{ Modified by Grace to keep same order to silk printing, and key touch sensitivity
static BYTE touch_value_table[TOUCK_KEY_MAX] = 
{
	TOUCH_0CH, TOUCH_1CH,  TOUCH_2CH,  TOUCH_3CH,  TOUCH_4CH,  TOUCH_0CH|TOUCH_1CH,
};

static UINT16 touch_key_table[TOUCK_KEY_MAX] = 
{
	IRC_VOLUME_DN, IRC_VOLUME_UP, IRC_PLAY_PAUSE, IRC_SRC, IRC_POWER,  IRC_DEFAULT_SETUP,
};

static UINT32 touch_sig_table[TOUCK_KEY_MAX] = 
{
	VOL_DOWN_SIG, VOL_UP_SIG, PLAY_SIG, SOURCE_SW_SIG, POWER_SIG, RESET_SIG,
		
};


/*
static BYTE touch_Trigglevel[] = 
{
	23, 10, 7, 6, 5, 0XFF, 0XFF, 0XFF, 
	//8, 5, 3, 2, 3, 0XFF, 0XFF, 0XFF, 
	
};

static BYTE touch_Standby_Trigglevel[] = 
{
	0XFF, 0XFF, 0XFF, 0XFF, 3, 0XFF, 0XFF, 0XFF, 
};
*/

static BYTE touch_Trigglevel[] = 

{
	8, 5, 3, 2, 3, 0XFF, 0XFF, 0XFF, 
};

static BYTE touch_Standby_Trigglevel[] = 

{
	0XFF, 0XFF, 0XFF, 2, 3, 0XFF, 0XFF, 0XFF, 
};

#if 1

static BOOL TouchApp_Write(UINT32 bRegAddr, BYTE *bBuf, UINT32 dLen)
{
	BOOL bRet = FALSE;
	int sdRet = -1;
	TOUCH_HANDLE *touch = &touch_handle;

	I2C_CFG *cfg = I2C_GetSrv(touch->fd);
	
	if(cfg == NULL)
		return -1;
	
	if(touch->status == NORMAL)
	{
		touch->cfg.delay = 20;
		touch->cfg.ack_delay = 100; 
	}
	else
	{
		touch->cfg.delay = 1;
		touch->cfg.ack_delay = 5; 
	}
	
	I2c_IO_ReInit(cfg);

	if(touch->fd >= 0)
	{
		sdRet = AnD_I2CSWWrite( touch->fd, I2C_ADDR, bRegAddr, bBuf, dLen);	
		if(sdRet > 0)
		{
			bRet = FALSE;
		//	LOGD("%s  fd %d,  Reg %x NG\n", __func__, touch->fd, bRegAddr);
		}
		else
		{
		//	bRet = TRUE;
		//	LOGD("%s  addr %x Reg %x OK\n", __func__,  bRegAddr);
		}
	}

	return (bRet);
}


static int TouchApp_Read(BYTE reg, BYTE *data)
{
	TOUCH_HANDLE *touch = &touch_handle;
	I2C_CFG *cfg = I2C_GetSrv(touch->fd);

	if(cfg == NULL || data == NULL)
		return -1;

	if(touch->status == NORMAL)
	{
		touch->cfg.delay = 20;
		touch->cfg.ack_delay = 100; 
	}
	else
	{
		touch->cfg.delay = 1;
		touch->cfg.ack_delay = 5;//20; 
	}

	I2c_IO_ReInit(cfg);

	if(AnD_I2CSWRead(touch->fd, I2C_ADDR, reg, data, 1) != eI2COK)
	{
		reg = reg;
		data = data;
		//LOGD(" TouchApp_Read reg %x  data %x NG   delay %d\n ", reg, *data , touch->cfg.delay);
		return -1;
	}
	else
	{
		//LOGD(" TouchApp_Read reg %x  data %x OK  delay %d\n ", reg, *data , touch->cfg.delay);
	}

	return 0;
}

BYTE touch_reg[27][2] = 
{
	{0x3a,0x5a},
	{0x23,0x00},
	{0x24,0x00},
	{0x25,0x00},
	{0x26,0x08},
	{0x27,0x02},
	{0x28,0x02},
	{0x29,0x80},
	{0x2a,0xff},
	{0x2b,0x04},
	{0x2c,0x00},
	{0x2d,0x00},
	{0x3a,0x00},

	{0x3a,0x5a},
	{0x21,0x50},
	{0x2c,0x08},
	{0x22,0x00},
	{0x23,0xe0},
	{0x20,0x06},
	{0x00,0x08},
	{0x01,0x05},
	{0x02,0x03},
	{0x03,0x02},
	{0x04,0x03},
	{0x05,0x08},
	{0x06,0x08},
	{0x07,0x08},
};

static int TouchApp_init(int para)
{
	TOUCH_HANDLE *touch = &touch_handle;
	I2C_CFG temp; 
	BYTE i=0;
	BYTE data;
	
	touch->cfg.clk = I2C_CLK;
	touch->cfg.dat = I2C_DAT;
	touch->status = para;
	
	if(para == NORMAL)
	{
		touch->cfg.delay = 10;
		touch->cfg.ack_delay = 50; 
	}
	else
	{
		touch->cfg.delay = 1;
		touch->cfg.ack_delay = 5; 
	}

	//Pinmux_GPIO_Free(TOUCH_ISR);
	//Pinmux_GPIO_Alloc(TOUCH_ISR,IO_GPIO, IO_RISC);
	//GPIO_OE_Set(TOUCH_ISR, IO_INPUT);
	
	if(touch->fd < 0)
	{
		touch->cfg.byte_type = I2C_BYTE_HIGH_TO_LOW;
		touch->cfg.high_voltage = I2C_HIGH_V_OUTPUT;
		touch->cfg.continue_start = I2C_START_SIG_CONTINUE_SUPPORT;
		touch->cfg.read_method = I2C_READ_HAVE_REG;
		touch->cfg.wr_method = I2C_WR_BYTE_NORMAL;

		touch->fd = I2c_IO_Check_Used(&touch->cfg);

		if(touch->fd < 0)
			touch->fd = I2c_IO_Init(&touch->cfg);

		touch->sig_tbl = touch_sig_table;
		touch->key_tbl = touch_key_table;
		touch->key.key_val = IRC_NULL;
		touch->key.key_pre_val = IRC_NULL;
		touch->key.key_type = 0;
		touch->key.key_tick = 0;
		
	}
	else
	{
		if(I2c_IO_Get_Cfg(touch->fd, &temp) == 0)
		{
			if(temp.delay != touch->cfg.delay)
				 I2c_IO_ReInit(&touch->cfg);
		}
		
		//LOGD1(" >>>>>>>>>>> I2c_IO_ReInit  (%d)\n ", touch->fd);
	}

/*

	3a  5a
	23  00
	24  00
	25  00
	26  08
	27  02
	28  02
	29  80
	2a  ff
	2b  04
	2c  00
	2d  00
	3a  00

	3a  5a
	21  50
	2c  08
	22  00
	23  e0
	20 06
	
	00  08
	01  05
	02  03
	03  02
	04  03
	05  08
	06  08
	07  08
	3a  00

	read 0x2c 08
	read 0x34 00
*/
	
	delay_1ms(50);

	data = 0x5a;  // 配置模式
	TouchApp_Write(0X3a, &data, 1);

#if 1
	data = 0x5a;  // 配置模式
	TouchApp_Write(0X3a, &data, 1);

	delay_1ms(30);

	data = 0x06; //0x0d; // 全局灵敏度默认02，最大0F, 0F灵敏度最高
	TouchApp_Write(0X20, &data, 1); 
	
	if(para == NORMAL)
       	{
       		data = 0xE0 ;
		TouchApp_Write(0X23, &data, 1);
		
		for(i=0; i<sizeof(touch_Trigglevel); i++)
			TouchApp_Write(i, &touch_Trigglevel[i], 1);

		data = 0x55;  // 500k 多按键模式
		TouchApp_Write(0X21, &data, 1);
	}
	else
	{
		data = 0XE7; 
		TouchApp_Write(0X23, &data, 1);	
	
		for(i=0; i<sizeof(touch_Standby_Trigglevel); i++)
			TouchApp_Write(i, &touch_Standby_Trigglevel[i], 1);

		data = 0x51;  // 500k 多按键模式
		TouchApp_Write(0X21, &data, 1);
	}

	data = 0x00; // 工作模式
	TouchApp_Write(0X3a, &data, 1); 
#else

	for(i=0; i<27; i++)
		TouchApp_Write(touch_reg[i][0], &touch_reg[i][1], 1);
#endif

	return 0;
}


#else

static BOOL TouchApp_Write(UINT32 bRegAddr, BYTE *bBuf, UINT32 dLen)
{
	BOOL bRet = FALSE;
	int sdRet = -1;
	TOUCH_HANDLE *touch = &touch_handle;

	I2cData_t i2cData;

	i2cData.buf = bBuf;
	i2cData.cmd = bRegAddr;
	i2cData.cmdLen = 1;
	i2cData.dataLen = dLen;
	i2cData.slaveAddr = I2C_ADDR;
	i2cData.showMsg = 0;

	if(I2C_Write(touch->i2c_str, &i2cData) < 0)
	{
		bRet = FALSE;
		LOGD("%s  fd %d,  Reg %x NG\n", __func__, touch->fd, bRegAddr);
	}
	else
	{
		bRet = TRUE;
		sdRet = 0;
		//LED_printf("dev addr %x Reg %x OK\n", bAddr,  bRegAddr);
	}

	return (bRet);
}


static int TouchApp_Read(BYTE bRegAddr, BYTE *bBuf)
{
	TOUCH_HANDLE *touch = &touch_handle;
	BOOL bRet = FALSE;
	int sdRet = -1;
	I2cData_t i2cData;

	i2cData.buf = bBuf;
	i2cData.cmd = bRegAddr;
	i2cData.cmdLen = 1;
	i2cData.dataLen = 1;
	i2cData.slaveAddr = I2C_ADDR;
	i2cData.showMsg = 0;

	if(I2C_Read(touch->i2c_str, &i2cData) < 0)
	{
		bRet = FALSE;
		LOGD("%s  fd %d,  Reg %x NG\n", __func__, touch->fd, bRegAddr);
	}
	else
	{
		bRet = TRUE;
		sdRet = 0;
		LOGD("buf 0x%x, Reg 0x%x OK\n", i2cData.buf[0],  bRegAddr);
	}

	return sdRet;
}

static int TouchApp_init(int para)
{
	TOUCH_HANDLE *touch = &touch_handle;
	//I2C_CFG temp; 
	BYTE i=0;
	BYTE data;
	
	touch->status = para;
	
	//Pinmux_GPIO_Free(TOUCH_ISR);
	//Pinmux_GPIO_Alloc(TOUCH_ISR,IO_GPIO, IO_RISC);
	//GPIO_OE_Set(TOUCH_ISR, IO_INPUT);

	touch->sig_tbl = touch_sig_table;
	touch->key_tbl = touch_key_table;
	touch->key.key_val = IRC_NULL;
	touch->key.key_pre_val = IRC_NULL;
	touch->key.key_type = 0;
	touch->key.key_tick = 0;
	
	delay_1ms(50);

	data = 0x5a;  // 配置模式
	TouchApp_Write(0X3a, &data, 1);

	data = 0x51;  // 500k
	TouchApp_Write(0X21, &data, 1);
	
	if(para == NORMAL)
       	{
       		data = 0xE0 ;
		TouchApp_Write(0X23, &data, 1);
		
		for(i=0; i<sizeof(touch_Trigglevel); i++)
			TouchApp_Write(i, &touch_Trigglevel[i], 1);
	}
	else
	{
		data = 0xE0 ; 
		TouchApp_Write(0X23, &data, 1);	
	
		for(i=0; i<sizeof(touch_Standby_Trigglevel); i++)
			TouchApp_Write(i, &touch_Standby_Trigglevel[i], 1);
	}

	
	data = 0x03; // 全局灵敏度默认02，最大0F, 0F灵敏度最高
	TouchApp_Write(0X20, &data, 1); 

	//data = 0x08; 
	//TouchApp_Write(0X2D, &data, 1); 
	//data = 0xFF; 
	//TouchApp_Write(0X2A, &data, 1); 

	delay_1ms(5);

	data = 0x00; // 工作模式
	TouchApp_Write(0X3a, &data, 1); 
	
	delay_1ms(500);

	return 0;
}

#endif

static UINT16 TouchApp_ValTable_Switch_KeyTable(BYTE val)
{
	TOUCH_HANDLE *touch = &touch_handle;
	BYTE i=0;
	for(i=0; i<TOUCK_KEY_MAX; i++)
	{
		if(val == touch_value_table[i])
		{
			//LOGD(" TouchKey index %d\n ", i);
			return touch->key_tbl[i];
		}
	}

	return IRC_NULL;
}


static UINT32 TouchApp_KeyTable_Switch_SigTable(BYTE val)
{
	TOUCH_HANDLE *touch = &touch_handle;
	BYTE i=0;
	for(i=0; i<TOUCK_KEY_MAX; i++)
	{
		if(val == touch->key_tbl[i])
			return touch->sig_tbl[i];
	}

	return CUSTOMER_PRIVATE_SIG_MAX;
}

static int TouchApp_get_key(void)  // polling 50ms
{
	int ret = 0;
	BYTE send_sig = 0;
	TOUCH_HANDLE *touch = &touch_handle;
	BYTE key_val = 0;
	UINT32 sig = CUSTOMER_PRIVATE_SIG_MAX;
	static BYTE cnt = 0;

	if(cnt++ < 1)
	{
	//	return 0;
	}

	cnt = 0;


	//if(GPIO_Intput_Read(TOUCH_ISR) == 0)
	{
		TouchApp_Read(0x2c, &key_val);
		if(TouchApp_Read(TOUCH_REG_KEY, &key_val) == 0)
		{	
			if(key_val == 0)
			{
				if(touch->key.key_val != IRC_NULL)
				{
					if(touch->key.key_type == TOUCH_KEY_PRESS)
					{
						 touch->key.key_type = TOUCH_KEY_RELEASE;
						LOGD(" %s : key release ISR\n ", __func__);
					}
					else
					{
						touch->key.key_type = TOUCH_KEY_LONG_RELEASE;
						LOGD(" %s : key long release ISR\n ", __func__);
					}

					send_sig = 1;
				}

				touch->key.key_val = IRC_NULL;
				
			}
			else
			{
				LOGD(" %s :key_val %d\n ", __func__, key_val);
			
				touch->key.key_val = TouchApp_ValTable_Switch_KeyTable(key_val);

				if(touch->key.key_pre_val != touch->key.key_val)
				{
					// press
					if(touch->key.key_val == IRC_NULL)
					{
						if(touch->key.key_type == TOUCH_KEY_PRESS)
						{
							 touch->key.key_type = TOUCH_KEY_RELEASE;
							LOGD(" %s : key release \n ", __func__);
						}
						else
						{
							touch->key.key_type = TOUCH_KEY_LONG_RELEASE;
							LOGD(" %s : key long release \n ", __func__);
						}

						send_sig = 1;
					}
					else if(touch->key.key_pre_val == IRC_NULL)
					{
						touch->key.key_tick = 0;
						touch->key.key_type = TOUCH_KEY_PRESS;
						LOGD(" %s : key press \n ", __func__);

						send_sig = 1;
					}
					
					
				}
				else
				{
					touch->key.key_tick++;
				
					if(touch->key.key_type == TOUCH_KEY_PRESS)
					{
						// long press
						if(touch->key.key_tick == 10) // 33  // 1s will set long press signal
						{
							touch->key.key_tick = 0;
							touch->key.key_type = TOUCH_KEY_LONG_PRESS;
							LOGD(" %s : key long press \n ", __func__);
							send_sig = 1;
						}	
					}
					else if(touch->key.key_type == TOUCH_KEY_LONG_PRESS || touch->key.key_type == TOUCH_KEY_TICK)
					{
						// long tick
						if(touch->key.key_tick == 2)  // 7 // every 200ms set tick signal  
						{
							touch->key.key_tick = 0;
							touch->key.key_type = TOUCH_KEY_TICK;
							LOGD(" %s : key tick \n ", __func__);
							send_sig = 1;
						}
					}
				} 
			}
		}
	}
/*
	else
	{
		if(touch->key.key_val != IRC_NULL)
		{
			if(touch->key.key_type == TOUCH_KEY_PRESS)
			{
				 touch->key.key_type = TOUCH_KEY_RELEASE;
				LOGD(" %s : key release ISR\n ", __func__);
			}
			else
			{
				touch->key.key_type = TOUCH_KEY_LONG_RELEASE;
				LOGD(" %s : key long release ISR\n ", __func__);
			}

			send_sig = 1;
		}
		
		touch->key.key_val = IRC_NULL;
	}
*/
	if(send_sig)
	{	
		 
		 if(touch->key.key_val == IRC_NULL)
			 sig = TouchApp_KeyTable_Switch_SigTable( touch->key.key_pre_val );
		 else
		 	 sig = TouchApp_KeyTable_Switch_SigTable( touch->key.key_val );

		 //LOGD(" %s : touch key send sig %d  %d(%d) \n ", __func__, touch->key.key_type, sig, VOL_UP_SIG);

		 if(touch->status == NORMAL)
		 {
		 	TouchCallEvt *e = Q_NEW(TouchCallEvt, TOUCH_CALL_SIG);
			e->sig = sig+touch->key.key_type;
			e->type =  touch->key.key_type;
			if(touch->key.key_val == IRC_NULL)
				e->key =  touch->key.key_pre_val;
			else
				e->key =  touch->key.key_val;
		 	QACTIVE_POST(UserApp_get(), (QEvt *)e, (void*)0);
			
		}
		else
		{
			 if(sig+touch->key.key_type == POWER_RELEASE_SIG || sig+touch->key.key_type == POWER_LONG_PRESS_SIG ||
				sig+touch->key.key_type == SOURCE_SW_RELEASE_SIG || sig+touch->key.key_type == SOURCE_SW_LONG_PRESS_SIG
			 	)
			 {
			 	ret = 1;
		 	 }
		}

	}
	
	touch->key.key_pre_val = touch->key.key_val;

	return ret;
}



static int TouchApp_Polling(void)
{
	return TouchApp_get_key();
}



TOUCH_FUNC ET8L10C_touch = 
{
	TouchApp_init,
	TouchApp_init,
	TouchApp_Polling,
	NULL,
};






