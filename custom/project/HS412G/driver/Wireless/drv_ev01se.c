#include <string.h>
#include "qpc.h"
#define LOG_MODULE_ID  ID_APUSR
#define LOG_EXTRA_STR  "[EV20]"
#include "log_utils.h"
#include "custom_sig.h"
#include "types.h"
#include "pinmux.h"
#include "busy_delay.h"
#include "AnD_I2C_Srv.h"
#include "wireless.h"
#include "exstd_interfaces.h"
#include "fsi_api.h"
#include "qp_pub.h"


/*
 * copy from EV20S and modify something
*/

#define NO_LINK_TO_SLEEP_TIME		(15*6)  // 15min
#define SLEEP_TIME_MS				(50) //5s   	// 2 - 2.2 -2.4 ... 5 
#define WAKEUP_TIME_MS 			(10) // 1s         // 1 - 1.2 -1.4 ... 4 
#define SLEEP_TIME_VALUE			((SLEEP_TIME_MS-20)/2)
#define WAKEUP_TIME_VALUE			((WAKEUP_TIME_MS-10)/2)


#define I2C_CLK	6
#define I2C_DAT	5

/*
enum
{
	TX = 1,
	RX_STEREO,
	RX_LEFT,
	RX_RIGHT,
	RX_SUBWOOFER,
};
*/

enum
{
	TX = 1,
	RX,
};
enum
{
	WIRELESS_CONNECTED,
	WIRELESS_DISCONNECTED,	
	WIRELESS_PAIRING,
	WIRELESS_SLEEP,
	WIRELESS_LED_MAX,
};

BYTE led_control[WIRELESS_LED_MAX][2] = 
{	
	// on, off
	{1,	0},
	{3,	7},
	{5,	5},
	{0,	1},
};


#define PACKET_HAND 0x77
#define EV01SE_I2C_ADDR		0X68

enum EV01SE_REG
{
	EV01SE_REG_TXVERSION 			= 0X00,  	// 0X00 ~ 0X01  R
	EV01SE_REG_SYS_CTL 			= 0X02,   // W/R
	EV01SE_REG_VOLUME 				= 0X03,   // W/R
	EV01SE_REG_PAIR_TONE 			= 0X04,   // W/R
	EV01SE_REG_SYS_STATUS 			= 0X06,   // R
	EV01SE_REG_I2S_SLAVE 			= 0X07,   // R
	EV01SE_REG_I2S_SAMPLERATE 		= 0X08,   // R
	EV01SE_REG_RF_POWER			= 0X09,   // W/R
	EV01SE_REG_PAIR_TIME			= 0X0A,   // W/R
	EV01SE_REG_CCH_DATA       	      = 0X10,     // 0X10 ~ 0X15   W

	EV01SE_REG_FW_CHECK			= 0X20,   // W/R

		EV01SE_REG_NEW_PAIR			= 0X26,

	EV01SE_REG_LOST_SLEEP			= 0X2E,   // W/R
	EV01SE_REG_WAKEUP_TIME		= 0X2F,   // W/R
	
	EV01SE_REG_RXTX_FLAG			= 0X40,   // R
	EV01SE_REG_SEAT_NUM			= 0X41,   // R
	
	EV01SE_REG_PAIR_MODE			= 0X43,   // W/R
	EV01SE_REG_BRAND_FLAG			= 0X44,   // W/R
	EV01SE_REG_ID_STORAGE			= 0X45,   // W/R
	EV01SE_REG_1_TO_N				= 0X46,   // W/R
	EV01SE_REG_SET_ID_NUM			= 0X47,   // W/R

	EV01SE_REG_LED_SYNC			= 0X4B,   // 0X4B ~ 0X4C   W/R
	EV01SE_REG_LED_LOST			= 0X4D,   // 0X4D ~ 0X4E   W/R
	EV01SE_REG_LED_PAIR			= 0X4F,   // 0X4F ~ 0X52   W/R
	EV01SE_REG_LED_SLEEP			= 0X54,   // =0->on !=0->off
//	EV01SE_REG_LED_SLEEP			= 0X53,   // 0X53 ~ 0X54   W/R

	EV01SE_REG_FW_CHKSUM			= 0X60,   // 0X60 ~ 0X63   R

	EV01SE_REG_SET_RF_CHANNEL       = 0X64,   //0X64 ~ 0X65   R

	EV01SE_REG_RXVERSION 			= 0X67,

	EV01SE_REG_UPG_DATA			= 0X6A,   // W   0x6a/0x6b is packet number

	EV01SE_REG_UPG_MASK			= 0X6C,   // 0X6C ~ 0X73   W

	EV01SE_REG_UPG_DATA1			= 0X74,   // W  

	EV01SE_REG_UPG_LAST			= 0X7D,   // W/R
	EV01SE_REG_UPG_ENANLE			= 0X7E,   // W  wirte 0xA5 enable upgrade
	EV01SE_REG_UPG_STATUS			= 0X7F,   // R

	EV01SE_REG_CCH_RECV			= 0X66,   // R

	EV01SE_REG_EMI_MODE			= 0X80,   // W/R
	EV01SE_REG_EMI_RF_CHN			= 0X81,   // W/R
	EV01SE_REG_EMI_5_2G			= 0X83,   // W/R

	
	EV01SE_REG_SUB_CCH_RECV			= 0XB0,   //sub R
	EV01SE_REG_STEREO_CCH_RECV			= 0XB6,   //sub R
};


enum
{
	EV01SE_POLL_CHECK_PAIR,
	EV01SE_POLL_CHECK_CCH,	
	EV01SE_POLL_CHECK_STATUS,
};

#define EV01SE_HANLE_DEMUTE	(0<<0) 
#define EV01SE_HANLE_MUTE	(1<<0)   //
#define EV01SE_HANLE_PAIR	(1<<1)   
#define EV01SE_HANLE_SLEEP	(1<<2)   //

#define EV01SE_SYN_STATUS	(1<<0)  
#define EV01SE_MUTE_STATUS	(1<<1)  
#define EV01SE_RUN_STATUS	(1<<2)   
#define EV01SE_PAIR_STATUS	(1<<4)

enum
{
	EV01SE_EXIT_PAIR,
	EV01SE_PAIR_OK,
	EV01SE_PAIR_FAIL,
	EV01SE_PAIRING,
};




#define CCH_10_VALID 				(1<<7)
#define CCH_10_EQ_EN 				(1<<6)
#define CCH_10_SURR_MUTE_EN		(1<<5)
#define CCH_10_SURR_SET_MUTE		(1<<4)
#define CCH_10_SUB_MUTE_EN		(1<<3)
#define CCH_10_SUB_SET_MUTE		(1<<2)
#define CCH_10_SURR_MUTE_REG_EN	(1<<1)
#define CCH_10_SURR_SET_REG_MUTE	(1<<0)

#define CCH_11_SUB_MUTE_REG_EN	(1<<7)
#define CCH_11_SUB_SET_REG_MUTE	(1<<6)
#define CCH_11_SURR_VOL_AMP		(1<<5)
#define CCH_11_SURR_VOL_REG		(1<<4)
#define CCH_11_SUB_VOL_AMP		(1<<3)
#define CCH_11_SUB_VOL_REG		(1<<2)



#define CCH_10_SURR_MUTE		(CCH_10_VALID|CCH_10_SURR_MUTE_EN|CCH_10_SURR_SET_MUTE)
#define CCH_10_SUB_MUTE		(CCH_10_VALID|CCH_10_SUB_MUTE_EN|CCH_10_SUB_SET_MUTE)
#define CCH_10_DEMUTE			(CCH_10_VALID|CCH_10_SURR_MUTE_EN|CCH_10_SUB_MUTE_EN)
#define CCH_10_SURR_EQ			(CCH_10_VALID|CCH_10_EQ_EN)
#define CCH_10_SUB_EQ			(CCH_10_VALID|CCH_10_EQ_EN)
#define CCH_10_VOL				(CCH_10_VALID)
#define CCH_11_SURR_VOL		(CCH_11_SURR_VOL_AMP)
#define CCH_11_SUB_VOL			(CCH_11_SUB_VOL_AMP)

#define CCH_11_SUB_REG_MUTE		(CCH_11_SUB_MUTE_REG_EN|CCH_11_SUB_SET_REG_MUTE)
#define CCH_11_SUB_REG_DEMUTE		(CCH_11_SUB_MUTE_REG_EN)

typedef struct _ev01se
{
	BYTE init;
	BYTE fd;
	I2C_CFG cfg;
	BYTE syn;
	BYTE mute;
	BYTE sleep;
	BYTE vol;
	BYTE eq;
	BYTE status;
	BYTE sub_mode_ver;
	BYTE sub_eeprom_ver;
	BYTE stereo_mode_ver;
	UINT16 stereo_eeprom_ver;
	BYTE sub_syn;
	BYTE stereo_syn;

	BYTE sub_pair_status;
	BYTE stereo_pair_status;	

	BYTE upg_flag;
}EV01SE;

EV01SE ev01se;


int EV01SE_Write(int iAddr, UINT8 *bData, int len)
{
	EV01SE *pEV01SE = &ev01se;

	BYTE ret;

	ret = AnD_I2CSWWrite(pEV01SE->fd, EV01SE_I2C_ADDR, iAddr, bData, len);

	if(ret != eI2COK)
	{
		LOGE(" EV01SE_Write reg 0x%x,  data 0x%x NG,  ret = %d\n ", iAddr, *bData, ret);
		return -1;
	}
	else
	{
		//LOGD(" EV01SE_Write reg 0x%x  data 0x%x OK\n ", iAddr, *bData);
	}

	return 0;
}


int EV01SE_Read(int iAddr, UINT8 *bData, int len)
{
	EV01SE *pEV01SE = &ev01se;

	if(AnD_I2CSWRead(pEV01SE->fd, EV01SE_I2C_ADDR, iAddr, bData, len) != eI2COK)
	{
		//LOGE(" EV01SE_Read reg %x  data %x NG\n ", iAddr, *bData);
		return -1;
	}
	else
	{
		//LOGD(" EV01SE_Read reg %x  data %x OK\n ", iAddr, *bData);
	}

	return 0;
}


void EV01SE_Check_Type(void)
{
	BYTE para = 0;
	if(EV01SE_Read(EV01SE_REG_RXTX_FLAG, &para, sizeof(para)) == 0)
	{
	/*
		if(para == TX)
		{
			EV20S_printf1(" [ 5.8G  TX ]\r\n"); 
		}
		else if(para == RX_STEREO)
		{
			EV20S_printf1(" [ 5.8G  RX  STEREO ]\r\n"); 
		}
		else if(para == RX_LEFT)
		{
			EV20S_printf1(" [ 5.8G  RX  LEFT ]\r\n"); 
		}
		else if(para == RX_RIGHT)
		{
			EV20S_printf1(" [ 5.8G  RX  RIGHT ]\r\n"); 
		}
		else if(para == RX_SUBWOOFER)
		{
			EV20S_printf1(" [ 5.8G  RX  SUBWOOFER ]\r\n"); 
		}
	*/
		if(para == TX)    //01->tx
		{
			LOGD(" [ 5.8G  TX ]\r\n"); 
		}
		else if(para == RX)   //02->rx
		{
			LOGD(" [ 5.8G  RX ]\r\n"); 
		}
		
	
	}
}


int EV01SE_Init(void)
{
	EV01SE *pEV01SE = &ev01se;
	BYTE para = 0;
	BYTE i=0;

	BYTE ev01se_pin[] = {I2C_CLK, I2C_DAT};

	for(i=0; i<sizeof(ev01se_pin); i++)
	{
		if((Pinmux_GPIO_CheckStatus(ev01se_pin[i]) == 0))
		{
			if(Pinmux_GPIO_Alloc(ev01se_pin[i],IO_GPIO,IO_RISC)==0)
			{
				GPIO_OE_Set(ev01se_pin[i], IO_OUTPUT);
			}	
			else
			{
				LOGE(" EV01SE i2c pin [%d] has use error 1111 \n ", i);
				//return -1;
			}
		}
		else
		{
			LOGE(" EV01SE i2c pin [%d] has use error 2222 \n ", i);
			//return -1;
		}
	}

	delay_1ms(50);

	pEV01SE->cfg.clk = I2C_CLK;
	pEV01SE->cfg.dat = I2C_DAT;
	pEV01SE->cfg.delay = 50;
	pEV01SE->cfg.ack_delay = 1000;  // > 300us
	pEV01SE->cfg.byte_type = I2C_BYTE_HIGH_TO_LOW;
	pEV01SE->cfg.high_voltage = I2C_HIGH_V_OUTPUT;
	pEV01SE->cfg.continue_start = I2C_START_SIG_CONTINUE_UNSUPPORT;
	pEV01SE->cfg.read_method = I2C_READ_HAVE_REG;
	pEV01SE->cfg.wr_method = I2C_WR_BYTE_NORMAL;
	pEV01SE->fd = I2c_IO_Init(&pEV01SE->cfg);

	LOGD("%s   fd : %d\n", __func__, pEV01SE->fd);


	pEV01SE->init = 1;

	para = 32; //  30s 
	EV01SE_Write(EV01SE_REG_PAIR_TIME, &para, 1);	

	para = 0X02;	
	EV01SE_Write(EV01SE_REG_NEW_PAIR, &para, 1);	

	pEV01SE->status = EV01SE_POLL_CHECK_PAIR;

	UINT16 version = 0;
	EV01SE_Read(EV01SE_REG_TXVERSION, (UINT8 *)&version, 2);
	LOGD("TX version : %d\n", version);

	return 0;
}


int EV01SE_Pair(BYTE status)
{
	EV01SE *pEV01SE = &ev01se;
	pEV01SE->status = EV01SE_POLL_CHECK_PAIR;
	BYTE para; 
	
	EV01SE_Read(EV01SE_REG_SYS_CTL, &para, sizeof(para));
	if(status)
		para |= EV01SE_HANLE_PAIR;  
	else
		para &= ~EV01SE_HANLE_PAIR;  
	EV01SE_Write(EV01SE_REG_SYS_CTL, &para, sizeof(para));

	LOGD(" %s %x\n ", __func__, para);
	
	return 0;
}

int EV01SE_Volume(BYTE vol)
{
	vol = vol;

	return 0;
}

int EV01SE_WooferVol(BYTE vol)
{
	EV01SE *pEV01SE = &ev01se;
	BYTE para[3] = {0};
	
	if(pEV01SE->sub_syn == 0)
		return -1;

	para[0] = CCH_10_VOL;
	para[1] = CCH_11_SUB_VOL;
	para[2] = vol;  // vol 

	EV01SE_Write(EV01SE_REG_CCH_DATA, para, sizeof(para));

	return 0;
}

int EV01SE_SurroundVol(BYTE vol)
{
	BYTE src_gain[] = {0x11,0x0e,0x11,0x11,0x11,0x11,0x11};

	EV01SE *pEV01SE = &ev01se;
	BYTE para[3] = {0};
	
	if(pEV01SE->sub_syn == 0)
		return -1;

	para[0] = CCH_10_VOL;
	para[1] = CCH_11_SURR_VOL;
	para[2] = src_gain[vol]; // vol

	EV01SE_Write(EV01SE_REG_CCH_DATA, para, sizeof(para));

	return 0;

}

int EV01SE_BassVol(BYTE vol)
{
	vol = vol;

	return 0;
}

int EV01SE_EQ(BYTE eq)
{
	EV01SE *pEV01SE = &ev01se;
	BYTE para[3] = {0};
	
	if(pEV01SE->sub_syn == 0)
		return -1;

	para[0] = CCH_10_SURR_EQ|CCH_10_SUB_EQ;
	para[1] = 0;
	para[2] = eq;

	EV01SE_Write(EV01SE_REG_CCH_DATA, para, sizeof(para));

	return 0;

}



int EV01SE_Mute(BYTE mute)
{
	//EV01SE *pEV01SE = &ev01se;
	
	//if(pEV01SE->sub_syn == 0)
	//	return -1;

	LOGD("%s : %d\n ", __func__, mute);

	BYTE para[3] = {0};
	if(mute == 1)     //mute
	{
		para[0] = CCH_10_SURR_MUTE|CCH_10_SUB_MUTE;
		para[1] = 0;
		para[2] = 0;
	}
	else
	{
		para[0] = CCH_10_DEMUTE;
		para[1] = 0;
		para[2] = 0;
	}

	EV01SE_Write(EV01SE_REG_CCH_DATA, para, sizeof(para));

/*	
	BYTE para;

	if(mute == 1)
	{
		LOGD(">>>>EV01SE_Mute_Set\n");
		para = 0x01;
	}
		
	else
	{
		LOGD(">>>>EV01SE_DeMute_Set\n");
		para = 0x00;
	}
		
	
	EV01SE_Write(EV01SE_REG_SYS_CTL, &para, sizeof(para));
*/
	return 0;
}



enum
{
	RX_VERSION,
	TX_VERSION,
};

int EV01SE_Get_Version(UINT8 *version)
{
	UINT8 param[2];
	EV01SE *pEV01SE = &ev01se;
	
	EV01SE_Read(EV01SE_REG_TXVERSION, param, 2);
	
	version[0] = param[1]+ param[0];  // TX
	version[1] = pEV01SE->stereo_mode_ver; // RX Stereo modul 
	version[2] = pEV01SE->stereo_eeprom_ver; // RX Stereo eeprom
	version[3] = pEV01SE->sub_mode_ver;  // RX Sub modul
	version[4] = pEV01SE->sub_eeprom_ver;  // RX Sub eeprom  

	//LOGD(">>>>param %x-%x-%x-%x-%x\n", version[0], version[1], version[2], version[3], version[4]);

	return 0;
}


int EV01SE_Polling(void)
{
	//static BYTE last_syn;
	static BYTE sub_syn, stereo_syn;
	//BYTE para = 0;
	EV01SE *pEV01SE = &ev01se;
	BYTE ret = 0;
	BYTE data[6];
	BYTE eq;
	static BYTE get_sub_ver = 0;
	static BYTE get_surr_ver = 0;
	static BYTE read_cch;
	
	switch(pEV01SE->status)
	{

		case EV01SE_POLL_CHECK_PAIR:
			
			//pEV01SE->status = EV01SE_POLL_CHECK_PAIR;
			// EV01SE_REG_SUB_CCH_RECV

			if(pEV01SE->init == 1)
			{
				read_cch ^= 1;

				if(read_cch == 0)
				{
					if(EV01SE_Read(EV01SE_REG_SUB_CCH_RECV, data, sizeof(data)) == 0)
					{
						if(data[0] & 0x01)
						{
							pEV01SE->sub_syn =1 ;
						}
						else
						{
							pEV01SE->sub_syn =0 ;
						}

						if(sub_syn != pEV01SE->sub_syn)
						{

							if(pEV01SE->sub_syn)
							{
								get_sub_ver = 1;
								pEV01SE->sub_mode_ver = data[1]+data[2];
								pEV01SE->sub_eeprom_ver = data[4];
								LOGD(" %s :  5.8G sub connected %d\n ", __func__, pEV01SE->sub_mode_ver);
								ret = 1;
							}
							else
							{
								get_sub_ver = 0;
								pEV01SE->sub_mode_ver = 0;
								pEV01SE->sub_eeprom_ver = 0;
		
								LOGD(" %s :  5.8G sub disconnected %d\n ", __func__, pEV01SE->sub_mode_ver);
							}
							sub_syn = pEV01SE->sub_syn;
						}

						if(pEV01SE->sub_syn)
						{
							if(get_sub_ver > 0)
							{
								if(pEV01SE->sub_mode_ver == (data[1]+data[2]) && pEV01SE->sub_eeprom_ver == data[4])
								{
									get_sub_ver = 0;
								//	LOGD(" sub version %s : mode %d, eeprom %d\n ", __func__, pEV01SE->sub_mode_ver, pEV01SE->sub_eeprom_ver);
								}
								else
								{
									pEV01SE->sub_mode_ver = data[1]+data[2];
									pEV01SE->sub_eeprom_ver = data[4];
								}
							}
						
							eq = data[3] & 0x0f;
							if(data[3] != 0xff && eq == 0xf)
							{
								//LOGD(" %s :  5.8G Sub re-connected 0x%x\n ", __func__, pEV01SE->sub_mode_ver);
							}
						}	

						//LOGD(" sub %x-%x-%x-%x-%x ", data[0], data[1], data[2], data[3], data[4]);
					}
				}
				else 
				{	
					if(EV01SE_Read(EV01SE_REG_STEREO_CCH_RECV, data, sizeof(data)) == 0)
					{
					
						if(data[0] & 0x01)
						{
							pEV01SE->stereo_syn =1 ;
						}
						else
						{
							pEV01SE->stereo_syn =0 ;
						}
						
						if(stereo_syn != pEV01SE->stereo_syn)
						{

							if(pEV01SE->stereo_syn)
							{
								get_surr_ver = 1;
								pEV01SE->stereo_mode_ver = data[1]+data[2];
								pEV01SE->stereo_eeprom_ver = data[4];
								LOGD(" %s :  5.8G Surround connected %d\n ", __func__, pEV01SE->stereo_mode_ver);
								ret = 1;
							}
							else
							{
								get_surr_ver = 0;
								pEV01SE->stereo_mode_ver = 0;
								pEV01SE->stereo_eeprom_ver = 0;
		
								LOGD(" %s :  5.8G Surround disconnected %d\n ", __func__, pEV01SE->stereo_mode_ver);
							}
							stereo_syn = pEV01SE->stereo_syn;
						}

						if(pEV01SE->stereo_syn)
						{
							if(get_surr_ver > 0)
							{
								if(pEV01SE->stereo_mode_ver == (data[1]+data[2]) && pEV01SE->stereo_eeprom_ver == data[4])
								{
									get_sub_ver = 0;
								//	LOGD(" Surround version %s : mode %d, eeprom %d\n ", __func__, pEV01SE->stereo_mode_ver, pEV01SE->stereo_eeprom_ver);
								}
								else
								{
									pEV01SE->stereo_mode_ver = data[1]+data[2];
									pEV01SE->stereo_eeprom_ver = data[4];
								}
							}
						
							eq = data[3] & 0x0f;
							if(data[3] != 0xff && eq == 0xf)
							{
								//LOGD(" %s :  5.8G Sub re-connected 0x%x\n ", __func__, pEV01SE->sub_mode_ver);
							}
						}			


						//LOGD(" Surround %x-%x-%x-%x-%x ", data[0], data[1], data[2], data[3], data[4]);
					}
					
					
				}
			}
			
			break;
	
		default:
			break;
	}

	return ret;
}


int EV01SE_GetSyn(void)
{

	//if(ev01se.sub_syn && ev01se.upg_flag == 0)
	if((ev01se.sub_syn || ev01se.stereo_syn) && ev01se.upg_flag == 0)
	 	return 1;

	 return 0;
}


static int EV01SE_Upgrade(QActive *me, ARFunc_t percent_func, eWirelessUpg type)
{

	#define EV21SC_END_ADDR 0Xdff
	#define EV21SC_UPG_PERCENT_PER	(EV21SC_END_ADDR/100)

	#define EV21SC_EEPROM_END_ADDR 0X1FF
	#define EV21SC_EEPROM_UPG_PERCENT_PER	( EV21SC_EEPROM_END_ADDR/100)

	enum
	{
		UPG_STATUS_SEQ_ERROR_00 = 0X00,
		UPG_STATUS_WRITE_OK = 0X01,
		UPG_STATUS_WAIT = 0X02,
		UPG_STATUS_CHECKSUM_FAIL= 0X04,
		UPG_STATUS_SEQ_ERROR_08= 0X08,
		
	};

	ev01se.upg_flag = 1;

	BYTE percent = 0;
	UINT16 upgrage_status = 0;
	BYTE error_cnt = 0;
	BYTE i=0;
	BYTE upg_status;
	UINT32 offset = 0;
	UINT16 checksum = 0;
	UINT16 seq_num = 0;             //packct num
	BYTE upg_data[10] = {0};
	BYTE enable = 0xA5;
	BYTE eeprom_size = 0x08;    //8k byte     eeprom size 0x01-0x08
	BYTE i2c_error_cnt = 0;
	int ret = 0;

	int fd = -1;
	int read_len;
	BYTE data[16];
	
	BYTE upg_mask[WIRELESS_UPG_MAX][8] = 
	{
		{0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88},  // TX
		{0x20,0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x00},  // RX surround
		{0x2F,0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x00},  // RX EEPROM surround
		{0x50,0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x00},  // RX subwoofer
		{0x5F,0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x00},  // RX EEPROM subwoofer
	};

	BYTE upg_path[WIRELESS_UPG_MAX][20] = 
	{
		{"USB:SQ20_TX.BIN"},
		{"USB:SQ20_RX.BIN"},
		{"USB:SQ20_EE.BIN"},
		{"USB:SQ20_RX_SUB.BIN"},
		{"USB:SQ20_EE_SUB.BIN"},
	};
	
	BYTE upg_head[16] = "everestek co ltd";

	LOGD(" wireless upg mode %d\n ", type);

	if(type >= WIRELESS_UPG_MAX)
	{
		LOGD(" >>>>>> EV01SE upg type error \n ");
		ret = -1;
		goto WL_UPG_EXIT;
	}

	fd = open(upg_path[type], O_RDONLY, 0);
	if(fd < 0)
	{
		LOGD(" >>>>>> EV01SE upg file can't open \n ");
		ret = -1;
		goto WL_UPG_EXIT;
	}

	read_len = read(fd, data, sizeof(data));    //read top 16 byte data
	if(read_len < 0)
	{
		close(fd);
		LOGD(" >>>>>> EV01SE upg file read error \n ");
		ret = -1;
		goto WL_UPG_EXIT;
	}
	
	if(strncmp(data, upg_head, 16))    //compare hand core
	{
		close(fd);
		LOGD(" >>>>>> EV01SE upg head error ,is not a ev20 upg file... \n ");
		ret = -1;
		goto WL_UPG_EXIT;
	}

	//data += 16;  // skip the head

	/* 
		Step 1: Enable flash update:
	 	I2C continue write registers 0x6C~0x73,
		If update TX FW:
			0x6C~0x73 as “0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88”
		If update Rx FW using OTA:
			0x6C~0x73 as “0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00”
		If update Rx EEPROM using OTA
			a. write register 0x6A with EEPROM size
			b. write registers 0x6C~0x73 as “0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0”, 
			c. write register 0x7E as 0xA5
	*/


		if(type == WIRELESS_UPG_EEPROM_SURROUND || type == WIRELESS_UPG_EEPROM_SUBWOOFER)      //upgrade EEPROM need write eeprom size to 0x6a  
		{     
			EV01SE_Write(EV01SE_REG_UPG_DATA, &eeprom_size, 1);
		}

	
		if(EV01SE_Write(EV01SE_REG_UPG_MASK, upg_mask[type], 8) < 0)
		{
			LOGD(" >>>>>> EV01SE upg I2C error ,No ACK \n ");
		}
		
		msleep(50);

	/*
		Step 2: 
		write 0x7E as 0xA5, ETK51 MCU will check 0x7E content, if 0x7E content is 0xA5, 
		then check registers0x6C~0x73 is “0x11, 0x22……0x88”, 
		if the content is also match, then enable flash update sequence.
	*/
	// External MCU must write this register twice to enable flash update sequence
	EV01SE_Write(EV01SE_REG_UPG_ENANLE, &enable, 1);
	msleep(50);
	//EV01SE_Write(EV01SE_REG_UPG_ENANLE, &enable, 1);
	//msleep(20);
	
	/*
		Step 3: 
		External MCU send two I2C write command, each I2C command will write 8 bytes into ETK51. So total 16
		bytes will write into ETK51. Each I2C command need extra 3ms delay for internal MCU to process data.
	*/

	while(1)
	{

RE_SEND_DATA:

		memset(data, 0, sizeof(data));
		read_len = read(fd, data, sizeof(data));          //每次读取16BYTE
/*
		EV20S_printf1(" EV20S data (seq_num 0x%x, read_len %d): \n", seq_num, read_len);

		if(seq_num == 0xa6b || seq_num == 0xa6a)
		{
			for(i=0; i<read_len; i++)
			{
				EV20S_printf1(" 0x%02x ", data[i]);
			}
			EV20S_printf1("\n");
		}
*/
		 /* step2 */
		// 0x6A write 8byte file data
		memset(upg_data, 0, sizeof(upg_data));

		upg_data[0] = (seq_num >> 8) & 0XFF;    //High byte -> 0x6a
		upg_data[1] = (seq_num & 0xFF);             //low byte -> 0x6b
			
		memcpy(&upg_data[2], data, 8);           //8 byte data -> 0x6c-0x73
		for(i=0; i<10; i++)
		{
			checksum += upg_data[i]; 
			
		//	if(i >= 2)
		//		LOGD(">>>start 0x6c end 0x73  data is  0x%x\n", upg_data[i]);
		}
		EV01SE_Write(EV01SE_REG_UPG_DATA, upg_data, 10);    
		msleep(3);

		memset(upg_data, 0, sizeof(upg_data));

		// 0x74 write 8byte file data
		memcpy(upg_data, data+8, 8);             //8 byte data -> 0x74-0x7b
		for(i=0; i<8; i++)
		{
			checksum += upg_data[i]; 

		//	LOGD(">>>start 0x74 end 0x7b  data is  0x%x\n", upg_data[i]);
		}
		upg_data[8] = checksum % 256;           //0x7c   //也就是取低8bit
		upg_data[9] = 0x01;                       //0x7d
		EV01SE_Write(EV01SE_REG_UPG_DATA1, upg_data, 10);    
		msleep(3);

		
RE_READ_STATUS:

		if(error_cnt > 50)    //连续出现错误则为升级失败
		{
			LOGD(" >>>>>> EV01SE upg fail %d\n ", upg_status);
			ret = -1;
			goto WL_UPG_EXIT;
		}

		// wirte 16bytes data, check upgrade status
		i2c_error_cnt = 10;
		while(i2c_error_cnt)
		{
			if(EV01SE_Read(EV01SE_REG_UPG_STATUS, &upg_status, 1)<0)
			{
				LOGD(" >>>>>> EV01SE read upg status reg error %d\n ", i2c_error_cnt);
				i2c_error_cnt--;
			}
			else
				i2c_error_cnt = 0;
			
			msleep(20);
		}

		//	LOGD("EV01SE_REG_UPG_STATUS  0x7E = 0x%x \n", upg_status);
		
		switch(upg_status & 0xf)
		{
			//case UPG_STATUS_SEQ_ERROR_00:
			//case UPG_STATUS_WAIT:   //busy
			default:
				msleep(50);
				error_cnt++;
				LOGD(" >>>>>> EV01SEupg status need  wait .... (%d)\n ", upg_status);
				goto RE_READ_STATUS;
				break;

			case UPG_STATUS_CHECKSUM_FAIL:
				error_cnt++;
				LOGD(" >>>>>> EV01SEupg status checksum error.... \n ");
				checksum = 0;       //need clear checksum as 0
				seq_num--;
				lseek(fd, -16, SEEK_CUR);    //if fail need set fd  offset forward 16 byte, write again.
				msleep(50);
				goto RE_SEND_DATA;
				break;

			
			case UPG_STATUS_SEQ_ERROR_08:
				error_cnt++;
				LOGD(" >>>>>> EV01SE upg status seq number error.... \n ");
				checksum = 0;
				seq_num--;
				lseek(fd, -16, SEEK_CUR);    
				msleep(50);
				goto RE_SEND_DATA;
				break;

			case UPG_STATUS_WRITE_OK:
				break;
				
			//default:
			//	LOGD(" >>>>>> EV01SE upg fail (maybe I2C read error ) %d .... \n ", upg_status);
			//	return -1;
			//	break;
		}

		error_cnt = 0;
		seq_num++;
		//data+=16;
		offset+=16;
		checksum = 0;

		/* step3 */
		if((type == WIRELESS_UPG_TX) || (type == WIRELESS_UPG_RX_SURROUND) || (type == WIRELESS_UPG_RX_SUBWOOFER))    //upgrade tx flash and rx flash
		{
			if(seq_num == EV21SC_END_ADDR+1)    //the  final packet is sent
			{
				/* MCU write register 0x7D to 0x81. When ETK51 get 0x7D==0x81, then start update main flash area */
				enable = 0x81;      
				EV01SE_Write(EV01SE_REG_UPG_LAST, &enable, 1);

				msleep(5000);
				
				do{
					msleep(1000);  
					EV01SE_Read(EV01SE_REG_UPG_LAST, &enable, 1);        //if 0x7D is 0x00 then update finished

					if(error_cnt++ > 10)
					{	
						LOGD(" >>>>>> EV01SE upg check upg last error %x .... \n ", enable);
						ret = -1;
						goto WL_UPG_EXIT;
					}
				}while(enable != 0);   //if 0x7D is 0x00 then update finished

				
				LOGD(" >>>>>> EV01SE upg finish .... \n ");
				break;
			}


			// When first five FW packets is sent, need extra 200 msec delay for client erase Flash.update eeprom no extra delay
			if(seq_num == 5)    //update flash 时的第一次发五个包之后需要delay 200ms
				msleep(200);  


			if(seq_num % EV21SC_UPG_PERCENT_PER == 0)    // 0x0Eff /100     //显示升级百分百
			{
				LOGD(" >>>>>> EV01SE upg  percent %d%% .... \n ", ((seq_num / EV21SC_UPG_PERCENT_PER) > 100)?100:(seq_num / EV21SC_UPG_PERCENT_PER));
				percent =  ((seq_num / EV21SC_UPG_PERCENT_PER) > 100)?  100 : (seq_num / EV21SC_UPG_PERCENT_PER);
				upgrage_status = percent;            //低位存升级百分百
				upgrage_status |= (type<<8);     //高位存升级类型
				percent_func(me, &upgrage_status);
				msleep(3);
			}
			
		}
		
		else if(type == WIRELESS_UPG_EEPROM_SUBWOOFER ||type == WIRELESS_UPG_EEPROM_SURROUND)     //upgrade rx eeprom
		{
			if(seq_num == EV21SC_EEPROM_END_ADDR+1)    //8k eeprom    0x1FF = 8*1024/16-1
			{
				/* MCU write register 0x7D to 0x81. When ETK51 get 0x7D==0x81, then start update main flash area */
				enable = 0x81;      
				EV01SE_Write(EV01SE_REG_UPG_LAST, &enable, 1);

				msleep(5000);
				
				do{
					msleep(1000);  
					EV01SE_Read(EV01SE_REG_UPG_LAST, &enable, 1);        //if 0x7D is 0x00 then update finished

					if(error_cnt++ > 10)
					{	
						LOGD(" >>>>>> EV01SE upg check upg last error %x .... \n ", enable);
						ret = -1;
						goto WL_UPG_EXIT;
					}
				}while(enable != 0);   //if 0x7D is 0x00 then update finished

				
				LOGD(" >>>>>> EV01SE upg finish .... \n ");
				break;
			}


			if(seq_num % EV21SC_EEPROM_UPG_PERCENT_PER == 0)    // 0x01ff /100     //显示升级百分百
			{
				LOGD(" >>>>>> EV01SE upg  percent %d%% .... \n ", ((seq_num / EV21SC_EEPROM_UPG_PERCENT_PER) > 100)?100:(seq_num / EV21SC_EEPROM_UPG_PERCENT_PER));
				percent =  ((seq_num / EV21SC_EEPROM_UPG_PERCENT_PER) > 100)?  100 : (seq_num / EV21SC_EEPROM_UPG_PERCENT_PER);
				upgrage_status = percent;          //低8bit 存百分百 
				upgrage_status |= (type<<8);   //高8bit 存升级类型
				percent_func(me, &upgrage_status);
			}	
		}


	}

#if 0
	enum
	{
		FW_CHECK_WIAT_FLASH_CONTENT=0X01,
		FW_CHECK_PASS = 0X66,
		FW_CHECK_FAIL = 0XDD,
		
	};

	error_cnt = 0;

	enable = 0X1;
	EV01SE_Write(EV20S_REG_FW_CHECK, &enable, 1);
	while(1)
	{
		msleep(1000); 
		EV01SE_Read(EV20S_REG_FW_CHECK, &enable, 1);
		switch(enable)
		{
			case FW_CHECK_PASS:
				LOGD(" >>> EV20S upg check pass \n ");
				goto UPG_OK;
				break;
			case FW_CHECK_FAIL:
				LOGD(" >>> EV20S upg check fail \n ");
				goto UPG_OK;
				break;
			default:
				error_cnt++;
				break;
		}

		if(error_cnt > 10)
		{
			LOGD(" >>> EV20S upg check error  %d\n ", enable);
			break;
		}
	}

UPG_OK:

#endif

WL_UPG_EXIT:
	if(ret == 0)
	{
		upgrage_status = WL_UPG_OK;          //低8bit 存百分百 
		upgrage_status |= (type<<8);   //高8bit 存升级类型
		percent_func(me, &upgrage_status);
	}
	else
	{
		upgrage_status = WL_UPG_NG;          //低8bit 存百分百 
		upgrage_status |= (type<<8);   //高8bit 存升级类型
		percent_func(me, &upgrage_status);
		LOGD(" >>>>>> EV01SE upg error exit .... \n ");
	}

	ev01se.upg_flag = 0;

	if(fd > 0)
		close(fd);
	
	return 0;
	
}



int EV01SE_standby(void)
{
	BYTE para[3] = {0x35, 0xaa, 0x55};

	EV01SE_Write(EV01SE_REG_CCH_DATA, para, sizeof(para));

	return 0;
}

WIRELESS_FUNC wl_ev01se = 
{
	EV01SE_Init,
	EV01SE_Pair,
	EV01SE_Volume,
	EV01SE_Mute,
	EV01SE_WooferVol,
	EV01SE_SurroundVol,
	EV01SE_BassVol,
	EV01SE_EQ,
	EV01SE_Get_Version,
	EV01SE_Polling,
	EV01SE_GetSyn,
	EV01SE_Upgrade,
	EV01SE_standby,
};



