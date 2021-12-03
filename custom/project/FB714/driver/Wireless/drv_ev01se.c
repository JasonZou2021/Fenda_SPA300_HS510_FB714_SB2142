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
#include "hw_gpio_list.h"
#include "Userapp.h"

extern QActive *UserApp_get(void);


/*
 * copy from EV20S and modify something
*/

#define NO_LINK_TO_SLEEP_TIME		(15*6)  // 15min
#define SLEEP_TIME_MS				(50) //5s   	// 2 - 2.2 -2.4 ... 5 
#define WAKEUP_TIME_MS 			(10) // 1s         // 1 - 1.2 -1.4 ... 4 
#define SLEEP_TIME_VALUE			((SLEEP_TIME_MS-20)/2)
#define WAKEUP_TIME_VALUE			((WAKEUP_TIME_MS-10)/2)


#define I2C_CLK	eHW_IO_6
#define I2C_DAT	eHW_IO_5


//#define REAR_L_DET_PIN	eHW_IO_42
//#define REAR_R_DET_PIN	eHW_IO_45
//#define REAR_POWER_EN_PIN	eHW_IO_46

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

#define EV01SE_I2C_ADDR		0X68
#define EV01SE_I2C2_ADDR		0X6a


typedef enum _eWirelessIndex
{
	eWireless1,
	eWireless2,	
	eWirelessMax,	
}eWirelessIndex;

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
#define CCH_10_SUB_MUTE_EN 		(1<<6)
#define CCH_10_SURR_MUTE_EN		(1<<5)
#define CCH_10_SUB_EQ_EN			(1<<1)
#define CCH_10_SURR_EQ_EN			(1<<0)


#define CCH_11_SURR_VOL_REG		(1<<7)
#define CCH_11_SURR_VOL_AMP		(1<<6)
#define CCH_11_SURR_MUTE			(3<<4)

#define CCH_11_SUB_VOL_REG		(1<<3)
#define CCH_11_SUB_VOL_AMP		(1<<2)
#define CCH_11_SUB_MUTE			(3<<0)


#define CCH_10_MUTE_EN		CCH_10_VALID|CCH_10_SUB_MUTE_EN|CCH_10_SURR_MUTE_EN
#define CCH_11_MUTE		CCH_11_SURR_MUTE|CCH_11_SUB_MUTE
#define CCH_11_DEMUTE		0

#define CCH_10_EQ_EN		CCH_10_VALID|CCH_10_SURR_EQ_EN


typedef struct _ev01se
{
	BYTE init;
	BYTE fd;
	I2C_CFG cfg;
	BYTE syn;
	BYTE status;
	BYTE dev_addr[eWirelessMax];
	
	BYTE sub_mode_ver;
	BYTE sub_eeprom_ver;
	BYTE TX1_stereo_mode_ver;
	BYTE TX1_stereo_eeprom_ver;
	BYTE TX2_stereo_mode_ver;
	BYTE TX2_stereo_eeprom_ver;
	
	BYTE sub_syn;
	BYTE TX1_stereo_syn;
	BYTE TX2_stereo_syn;

	BYTE sub_pair_status;
	BYTE stereo_pair_status;	

	BYTE rear_plugin_flag;

	BYTE upg_flag;
	UINT16 version[2];

	UINT16 seq_num; // for upg
	BYTE upg_num;
	BYTE upg_num_cnt;

}EV01SE;

EV01SE ev01se = 
{
	.dev_addr = {EV01SE_I2C_ADDR, EV01SE_I2C2_ADDR},
};


int EV01SE_Write(eWirelessIndex index, int iAddr, UINT8 *bData, int len)
{
	EV01SE *pEV01SE = &ev01se;

	BYTE ret;

	if(index == eWireless1)
		ret = AnD_I2CSWWrite(pEV01SE->fd, EV01SE_I2C_ADDR, iAddr, bData, len);
	else
		ret = AnD_I2CSWWrite(pEV01SE->fd, EV01SE_I2C2_ADDR, iAddr, bData, len);
	
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


int EV01SE_Read(eWirelessIndex index, int iAddr, UINT8 *bData, int len)
{
	EV01SE *pEV01SE = &ev01se;
	BYTE ret;
	
	if(index == eWireless1)
		ret = AnD_I2CSWRead(pEV01SE->fd, EV01SE_I2C_ADDR, iAddr, bData, len);
	else
		ret = AnD_I2CSWRead(pEV01SE->fd, EV01SE_I2C2_ADDR, iAddr, bData, len);

	if(ret != eI2COK)
	{
		LOGE(" EV01SE_Read reg %x  data %x NG\n ", iAddr, *bData);
		return -1;
	}
	else
	{
		//LOGD(" EV01SE_Read reg %x  data %x OK\n ", iAddr, *bData);
	}

	return 0;
}




int EV01SE_Init(void)
{
	EV01SE *pEV01SE = &ev01se;
	BYTE para = 0;
	BYTE i=0;
	//UINT16 version[2] = {0};

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
	pEV01SE->cfg.delay = 10; //50;
	pEV01SE->cfg.ack_delay = 200; //1000;  // > 300us
	pEV01SE->cfg.byte_type = I2C_BYTE_HIGH_TO_LOW;
	pEV01SE->cfg.high_voltage = I2C_HIGH_V_OUTPUT;
	pEV01SE->cfg.continue_start = I2C_START_SIG_CONTINUE_UNSUPPORT;
	pEV01SE->cfg.read_method = I2C_READ_HAVE_REG;
	pEV01SE->cfg.wr_method = I2C_WR_BYTE_NORMAL;
	pEV01SE->fd = I2c_IO_Init(&pEV01SE->cfg);

	LOGD("%s   fd : %d\n", __func__, pEV01SE->fd);

	pEV01SE->init = 0;

	for(i=0; i<eWirelessMax; i++)
	{
		if(EV01SE_Read(i, EV01SE_REG_TXVERSION, (UINT8 *)&pEV01SE->version[i], 2) == 0) 
		{
			para = 92; // 3min 
			EV01SE_Write(i, EV01SE_REG_PAIR_TIME, &para, 1);

			pEV01SE->status = EV01SE_POLL_CHECK_PAIR;
			
			LOGD("TX version : 0x%x\n", pEV01SE->version[i]);
		}
	}

	if(pEV01SE->version[0] != 0 && pEV01SE->version[1] != 0 )
		pEV01SE->init = 1;
	
	return 0;
}


int EV01SE_Pair(BYTE status)
{
	//EV01SE *pEV01SE = &ev01se;
	BYTE para = EV01SE_HANLE_PAIR; 

	status = status;

	// just only TX-1 can set pair mode
	EV01SE_Write(eWireless1, EV01SE_REG_SYS_CTL, &para, sizeof(para));

	LOGD(" %s %x\n ", __func__, para);
	
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
		para[0] = CCH_10_MUTE_EN;
		para[1] = CCH_11_MUTE;
		para[2] = 0;
	}
	else
	{
		para[0] = CCH_10_MUTE_EN;
		para[1] = CCH_11_DEMUTE;
		para[2] = 0;
	}

	EV01SE_Write(eWireless1, EV01SE_REG_CCH_DATA, para, sizeof(para));
	EV01SE_Write(eWireless2, EV01SE_REG_CCH_DATA, para, sizeof(para));
	
	return 0;
}


int EV01SE_EQ(BYTE eq)
{
//	EV01SE *pEV01SE = &ev01se;
	
	//if(pEV01SE->sub_syn == 0)
	//	return -1;

	LOGD("%s : %d\n ", __func__, eq);

	BYTE para[3] = {0};
#if 0
	if(eq == eWirelessEQ_25W && pEV01SE->rear_plugin_flag == 0)
	{
		LOGD("%s : rear plugout, no need set bypass mode\n ", __func__);
		return -1;
	}
#endif
	para[0] = CCH_10_EQ_EN;
	para[1] = 0;
	para[2] = eq;

	EV01SE_Write(eWireless1, EV01SE_REG_CCH_DATA, para, sizeof(para));
	EV01SE_Write(eWireless2, EV01SE_REG_CCH_DATA, para, sizeof(para));
	
	return 0;
}


int EV01SE_Get_Version(UINT8 *version)
{
	EV01SE *pEV01SE = &ev01se;

	BYTE TX_ver = (pEV01SE->version[0]>>8)+(pEV01SE->version[0]&0xff);

	//EV01SE_Read(eWireless1, EV01SE_REG_TXVERSION, (UINT8 *)&pEV01SE->version[0], 2);
	
	version[0] = TX_ver;  // TX-1
	version[1] = TX_ver;  // TX-2
	version[2] = pEV01SE->TX1_stereo_mode_ver; // RX Stereo modul 
	version[3] = pEV01SE->TX1_stereo_eeprom_ver; // RX Stereo eeprom
	version[4] = pEV01SE->TX2_stereo_mode_ver; // RX Stereo modul 
	version[5] = pEV01SE->TX2_stereo_eeprom_ver; // RX Stereo eeprom
	version[6] = pEV01SE->sub_mode_ver;  // RX Sub modul
	version[7] = pEV01SE->sub_eeprom_ver;  // RX Sub eeprom  

	LOGD(">>>>param %x-%x-%x-%x-%x\n", version[0], version[1], version[2], version[3], version[4]);

	return 0;
}
/*
void EV01SE_CheckRearPlugStatus(void)
{
	EV01SE *pEV01SE = &ev01se;

	enum
	{	
		REAR_PLUGOUT,
		REAR_PLUGIN,
		REAR_PLUG_MAX,
	};

	static BYTE last_status = REAR_PLUG_MAX;
	BYTE cur_status = REAR_PLUGOUT;
	BYTE plug_in_flag = 0;
	static BYTE last_plug_in_flag = 0;
	static BYTE plug_det_cnt; 

	if((GPIO_Intput_Read(REAR_L_DET_PIN) ==0) &&(GPIO_Intput_Read(REAR_R_DET_PIN)==0))
	{
		cur_status = REAR_PLUGIN;
		plug_in_flag = 1;
	}
	else
	{
		cur_status = REAR_PLUGOUT;
		plug_in_flag = 0;
	}

	if(cur_status != last_status)
	{
		plug_det_cnt = 5; // 500ms

		last_status = cur_status;
	}

	if(plug_det_cnt > 0)
	{
		plug_det_cnt--;
		if(plug_det_cnt == 0)
		{
			if(plug_in_flag != last_plug_in_flag)
			{
				last_plug_in_flag = plug_in_flag;

				pEV01SE->rear_plugin_flag = plug_in_flag;
			
				// SOD L DET 42, SOD R DET 45, both detected, SOD PWR EN 46 HIGH, provide 19V.
				if(plug_in_flag)  
				{
					GPIO_Output_Write(REAR_POWER_EN_PIN,IO_HIGH);

					EV01SE_EQ(eWirelessEQ_20W);
				
					LOGD(" %s :  REAR Plug in \n ", __func__);

					QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, REAR_PLUG_IN_SIG), 0);
				}
				else
				{
					GPIO_Output_Write(REAR_POWER_EN_PIN,IO_LOW);

					EV01SE_EQ(eWirelessEQ_10W);
					
					LOGD(" %s :  REAR Plug out \n ", __func__);

					QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, REAR_PLUG_OUT_SIG), 0);
				}
			}
		}
	}

}
*/

int EV01SE_Polling(void)
{
	static BYTE sub_syn,TX1_stereo_syn, TX2_stereo_syn;
	EV01SE *pEV01SE = &ev01se;
	BYTE ret = 0;
	BYTE TX1_data[12];
	BYTE TX2_data[12];
	static BYTE read_cch;

/*
	EV01SE_CheckRearPlugStatus();
*/	
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
					if(EV01SE_Read(eWireless1, EV01SE_REG_SUB_CCH_RECV, TX1_data, sizeof(TX1_data)) == 0)
					{
						if(TX1_data[0] & 0x01)
						{
							pEV01SE->sub_syn =1 ;
						}
						else
						{
							if(TX1_data[1] == 0)
								pEV01SE->sub_syn =0 ;
						}

						if(TX1_data[6] & 0x01)
						{
							pEV01SE->TX1_stereo_syn =1 ;
						}
						else
						{
							pEV01SE->TX1_stereo_syn =0 ;
						}


						if(sub_syn != pEV01SE->sub_syn)
						{
							if(pEV01SE->sub_syn)
							{
								pEV01SE->sub_mode_ver = TX1_data[1]+TX1_data[2];
								pEV01SE->sub_eeprom_ver = TX1_data[4];
								LOGD(" %s :  5.8G sub connected %d\n ", __func__, pEV01SE->sub_mode_ver);
/*
								if(pEV01SE->rear_plugin_flag)
									EV01SE_EQ(eWirelessEQ_20W);
								else
									EV01SE_EQ(eWirelessEQ_10W);
*/								
								ret = 1;
							}
							else
							{
								pEV01SE->sub_mode_ver = 0;
								pEV01SE->sub_eeprom_ver = 0;
								LOGD(" %s :  5.8G sub disconnected %x-%x-%x-%x-%x\n ", __func__, TX1_data[0], TX1_data[1], TX1_data[2], TX1_data[3], TX1_data[4]);
							}
							sub_syn = pEV01SE->sub_syn;
						}

						if(TX1_stereo_syn != pEV01SE->TX1_stereo_syn)
						{

							if(pEV01SE->TX1_stereo_syn)
							{
								pEV01SE->TX1_stereo_mode_ver = TX1_data[7]+TX1_data[8];
								pEV01SE->TX1_stereo_eeprom_ver = TX1_data[10];
								LOGD(" %s :  5.8G TX-1 stereo connected %d\n ", __func__, pEV01SE->TX1_stereo_mode_ver);
/*

								if(pEV01SE->rear_plugin_flag)
									EV01SE_EQ(eWirelessEQ_20W);
								else
									EV01SE_EQ(eWirelessEQ_10W);
*/							
								ret = 2;
							}
							else
							{
								pEV01SE->TX1_stereo_mode_ver = 0;
								pEV01SE->TX1_stereo_eeprom_ver = 0;
								LOGD(" %s :  5.8G TX-1 stereo disconnected %d\n ", __func__, pEV01SE->TX1_stereo_mode_ver);
							}
							TX1_stereo_syn = pEV01SE->TX1_stereo_syn;
						}


					//	LOGD(" sub %x-%x-%x-%x-%x ", TX1_data[0], TX1_data[1], TX1_data[2], TX1_data[3], TX1_data[4]);
					}
				}
				else 
				{	
					if(EV01SE_Read(eWireless2, EV01SE_REG_STEREO_CCH_RECV, TX2_data, sizeof(TX2_data)) == 0)
					{
					
						if(TX2_data[0] & 0x01)
						{
							pEV01SE->TX2_stereo_syn =1 ;
						}
						else
						{
							if(TX2_data[1] == 0)
								pEV01SE->TX2_stereo_syn =0 ;
						}
						
						if(TX2_stereo_syn != pEV01SE->TX2_stereo_syn)
						{
							if(pEV01SE->TX2_stereo_syn)
							{
						
								pEV01SE->TX2_stereo_mode_ver = TX2_data[1]+TX2_data[2];
								pEV01SE->TX2_stereo_eeprom_ver = TX2_data[4];
								LOGD(" %s :  5.8G TX-2 stereo connected %d\n ", __func__, pEV01SE->TX2_stereo_mode_ver);
/*
								if(pEV01SE->rear_plugin_flag)
									EV01SE_EQ(eWirelessEQ_20W);
								else
									EV01SE_EQ(eWirelessEQ_10W);
	*/							
								ret = 3;
							}
							else
							{
								
								pEV01SE->TX2_stereo_mode_ver = 0;
								pEV01SE->TX2_stereo_eeprom_ver = 0;
								LOGD(" %s :  5.8G TX-2 stereo disconnected %d\n ", __func__, pEV01SE->TX2_stereo_mode_ver);
							}
							TX2_stereo_syn = pEV01SE->TX2_stereo_syn;
						}

						//LOGD(" Surround %x-%x-%x-%x-%x ", data[0], data[1], data[2], data[3], data[4]);
					//	LOGD(" Surround %x-%x-%x-%x-%x ", TX2_data[0], TX2_data[1], TX2_data[2], TX2_data[3], TX2_data[4]);
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
	if((ev01se.sub_syn || ev01se.TX1_stereo_syn || ev01se.TX2_stereo_syn) && ev01se.upg_flag == 0)
	 	return 1;

	 return 0;
}


enum
{

	WLE_ACK_ERR = -3,
	WLE_ACK_WAIT = -2,
	WLE_ACK_RESEND = -1,
	WLE_ACK_OK = 0,
};




int EV01SE_UpgPerpar(eWirelessIndex index, eWirelessType type, eWLEUpgMethod method)
{
	int fd = -1; 
	BYTE data[16];
	int read_len = 0;
	BYTE enable = 0xA5;
	BYTE eeprom_size = 0x10;    //8k byte     eeprom size 0x01-0x08
	//UPGHEADER *pupgHeader=UserApp_getUpgHeader();
	//int offset;
	
	BYTE upg_mask[WIRELESS_Type_MAX][8] = 
	{
		{0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88},  // TX 
		{0x20,0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x00},  // RX surround
		{0x2F,0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x00},  // RX EEPROM surround
		{0x50,0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x00},  // RX subwoofer
		{0x5F,0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x00},  // RX EEPROM subwoofer
	};

	BYTE upg_path[WIRELESS_Type_MAX][20] = 
	{
		{"USB:S24_TX.BIN"},
		{"USB:S24_RX.BIN"},
		{"USB:S24_EE_SUR.BIN"},
		{"USB:S24_RX.BIN"},  // surround & subwoofer is one bin
		{"USB:S24_EE_SUB.BIN"},
	};

	method = method;
		
	BYTE upg_head[16] = "everestek co ltd";

	LOGD(" wireless upg mode [%s]\n ", upg_path[type]);

	if(type >= WIRELESS_Type_MAX)
	{
		LOGD(" >>>>>> EV01SE upg type error \n ");
		return -1;
	}
#if 0
	BYTE upg_all_path[] = "USB:app.BIN";

	if(method == WLE_UPG_METHOD_ALL_BIN)
	{
		fd = open(upg_all_path, O_RDONLY, 0);
		if(fd < 0)
		{
			LOGD(" >>>>>> EV01SE upg file can't open \n ");
			return -1;
		}

		switch(type)
		{
			case WIRELESS_Type_TX:
				offset = pupgHeader->tx_offset;
				break;
				
			case WIRELESS_Type_RX_SURROUND:	
			case WIRELESS_Type_RX_SUBWOOFER:
				offset = pupgHeader->rx_offset;
				break;

			case WIRELESS_Type_EEPROM_SURROUND:
				offset = pupgHeader->ee_offset;
				break;
				
			case WIRELESS_Type_EEPROM_SUBWOOFER:
				offset = pupgHeader->ee_sub_offset;
				break;
			default:
				close(fd);
				return -1;
				
		}
		
		lseek(fd, offset, SEEK_SET);
	}
	else 
	{
		fd = open(upg_path[type], O_RDONLY, 0);
		if(fd < 0)
		{
			LOGD(" >>>>>> EV01SE upg file can't open \n ");
			return -1;
		}
	}
#endif
	fd = open(upg_path[type], O_RDONLY, 0);
		if(fd < 0)
		{
			LOGD(" >>>>>> EV01SE upg file can't open \n ");
			return -1;
		}

	read_len = read(fd, data, sizeof(data));    //read top 16 byte data
	if(read_len < 0)
	{
		close(fd);
		LOGD(" >>>>>> EV01SE upg file read error \n ");
		return -1;
	}
	
	if(strncmp(data, upg_head, 16))    //compare hand core
	{
		close(fd);
		LOGD(" >>>>>> EV01SE upg head error ,is not a ev20 upg file... \n ");
		return -1;
	}

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


	if(type == WIRELESS_Type_EEPROM_SURROUND || type == WIRELESS_Type_EEPROM_SUBWOOFER)      //upgrade EEPROM need write eeprom size to 0x6a  
	{     
		EV01SE_Write(index, EV01SE_REG_UPG_DATA, &eeprom_size, 1);
	}


	if(EV01SE_Write(index, EV01SE_REG_UPG_MASK, upg_mask[type], 8) < 0)
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
	EV01SE_Write(index, EV01SE_REG_UPG_ENANLE, &enable, 1);
	msleep(50);
	
	return fd;
	
}

int EV01SE_UpgSendPacket(int fd, eWirelessIndex index)
{
	EV01SE *pEV01SE = &ev01se;
	int read_len = 0;
	int i =0;
	BYTE checksum = 0;
	BYTE upg_data[10] = {0};
	BYTE data[16];

	/*
		Step 3: 
		External MCU send two I2C write command, each I2C command will write 8 bytes into ETK51. So total 16
		bytes will write into ETK51. Each I2C command need extra 3ms delay for internal MCU to process data.
	*/
	checksum = 0;
	memset(data, 0, sizeof(data));
	read_len = read(fd, data, sizeof(data));          //每次读取16BYTE

	 /* step2 */
	// 0x6A write 8byte file data
	memset(upg_data, 0, sizeof(upg_data));

	upg_data[0] = (pEV01SE->seq_num >> 8) & 0XFF;    //High byte -> 0x6a
	upg_data[1] = (pEV01SE->seq_num & 0xFF);             //low byte -> 0x6b
		
	memcpy(&upg_data[2], data, 8);           //8 byte data -> 0x6c-0x73
	for(i=0; i<10; i++)
	{
		checksum += upg_data[i]; 
		
	//	if(i >= 2)
	//		LOGD(">>>start 0x6c end 0x73  data is  0x%x\n", upg_data[i]);
	}
	EV01SE_Write(index, EV01SE_REG_UPG_DATA, upg_data, 10);    
	msleep(2);

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
	EV01SE_Write(index, EV01SE_REG_UPG_DATA1, upg_data, 10);    
	msleep(2);

	pEV01SE->seq_num++;

	return pEV01SE->seq_num;
}

int EV01SE_UpgCheckACK(int fd, eWirelessIndex index)
{
	BYTE upg_status;
	static BYTE error_cnt;
	int i2c_error_cnt = 0;
	EV01SE *pEV01SE = &ev01se;
	int ret = WLE_ACK_OK;
	
	enum
	{
		UPG_STATUS_SEQ_ERROR_00 = 0X00,
		UPG_STATUS_WRITE_OK = 0X01,
		UPG_STATUS_WAIT = 0X02,
		UPG_STATUS_CHECKSUM_FAIL= 0X04,
		UPG_STATUS_SEQ_ERROR_08= 0X08,
		
	};

	error_cnt = 0;

	//msleep(20);

	do
	{
		if(error_cnt > 80)    //连续出现错误则为升级失败
		{
			LOGD(" >>>>>> EV01SE upg wait timeout fail %d\n ", upg_status);
			return WLE_ACK_ERR;
		}

		// wirte 16bytes data, check upgrade status
		i2c_error_cnt = 16;
		while(i2c_error_cnt)
		{
			if(EV01SE_Read(index, EV01SE_REG_UPG_STATUS, &upg_status, 1)<0)
			{
				LOGD(" >>>>>> EV01SE I2C read upg status reg error %d\n ", i2c_error_cnt);
				i2c_error_cnt--;
				msleep(20);
				if (i2c_error_cnt==0)
				return WLE_ACK_ERR;

			}
			else
				i2c_error_cnt = 0;
		}

		//LOGD("EV01SE_REG_UPG_STATUS  0x7E = 0x%x \n", upg_status);
		
		switch(upg_status & 0xf)
		{
			//case UPG_STATUS_SEQ_ERROR_00:
			//case UPG_STATUS_WAIT:   //busy
			default:
				msleep(200);
				error_cnt++;
				//LOGD(" >>>>>> EV01SEupg status need  wait .... (%d)\n ", upg_status);
				ret = WLE_ACK_WAIT;
				break;

			case UPG_STATUS_CHECKSUM_FAIL:
				error_cnt++;
				LOGD(" >>>>>> EV01SEupg status checksum error.... \n ");
				pEV01SE->seq_num--;
				lseek(fd, -16, SEEK_CUR);    //if fail need set fd  offset forward 16 byte, write again.
				msleep(50);
				ret = WLE_ACK_RESEND;
				break;

			
			case UPG_STATUS_SEQ_ERROR_08:
				error_cnt++;
				LOGD(" >>>>>> EV01SE upg status seq number error.... \n ");
				pEV01SE->seq_num--;
				lseek(fd, -16, SEEK_CUR);    
				msleep(50);
				ret = WLE_ACK_RESEND;
				break;

			case UPG_STATUS_WRITE_OK:
				ret = WLE_ACK_OK;
				break;
				
		}
	}while(ret == WLE_ACK_WAIT);


	return ret;
}


int EV01SE_UpgCheckFinish(QActive *me, eWirelessIndex index, ARFunc_t percent_func, eWirelessType type, UINT16 seq_num)
{
	#define EV21SC_END_ADDR 0Xdff
	#define EV21SC_UPG_PERCENT_PER	(EV21SC_END_ADDR/100)

	#define EV21SC_SUB_EEPROM_END_ADDR 0X1FF
	#define EV21SC_SUB_EEPROM_UPG_PERCENT_PER	( EV21SC_SUB_EEPROM_END_ADDR/100)

	#define EV21SC_EEPROM_END_ADDR 0X3FF
	#define EV21SC_EEPROM_UPG_PERCENT_PER	( EV21SC_EEPROM_END_ADDR/100)

	BYTE enable = 0;
	BYTE error_cnt = 0;
	BYTE finished_flag;
	BYTE percent;
	int ret = 0;
	UINT16 upgrage_status;
	EV01SE *pEV01SE = &ev01se;

	if((type == WIRELESS_Type_TX))
		finished_flag = 0;
	else
		finished_flag = 0x66;


	/* step3 */
	if((type == WIRELESS_Type_TX) || (type == WIRELESS_Type_RX_SURROUND) || (type == WIRELESS_Type_RX_SUBWOOFER))    //upgrade tx flash and rx flash
	{
		
		if(seq_num == EV21SC_END_ADDR+1)    //the  final packet is sent
		{
			ret = 1;
			/* MCU write register 0x7D to 0x81. When ETK51 get 0x7D==0x81, then start update main flash area */
			enable = 0x81;      
			EV01SE_Write(index, EV01SE_REG_UPG_LAST, &enable, 1);

			msleep(5000);
			
			do{
				msleep(1000);  
				EV01SE_Read(index, EV01SE_REG_UPG_LAST, &enable, 1);        //if 0x7D is 0x00 then update finished

				if(error_cnt++ > 10)
				{	
					LOGD(" >>>>>> EV01SE upg check upg last error %x .... \n ", enable);
					ret = -1;
					break;
				}
			}while(enable != finished_flag);   //if 0x7D is 0x00 then update finished

			
			LOGD(" >>>>>> EV01SE upg finish .... \n ");
		}


		// When first five FW packets is sent, need extra 200 msec delay for client erase Flash.update eeprom no extra delay
		if(seq_num == 5)    //update flash 时的第一次发五个包之后需要delay 200ms
			msleep(200);  


		if(seq_num % EV21SC_UPG_PERCENT_PER == 0)    // 0x0Eff /100     //显示升级百分百
		{
			//LOGD(" >>>>>> EV01SE upg  percent %d%% .... \n ", ((seq_num / EV21SC_UPG_PERCENT_PER) > 100)?100:(seq_num / EV21SC_UPG_PERCENT_PER));
			percent =  ((seq_num / EV21SC_UPG_PERCENT_PER) > 100)?  100 : (seq_num / EV21SC_UPG_PERCENT_PER);
			//upgrage_status = percent;            //低位存升级百分百
			upgrage_status = percent/pEV01SE->upg_num + (100/pEV01SE->upg_num)*(pEV01SE->upg_num_cnt-1);
			LOGD(" >>>>>> EV01SE upg  percent %d%% .... \n ", upgrage_status);
			upgrage_status |= (type<<8);     //高位存升级类型
			percent_func(me, &upgrage_status);
		}
		
	}
	else if(type == WIRELESS_Type_EEPROM_SURROUND)     //upgrade rx eeprom
	{
		
		if(seq_num == EV21SC_EEPROM_END_ADDR+1)    //8k eeprom    0x1FF = 8*1024/16-1
		{
			ret = 1;
			/* MCU write register 0x7D to 0x81. When ETK51 get 0x7D==0x81, then start update main flash area */
			enable = 0x81;      
			EV01SE_Write(index,EV01SE_REG_UPG_LAST, &enable, 1);

			msleep(5000);
			
			do{
				msleep(1000);  
				EV01SE_Read(index,EV01SE_REG_UPG_LAST, &enable, 1);        //if 0x7D is 0x00 then update finished

				if(error_cnt++ > 10)
				{	
					LOGD(" >>>>>> EV01SE upg check upg last error %x .... \n ", enable);
					ret = -1;
					break;
				}
			}while(enable != finished_flag);   //if 0x7D is 0x00 then update finished

			
			LOGD(" >>>>>> EV01SE upg finish .... \n ");
		}


		if(seq_num % EV21SC_EEPROM_UPG_PERCENT_PER == 0)    // 0x01ff /100     //显示升级百分百
		{
			
			percent =  ((seq_num / EV21SC_EEPROM_UPG_PERCENT_PER) > 100)?  100 : (seq_num / EV21SC_EEPROM_UPG_PERCENT_PER);
			//upgrage_status = percent;          //低8bit 存百分百 

			upgrage_status = (percent/pEV01SE->upg_num) + (100/pEV01SE->upg_num)*(pEV01SE->upg_num_cnt-1);
			LOGD(" >>>>>> EV01SE upg  percent %d%% ....  \n ", upgrage_status);
			upgrage_status |= (type<<8);   //高8bit 存升级类型
			percent_func(me, &upgrage_status);

			
		}	
	}
	else if(type == WIRELESS_Type_EEPROM_SUBWOOFER)
	{
		if(seq_num == EV21SC_SUB_EEPROM_END_ADDR+1)    //8k eeprom    0x1FF = 8*1024/16-1
		{
			ret = 1;
			/* MCU write register 0x7D to 0x81. When ETK51 get 0x7D==0x81, then start update main flash area */
			enable = 0x81;      
			EV01SE_Write(index,EV01SE_REG_UPG_LAST, &enable, 1);

			msleep(5000);
			
			do{
				msleep(1000);  
				EV01SE_Read(index,EV01SE_REG_UPG_LAST, &enable, 1);        //if 0x7D is 0x00 then update finished

				if(error_cnt++ > 10)
				{	
					LOGD(" >>>>>> EV01SE upg check upg last error %x .... \n ", enable);
					ret = -1;
					break;
				}
			}while(enable != finished_flag);   //if 0x7D is 0x00 then update finished

			
			LOGD(" >>>>>> EV01SE upg finish .... \n ");
		}


		if(seq_num % EV21SC_SUB_EEPROM_UPG_PERCENT_PER == 0)    // 0x01ff /100     //显示升级百分百
		{
			
			percent =  ((seq_num / EV21SC_SUB_EEPROM_UPG_PERCENT_PER) > 100)?  100 : (seq_num / EV21SC_SUB_EEPROM_UPG_PERCENT_PER);
			//upgrage_status = percent;          //低8bit 存百分百 

			upgrage_status = (percent/pEV01SE->upg_num) + (100/pEV01SE->upg_num)*(pEV01SE->upg_num_cnt-1);
			LOGD(" >>>>>> EV01SE upg  percent %d%% ....  \n ", upgrage_status);
			upgrage_status |= (type<<8);   //高8bit 存升级类型
			percent_func(me, &upgrage_status);

			
		}	
	}

	return ret;
}


int EV01SE_DoUpgrade(QActive *me, eWirelessIndex index, ARFunc_t percent_func, eWirelessType type, eWLEUpgMethod method)
{
	EV01SE *pEV01SE = &ev01se;
	int ret = -1;
	UINT16 upgrage_status;
	int fd = -1;

	pEV01SE->seq_num = 0;
	pEV01SE->upg_num_cnt++;

	fd = EV01SE_UpgPerpar(index, type, method);
	if(fd < 0)
		return -1;

	while(1)
	{

		do
		{
			EV01SE_UpgSendPacket(fd, index);
			ret = EV01SE_UpgCheckACK(fd, index);
		}	
		while(ret == WLE_ACK_RESEND);

		if(ret == WLE_ACK_ERR)
			break;
		
		ret = EV01SE_UpgCheckFinish(me, index, percent_func, type, pEV01SE->seq_num);

		if(ret != 0)
			break;
			
	}

	if(pEV01SE->upg_num_cnt == pEV01SE->upg_num)
	{
		if(ret == 1)
		{
			upgrage_status = WL_UPG_OK;          //低8bit 存百分百 
			upgrage_status |= (type<<8);   //高8bit 存升级类型
			percent_func(me, &upgrage_status);
			LOGD(" >>>>>> EV01SE upg ok .... \n ");
		}
		else
		{
			upgrage_status = WL_UPG_NG;          //低8bit 存百分百 
			upgrage_status |= (type<<8);   //高8bit 存升级类型
			percent_func(me, &upgrage_status);
			LOGD(" >>>>>> EV01SE upg error exit .... \n ");
		}
	}
	else
	{
		if(ret == WLE_ACK_ERR)
		{
			upgrage_status = WL_UPG_NG;          //低8bit 存百分百 
			upgrage_status |= (type<<8);   //高8bit 存升级类型
			percent_func(me, &upgrage_status);
			LOGD(" >>>>>> EV01SE upg error exit .... \n ");
		}
	      
	}

	if(fd > 0)
		close(fd);

	return 0;
}


static int EV01SE_Upgrade(QActive *me, ARFunc_t percent_func, eWirelessUpg type, eWLEUpgMethod upg_method)
{

	EV01SE *pEV01SE = &ev01se;
	
	pEV01SE->upg_flag = 1;
	pEV01SE->upg_num_cnt = 0;

	if(type == WLE_UPG_TX)
	{
		pEV01SE->upg_num = 2;
		EV01SE_DoUpgrade(me, eWireless1, percent_func, WIRELESS_Type_TX, upg_method);
		EV01SE_DoUpgrade(me, eWireless2, percent_func, WIRELESS_Type_TX, upg_method);	
	}
	else if(type == WLE_UPG_RX)
	{
		pEV01SE->upg_num = 3;
		EV01SE_DoUpgrade(me, eWireless1, percent_func, WIRELESS_Type_RX_SURROUND, upg_method);
		EV01SE_DoUpgrade(me, eWireless1, percent_func, WIRELESS_Type_RX_SUBWOOFER, upg_method);
		
		EV01SE_DoUpgrade(me, eWireless2, percent_func, WIRELESS_Type_RX_SURROUND, upg_method);
		
	}
	else if(type == WLE_UPG_EE_SUR)
	{
		pEV01SE->upg_num = 2;
		EV01SE_DoUpgrade(me, eWireless1, percent_func, WIRELESS_Type_EEPROM_SURROUND, upg_method);
		EV01SE_DoUpgrade(me, eWireless2, percent_func, WIRELESS_Type_EEPROM_SURROUND, upg_method);
		
	}
	else if(type == WLE_UPG_EE_SWF)
	{
		pEV01SE->upg_num = 1;
		EV01SE_DoUpgrade(me, eWireless1, percent_func, WIRELESS_Type_EEPROM_SUBWOOFER, upg_method);
	}
	
	pEV01SE->upg_flag = 0;

	return 0;
	
}




int EV01SE_Get_PlugStatus(void)
{
	return ev01se.rear_plugin_flag;
}

WIRELESS_FUNC wl_ev01se = 
{
	EV01SE_Init,
	EV01SE_Pair,
	NULL,
	EV01SE_Mute,
	NULL,
	NULL,
	NULL,
	EV01SE_EQ,
	EV01SE_Get_Version,
	EV01SE_Polling,
	EV01SE_GetSyn,
	EV01SE_Upgrade,
	NULL,
	EV01SE_Get_PlugStatus,
};


