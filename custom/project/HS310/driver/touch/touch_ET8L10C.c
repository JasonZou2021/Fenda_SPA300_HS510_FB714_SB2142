
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

#define I2C_CLK	44
#define I2C_DAT	43
#define TOUCH_ISR	30

#define I2C_ADDR	0xAE

#define I2C_DELAY_NORMAL	1000
#define I2C_DELAY_STANDBY 	3


#define TOUCH_REG_KEY 0x34
#define TOUCH_REG_FW 0x52
/****************************************************

0£ºPress  1£ºRelease  2£ºLong Press  3£ºLong Release  4£ºTick

******************************************************/


#define TOUCH_NEW_5CH	(1<<5)
#define TOUCH_NEW_4CH	(1<<4)
#define TOUCH_NEW_0CH	(1<<0)
#define TOUCH_NEW_13CH	(1<<13)

#define TOUCH_NEW_2CH 	(1<<2)  // vol-
#define TOUCH_NEW_1CH		(1<<1)  // vol+


// 0x23
#define TOUCH_1CH	(1<<1)
#define TOUCH_2CH	(1<<2)
#define TOUCH_5CH	(1<<5)
#define TOUCH_7CH	(1<<7)


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
	TOUCH_KEY key;
	UINT16 *key_tbl;
	UINT32 *sig_tbl;

	BYTE new_flag;

	BYTE version;
	
}TOUCH_HANDLE;

static TOUCH_HANDLE touch_handle ={.fd = -1,};



// ================for APT8L10 & APT8L14 =============================================

#define	TOUCK_NEW_KEY_MAX		14


static UINT16 touch_new_value_table[TOUCK_NEW_KEY_MAX] = 
{
	TOUCH_NEW_4CH, 
	TOUCH_NEW_0CH, 
	TOUCH_NEW_5CH,  
	TOUCH_NEW_13CH,  // vol up

//===== for APT8L14 ====		
	TOUCH_NEW_1CH,   // vol up
	TOUCH_NEW_2CH, 
//=====	
	
};

static BYTE touch_new_Trigglevel[] = 

{
	30, 30, 30, 0xff, 30, 30, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 30, 0xff	
	
};

static BYTE touch_new_Standby_Trigglevel[] = 

{
	0xff, 0xff, 0xff, 0xff, 0xff, 30, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff	
};


static UINT16 touch_new_key_table[TOUCK_NEW_KEY_MAX] = 
{
	IRC_SRC, 
	IRC_VOLUME_DN,  
	IRC_POWER, 
	IRC_VOLUME_UP, 
//===== for APT8L14 ====	
	IRC_VOLUME_UP,  
	IRC_VOLUME_DN, 
//=====		

};

static UINT32 touch_new_sig_table[TOUCK_NEW_KEY_MAX] = 
{
	 SOURCE_SW_SIG, 
	 VOL_DOWN_SIG, 
	 POWER_SIG, 
	 VOL_UP_SIG, 

	  VOL_UP_SIG, 
	 VOL_DOWN_SIG,
	 
		
};

//=======

static BOOL TouchApp_Write(UINT32 bRegAddr, void *bBuf, UINT32 dLen)
{
	BOOL bRet = FALSE;
	int sdRet = -1;
	UINT8 bAddr = I2C_ADDR;
	TOUCH_HANDLE *touch = &touch_handle;

	I2C_CFG *cfg = I2C_GetSrv(touch->fd);
	if(touch->status == NORMAL)
	{
		touch->cfg.delay = 10;
		touch->cfg.ack_delay = 50; 
	}
	else
	{
		touch->cfg.delay = 50;
		touch->cfg.ack_delay = 200; 
	}
	cfg->wr_method = I2C_WR_BYTE_NORMAL;
	
	I2c_IO_ReInit(cfg);

	if(touch->fd >= 0)
	{
		sdRet = AnD_I2CSWWrite( touch->fd, bAddr, bRegAddr, (UINT8 *)bBuf, dLen);	
		if(sdRet > 0)
		{
			bRet = FALSE;
			LOGD("%s  fd %d, dev addr %x Reg %x NG\n", __func__, touch->fd, bAddr, bRegAddr);
		}
		else
		{
			bRet = TRUE;
			//LED_printf("dev addr %x Reg %x OK\n", bAddr,  bRegAddr);
		}
	}

	return (bRet);
}


static int TouchApp_Read(BYTE reg, BYTE *data, BYTE len)
{
	TOUCH_HANDLE *touch = &touch_handle;
	I2C_CFG *cfg = I2C_GetSrv(touch->fd);

	if(cfg == NULL)
		return -1;

	if(touch->status == NORMAL)
	{
		touch->cfg.delay = 10;
		touch->cfg.ack_delay = 50; 
	}
	else
	{
		touch->cfg.delay =50;
		touch->cfg.ack_delay = 200;//20; 
	}
	cfg->wr_method = I2C_WR_BYTE_NORMAL;
	I2c_IO_ReInit(cfg);

	if(AnD_I2CSWRead(touch->fd, I2C_ADDR, reg, data, len) != eI2COK)
	{
		reg = reg;
		data = data;
		LOGD(" TouchApp_Read reg %x  data %x NG   delay %d\n ", reg, *data , touch->cfg.delay);
		return -1;
	}

	return 0;
}

static int TouchApp_init(int para)
{
	TOUCH_HANDLE *touch = &touch_handle;
	I2C_CFG temp; 
	BYTE i=0;
	BYTE data = 0;
	
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
		touch->cfg.delay = 15;
		touch->cfg.ack_delay = 200; 
	}

	Pinmux_GPIO_Free(TOUCH_ISR);
	Pinmux_GPIO_Alloc(TOUCH_ISR,IO_GPIO, IO_RISC);
	GPIO_OE_Set(TOUCH_ISR, IO_INPUT);
	
	LOGD(" >>> TouchApp_init  fd  (%d) , status %d\n ", touch->fd ,touch->status);

	if(touch->fd < 0)
	{
		touch->cfg.byte_type = I2C_BYTE_HIGH_TO_LOW;
		touch->cfg.high_voltage = I2C_HIGH_V_OUTPUT;
		touch->cfg.continue_start = I2C_START_SIG_CONTINUE_SUPPORT;
		touch->cfg.read_method = I2C_READ_HAVE_REG;
		touch->cfg.wr_method = I2C_WR_BYTE_WAIT;

		touch->fd = I2c_IO_Check_Used(&touch->cfg);

		if(touch->fd < 0)
			touch->fd = I2c_IO_Init(&touch->cfg);

	
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

	for(i=0; i<5; i++)
	{
		TouchApp_Read(0X51, &data, 1); // user ID
		if(data == 0x55)
			break;

		delay_1ms(10);
	}

	LOGD("%s : user ID 0x%x, fd %d\n", __func__, data, touch->fd);

	TouchApp_Read(TOUCH_REG_FW, &data, 1);

	touch->version = data;

	UINT16 en = 0;

	touch->new_flag = 1;

	touch->sig_tbl = touch_new_sig_table;
	touch->key_tbl = touch_new_key_table;

	if(para == NORMAL)
	{
	   	en = TOUCH_NEW_0CH|TOUCH_NEW_4CH|TOUCH_NEW_5CH|TOUCH_NEW_13CH|TOUCH_NEW_1CH|TOUCH_NEW_2CH;
		data = en&0xff;
		TouchApp_Write(0X26, &data, 1);
		data = en>>8;
		TouchApp_Write(0X27, &data, 1);
		
		for(i=0; i<sizeof(touch_new_Trigglevel); i++)
			TouchApp_Write(i, &touch_new_Trigglevel[i], 1);
	}
	else
	{
		en = TOUCH_NEW_5CH; 
		data = en&0xff;
		TouchApp_Write(0X26, &data, 1);
		data = en>>8;
		TouchApp_Write(0X27, &data, 1);	
		
		for(i=0; i<sizeof(touch_new_Standby_Trigglevel); i++)
			TouchApp_Write(i, &touch_new_Standby_Trigglevel[i], 1);
	}

	data = 0x14;
	TouchApp_Write(0X25, &data, 1);

	data = 0x3;
	TouchApp_Write(0X13, &data, 1);	

	data = 0;
	TouchApp_Write(0X28, &data, 1);

	data = 0x01;
	TouchApp_Write(0X11, &data, 1); 

	return 0;
}

static UINT16 TouchApp_ValTable_Switch_KeyTable(UINT16 val)
{
	TOUCH_HANDLE *touch = &touch_handle;
	BYTE i=0;
	
	for(i=0; i<TOUCK_NEW_KEY_MAX; i++)
	{
		if(val == touch_new_value_table[i])
		{
			LOGD(" TouchKey index %d\n ", i);
			return touch->key_tbl[i];
		}
	}

	return IRC_NULL;
}


static UINT32 TouchApp_KeyTable_Switch_SigTable(BYTE val)
{
	TOUCH_HANDLE *touch = &touch_handle;
	BYTE i=0;

	for(i=0; i<TOUCK_NEW_KEY_MAX; i++)
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
	UINT16 key_val = 0;
	BYTE key_val1 = 0;
	BYTE key_val2 = 0;
	UINT32 sig = CUSTOMER_PRIVATE_SIG_MAX;
	//volatile int *pRsm_Magic = (int * )SRAM_ADDR_ISP_UPGRADE;
	static BYTE cnt = 0;

	if(cnt++ < 3)
	{
		return 0;
	}

	cnt = 0;


	if(GPIO_Intput_Read(TOUCH_ISR) == 0)
	{

		if(TouchApp_Read(TOUCH_REG_KEY, &key_val1, 1) == 0)
		{

			TouchApp_Read(TOUCH_REG_KEY+1, &key_val2, 1);
			key_val = key_val2;
			key_val = (key_val<<8)|key_val1;

			LOGD(" %s :key_val 0x%x\n ", __func__, key_val);
		
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
					if(touch->key.key_tick == 25)//33)  // 1s will set long press signal
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
					if(touch->key.key_tick == 4)//7)  // every 200ms set tick signal  
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

			LOGD(" %s : %d\n  ",__func__, e->key);
			
		}
		else
		{
			 if(sig+touch->key.key_type == POWER_RELEASE_SIG || sig+touch->key.key_type == POWER_LONG_PRESS_SIG)
			 {
			 	//*pRsm_Magic = 0;
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


static int TouchApp_Verion(int para)
{
	para = para;

	return touch_handle.version;
}


TOUCH_FUNC ET8L10C_touch = 
{
	TouchApp_init,
	TouchApp_init,
	TouchApp_Polling,
	TouchApp_Verion,
};





