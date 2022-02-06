#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "pinmux.h"
//#include "std_interfaces.h"
#include "busy_delay.h"
#include "exstd_interfaces.h"
#define LOG_MODULE_ID  ID_APUSR
#define LOG_EXTRA_STR  "[ITE]"
#include "log_utils.h"
#include "AnD_I2C_Srv.h"
#include "hdmi_app.h"
#include "datarammap.h"
#include "custom_sig.h"
#include "ITE_REP.h"
#include "fsi_api.h"
#include "VFD.h"
#include "IT6622.h"


#define SB_VOL_DEFAULT		(10)
//#define SB_VOL_MAX		(33)

iTE_u8	g_u8SbRxSel = 0;
iTE_u8	g_u8SbAudioMode = 0;
iTE_u8	g_u8SbTxSta = 0;
iTE_u8	g_u8SbRxEdid = 0;
iTE_u8	g_u8SbPower;
iTE_u8	g_u8SbCecMode = SB_CEC_AUTO;
iTE_u8	g_u8IteRdy = 0;
iTE_u8	g_u8SbInfoCA = 0;
iTE_u32	g_u32SbAdoDecInfo = 0;
iTE_u8	g_u8SbVol = 0;

#define ITE_RDY_RAM_FLAG		0x12568900
BOOL IT6622_Write(BYTE, BYTE reg, int len, BYTE *bBuf);
BOOL IT6622_Read(BYTE, BYTE reg, int len, BYTE *buf);

int IT6622_MCU_UPG    (void);

#define ITE_ADDR  0x80
#define SB_I2C_Write(u8Offset, u16Len, u8Buf)	IT6622_Write(ITE_ADDR, u8Offset, u16Len, u8Buf)
#define SB_I2C_Read(u8Offset, u16Len, u8Buf)	IT6622_Read(ITE_ADDR, u8Offset, u16Len, u8Buf)
#define SB_SysI2cChange(u8I2cChg)		IT6622_I2cChange(u8I2cChg)

enum
{
	CEC_POWER_NULL,
	CEC_POWER_ON,
	CEC_POWER_OFF,	
};

typedef enum
{
	CecSpk_TV,
	CecSpk_SB,
	CecSpk_MAX
	
}eCecSpk;

#define I2C_CLK 			11
#define I2C_DAT 			10
#define HDMI_INT		46 // ??
#define HDMI_INT2		47
//#define HDMI_PWR			2

#define ARC_DET			64
#define HDMI_MUTE		65

QActive *UserApp_get(void);

typedef struct _ite
{
	int fd;
	I2C_CFG cfg;
	BYTE read_flag;
	BYTE volume;
	BYTE volmax;
	BYTE power;
	BYTE mute;
	BYTE audio_format[4];
	BYTE channel_allocation;
	BYTE mode;
	BYTE src;
	BYTE eARC;
	BYTE ARC;
	BYTE vol_timeout;
	BYTE audio_type;	
	BYTE upg_flag;
	BYTE ver;
	BYTE hpd_in;
	UINT16 hpd_in_cnt;
	BYTE sys_audio_cnt;
	BYTE sys_audio_time;
	eCecSpk spk_mode;

		BYTE switch_src_delay;

}ITE_HANDLE;

static ITE_HANDLE ite = {.fd = -1};

BOOL IT6622_Write(BYTE addr, BYTE reg, int len, BYTE *bBuf)
{
	ITE_HANDLE *pite = &ite; 
	BOOL bRet = FALSE;
	int sdRet = -1;


	pite->cfg.delay = 15;
	pite->cfg.ack_delay = 80; 
	
	
	I2c_IO_ReInit(&pite->cfg);

	if(pite->fd >= 0)
	{
		sdRet = AnD_I2CSWWrite(pite->fd, addr, reg, bBuf, len);	
		if(sdRet > 0)
		{
			bRet = FALSE;
			LOGD("%s Reg %x NG\n", __func__, reg);
		}
		else
		{
			bRet = TRUE;
			//amp_printf_E("dev Reg %x data %x OK\n", bRegAddr, *(UINT8 *)bBuf);
		}
	}

	delay_1ms(10);

	return (bRet);
}


BOOL IT6622_Read(BYTE addr, BYTE reg, int len, BYTE *buf)
{
	ITE_HANDLE *pite = &ite;
	int sdRet = -1;
	BOOL bRet = FALSE;


	pite->cfg.delay = 15;
	pite->cfg.ack_delay = 80; 
	

	I2c_IO_ReInit(&pite->cfg);

	if(pite->fd >= 0)
	{
		sdRet = AnD_I2CSWRead( pite->fd, addr, reg, buf, len);	
		if(sdRet > 0)
		{
			bRet = FALSE;
			LOGD("%s Reg %x NG\n", __func__, reg);
		}
		else
		{
			bRet = TRUE;
			//amp_printf_E("dev Reg %x data %x OK\n", bRegAddr, *(UINT8 *)bBuf);
		}
	}

	return (bRet);
}

static int IT6622_Cfg(BYTE mode, BYTE src, BYTE vol, BYTE volmax)
{

	ITE_HANDLE *pite = &ite; 
	
	volatile int *pIteRdyRAM = (int * )SRAM_ADDR_ISP_UPGRADE; // SRAM_ADDR_HDMI_MCU_RDY_FLAG;
	if(((*pIteRdyRAM) &0xffffff00)== ITE_RDY_RAM_FLAG)
	{
		g_u8IteRdy = 1;
		pite->ver = (*pIteRdyRAM)&0xff;
	}
	else
		g_u8IteRdy = 0;


	pite->mode = mode;

	//if(mode == HDMI_SYS_STANDBY && g_u8IteRdy == 1)
	//	pite->switch_src_delay = STANDBY_DELAY_CNT;

	if(pite->fd < 0)
	{
		//Pinmux_GPIO_Free(HDMI_MUTE);
		//Pinmux_GPIO_Alloc(HDMI_MUTE,IO_GPIO,IO_RISC);
		//GPIO_OE_Set(HDMI_MUTE, IO_INPUT);

		//Pinmux_GPIO_Free(HDMI_INT);
		//Pinmux_GPIO_Alloc(HDMI_INT,IO_GPIO,IO_RISC);
		//GPIO_OE_Set(HDMI_INT, IO_INPUT);

		Pinmux_GPIO_Free(HDMI_INT2);
		Pinmux_GPIO_Alloc(HDMI_INT2,IO_GPIO,IO_RISC);
		GPIO_OE_Set(HDMI_INT2, IO_INPUT);

		//Pinmux_GPIO_Free(HDMI_PWR);
		//Pinmux_GPIO_Alloc(HDMI_PWR,IO_GPIO,IO_RISC);
		//GPIO_OE_Set(HDMI_PWR, IO_OUTPUT);
		//GPIO_Output_Write(HDMI_PWR, IO_HIGH);
		
		pite->cfg.clk = I2C_CLK;
		pite->cfg.dat = I2C_DAT;
		pite->cfg.delay = 15;
		pite->cfg.ack_delay = 80;
		pite->cfg.high_voltage = I2C_HIGH_V_OUTPUT;
		pite->cfg.byte_type = I2C_BYTE_HIGH_TO_LOW;
		pite->cfg.continue_start = I2C_START_SIG_CONTINUE_SUPPORT;
		pite->cfg.read_method = I2C_READ_HAVE_REG;
		pite->cfg.wr_method = I2C_WR_BYTE_NORMAL;

		pite->fd = I2c_IO_Check_Used(&pite->cfg);

		if(pite->fd < 0)
			pite->fd = I2c_IO_Init(&pite->cfg);

		
	}

	pite->volume = vol;
	pite->volmax = volmax;
	pite->src = src;	
	pite->mute = 0;
	pite->spk_mode = CecSpk_SB;

	LOGD("IT66321 Cfg %d, vol %d, ver %d, g_u8IteRdy %d\n", pite->fd, pite->volume, pite->ver, g_u8IteRdy);

	return 0;
}




//iTE_u8 const u8SBSpkAlloc[] = { 0x83, 0x6F, 0x0F, 0x0C };
iTE_u8 const u8SBAdb[] = {
	0x38,
	0x09, 0x0F, 0x07,
	0x0F, 0x17, 0x07,
	0x57, 0x04, 0x03,
	0x15, 0x07, 0x50,
	0x67, 0x7E, 0x03,
	0x5F, 0x7F, 0x07,
	0x3E, 0x06, 0xC0,
	0x5F, 0x7F, 0x01, 

};
#define SB_ADB_LEN	(sizeof(u8SBAdb))

iTE_u8 const EARC_SB_ADO_CAP_TABLE[]=
{
	0x01, 
	0x01, 
	0x1D,//ADB+SpkAlloc length

	0x38,
	0x09, 0x0F, 0x07,
	0x0F, 0x17, 0x07,
	0x57, 0x04, 0x03,
	0x15, 0x07, 0x50,
	0x67, 0x7E, 0x03,
	0x5F, 0x7F, 0x07,
	0x3E, 0x06, 0xC0,
	0x5F, 0x7F, 0x01, 

	0x83,	0x6F, 0x0F, 0x0C
};
#define EARC_SB_ADO_CAP_TABLE_SIZE		sizeof(EARC_SB_ADO_CAP_TABLE)


void IT6622_I2cChange(iTE_u8 u8I2cChg)
{
	if (u8I2cChg) {
		SB_I2C_Write(I2C_SYS_CHANGE, 1, &u8I2cChg);
		LOGD("SB I2C chg = %X\n", u8I2cChg);
	//	I2C_Slave_Dump();
	}
}

void SB_I2C_Dump(void)
{
	iTE_u8	u8Cnt;
	iTE_u8	u8Data[0x10];

	for (u8Cnt = 0; u8Cnt < 0x10; u8Cnt++) {
		SB_I2C_Read(u8Cnt << 4, 0x10, u8Data);
		LOGD("0x%02X: %02X %02X %02X %02X  "
					"%02X %02X %02X %02X    "
					"%02X %02X %02X %02X  "
					"%02X %02X %02X %02X\n",
			u8Cnt << 4, u8Data[0], u8Data[1], u8Data[2], u8Data[3],
			u8Data[4], u8Data[5], u8Data[6], u8Data[7],
			u8Data[8], u8Data[9], u8Data[10], u8Data[11],
			u8Data[12], u8Data[13], u8Data[14], u8Data[15]);
	}
}


iTE_u8 SB_DefaultSetting(void)
{
	ITE_HANDLE *pite = &ite; 
	iTE_u8	u8I2cReg[3];
	iTE_u8	u8RxMode;
	iTE_u8	u8I2cChg = 0;
	LOGD("SB_DefaultSetting \n");

	u8I2cReg[0] = pite->volume;
	SB_I2C_Write(I2C_ADO_VOL, 1, &u8I2cReg[0]);
	u8I2cReg[0] = pite->volmax;
	u8I2cReg[1] = 0;
	u8I2cReg[2] = 0;
	SB_I2C_Write(I2C_VOLUME_MAX, 3, u8I2cReg);
	u8I2cChg |= 2;

	u8I2cReg[0] = 8;
	SB_I2C_Write(I2C_EDID_UPDATE, 1, u8I2cReg);
//	SB_I2C_Write(I2C_EDID_SPK_ALLOC, 4, (iTE_u8 *)u8SBSpkAlloc);
	SB_I2C_Write(I2C_EDID_ADB0, 31, (iTE_u8 *)u8SBAdb);
	u8I2cChg |= 8;

//	g_u8SbAudioMode = 0x20;
//	IT66322_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);

//	u8RxMode = 0x44; //eUserControlPressed;
//	IT66322_I2C_Write(I2C_SYS_CEC_LATCH_CMD, 1, &u8RxMode);
	u8RxMode = 0;
	SB_I2C_Write(I2C_SYS_CEC_TRANS_CNT, 1, &u8RxMode);
	u8RxMode = 0xA0; //eVendorCommandWithID
	SB_I2C_Write(I2C_SYS_CEC_LATCH_CNT, 1, &u8RxMode);

	u8I2cChg |= 0xC0;
	return u8I2cChg;
}

iTE_u8 SB_PowerOff(void)
{
	iTE_u8	u8I2cChg;
	LOGD("SB_PowerOff \n");
	g_u8SbPower = 0;
	u8I2cChg = SB_DefaultSetting();
	// 00: power down mode
	g_u8SbAudioMode &= 0xFC;
	SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
	u8I2cChg |= 0x80;

	return u8I2cChg;
}

iTE_u8 SB_PowerOn(void)
{
	iTE_u8 u8I2cChg = 0;

	LOGD("SB_PowerOn \n");
	g_u8SbPower = SB_POWER_ON;

	// 1: power on mode
	g_u8SbAudioMode = 0x39;	//0x41//0x61;
	SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);

	// SB CEC auto mode
	g_u8SbCecMode = SB_CEC_AUTO;

	u8I2cChg |= I2C_UPD_SYS_SET;

	return u8I2cChg;
}

iTE_u8 IT66322_SB_Standby(void)
{
	LOGD("SB_Standby \n");
	g_u8SbPower = SB_STANDBY;
	// 10: standby mode
	I2C_STA_CHANGE(g_u8SbAudioMode, I2C_MODE_POWER_MASK | I2C_MODE_CEC_MASK,
			I2C_MODE_POWER_STANDBY | I2C_MODE_CEC_EN);
//	g_u8SbAudioMode &= 0xFC;
//	g_u8SbAudioMode |= 0x22;
	SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
	return I2C_UPD_SYS_SET;
}

iTE_u8 SB_PassThrough(void)
{
	LOGD("SB_PassThrough \n");
	//g_u8SbPower = SB_PASS_THROUGH;

	// Disable HDMI Audio decode
//	g_u8SbRxSel |= 0xF0;
	// Power On, Audio System Off, eARC/ARC Off, SB CEC Off, Tx Output & audio out.
	//g_u8SbCecMode = SB_CEC_OFF;
	g_u8SbAudioMode = I2C_MODE_TX_OUT_EN | I2C_MODE_TX_ADO_EN |
			I2C_MODE_CEC_DIS | I2C_MODE_EARC_DIS |
			I2C_MODE_ADO_SYS_DIS | I2C_MODE_POWER_ON;// 0x01;
	SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
	// Rx active when Tx select to.
//	u8RxMode = 0x00;


	return I2C_UPD_SYS_SET;
}

unsigned char SB_EdidChg(iTE_u1 u8EdidChg)
{
	iTE_u8 u8I2cChg = 0;
	iTE_u8 u8AdoSel  = 0;

	SB_I2C_Read(I2C_ADO_SEL, 1, &u8AdoSel);

	if (u8EdidChg != u8AdoSel) {
		SB_I2C_Write(I2C_ADO_SEL, 1, &u8EdidChg);
		u8I2cChg |= 0x02;
	}

	return u8I2cChg;
}

iTE_u8 SB_CecPower(iTE_u1 bEn)
{
	iTE_u8	u8I2cChg = 0;

	if (bEn) {
		if ((g_u8SbAudioMode & 0x03) != 0x01) {
			g_u8SbAudioMode &= 0xFC;
			g_u8SbAudioMode |= 0x01;
			SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
			u8I2cChg |= 0x80;
				LOGD("CEC -> Power On \n");
		}
	} else {
		g_u8SbAudioMode &= 0xFC;
		g_u8SbAudioMode |= 0x02;
		SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
		u8I2cChg |= 0x80;
			LOGD("CEC -> Standby \n");
	}

	return u8I2cChg;
}

int IT6622_CheckEARC(void)
{
	return ite.eARC;
}

int IT6622_GetVersion(void)
{
	int ret = -1;
	ITE_HANDLE *pite = &ite; 
	volatile int *pIteRdyRAM = (int * )SRAM_ADDR_ISP_UPGRADE; // SRAM_ADDR_HDMI_MCU_RDY_FLAG;
	if(((*pIteRdyRAM) &0xffffff00)== ITE_RDY_RAM_FLAG)
	{
		g_u8IteRdy = 1;
		pite->ver = (*pIteRdyRAM)&0xff;
	}
	else
		g_u8IteRdy = 0;

	ret = pite->ver;
	LOGD("ite.ver = %d", ret);
	return ret;
	
}


//****************************************************************************
void SB_Varinit(void){
	g_u8SbRxSel = 0;
	g_u8SbAudioMode = 0;
	g_u8SbTxSta = 0;
	g_u8SbRxEdid = 0;
	g_u8SbPower = SB_POWER_OFF;
	g_u8SbCecMode = SB_CEC_AUTO;
	g_u8SbInfoCA = 0;
	g_u32SbAdoDecInfo = 0;
	g_u8SbVol = 0;
}

static int IT6622_GetStatus(BYTE status)
{
	// SW3 press
	iTE_u8	u8I2cChg = 0;
	iTE_u8	u8I2cInt;
	static BYTE polling_cnt;
	BYTE poll_time = 0;
	ITE_HANDLE *pite = &ite;
	volatile int *pIteRdyRAM = (int * )SRAM_ADDR_ISP_UPGRADE; //SRAM_ADDR_HDMI_MCU_RDY_FLAG;
	//volatile int *pIteHdmiSrcRAM = (int * )SRAM_ADDR_EP_WAKE_UP_FLAG;
	BYTE 	u8EARC;
	BYTE 	u8ARC;
	BYTE ite_ver;
	
	//printf("SB_I2CIRQ\n");

	if(status == HDMI_SYS_STANDBY)
		poll_time = HDMI_STANDBY_POLL_TIME;
	else
		poll_time = HDMI_POLL_TIME;

	if(pite->vol_timeout > 0)
		pite->vol_timeout--;

	if(++polling_cnt < poll_time)
	{
		return 0;
	}
	polling_cnt = 0;
	
	SB_I2C_Read(I2C_SYS_CHANGE, 1, &u8I2cChg);
	SB_I2C_Read(I2C_SYS_INT, 1, &u8I2cInt);
	
	if (u8I2cInt) {
		
		iTE_u8	Data0 = 0;
		//iTE_u8	Data1[3] = {0};
		//static iTE_u8 ucRevID = 0x00;		
			LOGD("I2C Update = %X  %X  %X\n",u8I2cChg , u8I2cInt,g_u8IteRdy);	
        Data0 = 0xff;		
		SB_I2C_Write(I2C_SYS_INT, 1, &Data0);//clear I2C_SYS_INT	
		SB_I2C_Dump();
		
		if ((u8I2cInt & I2C_INT_READY_MASK) == I2C_INT_READY_SET) {
			iTE_u8	u8Ver[9];
			iTE_u8	u8Cnt, u8WrStart;

				LOGD("IT6622 Version:\n");
			SB_I2C_Read(I2C_ITE_CHIP, 9, u8Ver);

			for (u8Cnt = 0; u8Cnt < 9; u8Cnt++) {
					LOGX(" %02X", u8Ver[u8Cnt]);
			}
			LOGD("\n");
								
			if((u8Ver[0] == 0x66) 
				&& (u8Ver[1] == 0x22)){
				

				LOGD("g_u8IteRdy:%d\r\n", g_u8IteRdy);
				switch(g_u8IteRdy){
					case 0:
							g_u8IteRdy = 1 ;
							if(EARC_SB_ADO_CAP_TABLE_SIZE >= 0x80){
								SB_I2C_Write(0x40, 0x80, (iTE_u8*)EARC_SB_ADO_CAP_TABLE) ;
							}else{
								SB_I2C_Write(0x40, EARC_SB_ADO_CAP_TABLE_SIZE, (iTE_u8*)EARC_SB_ADO_CAP_TABLE) ;
								Data0 = 0x00 ;
								for(u8Cnt = EARC_SB_ADO_CAP_TABLE_SIZE; u8Cnt < 0x80; u8Cnt++){
									SB_I2C_Write(0x40 + u8Cnt, 0x01, &Data0) ;
								}
							}
							u8I2cChg = 0x01;
						break ;
					case 1:
							g_u8IteRdy = 2 ;
							if(EARC_SB_ADO_CAP_TABLE_SIZE > 0x80){
								SB_I2C_Write(0x40, (iTE_u8)(EARC_SB_ADO_CAP_TABLE_SIZE - 0x80), (iTE_u8*)&EARC_SB_ADO_CAP_TABLE[0x80]) ;
								u8WrStart = (iTE_u8)(EARC_SB_ADO_CAP_TABLE_SIZE - 0x80) ;
							}else{
								u8WrStart = 0 ;
							}
							Data0 = 0x00 ;
							for(u8Cnt = u8WrStart; u8Cnt < 0x80; u8Cnt++){
								SB_I2C_Write(0x40 + u8Cnt, 0x01, &Data0) ;
							}
							u8I2cChg = 0x03 ;
						break ;
					case 2:
					g_u8IteRdy = 3 ;
					SB_I2C_Write(0x40, 0x03, (iTE_u8 *)CEC_VENDOR_ID); //CEC Vendor ID
					SB_I2C_Write(0x43, 0x0D, (iTE_u8 *)CEC_OSD_STRING); //CEC OSD string
					SB_I2C_Write(0x50, 0x0E, (iTE_u8 *)CEC_OSD_NAME); //CEC OSD Name
					u8I2cChg = 0x05;
					break;
					case 3:
					g_u8IteRdy = 4 ;	
					SB_Varinit() ;
					SB_I2C_Read(I2C_SYS_ADO_MODE, 1, &Data0) ;
					u8I2cChg |= SB_DefaultSetting();
					//if() //DUT POWER ON
					u8I2cChg |= SB_PowerOn();
					//else if()//DUT POWER standby
					//u8I2cChg |= IT66322_SB_Standby();

						SB_I2C_Read(0x03, 1,&ite_ver);
						*pIteRdyRAM = ITE_RDY_RAM_FLAG|ite_ver;
						g_u8IteRdy = 1;

					break;					
				default:
					//g_u8IteRdy = 0;
					break ;
				}

			}
		} 
if((u8I2cInt & I2C_INT_READY_MASK) == I2C_INT_READY_CLR){
	    Data0 = 0xff;
		SB_I2C_Write(I2C_SYS_INT, 1, &Data0);//clear I2C_SYS_INT
		if ((u8I2cInt & I2C_INT_AUDIO_MASK) == I2C_INT_AUDIO_SET) {
			iTE_u8 u8IT66322AdoInfo[5];
			iTE_u32 u32IT66322AdoInfo;
			iTE_u8 u8IT66322CA;
			iTE_u8 u8AudioVolume;
			iTE_u8 u8AudioSrcSta;
			
			SB_I2C_Read(I2C_ADO_SRC_STA, 1, &u8AudioSrcSta);
			u8EARC = (u8AudioSrcSta&I2C_ADO_ACTIVE_EARC_MASK)?1:0;
			u8ARC = (u8AudioSrcSta&I2C_ADO_ACTIVE_ARC_MASK)?1:0;

			if(pite->ARC != u8ARC)
			{
				pite->ARC = u8ARC;
			
				if(u8ARC){
					LOGD("ARC on\n");					
				}
				else{
					LOGD("ARC off\n");
				}
			}
			

			if(u8EARC){
				LOGD("eARC audio out\n");
			}
			else{
				LOGD("eARC audio off\n");
			}
					

			SB_I2C_Read(I2C_ADO_VOL, 1, &u8AudioVolume);
					if(pite->vol_timeout > 0)
					{
						LOGD(">> Vol %d, timeout %d\n", pite->volume, pite->vol_timeout);
						//pite->vol_timeout  = 0;
						SB_I2C_Write(I2C_ADO_VOL, 1, &pite->volume);
						u8I2cChg |= 0x02;
						
					}
					else
					{
						if(pite->volume != u8AudioVolume)
						{
							LOGD("+u8SbVol=%d, u8AudioVolume=%d\n", pite->volume, u8AudioVolume);

							if((pite->volume & I2C_ADO_VOL_VALUE_MASK) != (u8AudioVolume & I2C_ADO_VOL_VALUE_MASK))//volume change
							{
								LOGD("volume change!!!\n");
								pite->volume = u8AudioVolume;
								pite->mute = 0;//change volume unmute
								if((u8AudioVolume&I2C_ADO_VOL_MUTE_MASK)?1:0)//mute
								{
									pite->volume &= I2C_ADO_VOL_VALUE_MASK;//unmute
									LOGD("unmute  %d!!!\n", pite->volume);
									pite->vol_timeout = VOL_TIMEOUT_CNT;
									SB_I2C_Write(I2C_ADO_VOL, 1, &pite->volume);
									u8I2cChg |= 0x02;
								}
							
							}
							else
							{
								pite->volume = u8AudioVolume;
								pite->mute = (u8AudioVolume&I2C_ADO_VOL_MUTE_MASK)?1:0;
								LOGD("mute change %d!!!\n",pite->mute);
							}
			
							LOGD("-u8SbVol=%d, u8AudioVolume=%d, mute %d\n", pite->volume, u8AudioVolume, pite->mute);


							if(g_u8SbPower == SB_POWER_ON)
							{
								HdmiEvt *evt = Q_NEW(HdmiEvt, HDMI_VOL_SIG);
								evt->mute = pite->mute;
								evt->vol = pite->volume&0x7f;
								QACTIVE_POST(UserApp_get(), (QEvt *)evt, UserApp_get());	
							}

						}
					}
					
			SB_I2C_Read(I2C_ADO_INFO, 5, u8IT66322AdoInfo);

			u8IT66322CA = u8IT66322AdoInfo[4];
			u32IT66322AdoInfo = u8IT66322AdoInfo[3];
			u32IT66322AdoInfo <<= 8;
			u32IT66322AdoInfo |= u8IT66322AdoInfo[2];
			u32IT66322AdoInfo <<= 8;
			u32IT66322AdoInfo |= u8IT66322AdoInfo[1];
			u32IT66322AdoInfo <<= 8;
			u32IT66322AdoInfo |= u8IT66322AdoInfo[0];

			//audio_info.audio_dec_info = u32IT66322AdoInfo;
			//audio_info.channel_alloc = u8IT66322CA;

			if ((g_u32SbAdoDecInfo != u32IT66322AdoInfo) ||
					(g_u8SbInfoCA != u8IT66322CA)) {
				LOGD("g_u32SbAdoDecInfo = %lx, u32IT66322AdoInfo = %lx, g_u8SbInfoCA = %X, u8IT66322CA = %X\n", g_u32SbAdoDecInfo, u32IT66322AdoInfo, g_u8SbInfoCA, u8IT66322CA);
				g_u32SbAdoDecInfo = u32IT66322AdoInfo;

				if(g_u32SbAdoDecInfo & ADO_CONF_ACTIVE_MASK)
				{
					pite->audio_type = (g_u32SbAdoDecInfo & ADO_CONF_TYPE_MASK)>>ADO_CONF_TYPE_SHIFT;
								
					if((g_u32SbAdoDecInfo & (ADO_CONF_SEL_MASK)) == (ADO_CONF_SEL_SPDIF))
					{
						if((g_u32SbAdoDecInfo & ADO_CONF_CH_MASK) == ADO_CONF_CH_0)
						{	// FOR ARC SPDIF IN

							LOGD("ARC input \n");
						}

						if(pite->src == CMD_MODE_ARC && g_u8SbPower == SB_POWER_ON)
						{
							QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, HDMI_SPDIF_MODE_SIG), UserApp_get());	
						}
					

						if(pite->src == CMD_MODE_ARC)
							pite->eARC = 0;
				
						
					}
					else
					{	// FOR I2S IN
					
					
						//g_u8TxAdoEncSetting = 0;
						LOGD("I2S input \n");

						if(g_u8SbPower == SB_POWER_ON)
						{
							if( (pite->spk_mode == CecSpk_SB && pite->src == CMD_MODE_ARC ) || pite->src == CMD_MODE_HDMI_1 ||pite->src == CMD_MODE_HDMI_2)
							{
								if(((g_u32SbAdoDecInfo & ADO_CONF_LAYOUT_MASK) == ADO_CONF_LAYOUT_0) && pite->audio_type != ADO_TYPE_HBR)
									QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, HDMI_D0_MODE_SIG), UserApp_get());	
								else if(((g_u32SbAdoDecInfo & ADO_CONF_LAYOUT_MASK) == ADO_CONF_LAYOUT_1) && ((g_u32SbAdoDecInfo & ADO_CONF_CH_MASK) == ADO_CONF_CH_2) && (pite->audio_type == ADO_TYPE_LPCM))
								{
									QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, HDMI_D0_MODE_SIG), UserApp_get());	
								}
								else
									QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, HDMI_D4_MODE_SIG), UserApp_get());
							}
						}

						if(pite->src == CMD_MODE_ARC)
							pite->eARC = 1;
					}		
					
					LOGD("HDMI audio_type = 0x%x, spk_mode %d, layout 0x%x, channel 0x%x\n", pite->audio_type, pite->spk_mode, (g_u32SbAdoDecInfo & ADO_CONF_LAYOUT_MASK), (g_u32SbAdoDecInfo & ADO_CONF_CH_MASK));
					
				}
				
				g_u8SbInfoCA = u8IT66322CA;
			}

			// uevent
			//notify_uevent_user(IT6622_UEVENT_STATE_AUDIO);
		}
}
		if ((u8I2cInt & I2C_INT_SYS_MASK) == I2C_INT_SYS_SET) {
			iTE_u8 u8I2c0xF1[4];
			iTE_u8 u8Temp;

			SB_I2C_Read(I2C_SYS_ADO_MODE, 4, u8I2c0xF1);
			u8Temp = g_u8SbTxSta ^ u8I2c0xF1[1];

			LOGD("g_u8SbTxSta:%d, u8I2c0xF2:%d\n", g_u8SbTxSta, u8I2c0xF1[1]);

			g_u8SbTxSta = u8I2c0xF1[1];
			//system_status.hdmitx_status = g_u8SbTxSta;
			//system_status.cec_cmd_recv1 = u8I2c0xF1[2];
			//system_status.cec_cmd_recv2 = u8I2c0xF1[3];

			if (u8Temp) {
				if (u8Temp & I2C_TV_STA_HPD_MASK) {
					LOGD("TV HPD = %d\n", g_u8SbTxSta & I2C_TV_STA_HPD_MASK);
				}

				if (u8Temp & I2C_TV_STA_CEC_MASK) {
					if ((g_u8SbTxSta & I2C_TV_STA_CEC_MASK) == I2C_TV_STA_CEC_ON) {
						LOGD("TV CEC ON \n");
						if (g_u8SbCecMode == SB_CEC_AUTO) {
							g_u8SbAudioMode |= I2C_MODE_CEC_EN | I2C_MODE_EARC_EN | I2C_MODE_ADO_SYS_EN;// 0x38;//0x20;
							LOGD("SB CEC ON, Audio system On, eARC/ARC Enable \n");
						}
					} else {
						LOGD("TV CEC OFF \n");
						if (g_u8SbCecMode == SB_CEC_AUTO) {
//							g_u8SbAudioMode &= ~(I2C_MODE_CEC_EN | I2C_MODE_EARC_EN | I2C_MODE_ADO_SYS_EN);//~0x38;//0x20;
							LOGD("SB CEC OFF, Audio system OFF, eARC/ARC Disable \n");
						}

					}
					SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
					u8I2cChg |= 0x80;
				}

				if (u8Temp & 0x10) {
					iTE_u8 u8AdoSel;

					if ((g_u8SbAudioMode & I2C_MODE_CEC_MASK) == I2C_MODE_CEC_EN) {
						if ((g_u8SbTxSta & I2C_TV_STA_REQ_ADO_MASK) == I2C_TV_STA_REQ_ADO_SET) {
							LOGD("CEC On switch to TV\n");
							u8AdoSel = 1;
						} else {
							LOGD("CEC On switch to HDMI\n");
							u8AdoSel = 0;
						}
					} else {
						LOGD("CEC Off switch to HDMI\n");
						u8AdoSel = 0;
						I2C_STA_CHANGE(g_u8SbTxSta, I2C_TV_STA_REQ_ADO_MASK, I2C_TV_STA_REQ_ADO_CLR);
					}

					SB_I2C_Write(I2C_ADO_SEL, 1, &u8AdoSel);
					u8I2cChg |= 0x02;
				}
			}
			if(g_u8SbAudioMode & 0x20){
			if (u8I2c0xF1[2]) {
				iTE_u8 u8IteCecReq = u8I2c0xF1[2];
				LOGD("CEC REQ [0x%02X]\n", u8IteCecReq);


				if (u8IteCecReq & 0x40) {
					iTE_u8 u8CecCmd[0x10], u8Cnt;
					SB_I2C_Read(I2C_CEC_LATCH_DATA, 0x10, u8CecCmd);
					LOGD("CEC Command Latch 0x%02X\n", u8CecCmd[1]);

					for (u8Cnt = 0; u8Cnt < 0x10; u8Cnt++) {
						LOGX(" %02X", u8CecCmd[u8Cnt]);
					}
					LOGD("\n");
				}

				if (u8IteCecReq & 0x30) {
					u8I2cChg |= SB_CecPower(1);
				}

				if (u8IteCecReq & 0x01) {
					u8I2cChg |= SB_CecPower(0);
				}

				if (u8IteCecReq & 0x20) {
					u8I2cChg |= SB_EdidChg(1);
				} else if(u8IteCecReq & 0x02) {
					u8I2cChg |= SB_EdidChg(0);
				}

				u8Temp = 0;
				SB_I2C_Write(I2C_SYS_CEC_RECEIVE, 1, &u8Temp);
			}

			if (u8I2c0xF1[3]) {
				iTE_u8 u8CecReq2 = u8I2c0xF1[3];
				LOGD("CEC REQ2 [0x%02X]\n", u8CecReq2);

				if (u8CecReq2 & I2C_CEC_CMD_POWER_MASK) {
					if ((g_u8SbAudioMode & 0x03) == 0x01) {
						u8I2cChg |= SB_CecPower(0);
					} else {
						u8I2cChg |= SB_CecPower(1);
					}
				}

				if (u8CecReq2 & I2C_CEC_CMD_POWER_ON_MASK) {
					u8I2cChg |= SB_CecPower( 1);
				}

				if (u8CecReq2 & I2C_CEC_CMD_POWER_OFF_MASK) {
					u8I2cChg |= SB_CecPower(0);
				}

				u8CecReq2 = 0;
				SB_I2C_Write(I2C_SYS_CEC_RECEIVE2, 1, &u8CecReq2);
			}
			}else{// CEC off
				u8Temp = 0;
				if (u8I2c0xF1[2]) {
					SB_I2C_Write(I2C_SYS_CEC_RECEIVE, 1, &u8Temp);
				}
				if (u8I2c0xF1[3]) {
					SB_I2C_Write(I2C_SYS_CEC_RECEIVE2, 1, &u8Temp);
				}
			}

			//notify_uevent_user(IT6622_UEVENT_STATE_SYSTEM);
		}
		
		SB_SysI2cChange(u8I2cChg);
		SB_I2C_Dump();
	}

	return 0;
}

//#define CMD_EARC_CAP		(0xAA)
static int IT6622_CmdHandle(BYTE u8Cmd, BYTE para1, BYTE para2){

	ITE_HANDLE *pite = &ite;
	para1 = para1;
	para2  =para2;

	if(pite->upg_flag)
		return 0;

	if(g_u8IteRdy == 4){
		iTE_u8	u8I2cChg = 0,u8AdoSel;
		SB_I2C_Read(I2C_SYS_CHANGE, 1, &u8I2cChg);	
			LOGD("SB_HostControl %X\n",u8Cmd);
		switch(u8Cmd)
		{		

				case 	CMD_ARC_SYS_CLOSE:
						LOGD("CMD_ARC_SYS_AUDIO_CLOSE\n");
						g_u8SbAudioMode &= ~0x18; // audio system off & ARC off
						SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
	
						u8I2cChg |= I2C_UPD_SYS_SET;
						break;
						
				case	CMD_POWER_STANDBY:
						LOGD("CMD_POWER_STANDBY\n");
						u8I2cChg |= SB_PowerOff();
						break;
				case	CMD_PASSTHROUGH:
						LOGD("SB Passthrough\n");
						u8I2cChg |= SB_PassThrough();
					break;
				case	CMD_POWER_ON:
						LOGD("SB Power ON\n");
						g_u8SbPower = SB_POWER_ON;

						
					break;
				case	CMD_POWER_OFF:
						LOGD("SB Power OFF \n");
						u8I2cChg |= SB_PowerOff();
					break;
				case	CMD_DUMP_I2C:
//						I2C_Slave_Dump();
					break;
		}

		if(g_u8SbPower == SB_POWER_ON){
					switch(u8Cmd){
						
						case	CMD_VOLUME_MUTE:					// SB Mute/UnMute
	
							if(para1 == 0)  //UnMute
								pite->volume &= ~0x80;
							else
								pite->volume |= 0x80;
							SB_I2C_Write(I2C_ADO_VOL, 1, &pite->volume);
							u8I2cChg |= 0x02;
							if(pite->volume & 0x80){
								LOGD("Mute \n");
							}else{
								LOGD("UnMute \n");
							}
							break;
						case	CMD_VOLUME_UP:					// SB Volume Up
								pite->volume = para2;

						pite->vol_timeout = VOL_TIMEOUT_CNT;
					
						SB_I2C_Write(I2C_ADO_VOL, 1, &pite->volume);
						u8I2cChg |= 0x02;
						LOGD("Vol+ %d\n",pite->volume);
					break;
				case	CMD_VOLUME_DOWN:					// SB Volume Down
				
						pite->volume = para2;

						pite->vol_timeout = VOL_TIMEOUT_CNT;
						
						SB_I2C_Write(I2C_ADO_VOL, 1, &pite->volume);
						u8I2cChg |= 0x02;
						LOGD("Vol- %d\n", pite->volume);
					break;
									
						case	CMD_MODE_ARC:					// SB ARC	
							LOGD("CMD_MODE_ARC_EARC\n");
						u8AdoSel = 1;
						SB_I2C_Write(I2C_ADO_SEL, 1, &u8AdoSel);
						g_u8SbAudioMode = 0x39; 
						SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
						u8I2cChg |= 0x82;
						pite->src = CMD_MODE_ARC;
						     break;	
						case	CMD_MODE_BT:
							LOGD("CMD_MODE_BT\n");
						u8AdoSel = 2;
						SB_I2C_Write(I2C_ADO_SEL, 1, &u8AdoSel);
						g_u8SbAudioMode &= 0xF3; 
						SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
						u8I2cChg |= 0x82;			
						pite->src = CMD_MODE_BT;
							break;					
						default:
							LOGD("NoSupport\n");
//							I2C_Slave_Dump();
						break;
					}
				}

		SB_SysI2cChange(u8I2cChg);
	}

	return 0;
}


HDMI_FUNC ite_it6622 = 
{
	IT6622_Cfg,
	NULL,
	IT6622_GetStatus, //SB_I2cIrq,
	IT6622_CmdHandle, //SB_HostControl,
	IT6622_MCU_UPG,
	IT6622_CheckEARC,
	NULL,
	IT6622_GetVersion,
};


