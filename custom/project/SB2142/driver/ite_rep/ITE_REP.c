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

#define ITE_RDY_RAM_FLAG		0x12568900

#define SWITCH_SRC_DELAY_CNT	(80) // 4s timeout
#define STANDBY_DELAY_CNT	(20)  // 4s timeout

BYTE	g_u8SysModeCnt = 0;
BYTE	g_u8Key = 0;
BYTE	g_u8TxEdidChg = 0;
BYTE	g_u8SupAdoChg = 1;
BYTE	g_u8AdoMode = 1;
BYTE	g_bSbVolUpDown = 0;
UINT32 	g_u32HdmiAdoDecInfo = 0;
BYTE	g_u8SbVolUpDownCnt = 0;
BYTE	g_u8IteCecReq;
BYTE	g_u8IteRdy = 0;
BYTE	g_u8SbPower = 0;
BYTE	g_u8SbCecMode = SB_CEC_AUTO;
BYTE	g_u8ArcRetryDelayCnt = 0;

UINT32 g_u32SbAdoDecInfo = 0;
UINT32 g_u32TxAdoEncConf = 0;
BYTE g_u8TxInfoCA = 0;

#define IgnorAdoSta  (0)//(ADO_CONF_ACTIVE_MASK)

BYTE	g_u8SbRxSel = 0;			// 0xF0
BYTE	g_u8SbAudioMode = 0;		// 0xF1
BYTE	g_u8SbTxSta = 0;			// 0xF2
BYTE	g_u8SbRxEdid = 0; 			// 0xF5

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

//#define SUPPORT_HDMI_AUTO_SWITCH_SRC


#define I2C_CLK 			11
#define I2C_DAT 			10
#define HDMI_INT		46 // ??
#define HDMI_INT2		47
#define HDMI_PWR			2

#define ARC_DET			64
#define HDMI_MUTE		65
#define ITE_ADDR 	0XA8 //0X80

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

static BOOL IT66321_Write(BYTE reg, int len, BYTE *bBuf)
{
	ITE_HANDLE *pite = &ite; 
	BOOL bRet = FALSE;
	int sdRet = -1;


	pite->cfg.delay = 15;
	pite->cfg.ack_delay = 80; 
	
	
	I2c_IO_ReInit(&pite->cfg);

	if(pite->fd >= 0)
	{
		sdRet = AnD_I2CSWWrite(pite->fd, ITE_ADDR, reg, bBuf, len);	
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


static BOOL IT66321_Read( BYTE reg, int len, BYTE *buf)
{
	ITE_HANDLE *pite = &ite;
	int sdRet = -1;
	BOOL bRet = FALSE;


	pite->cfg.delay = 15;
	pite->cfg.ack_delay = 80; 
	

	I2c_IO_ReInit(&pite->cfg);

	if(pite->fd >= 0)
	{
		sdRet = AnD_I2CSWRead( pite->fd, ITE_ADDR, reg, buf, len);	
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

static int IT66321_Cfg(BYTE mode, BYTE src, BYTE vol, BYTE volmax)
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

	if(mode == HDMI_SYS_STANDBY && g_u8IteRdy == 1)
		pite->switch_src_delay = STANDBY_DELAY_CNT;

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

		Pinmux_GPIO_Free(HDMI_PWR);
		Pinmux_GPIO_Alloc(HDMI_PWR,IO_GPIO,IO_RISC);
		GPIO_OE_Set(HDMI_PWR, IO_OUTPUT);
		GPIO_Output_Write(HDMI_PWR, IO_HIGH);
		
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

int ITE66321_CheckEARC(void)
{
	return ite.eARC;
}

int ITE66321_CheckADOType(void)
{
	return ite.audio_type;
}

int ITE66321_GetVersion(void)
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


void I2C_Slave_Dump(void)
{
	UINT16 u16Cnt;
	BYTE	u8I2cData[0x100];
	BYTE*	pu8I2cData;

	for(u16Cnt = 0; u16Cnt < 0x100; u16Cnt+= 0x10){
		IT66321_Read(u16Cnt, 0x10, &u8I2cData[u16Cnt]);
	}
	for(u16Cnt = 0; u16Cnt < 0x100; u16Cnt += 0x10){
		pu8I2cData = &u8I2cData[u16Cnt];
		LOGD("0x%02x: %02x %02x %02x %02x  %02x %02x %02x %02x   %02x %02x %02x %02x  %02x %02x %02x %02x\n", u16Cnt, pu8I2cData[0], pu8I2cData[1],
			pu8I2cData[2], pu8I2cData[3], pu8I2cData[4], pu8I2cData[5], pu8I2cData[6], pu8I2cData[7], pu8I2cData[8], pu8I2cData[9], pu8I2cData[10], pu8I2cData[11],
			pu8I2cData[12], pu8I2cData[13], pu8I2cData[14], pu8I2cData[15]);
	}
	if(1){
		LOGD("SB_Power[%d]\n", (u8I2cData[I2C_SYS_ADO_MODE] & 0x03));
		LOGD("Rx_Sel[%d], AdoDecode_RxSel[%d]\n", (u8I2cData[I2C_SYS_RX_SEL] & 0x0F), (u8I2cData[I2C_SYS_RX_SEL] >> 4));
		LOGD("Tx_DisOut[%d], Tx_AdoMute[%d]\n", (u8I2cData[I2C_SYS_ADO_MODE] & 0x80) >> 7, (u8I2cData[I2C_SYS_ADO_MODE] & 0x40) >> 6);
		LOGD("TV_CEC[%d], SB_CEC[%d]\n", (u8I2cData[I2C_SYS_TX_STA] & 0x02) >> 1, (u8I2cData[I2C_SYS_ADO_MODE] & 0x20) >> 5);
		if(g_u8SbCecMode){
			LOGD("SB CEC Off\n");
		}else{
			LOGD("SB CEC Auto\n");
		}
		LOGD("AudioSystemEnable[%d], eARC/ARC_enable[%d]\n", (u8I2cData[I2C_SYS_ADO_MODE] & 0x08) >> 3, (u8I2cData[I2C_SYS_ADO_MODE] & 0x10) >> 4);
		LOGD("Audio Ready, HDMI[%d], eARC[%d], ARC[%d]\n", ((u8I2cData[I2C_ADO_SRC_STA] & 0x4) >> 2),((u8I2cData[I2C_ADO_SRC_STA] & 0x2) >> 1), (u8I2cData[I2C_ADO_SRC_STA] & 0x1) );
		LOGD("Audio Sourc Sel[%d] \n", u8I2cData[I2C_ADO_SEL]);

		LOGD("SbTxSta = %X, SbAudioMode\n", g_u8SbTxSta, g_u8SbAudioMode);
	}
	LOGD("\n\n\n");
}

void SB_SysI2cChange(BYTE u8I2cChg)
{
	if(u8I2cChg){
		
		IT66321_Write(I2C_SYS_CHANGE, 1, &u8I2cChg);
		LOGD("I2C chg = %x\n", u8I2cChg);
		I2C_Slave_Dump();//Don't remove this line, or volume will be zero
	}
}

BYTE const u8SBSpkAlloc[] = {0x83,	0x6F, 0x0F, 0x0C};
BYTE const u8SBAdb[] = {
#if 0
    0x38,				// Tag=1 (Audio Data Block), Length=27
    //0x2F,
    0x0F, 0x7F, 0x07,	// LPCM : 8-ch, 32~192K
    0x15, 0x07, 0x50,	// AC-3 : 6-ch, 32~48K
    0x35, 0x06, 0x3C,	// AAC	: 6-ch, 44~48K
    0x3E, 0x1E, 0xC0,	// DTS	: 7-ch, 44~96K
    0x4D, 0x02, 0x00,	// DSD	: 6-ch, 44K
    0x57, 0x06, 0x00,	// HBR	: 8-ch, 44~48K (Dolby Digital)
    0x5F, 0x7E, 0x01,	// HBR	: 8-ch, 44~192K (DTS-HD)
    0x67, 0x7E, 0x00,	// HBR	: 8-ch, 44~192K (Dolby TrueHD) MAT
//0xFF, 0x7F, 0x6F,	// LPCM : 16-ch, 32~192K (3D Audio)
#else
	0x2F,
	0x09, 0x0F, 0x07,	//1-0(Linear PCM), up to 2 ch., 32/44.1/48/88.2 kHz, 16/20/24 bits
	0x0F, 0x7F, 0x07,	//1-0(Linear PCM), up to 8 ch., 32/44.1/48/88.2/96/176.4/192 kHz, 16/20/24 bits
	0x67, 0x7E, 0x03,	//12-0(MAT (MLP, Dolby TrueHD)), up to 8 ch., 44.1/48/88.2/96/176.4/192 kHz, Val=0x03 (With Atmos, MAT 2.0 hashing optional)
	0x57, 0x04, 0x03,	//10-0(E-AC-3 (Dolby Digital Plus)), up to 8 ch., 48 kHz, Val=0x03 (With Atmos, With ACMOD 28)
	0x15, 0x07, 0x50,	//2-0(AC-3 (Dolby Digital)), up to 6 ch., 32/44.1/48 kHz, Max bitrate: 640 kbps
	//0x5F, 0x7F, 0x01,	//11-0(DTS-HD), up to 8 ch., 32/44.1/48/88.2/96/176.4/192 kHz, Val=0x01 (With Master Audio, Without DTS:X)
	//0x3E, 0x06, 0xC0,	//7-0(DTS), up to 7 ch., 44.1/48 kHz, Max bitrate: 1536 kbps
	//0x4D, 0x02, 0x00,	//9-0(One Bit Audio), up to 6 ch., 44.1 kHz, Val=0x00
#endif
};

#define SB_ADB_LEN	(sizeof(u8SBAdb))



BYTE	const  u8eArcCapTable[] = {
		0x01,				// Capabilities Data Structure Version = 0x01

		0x01,
		0x14, 		// BLOCK_ID=1, 44-byte
		
		0x2F,
		0x09, 0x0F, 0x07,	//1-0(Linear PCM), up to 2 ch., 32/44.1/48/88.2 kHz, 16/20/24 bits
		0x0F, 0x7F, 0x07,	//1-0(Linear PCM), up to 8 ch., 32/44.1/48/88.2/96/176.4/192 kHz, 16/20/24 bits
		0x67, 0x7E, 0x03,	//12-0(MAT (MLP, Dolby TrueHD)), up to 8 ch., 44.1/48/88.2/96/176.4/192 kHz, Val=0x03 (With Atmos, MAT 2.0 hashing optional)
		0x57, 0x04, 0x03,	//10-0(E-AC-3 (Dolby Digital Plus)), up to 8 ch., 48 kHz, Val=0x03 (With Atmos, With ACMOD 28)
		0x15, 0x07, 0x50,	//2-0(AC-3 (Dolby Digital)), up to 6 ch., 32/44.1/48 kHz, Max bitrate: 640 kbps
							//0x20 offset
		0x83,				// Tag=4 (Speaker Allocation Data Block), 3-bye
		0x6F, 0x0F, 0x0C,
		

};
#define EARC_CAP_TABLE_SIZE		sizeof(u8eArcCapTable)
//#if (EARC_CAP_TABLE_SIZE > 0x100)
//#pragma message("EARC_CAP_TABLE_SIZE > 0x100")
//#endif
BYTE	const  u8CecVendorId[3] = {0x00, 0x00, 0x01};
BYTE	const  u8CecOsdName[14] = "SB2142";



//****************************************************************************
void SB_I2cWriteEarcCap(BYTE u8Bank)
{
#if 1
	#define EARC_CAP_BANK_SIZE		(0x80)
	BYTE	u8eArcCap[EARC_CAP_BANK_SIZE];
	BYTE	u8Cnt, u8Temp;
	BYTE*	pu8Ptr = u8eArcCap;
	if(u8Bank == 0){
		if(EARC_CAP_TABLE_SIZE > EARC_CAP_BANK_SIZE){
			u8Cnt = EARC_CAP_BANK_SIZE;
		}else{
			u8Cnt = EARC_CAP_TABLE_SIZE;
		}
		IT66321_Write(0x40, u8Cnt, (BYTE*)&u8eArcCapTable[0]);
	}else{
		if(EARC_CAP_TABLE_SIZE > EARC_CAP_BANK_SIZE){
			if(EARC_CAP_TABLE_SIZE > 0x100){
				u8Cnt = EARC_CAP_BANK_SIZE;
				LOGD("ERROR, Wrong eARC capability size %d\n", EARC_CAP_TABLE_SIZE);
			}else{
				u8Cnt = (BYTE)EARC_CAP_TABLE_SIZE - EARC_CAP_BANK_SIZE;
			}
			IT66321_Write(0x40, u8Cnt, (BYTE*)&u8eArcCapTable[EARC_CAP_BANK_SIZE]);
		}else{
			u8Cnt = 0;
		}
	}
	u8Temp = u8Cnt;
	for(;u8Temp < EARC_CAP_BANK_SIZE; u8Temp++){
		*pu8Ptr++ = 0;
	}
	if(u8Cnt < EARC_CAP_BANK_SIZE){
		IT66321_Write(0x40 + u8Cnt, EARC_CAP_BANK_SIZE - u8Cnt, u8eArcCap);
	}
#endif
}

//#define SUPPORT_CLOSE_RX

//****************************************************************************

//****************************************************************************



BYTE SB_CecSource(BYTE src)
{
	ITE_HANDLE *pite = &ite;
	BYTE u8I2cChg = 0;
	

#ifdef SUPPORT_CLOSE_RX
	BYTE sys_mode;
	if(pite->src == CMD_MODE_HDMI_1 && src != CMD_MODE_HDMI_1)
	{
		// HDMI1 -> OTHER source
		IT66321_Read(I2C_SYS_RX_MODE_0, 1, &sys_mode);
		sys_mode &= ~(I2C_RX0_ACTIVE_FORCE);//u8RxMode;
		IT66321_Write(I2C_SYS_RX_MODE_0, 1, &sys_mode);
	}

	if(pite->src == CMD_MODE_HDMI_2 && src != CMD_MODE_HDMI_2)
	{
		// HDMI2 -> OTHER source
		IT66321_Read(I2C_SYS_RX_MODE_1, 1, &sys_mode);
		sys_mode &= ~(I2C_RX1_ACTIVE_FORCE);//u8RxMode;
		IT66321_Write(I2C_SYS_RX_MODE_1, 1, &sys_mode);
	}

#endif
	BYTE u8Temp = 0;
	IT66321_Write(I2C_SYS_CEC_RECEIVE, 1, &u8Temp);

	pite->switch_src_delay = SWITCH_SRC_DELAY_CNT;
	
	switch(src)
	{
		case CMD_MODE_HDMI_1:					// SB HDMI 1
			LOGD("ITE HDMI 1 \n");
			pite->src = CMD_MODE_HDMI_1;
			pite->spk_mode = CecSpk_SB;

#ifdef SUPPORT_CLOSE_RX
			IT66321_Read(I2C_SYS_RX_MODE_0, 1, &sys_mode);
			sys_mode |= I2C_RX0_ACTIVE_FORCE;//u8RxMode;
			IT66321_Write(I2C_SYS_RX_MODE_0, 1, &sys_mode);
#endif
			// select HDMI port {{ 0XF0 /\ bit 1:0 port [0 : HDMI_0  1: HDMI_1  2:disable]  /\   bit 5:4 audio deocde port [0 : HDMI_0  1: HDMI_1  2:disable]}}
			g_u8SbRxSel = 0x00;
			IT66321_Write(I2C_SYS_RX_SEL, 1, &g_u8SbRxSel);

			/*
				bit		func
				1 : 0		iTE Power Down/On;  00: Power Down, 01: Power On; 10: Power Standby; 11: Reserved
				2		Reserved
				3		Audio System Off/On; 0: Audio System Off, 1: Audio System On
				4		eARC/ARC Enable; 0: eARC/ARC disable, 1: eARC/ARC Enable
				5		SB Cec On/Off; 0: CEC Off, 1: CEC On
				6		Tx Audio Mute; 0: HDMI Tx output with audio, 1: HDMI Tx output without audio
				7		Tx Disable output; 0: HDMI Tx enable output, 1: HDMI Tx Disable output 
				
			*/
			//if(g_u8SbAudioMode & 0x80)
			{
				g_u8SbAudioMode &= 0x7F; // enable HDMI ouput

				g_u8SbAudioMode |= 0x08; // audio system on
				
				IT66321_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
			}

				


			// select HDMI input {{ 0X1C, 0:HDMI  1:ARC }}
			u8I2cChg |= 0x80;
			if(1){
				BYTE	u8AdoSel = 0;
				IT66321_Write(I2C_ADO_SEL, 1, &u8AdoSel);
				u8I2cChg |= 0x02;
			}
			break;
			
		case CMD_MODE_HDMI_2:					// SB HDMI 2
			LOGD("ITE HDMI 2 \n");
			pite->src = CMD_MODE_HDMI_2;
			pite->spk_mode = CecSpk_SB;
#ifdef SUPPORT_CLOSE_RX
			IT66321_Read(I2C_SYS_RX_MODE_1, 1, &sys_mode);
			sys_mode |= I2C_RX1_ACTIVE_FORCE;//u8RxMode;
			IT66321_Write(I2C_SYS_RX_MODE_1, 1, &sys_mode);
#endif
			// select HDMI port {{ 0XF0 /\ bit 1:0 port [0 : HDMI_0  1: HDMI_1  2:disable]  /\   bit 5:4 audio deocde port [0 : HDMI_0  1: HDMI_1  2:disable]}}
			g_u8SbRxSel = 0x11;
			IT66321_Write(I2C_SYS_RX_SEL, 1, &g_u8SbRxSel);

			
			//if(g_u8SbAudioMode & 0x80)
			{
				g_u8SbAudioMode &= 0x7F; // enable HDMI ouput

				g_u8SbAudioMode |= 0x08; // audio system on
				
				IT66321_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
			}

			// select HDMI input {{ 0X1C, 0:HDMI  1:ARC  2:Ext I2S }}
			u8I2cChg |= 0x80;
			if(1){
				BYTE	u8AdoSel = 0;
				IT66321_Write(I2C_ADO_SEL, 1, &u8AdoSel);
				u8I2cChg |= 0x02;
			}
			break;
			
		case CMD_MODE_ARC:					// SB ARC

			LOGD("ITE ARC \n");
			pite->src = CMD_MODE_ARC;
			pite->spk_mode = CecSpk_SB;

			// select HDMI port {{ 0XF0 /\ bit 1:0 port [0 : HDMI_0  1: HDMI_1  2:disable]  /\   bit 5:4 audio deocde port [0 : HDMI_0  1: HDMI_1  2:disable]}}
			//g_u8SbRxSel = 0x20;
			//IT66321_Write(I2C_SYS_RX_SEL, 1, &g_u8SbRxSel);
	

			g_u8SbAudioMode |= 0x38; // enable CEC / ARC /AUDIO SYS on
			g_u8SbAudioMode |= 0x80; // disable HDMI ouput
			IT66321_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
			u8I2cChg |= 0x80;

			// select ARC input {{ 0X1C, 0:HDMI  1:ARC  2:Ext I2S }}
			if(1){
				BYTE	u8AdoSel = 1;
				IT66321_Write(I2C_ADO_SEL, 1, &u8AdoSel);
				u8I2cChg |= 0x02;
			}
			
			break;
			
		case CMD_MODE_BT:
			LOGD("ITE Other/BT Mode \n");
			pite->src = CMD_MODE_BT;

			//g_u8SbRxSel = 0x20;
			//IT66321_Write(I2C_SYS_RX_SEL, 1, &g_u8SbRxSel);
		
			g_u8SbAudioMode |= 0x80;  // disable HDMI ouput

			g_u8SbAudioMode &= ~0x18; // audio system off & ARC off
			//g_u8SbAudioMode &= ~0x10;
			
			IT66321_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
			u8I2cChg |= 0x80;

			// select Ext I2S input {{ 0X1C, 0:HDMI  1:ARC  2:Ext I2S }}
			if(1){
				BYTE	u8AdoSel = 2;
				IT66321_Write(I2C_ADO_SEL, 1, &u8AdoSel);
				u8I2cChg |= 0x02;
				
			}

			break;
			
		default:
			break;
	}

	LOGD("%s : g_u8SbRxSel 0x%x, g_u8SbAudioMode 0x%x\n", __func__, g_u8SbRxSel, g_u8SbAudioMode);

	return u8I2cChg;
}


BYTE SB_DefaultSetting(void)
{
	BYTE	u8I2cReg[3];
	BYTE	u8RxMode;
	BYTE	u8I2cChg = 0;
	LOGD("SB_DefaultSetting \n");

	ITE_HANDLE *pite = &ite; 

	u8I2cReg[0] = pite->volume;
	IT66321_Write(I2C_ADO_VOL, 1, u8I2cReg);
	u8I2cReg[0] = pite->volmax;
	IT66321_Write(I2C_VOLUME_MAX, 1, u8I2cReg);
	u8I2cChg |= 2;

	u8I2cReg[0] = 8;
	IT66321_Write(I2C_EDID_UPDATE, 1, u8I2cReg);
	IT66321_Write(I2C_EDID_SPK_ALLOC, 4, (BYTE *)u8SBSpkAlloc);
	IT66321_Write(I2C_EDID_ADB0, SB_ADB_LEN, (BYTE *)u8SBAdb);
	//IT66321_Write(I2C_EDID_ADB0, 31, (BYTE *)u8SBAdb);
	u8I2cChg |= 8;

	g_u8SbRxSel = 00;
	IT66321_Write(I2C_SYS_RX_SEL, 1, &g_u8SbRxSel);
	//u8I2cChg |= SB_CecSource(src);

	u8RxMode = 0x85;
	IT66321_Write(I2C_SYS_RX_MODE_0, 1, &u8RxMode);
	IT66321_Write(I2C_SYS_RX_MODE_1, 1, &u8RxMode);


//	g_u8SbAudioMode = 0x20;
//	SB_I2C_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);

	u8RxMode = 0;
	IT66321_Write(I2C_SYS_CEC_TRANS_CNT, 1, &u8RxMode);
	u8RxMode = 0xA0; //eVendorCommandWithID
	IT66321_Write(I2C_SYS_CEC_LATCH_CNT, 1, &u8RxMode);

	u8I2cChg |= 0xC0;
	return u8I2cChg;
}
//****************************************************************************
BYTE SB_PowerOff(void)
{
	BYTE	u8I2cChg;
	LOGD("SB_PowerOff \n");
	g_u8SbPower = 0;
	u8I2cChg = SB_DefaultSetting();
	// 00: power down mode
	g_u8SbAudioMode &= 0xFC;
	IT66321_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
	u8I2cChg |= 0x80;
	return u8I2cChg;
}
//****************************************************************************
BYTE SB_PowerOn(BYTE src)
{
	BYTE	u8I2cChg = 0;
	BYTE 	u8RxMode;
	LOGD("SB_PowerOn \n");
	g_u8SbPower = SB_POWER_ON;
	// Enable HDMI audio decode

/*	
	switch(g_u8SbRxSel & 0x0F){
		case 	0:	g_u8SbRxSel = I2C_HDMI_SELECT_R0 | I2C_HDMI_ADO_SEL_R0;	break;
		case	1:	g_u8SbRxSel = I2C_HDMI_SELECT_R1 | I2C_HDMI_ADO_SEL_R1;	break;
		default:	g_u8SbRxSel = I2C_HDMI_SELECT_R0 | I2C_HDMI_ADO_SEL_R0;	break;
	}
//	g_u8SbRxSel &= 0xF;
//	g_u8SbRxSel |= g_u8SbRxSel << 4;
	IT66321_Write(I2C_SYS_RX_SEL, 1, &g_u8SbRxSel);
	// 1: power on mode
	g_u8SbAudioMode = 0x39;//0x41//0x61;
	IT66321_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
*/
	// 1: power on mode
	g_u8SbAudioMode = 0x39;
	u8I2cChg |= SB_CecSource(src);
	
	// SB CEC auto mode
	g_u8SbCecMode = SB_CEC_AUTO;
	// Rx Force ON & using SB ADB
	u8RxMode = I2C_RX0_ACTIVE_FORCE | I2C_RX0_HDCP_RPT_SET | I2C_RX0_HDCP_VER_23 | I2C_RX0_EDID_SB;//u8RxMode;
	IT66321_Write(I2C_SYS_RX_MODE_0, 1, &u8RxMode);
	u8RxMode = I2C_RX1_ACTIVE_FORCE | I2C_RX1_HDCP_RPT_SET | I2C_RX1_HDCP_VER_23 | I2C_RX1_EDID_SB;//u8RxMode;
	IT66321_Write(I2C_SYS_RX_MODE_1, 1, &u8RxMode);

	u8I2cChg |= I2C_UPD_SYS_SET;

	return u8I2cChg;
}
//****************************************************************************
BYTE SB_Standby(void)
{
	BYTE	u8RxMode;
	BYTE	u8I2cChg = 0;
	
	g_u8SbPower = SB_STANDBY;
	// 10: standby mode
	I2C_STA_CHANGE(g_u8SbAudioMode, I2C_MODE_POWER_MASK | I2C_MODE_CEC_MASK, I2C_MODE_POWER_STANDBY | I2C_MODE_CEC_EN);
//	g_u8SbAudioMode &= 0xFC;
//	g_u8SbAudioMode |= 0x22;
	IT66321_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
	u8I2cChg |= I2C_UPD_SYS_SET;


//	g_u8SbRxSel = 0x11;
//	IT66321_Write(I2C_SYS_RX_SEL, 1, &g_u8SbRxSel);


	LOGD("SB_Standby 0x%x\n", g_u8SbAudioMode);

	u8RxMode = I2C_RX0_ACTIVE_FORCE | I2C_RX0_HDCP_RPT_SET | I2C_RX0_HDCP_VER_23 | I2C_RX0_EDID_SB;//u8RxMode;
//	SB_I2C_Write(I2C_SYS_RX_MODE_0, 1, &u8RxMode);
	u8RxMode = I2C_RX1_ACTIVE_FORCE | I2C_RX1_HDCP_RPT_SET | I2C_RX1_HDCP_VER_23 | I2C_RX1_EDID_SB;//u8RxMode;
//	SB_I2C_Write(I2C_SYS_RX_MODE_1, 1, &u8RxMode);

	return u8I2cChg;
}
//****************************************************************************
BYTE SB_PassThrough(void)
{
	BYTE	u8RxMode;
	LOGD("SB_PassThrough \n");
	g_u8SbPower = SB_PASS_THROUGH;
	// Disable HDMI Audio decode
//	g_u8SbRxSel |= 0xF0;
	I2C_STA_CHANGE(g_u8SbRxSel, I2C_HDMI_ADO_SEL_MASK, 	I2C_HDMI_ADO_SEL_NONE);
	IT66321_Write(I2C_SYS_RX_SEL, 1, &g_u8SbRxSel);
	// Power On, Audio System Off, eARC/ARC Off, SB CEC Off, Tx Output & audio out.
	g_u8SbCecMode = SB_CEC_OFF;
	g_u8SbAudioMode = I2C_MODE_TX_OUT_EN | I2C_MODE_TX_ADO_EN | I2C_MODE_CEC_DIS | I2C_MODE_EARC_DIS | I2C_MODE_ADO_SYS_DIS | I2C_MODE_POWER_ON;// 0x01;
	IT66321_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
	// Rx active when Tx select to.
//	u8RxMode = 0x00;
	u8RxMode = I2C_RX0_ACTIVE_BY_SEL | I2C_RX0_HDCP_RPT_CLR | I2C_RX0_HDCP_VER_AUTO | I2C_RX0_EDID_TV;//u8RxMode;
	IT66321_Write(I2C_SYS_RX_MODE_0, 1, &u8RxMode);
	u8RxMode = I2C_RX1_ACTIVE_BY_SEL | I2C_RX1_HDCP_RPT_CLR | I2C_RX1_HDCP_VER_AUTO | I2C_RX1_EDID_TV;
	IT66321_Write(I2C_SYS_RX_MODE_1, 1, &u8RxMode);
	return I2C_UPD_SYS_SET;
}
//****************************************************************************
BYTE SB_EdidChg(BYTE u8EdidChg)
{
	BYTE u8I2cChg = 0;
	BYTE	u8EdidMode[2];

	IT66321_Read(I2C_SYS_RX_MODE_0, 2, u8EdidMode);

	if(u8EdidChg != (u8EdidMode[0] & 0x01)){
		u8EdidMode[0] &= 0xFE;
		u8EdidMode[0] |= u8EdidChg;
		u8I2cChg = 0x80;
	}
	if(u8EdidChg != (u8EdidMode[1] & 0x01)){
		u8EdidMode[1] &= 0xFE;
		u8EdidMode[1] |= u8EdidChg;
		u8I2cChg = 0x80;
	}
	if(u8I2cChg){
		IT66321_Write(I2C_SYS_RX_MODE_0, 2, u8EdidMode);
		IT66321_Write(I2C_ADO_SEL, 1, &u8EdidChg);
		u8I2cChg |= 0x02;
	}
	return u8I2cChg;
}
//****************************************************************************

BYTE SB_CecPower(BYTE bEn)
{
	BYTE	u8I2cChg = 0;
	if(bEn){

		if((g_u8SbAudioMode & 0x03) != 0x01){
			g_u8SbAudioMode &= 0xFC;
			g_u8SbAudioMode |= 0x01;
			IT66321_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
			u8I2cChg |= 0x80;
			LOGD("CEC -> Power On \n");
		}
		
	}else{

		g_u8SbAudioMode &= 0xFC;
		g_u8SbAudioMode |= 0x02;
		IT66321_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
		u8I2cChg |= 0x80;
		LOGD("CEC -> Standby \n");
	}
	return u8I2cChg;
}
//**************************************************************************
int CECSwitchSource(BYTE hdmiTXStatus, BYTE* u8I2cChg, int* src_source)
{
	BYTE	u8Temp;
	
	LOGD("Enter CECSwitchSource 0x%x\n", hdmiTXStatus);
	if(hdmiTXStatus & 0x10)//from TV
	{
		
		if((hdmiTXStatus & 0x0C) == 0x0C)//TV source
		{
			*src_source	= ARC_SRC_SIG;
			LOGD("CEC switch source to ARC!\n");
		}
		
	}
	else
	{
		
		// 0xF2[2:3] CEC request switch to; 00: Rx0, 01: Rx1, 10: reserved, 11: TV's Source
		if((hdmiTXStatus & 0x0C) == 0 || (hdmiTXStatus & 0x0C) == 0x04)
		{
			switch(hdmiTXStatus & 0x0C)
			{
				case 0x00:
					u8Temp = 0x00;  // HDMI 1
					*src_source  = HDMI1_SRC_SIG;
					break;
				case 0x04:
					u8Temp = 0x11;  // HDMI 2
					*src_source  = HDMI2_SRC_SIG;
					break;
				default:
					break;
			}

/* {{ add for ACC in wakup, detect the HDMI source is connected }} */
			BYTE rx_status;
			IT66321_Read(I2C_SYS_RX_STATUS, 1, &rx_status);

			if(*src_source == HDMI1_SRC_SIG && (rx_status & I2C_R0_STA_MASK) == I2C_R0_STA_NO)
			{
				*src_source = 0;
				LOGD(" HDMI 1 is no connect, no need wakup\n");
				return 0;
			}
			if(*src_source == HDMI2_SRC_SIG && (rx_status & I2C_R1_STA_MASK) == I2C_R1_STA_NO)
			{
				*src_source = 0;
				LOGD(" HDMI 2 is no connect, no need wakup\n");
				return 0;
			}

			
			g_u8SbRxSel = u8Temp;
			IT66321_Write(I2C_SYS_RX_SEL, 1, &u8Temp);
			if(g_u8SbAudioMode & 0x80)
			{
				g_u8SbAudioMode &= 0x7F;
				IT66321_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
			}
			*u8I2cChg |= 0x80;
			
			LOGD("CEC Routing Change to R%d\n", g_u8SbRxSel & 0xF);
		}

	}

	return 0;
}

//****************************************************************************


int ITE66321_GetStatus(BYTE status)
{			
	ITE_HANDLE *pite = &ite;
	volatile int *pIteRdyRAM = (int * )SRAM_ADDR_ISP_UPGRADE; //SRAM_ADDR_HDMI_MCU_RDY_FLAG;
	volatile int *pIteHdmiSrcRAM = (int * )SRAM_ADDR_EP_WAKE_UP_FLAG;
	int res = 0;
	BYTE sys_status[2];
	BYTE	u8I2cChg = 0;
	BYTE	u8I2cInt;
	static BYTE polling_cnt;
	BYTE poll_time = 0;
	int 	src_sw_sig = 0;
	BYTE power_flag = CEC_POWER_NULL;  

	if(pite->upg_flag)
		return 0;

	if(pite->sys_audio_time > 0)
		pite->sys_audio_time--;

	if(status == HDMI_SYS_STANDBY)
		poll_time = HDMI_STANDBY_POLL_TIME;
	else
		poll_time = HDMI_POLL_TIME;

	if(pite->hpd_in_cnt  > 0)
	{
		pite->hpd_in_cnt --;
		if(pite->hpd_in_cnt == 0)
			LOGD("hpd_in_cnt  == 0\n");
	}

	if(pite->switch_src_delay > 0)
	{
		pite->switch_src_delay--;
		if(pite->switch_src_delay%10 == 0)
			LOGD("switch_src_delay %d\n", pite->switch_src_delay);
	}
	
	if(pite->vol_timeout > 0)
		pite->vol_timeout--;

	if(++polling_cnt < poll_time)
	{
		return 0;
	}
	polling_cnt = 0;
	
	
	if(IT66321_Read(I2C_SYS_CHANGE, 2, sys_status) == true)
	{
		//IT66321_Read(I2C_SYS_INT, 1, &u8I2cInt);

		u8I2cChg = sys_status[0];
		u8I2cInt = sys_status[1];
		
		if(u8I2cInt){

			LOGD("I2C Update = %x\n", u8I2cInt);
			
			if((u8I2cInt & I2C_INT_READY_MASK) == I2C_INT_READY_SET){

			
				u8I2cInt &= 0xFE;
				IT66321_Write(I2C_SYS_INT, 1, &u8I2cInt);
				if(u8I2cChg == 0){
					SB_I2cWriteEarcCap(0);
					u8I2cChg = 0x81;
				}else if(u8I2cChg == 1){
					SB_I2cWriteEarcCap(1);
					u8I2cChg = 0x83;
				}else if(u8I2cChg == 3){
					IT66321_Write(0x40, 0x03, (BYTE*)u8CecVendorId);
					IT66321_Write(0x50, 0x14, (BYTE*)u8CecOsdName);
					u8I2cChg = 0x85;
				}else{

					BYTE	u8Ver[9];
					u8I2cChg = 0;

					LOGD("IT66322 Version:\n");
					IT66321_Read(I2C_ITE_CHIP, 9, u8Ver);
					LOGD(" %02x %02x %02x %02x %02x %02x %02x %02x %02x\n", u8Ver[0], u8Ver[1], u8Ver[2], u8Ver[3], u8Ver[4], u8Ver[5], u8Ver[6], u8Ver[7], u8Ver[8]);
					if((u8Ver[0] == IT66322_ID_0) && (u8Ver[1] == IT66322_ID_1) && (u8Ver[2] == IT66322_ID_2)){

						
						*pIteRdyRAM = ITE_RDY_RAM_FLAG|u8Ver[3];
						g_u8IteRdy = 1;
						
						u8I2cChg |= SB_DefaultSetting();

						if(status == HDMI_SYS_STANDBY)
							u8I2cChg |= SB_Standby();
						else 
							u8I2cChg |= SB_PowerOn(pite->src);//SB_Standby();
					}

					LOGD("I2C chg = %X\n", u8I2cChg);

				}
			}
			else
			{
							BYTE	Data0 = 0;
				IT66321_Write(I2C_SYS_INT, 1, &Data0);
	  
	// DDP      u32IT66322AdoInfo = 400e8e2, g_u8TxInfoCA = 0, u8IT66322CA = 0  1110 1000 1110 0010
	// TrueHD  u32IT66322AdoInfo = 400f092, g_u8TxInfoCA = 0, u8IT66322CA = 0  1111 0000 1001 0010	

	
			
			if((u8I2cInt & I2C_INT_SYS_MASK) == I2C_INT_SYS_SET){
				BYTE	u8I2c0xF1[5];
				BYTE	u8Temp;
				
				/* 
					[0]  I2C_SYS_ADO_MODE  
					[1]  I2C_SYS_TX_STA  
					[2]  I2C_SYS_CEC_RECEIVE  
					[3]  I2C_SYS_CEC_RECEIVE2  

				*/
				IT66321_Read(I2C_SYS_ADO_MODE, 5, u8I2c0xF1);
				u8Temp = g_u8SbTxSta ^ u8I2c0xF1[1];
				g_u8SbTxSta = u8I2c0xF1[1];

				LOGD(" sys mask 0x%x-0x%x-0x%x-0x%x-0x%x\n ", u8I2c0xF1[0], u8I2c0xF1[1], u8I2c0xF1[2], u8I2c0xF1[3], u8I2c0xF1[4]);

				if(u8Temp && u8I2c0xF1[3] != 0xff){
					if(u8Temp & I2C_TV_STA_HPD_MASK){
						LOGD("TV HPD = %d\n", g_u8SbTxSta & I2C_TV_STA_HPD_MASK);

						pite->hpd_in =  g_u8SbTxSta & I2C_TV_STA_HPD_MASK;

						if(pite->hpd_in == 1)
							pite->hpd_in_cnt = 45;  // 2.8s =  45*60ms
						else
							pite->hpd_in_cnt = 0;

						if(g_u8SbPower == SB_POWER_ON)
						{
							HdmiEvt *evt = Q_NEW(HdmiEvt, HDMI_HPD_SIG);
							evt->vol =  pite->hpd_in;
							QACTIVE_POST(UserApp_get(), (QEvt *)evt, UserApp_get());	
						}

					}
					if(u8Temp & I2C_TV_STA_CEC_MASK)
					{
					#if 0
						if((g_u8SbTxSta & I2C_TV_STA_CEC_MASK) == I2C_TV_STA_CEC_ON){
							LOGD("TV CEC ON \n");
							if(g_u8SbCecMode == SB_CEC_AUTO){
								g_u8SbAudioMode |= I2C_MODE_CEC_EN | I2C_MODE_EARC_EN | I2C_MODE_ADO_SYS_EN;// 0x38;//0x20;
								LOGD("SB CEC ON, Audio system On, eARC/ARC Enable \n");
							}
						}else{
							LOGD("TV CEC OFF \n");
							if(g_u8SbCecMode == SB_CEC_AUTO){
								//g_u8SbAudioMode &= ~(I2C_MODE_CEC_EN | I2C_MODE_EARC_EN | I2C_MODE_ADO_SYS_EN);//~0x38;//0x20;
								LOGD("SB CEC OFF, Audio system OFF, eARC/ARC Disable \n");
							}

						}
						IT66321_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
						u8I2cChg |= 0x80;
					#endif
					}
					if(u8Temp & 0x10){
						
						
			#if 0//def SUPPORT_HDMI_AUTO_SWITCH_SRC
						BYTE	u8AdoSel;
						if((g_u8SbAudioMode & I2C_MODE_CEC_MASK) == I2C_MODE_CEC_EN){
			
							if((g_u8SbTxSta & I2C_TV_STA_REQ_ADO_MASK) == I2C_TV_STA_REQ_ADO_SET){
								LOGD("CEC On switch to TV\n");
								u8AdoSel = 1;
								src_sw_sig = ARC_SRC_SIG;
							}else{
								LOGD("CEC On switch to HDMI\n");
								u8AdoSel = 0;
								if(g_u8SbRxSel == 0)
									src_sw_sig  = HDMI1_SRC_SIG;
								else
									src_sw_sig  = HDMI2_SRC_SIG;
							}
						}else{
							LOGD("CEC Off switch to HDMI\n");
							u8AdoSel = 0;
							STA_CHANGE(g_u8SbTxSta, I2C_TV_STA_REQ_ADO_MASK, I2C_TV_STA_REQ_ADO_CLR);
						}

						IT66321_Write(I2C_ADO_SEL, 1, &u8AdoSel);
						u8I2cChg |= 0x02;
			#endif
					}
				}

				if(u8I2c0xF1[2])
				{
					g_u8IteCecReq = u8I2c0xF1[2];
					LOGD("CEC REQ [0x%02x], TX_STA [0x%x]\n", g_u8IteCecReq, u8I2c0xF1[1] );

					if(g_u8IteCecReq & I2C_CEC_CMD_SYS_ADO_OFF_SET)//SAMR 0
					{
						pite->spk_mode = CecSpk_TV;
						
						LOGD("CEC send sys audio mode off cmd\n");

						if((pite->src == CMD_MODE_HDMI_1) || (pite->src == CMD_MODE_HDMI_2))
						{

							HdmiEvt *evt = Q_NEW(HdmiEvt, HDMI_VOL_SIG);
							evt->mute = 1;//mute
							evt->vol = pite->volume&0x7f;
							QACTIVE_POST(UserApp_get(), (QEvt *)evt, UserApp_get());	
						}

						u8I2cChg |= SB_EdidChg(0); // user TV EDID
					}
					else if(g_u8IteCecReq & I2C_CEC_CMD_SYS_ADO_ON_SET)//audio mode on
					{
						LOGD("CEC sys audio mode on cmd status %d, spk %d, src %d\n", status, pite->spk_mode, pite->src);
						if(status == HDMI_SYS_STANDBY)
						{	
							if(pite->switch_src_delay > 0)
							{
								LOGD(" CEC sys audio mode on cmd  .... power off doing, not wakeup \n");
								g_u8SbAudioMode &= ~0x18; // audio system off & ARC off
								IT66321_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
								u8I2cChg |= I2C_UPD_SYS_SET;
							}
							else
							{
								power_flag = CEC_POWER_ON;
								src_sw_sig = ARC_SRC_SIG;
	
								LOGD("CEC sys audio mode on cmd, power on\n");
							}
						}
						else
						{	
							if(pite->src == CMD_MODE_BT) // && pite->spk_mode == CecSpk_TV)
							{
								src_sw_sig = ARC_SRC_SIG;
								LOGD("CEC sys audio mode on cmd, ARC\n");
							}

							if((pite->src == CMD_MODE_HDMI_1) || (pite->src == CMD_MODE_HDMI_2))
							{

								HdmiEvt *evt = Q_NEW(HdmiEvt, HDMI_VOL_SIG);
								evt->mute = 0;//unmute
								evt->vol = pite->volume&0x7f;
								QACTIVE_POST(UserApp_get(), (QEvt *)evt, UserApp_get());	
							}

							pite->spk_mode = CecSpk_SB;

							u8I2cChg |= SB_EdidChg(1); // user SB EDID
						}

					}
					else
					{		
						if(g_u8IteCecReq & 0x80)//Routing change
						{
							LOGD(" Routing change to \n");

							if(pite->hpd_in_cnt > 0 && pite->spk_mode == CecSpk_TV)
							{
								LOGD("TV spk get active source, hpd_in_cnt %d\n", pite->hpd_in_cnt);// Tv switch to TV spk will plugout then plugin HPD, lead to DVD send active source and sb switch to ARC
							}
							else if(pite->switch_src_delay > 0)
							{
								LOGD(" \n\n\n\n\n switch source timeout .... no switch source \n\n\n\n\n\n");
							}
							else
							{
								#ifdef SUPPORT_HDMI_AUTO_SWITCH_SRC
								CECSwitchSource(u8I2c0xF1[1], &u8I2cChg, &src_sw_sig);
								
								if(status == HDMI_SYS_STANDBY)
								{
									if(src_sw_sig == HDMI1_SRC_SIG || src_sw_sig == HDMI2_SRC_SIG)
										power_flag = CEC_POWER_ON;
								}
								#endif
								
							}
						}
							
						if(g_u8IteCecReq & I2C_CEC_CMD_ACTIVE_SRC_SET)//active source
						{
							LOGD("Active source switch %d\n", u8I2c0xF1[1] & 0x0C) ;

							#ifdef SUPPORT_HDMI_AUTO_SWITCH_SRC
							CECSwitchSource(u8I2c0xF1[1], &u8I2cChg, &src_sw_sig);
							#endif

						/*	
							if(status == HDMI_SYS_STANDBY)
							{	
								//power_flag = CEC_POWER_ON;
								if(pite->switch_src_delay > 0)
								{
									LOGD(" \n\n\n\n\n active source switch source timeout .... no switch source \n\n\n\n\n\n");
								}
								else
								{	
									if(u8I2c0xF1[1]  & 0x10)//from TV
									{
										if((u8I2c0xF1[1]  & 0x0C) == 0x0C)//TV source
										{
											src_sw_sig	= ARC_SRC_SIG;
											LOGD("Active source wakeup ARC!\n");
											power_flag = CEC_POWER_ON;
										}
									}
									else if((u8I2c0xF1[1] & 0x0C) == 0 || (u8I2c0xF1[1] & 0x0C) == 0x04)
									{
										switch(u8I2c0xF1[1] & 0x0C)
										{
											case 0x00:
												g_u8SbRxSel = 0x00;  // HDMI 1
												src_sw_sig  = HDMI1_SRC_SIG;
												break;
											case 0x04:
												g_u8SbRxSel = 0x11;  // HDMI 2
												src_sw_sig  = HDMI2_SRC_SIG;
												break;
											default:
												break;
										}
										//u8Temp = 0x08 | g_u8SbRxSel;
										//IT66321_Write(I2C_SYS_RX_SEL, 1, &u8Temp);
										if(g_u8SbAudioMode & 0x80)
										{
											g_u8SbAudioMode &= 0x7F;
											IT66321_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
										}
										u8I2cChg |= 0x80;
										
										LOGD("Active source wakeup R%d\n", g_u8SbRxSel & 0xF);

										power_flag = CEC_POWER_ON;
									}
									
								}	
							}
						*/
						}

					}
					
					if(g_u8IteCecReq & 0x01)
					{  // standby cec cmd need power off
						//u8I2cChg |= SB_CecPower(0);
						LOGD("CEC standby cmd\n");
						power_flag = CEC_POWER_OFF;
						src_sw_sig = 0;
					}


					u8Temp = 0;
					IT66321_Write(I2C_SYS_CEC_RECEIVE, 1, &u8Temp);
				}

				if(u8I2c0xF1[3]){
					BYTE	u8CecReq2 = u8I2c0xF1[3];
					LOGD("CEC REQ2 [0x%02x]\n", u8CecReq2);
					if(u8CecReq2 & I2C_CEC_CMD_POWER_MASK)
					{   // cec power key

						LOGD(" cec power key, sys pwr status %d\n", (g_u8SbAudioMode & 0x03) );
					
						if((g_u8SbAudioMode & 0x03) == 0x01)
						{
							power_flag = CEC_POWER_OFF;
							src_sw_sig = 0;
						}
						else
						{
							power_flag = CEC_POWER_ON;
						}
					}
					
					if(u8CecReq2 & I2C_CEC_CMD_POWER_ON_MASK)
					{  // cec power on key
						
						power_flag = CEC_POWER_ON;
						LOGD(" cec power on key \n");
					}
					
					if(u8CecReq2 & I2C_CEC_CMD_POWER_OFF_MASK)
					{ // cec power off key
						
						power_flag = CEC_POWER_OFF;
						src_sw_sig = 0;
						LOGD(" cec power off key \n");
					}
					u8CecReq2 = 0;
					IT66321_Write(I2C_SYS_CEC_RECEIVE2, 1, &u8CecReq2);
				}

				static BYTE R0_status, R1_status;
				BYTE plug_status = 0;
			
				if(R0_status != (u8I2c0xF1[4] & I2C_R0_STA_MASK))
				{
					switch(u8I2c0xF1[4] & I2C_R0_STA_MASK)
					{
						case I2C_R0_STA_NO:
							LOGD(" HDMI 1 NO connect \n");
							plug_status = 0;
							break;

						case I2C_R0_STA_5V:
							LOGD(" HDMI 1 connected \n");
							plug_status = 1;
							break;

						case I2C_R0_STA_IN:
							LOGD(" HDMI 1 have signal \n");
							plug_status = 2;
							break;

						default:
							plug_status = 0;
							break;
					}

					R0_status = u8I2c0xF1[4] & I2C_R0_STA_MASK;

					LOGD(" HDMI 1 R0_status %x\n", R0_status);

					if(g_u8SbPower == SB_POWER_ON && plug_status != 1)
					{
						HdmiEvt *evt = Q_NEW(HdmiEvt, HDMI1_IN_PLUGIN_SIG);
						evt->vol =  plug_status;
						QACTIVE_POST(UserApp_get(), (QEvt *)evt, UserApp_get());	
					}
					
				}

				if(R1_status != (u8I2c0xF1[4] & I2C_R1_STA_MASK))
				{
					switch(u8I2c0xF1[4] & I2C_R1_STA_MASK)
					{
						case I2C_R1_STA_NO:
							LOGD(" HDMI 2 NO connect \n");
							plug_status = 0;
							break;

						case I2C_R1_STA_5V:
							LOGD(" HDMI 2 connected \n");
							plug_status = 1;
							break;

						case I2C_R1_STA_IN:
							LOGD(" HDMI 2 have signal \n");
							plug_status = 2;
							break;

						default:
							break;

					}

					R1_status = u8I2c0xF1[4] & I2C_R1_STA_MASK;

					LOGD(" HDMI 2 R1_status %x\n", R1_status);

					if(g_u8SbPower == SB_POWER_ON && plug_status != 1)
					{
						HdmiEvt *evt = Q_NEW(HdmiEvt, HDMI2_IN_PLUGIN_SIG);
						evt->vol =  plug_status;
						QACTIVE_POST(UserApp_get(), (QEvt *)evt, UserApp_get());	
					}

				}

			}
			
			if((u8I2cInt & I2C_INT_AUDIO_MASK) == I2C_INT_AUDIO_SET){
			
					BYTE	u8AudioVolume;
					BYTE	u8AudioSta;
					BYTE 	u8EARC;
					BYTE 	u8ARC;
					BYTE 	audio_buf[2];
						
					IT66321_Read(I2C_ADO_SRC_STA, 2, audio_buf);
					u8AudioSta = audio_buf[0];
					
					u8EARC = (u8AudioSta&I2C_ADO_ACTIVE_EARC_MASK)?1:0;
					u8ARC = (u8AudioSta&I2C_ADO_ACTIVE_ARC_MASK)?1:0;

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
					

					//IT66321_Read(I2C_ADO_VOL, 1, &u8AudioVolume);
					u8AudioVolume = audio_buf[1];

					if(pite->vol_timeout > 0)
					{
						LOGD(">> Vol %d, timeout %d\n", pite->volume, pite->vol_timeout);
						//pite->vol_timeout  = 0;
						IT66321_Write(I2C_ADO_VOL, 1, &pite->volume);
						u8I2cChg |= 0x02;
						
					}
					else if( power_flag != CEC_POWER_OFF)
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
									IT66321_Write(I2C_ADO_VOL, 1, &pite->volume);
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


							if(g_u8SbPower == SB_POWER_ON && power_flag != CEC_POWER_OFF)
							{
								HdmiEvt *evt = Q_NEW(HdmiEvt, HDMI_VOL_SIG);
								evt->mute = pite->mute;
								evt->vol = pite->volume&0x7f;
								QACTIVE_POST(UserApp_get(), (QEvt *)evt, UserApp_get());	
							}

						}


					
						BYTE u8IT66322AdoInfo[5];
						BYTE u8IT66322CA;
						UINT32 u32IT66322AdoInfo;
						IT66321_Read(I2C_ADO_INFO, 5, u8IT66322AdoInfo);
						u8IT66322CA = u8IT66322AdoInfo[4];
						u32IT66322AdoInfo = u8IT66322AdoInfo[3];
						u32IT66322AdoInfo <<= 8;
						u32IT66322AdoInfo |= u8IT66322AdoInfo[2];
						u32IT66322AdoInfo <<= 8;
						u32IT66322AdoInfo |= u8IT66322AdoInfo[1];
						u32IT66322AdoInfo <<= 8;
						u32IT66322AdoInfo |= u8IT66322AdoInfo[0];

						if(((g_u32SbAdoDecInfo & (~IgnorAdoSta)) != (u32IT66322AdoInfo & (~IgnorAdoSta))) ||(g_u8TxInfoCA != u8IT66322CA))
						{
							LOGD("u32IT66322AdoInfo = %x, g_u8TxInfoCA = %x, u8IT66322CA = %x\n",  u32IT66322AdoInfo, g_u8TxInfoCA, u8IT66322CA);
						
							g_u32SbAdoDecInfo = u32IT66322AdoInfo;
						
							if(g_u32SbAdoDecInfo & ADO_CONF_ACTIVE_MASK)
							{
								pite->audio_type = (g_u32SbAdoDecInfo & ADO_CONF_TYPE_MASK)>>ADO_CONF_TYPE_SHIFT;
											
								if((g_u32SbAdoDecInfo & (ADO_CONF_SEL_MASK)) == (ADO_CONF_SEL_SPDIF))
								{
									if((g_u32SbAdoDecInfo & ADO_CONF_CH_MASK) == ADO_CONF_CH_0)
									{	// FOR ARC SPDIF IN
									
										g_u32TxAdoEncConf = 0;
										STA_CHANGE(g_u32TxAdoEncConf, ADO_CONF_SEL_MASK, ADO_CONF_SEL_SPDIF);
										STA_CHANGE(g_u32TxAdoEncConf, ADO_CONF_CH_MASK, ADO_CONF_CH_2);
										
										LOGD("ARC input \n");
										
									}
									else
									{															// FOR SPDIF IN SUPPORT HBR and ADO_ENC_NO_INFO_SET
										
										LOGD("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& SPDIF, not ARC \n");
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
								
									g_u32TxAdoEncConf = g_u32SbAdoDecInfo;
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
						}
						LOGD("g_u32TxAdoEncConf = %x\n", g_u32TxAdoEncConf);
					}
				}

				// for CEC auto power on/off function
				if(power_flag == CEC_POWER_OFF && status == HDMI_SYS_POWER_ON)// && pite->src != CMD_MODE_BT)
				{	
					LOGD(" cec auto power off \n");
					//u8I2cChg |= SB_Standby();
					QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, CEC_POWER_OFF_SIG), UserApp_get());	
				}
				else if(power_flag == CEC_POWER_ON && status == HDMI_SYS_STANDBY)
				{
				
					if(src_sw_sig == HDMI1_SRC_SIG)	
						*pIteHdmiSrcRAM = HDMI_WAKEUP_HDMI_1;
					else if(src_sw_sig == HDMI2_SRC_SIG)
						*pIteHdmiSrcRAM = HDMI_WAKEUP_HDMI_2;
					else if(src_sw_sig == ARC_SRC_SIG)
						*pIteHdmiSrcRAM = HDMI_WAKEUP_ARC;
					else
						*pIteHdmiSrcRAM = 0;

					LOGD(" cec auto power on 0x%x \n", *pIteHdmiSrcRAM);

					if(*pIteHdmiSrcRAM != 0)
					{
						res = 1;
					}
				}

				power_flag = CEC_POWER_NULL;

		// for CEC auto switch src function
			#ifdef SUPPORT_HDMI_AUTO_SWITCH_SRC
					if(src_sw_sig != 0 && status == HDMI_SYS_POWER_ON && res == 0)
					{
						if(src_sw_sig == HDMI1_SRC_SIG)	
							LOGD(" cec auto switch src [HDMI 1] \n");
						else if(src_sw_sig == HDMI2_SRC_SIG)
							LOGD(" cec auto switch src [HDMI 2] \n");
						else
							LOGD(" cec auto switch src [ARC] \n");
						QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, src_sw_sig), UserApp_get());	
					}
					src_sw_sig = 0;
			#endif
				
				

				
			
			}

			SB_SysI2cChange(u8I2cChg);
		}
	}
	return res;
}



int ITE66321_CmdHandle(BYTE u8Cmd, BYTE para1, BYTE para2)
{
	ITE_HANDLE *pite = &ite;
//	BYTE sys_mode;
	
	para1 = para1;
	para2  =para2;

	if(pite->upg_flag)
		return 0;

	if(g_u8IteRdy == 1){
		BYTE	u8I2cChg = 0;
		BYTE	u8Temp;
		IT66321_Read(I2C_SYS_CHANGE, 1, &u8I2cChg);
		switch(u8Cmd){

				case 	CMD_ARC_SYS_CLOSE:
						LOGD("CMD_ARC_SYS_AUDIO_CLOSE\n");
						g_u8SbAudioMode &= ~0x18; // audio system off & ARC off
						IT66321_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
						//g_u8SbRxSel = 0x22;
						//IT66321_Write(I2C_SYS_RX_SEL, 1, &g_u8SbRxSel);


					//	IT66321_Read(I2C_SYS_RX_MODE_0, 1, &sys_mode);
					//	sys_mode &= ~(I2C_RX0_ACTIVE_FORCE);//u8RxMode;
					//	IT66321_Write(I2C_SYS_RX_MODE_0, 1, &sys_mode);


					//	IT66321_Read(I2C_SYS_RX_MODE_1, 1, &sys_mode);
					//	sys_mode &= ~(I2C_RX1_ACTIVE_FORCE);//u8RxMode;
					//	IT66321_Write(I2C_SYS_RX_MODE_1, 1, &sys_mode);
	
						
						u8I2cChg |= I2C_UPD_SYS_SET;
						break;
			
				case	CMD_POWER_STANDBY:
						LOGD("CMD_POWER_STANDBY\n");
	
						pite->sys_audio_time = HDMI_STANDBY_SYS_AUDIO_TIME;
					
						u8I2cChg |= SB_Standby();
					break;
				case	CMD_PASSTHROUGH:
						LOGD("SB Passthrough\n");
						u8I2cChg |= SB_PassThrough();
					break;
				case	CMD_POWER_ON:
						
						g_u8SbPower = SB_POWER_ON;
					/*	
						g_u8SbRxSel &= 0xF;
						g_u8SbRxSel |= g_u8SbRxSel << 4;
						IT66321_Write(I2C_SYS_RX_SEL, 1, &g_u8SbRxSel);
						
						IT66321_Read(I2C_SYS_TX_STA, 1, &u8Temp);
						if(u8Temp & 0x02){
							g_u8SbAudioMode = 0x39;
						}else{
							g_u8SbAudioMode = 0x01;//0x41//0x61;
						}
						IT66321_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);

					*/	
						IT66321_Read(I2C_SYS_TX_STA, 1, &u8Temp);
						if(u8Temp & 0x02){
							g_u8SbAudioMode = 0x39;
						}else{
							g_u8SbAudioMode = 0x01;//0x41//0x61;
						}
						pite->src = para1;
						u8I2cChg |= SB_CecSource(para1);

						if(para1 == CMD_MODE_HDMI_1)
							LOGD("SB Power ON [CMD_MODE_HDMI_1] \n");
						else if(para1 == CMD_MODE_HDMI_2)
							LOGD("SB Power ON [CMD_MODE_HDMI_2] \n");
						else if(para1 == CMD_MODE_ARC)
							LOGD("SB Power ON [CMD_MODE_ARC] \n");
						else if(para1 == CMD_MODE_BT)
							LOGD("SB Power ON [CMD_MODE_BT] \n");
						else
							LOGD("SB Power ON [ NULL ] \n");
					
						g_u8SbCecMode = SB_CEC_AUTO;
						u8Temp = 0x85;
						IT66321_Write(I2C_SYS_RX_MODE_0, 1, &u8Temp);
						IT66321_Write(I2C_SYS_RX_MODE_1, 1, &u8Temp);
						u8I2cChg |= 0x80;

						pite->switch_src_delay = SWITCH_SRC_DELAY_CNT;
						
						pite->vol_timeout = 13;
						IT66321_Write(I2C_ADO_VOL, 1, &pite->volume);
						u8I2cChg |= 0x02;

						pite->power = 0;
						
					break;
				case	CMD_POWER_OFF:
						LOGD("SB Power OFF \n");
						u8I2cChg |= SB_PowerOff();
					break;
				case	CMD_DUMP_I2C:
						I2C_Slave_Dump();
					break;
		}

		if(g_u8SbPower == SB_POWER_ON){
			switch(u8Cmd){

				case 	CMD_ARC_SYS_CLOSE:
				case 	CMD_POWER_ON:
						break;
				
				case	CMD_VOLUME_MUTE:					// SB Mute/UnMute

						if(para1 == 0)  //UnMute
							pite->volume &= ~0x80;
						else
							pite->volume |= 0x80;
						IT66321_Write(I2C_ADO_VOL, 1, &pite->volume);
						u8I2cChg |= 0x02;
						if(pite->volume & 0x80){
							LOGD("Mute \n");
						}else{
							LOGD("UnMute \n");
						}
					//	IT66322_SysCecUserCtlMute();
					break;
				case	CMD_VOLUME_UP:					// SB Volume Up
						
						pite->volume = para2;

						pite->vol_timeout = VOL_TIMEOUT_CNT;
					
						IT66321_Write(I2C_ADO_VOL, 1, &pite->volume);
						u8I2cChg |= 0x02;
						LOGD("Vol+ %d\n",pite->volume);
					break;
				case	CMD_VOLUME_DOWN:					// SB Volume Down
				
						pite->volume = para2;

						pite->vol_timeout = VOL_TIMEOUT_CNT;
						
						IT66321_Write(I2C_ADO_VOL, 1, &pite->volume);
						u8I2cChg |= 0x02;
						LOGD("Vol- %d\n", pite->volume);
					break;
					
				case	CMD_MODE_HDMI_1:					// SB HDMI 1
				case	CMD_MODE_HDMI_2:					// SB HDMI 2			
				case	CMD_MODE_ARC:					// SB ARC		
				case	CMD_MODE_BT:
						
						u8I2cChg |= SB_CecSource(u8Cmd);;
					
					break;
						
				case	CMD_ADO_SYS:
						g_u8SbAudioMode ^= 0x08;
						IT66321_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
						u8I2cChg |= 0x80;
						if(g_u8SbAudioMode & 0x08){
							LOGD("Audio System ON \n");
						}else{
							LOGD("Audio System OFF \n");
						}
					break;
				case	CMD_EARC:
						g_u8SbAudioMode ^= 0x10;
						if(g_u8SbAudioMode & 0x10){
							g_u8SbAudioMode |= 0x08;
						}
						IT66321_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
						u8I2cChg |= 0x80;
						if(g_u8SbAudioMode & 0x10){
							LOGD("eARC/ARC ON \n");
						}else{
							LOGD("eARC/ARC OFF \n");
						}
					break;
				case	CMD_SB_CEC:
						if(g_u8SbCecMode){
							BYTE	u8TxSta;
							g_u8SbCecMode = SB_CEC_AUTO;
							IT66321_Read(I2C_SYS_TX_STA, 1, &u8TxSta);
							if(u8TxSta & 0x02){
								g_u8SbAudioMode |= 0x20;
							}else{
								g_u8SbAudioMode &= ~0x20;
							}
						}else{
							g_u8SbCecMode = SB_CEC_OFF;
							g_u8SbAudioMode &= ~0x20;
						}

						IT66321_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
						u8I2cChg |= 0x80;
						if(g_u8SbAudioMode & 0x20){
							LOGD("SB CEC ON \n");
						}else{
							LOGD("SB CEC OFF \n");
						}
					break;
				case	CMD_TX_AUDIO:
						g_u8SbAudioMode ^= 0x40;
						IT66321_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
						u8I2cChg |= 0x80;
						if(g_u8SbAudioMode & 0x40){
							LOGD("Tx Audio Mute \n");
						}else{
							LOGD("Tx Audio UnMute \n");
						}
					break;
				case	CMD_TX_OUTPUT:
						g_u8SbAudioMode ^= 0x80;
						IT66321_Write(I2C_SYS_ADO_MODE, 1, &g_u8SbAudioMode);
						u8I2cChg |= 0x80;
						if(g_u8SbAudioMode & 0x80){
							LOGD("Tx Disable Output \n");
						}else{
							LOGD("Tx Output \n");
						}
					break;
				case	CMD_HDCP_RPT:
						IT66321_Read(I2C_SYS_RX_MODE_0, 1, &u8Temp);
						u8Temp ^= 0x04;
						IT66321_Write(I2C_SYS_RX_MODE_0, 1, &u8Temp);
						LOGD("HDMI 0 -> HDCP Repeater = %d\n", (u8Temp & 0x04) >> 2);
						IT66321_Read(I2C_SYS_RX_MODE_1, 1, &u8Temp);
						u8Temp ^= 0x04;
						IT66321_Write(I2C_SYS_RX_MODE_1, 1, &u8Temp);
						LOGD("HDMI 1 -> HDCP Repeater = %d\n", (u8Temp & 0x04) >> 2);
						u8I2cChg |= 0x80;
					break;
				default:
						LOGD("NoSupport\n");
						I2C_Slave_Dump();
					break;
			}
		}
		SB_SysI2cChange(u8I2cChg);
	}

	return 0;
}


// =================================================================================


#define APP_ERASE_PAGE_SIZE         (256L)  
#define APP_PROGRAM_PAGE_SIZE       (256L)
#define APP_IMAGE_START_ADDR        0x800UL   
#define APP_MCU_ROM_SIZE        0x20000UL   

/* Definitions */
#define APP_PROTOCOL_HEADER_MAX_SIZE                9


#define APP_BL_STATUS_BIT_BUSY                      (0x01 << 0)
#define APP_BL_STATUS_BIT_INVALID_COMMAND           (0x01 << 1)
#define APP_BL_STATUS_BIT_INVALID_MEM_ADDR          (0x01 << 2)
#define APP_BL_STATUS_BIT_COMMAND_EXECUTION_ERROR   (0x01 << 3)      //Valid only when APP_BL_STATUS_BIT_BUSY is 0
#define APP_BL_STATUS_BIT_CRC_ERROR                 (0x01 << 4)
#define APP_BL_STATUS_BIT_ALL                       (APP_BL_STATUS_BIT_BUSY | APP_BL_STATUS_BIT_INVALID_COMMAND | APP_BL_STATUS_BIT_INVALID_MEM_ADDR | \
                                                    APP_BL_STATUS_BIT_COMMAND_EXECUTION_ERROR | APP_BL_STATUS_BIT_CRC_ERROR)

typedef enum
{
    APP_BL_COMMAND_UNLOCK = 0xA0,
    APP_BL_COMMAND_ERASE = 0xA1,
    APP_BL_COMMAND_PROGRAM = 0xA2,
    APP_BL_COMMAND_VERIFY = 0xA3,
    APP_BL_COMMAND_RESET = 0xA4,
    APP_BL_COMMAND_READ_STATUS = 0xA5
}APP_BL_COMMAND;

typedef enum
{
	APP_STATE_SEND_START_COMMAND,
    APP_STATE_SEND_UNLOCK_COMMAND,
    APP_STATE_WAIT_UNLOCK_COMMAND_TRANSFER_COMPLETE,
    APP_STATE_SEND_ERASE_COMMAND,
    APP_STATE_WAIT_ERASE_COMMAND_TRANSFER_COMPLETE,
    APP_STATE_SEND_WRITE_COMMAND,
    APP_STATE_WAIT_WRITE_COMMAND_TRANSFER_COMPLETE,
    APP_STATE_SEND_VERIFY_COMMAND,
    APP_STATE_WAIT_VERIFY_COMMAND_TRANSFER_COMPLETE,
    APP_STATE_SEND_RESET_COMMAND,
    APP_STATE_WAIT_RESET_COMMAND_TRANSFER_COMPLETE,
    APP_STATE_SUCCESSFUL,
    APP_STATE_ERROR,
    APP_STATE_IDLE,

} APP_STATES;

typedef enum
{
    APP_TRANSFER_STATUS_IN_PROGRESS,
    APP_TRANSFER_STATUS_SUCCESS,
    APP_TRANSFER_STATUS_ERROR,
    APP_TRANSFER_STATUS_IDLE,

} APP_TRANSFER_STATUS;

typedef struct
{
    APP_STATES                      state;
    APP_STATES                      nextState;
    bool    					trasnferStatus;
    uint32_t                        appMemStartAddr;
    uint32_t                        nBytesWritten;
    uint32_t                        nBytesWrittenInErasedPage;
    uint8_t                         status;
    uint8_t                         *wrBuffer;

} APP_DATA;

/* Global Data */
APP_DATA                            appData;

static uint32_t APP_CRCGenerate(void)
{
    uint32_t crc  = 0;

    return crc;
}

static int APP_Initialize(void)
{
    appData.appMemStartAddr = APP_IMAGE_START_ADDR;
    appData.nBytesWritten = 0;
    appData.state = APP_STATE_SEND_START_COMMAND;

	appData.wrBuffer = malloc( APP_PROGRAM_PAGE_SIZE + APP_PROTOCOL_HEADER_MAX_SIZE);
	if(appData.wrBuffer != NULL)
		return 0;

	return -1;
}


static uint32_t APP_ImageDataWrite(int fd, uint32_t memAddr, uint32_t nBytes)
{
    uint32_t nTxBytes = 0;

    appData.wrBuffer[nTxBytes++] = APP_BL_COMMAND_PROGRAM;
    appData.wrBuffer[nTxBytes++] = (nBytes >> 24);
    appData.wrBuffer[nTxBytes++] = (nBytes >> 16);
    appData.wrBuffer[nTxBytes++] = (nBytes >> 8);
    appData.wrBuffer[nTxBytes++] = (nBytes);
    appData.wrBuffer[nTxBytes++] = (memAddr >> 24);
    appData.wrBuffer[nTxBytes++] = (memAddr >> 16);
    appData.wrBuffer[nTxBytes++] = (memAddr >> 8);
    appData.wrBuffer[nTxBytes++] = (memAddr);


    read(fd, &appData.wrBuffer[nTxBytes],nBytes);

    return nTxBytes+nBytes;
}

static uint32_t APP_UnlockCommandSend(uint32_t appStartAddr, uint32_t appSize)
{
    uint32_t nTxBytes = 0;

    appData.wrBuffer[nTxBytes++] = APP_BL_COMMAND_UNLOCK;
    appData.wrBuffer[nTxBytes++] = (appStartAddr >> 24);
    appData.wrBuffer[nTxBytes++] = (appStartAddr >> 16);
    appData.wrBuffer[nTxBytes++] = (appStartAddr >> 8);
    appData.wrBuffer[nTxBytes++] = (appStartAddr);
    appData.wrBuffer[nTxBytes++] = (appSize >> 24);
    appData.wrBuffer[nTxBytes++] = (appSize >> 16);
    appData.wrBuffer[nTxBytes++] = (appSize >> 8);
    appData.wrBuffer[nTxBytes++] = (appSize);

    return nTxBytes;
}

static uint32_t APP_VerifyCommandSend(uint32_t crc)
{
    uint32_t nTxBytes = 0;

    appData.wrBuffer[nTxBytes++] = APP_BL_COMMAND_VERIFY;
    appData.wrBuffer[nTxBytes++] = (crc >> 24);
    appData.wrBuffer[nTxBytes++] = (crc >> 16);
    appData.wrBuffer[nTxBytes++] = (crc >> 8);
    appData.wrBuffer[nTxBytes++] = (crc);

    return nTxBytes;
}

static uint32_t APP_EraseCommandSend(uint32_t memAddr)
{
    uint32_t nTxBytes = 0;

    appData.wrBuffer[nTxBytes++] = APP_BL_COMMAND_ERASE;
    appData.wrBuffer[nTxBytes++] = (memAddr >> 24);
    appData.wrBuffer[nTxBytes++] = (memAddr >> 16);
    appData.wrBuffer[nTxBytes++] = (memAddr >> 8);
    appData.wrBuffer[nTxBytes++] = (memAddr);

    return nTxBytes;
}

static int get_file_size(const char* file) 
{
    struct _sTat tbuf;
    stat(file, &tbuf);
    return tbuf.st_size;
}



int ITE66321_UPG_DISPLAY(BYTE vol)
{
	BYTE strBuf[10] = {0};

	if(vol == HDMI_UPG_OK)
	{
		sprintf(strBuf, "HD OK");
		//QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, POWER_RELEASE_SIG), NULL);
	}
	else if(vol == HDMI_UPG_NG)
	{
		sprintf(strBuf, "HD NG");
	}
	else if(vol == 100)
	{
		sprintf(strBuf, "HD%03d", vol);
	}
	else if(vol > 9)
	{
		sprintf(strBuf, "HD %02d", vol);
	}
	else
	{
		sprintf(strBuf, "HD  %01d", vol);
	}

	Vfd_Set_Str(strBuf);

	return 0;

}


int ITE66321_MCU_UPG(void)
{

	uint32_t nTxBytes;
    	uint32_t crc;
	BYTE upg_path[] = {"USB:MCU.BIN"};
	BYTE i=0;
	BYTE upf_flag[5] = {0x12,0x34,0x56,0x78,0x90};
	static BYTE last_vol;
	BYTE cur_vol;

	int fd = open(upg_path, O_RDONLY, 0);
	int file_size = get_file_size(upg_path);
	if(fd < 0)
	{
		LOGD(" HDMI upg open file error\n ");
		//HdmiEvt *e = Q_NEW(HdmiEvt, HDMI_UPG_SIG);
		//e->vol = HDMI_UPG_NG;
		// QACTIVE_POST(UserApp_get(), (QEvt *)e, (void*)0);
		ITE66321_UPG_DISPLAY(HDMI_UPG_NG);
		return -1;
	}

	if(APP_Initialize() < 0)
	{
		LOGD(" HDMI upg buff malloc error\n ");
		close(fd);
		//HdmiEvt *e = Q_NEW(HdmiEvt, HDMI_UPG_SIG);
		//e->vol = HDMI_UPG_NG;
		// QACTIVE_POST(UserApp_get(), (QEvt *)e, (void*)0);
		ITE66321_UPG_DISPLAY(HDMI_UPG_NG);
		return -1;
	}

	ite.upg_flag = 1;

	LOGD(" ITE66321_MCU_UPG\n ");

	while(1)
	{
		switch(appData.state)
		{

		case APP_STATE_SEND_START_COMMAND:
			LOGDT(" 0: HDMI MCU Upg prepare : wait MCU reboot to bootloader\n");
			IT66321_Write(I2C_ITE_CHIP, 5, upf_flag);
			delay_1ms(2000);
			appData.state = APP_STATE_SEND_UNLOCK_COMMAND;
			break;

		 case APP_STATE_SEND_UNLOCK_COMMAND:

				LOGDT(" 1: HDMI MCU Upg start : file size 0x%x\n ", file_size);
					
	                     	nTxBytes = APP_UnlockCommandSend(APP_IMAGE_START_ADDR, APP_MCU_ROM_SIZE-APP_IMAGE_START_ADDR); //file_size);

	               	 appData.trasnferStatus = IT66321_Write(appData.wrBuffer[0], nTxBytes-1, &appData.wrBuffer[1]);
					
	               	 appData.state = APP_STATE_WAIT_UNLOCK_COMMAND_TRANSFER_COMPLETE;	
	                break;

	            case APP_STATE_WAIT_UNLOCK_COMMAND_TRANSFER_COMPLETE:
					
	                if (appData.trasnferStatus == TRUE)
	                {
	                    appData.state = APP_STATE_SEND_ERASE_COMMAND;
	                }
	                else
	                {
	                    appData.state = APP_STATE_ERROR;
	                }
	                break;

	            case APP_STATE_SEND_ERASE_COMMAND:

			 LOGDT("2: HDMI MCU Send erase MCU flash 0x%x\n", appData.appMemStartAddr + appData.nBytesWritten);
					
	                if ((appData.appMemStartAddr + appData.nBytesWritten) < (APP_MCU_ROM_SIZE) ) //APP_IMAGE_START_ADDR + file_size))
	                {
	                    nTxBytes = APP_EraseCommandSend((appData.appMemStartAddr + appData.nBytesWritten));
	                    if (nTxBytes > 0)
	                    {
	                        appData.nBytesWrittenInErasedPage = 0;
							
	                        appData.trasnferStatus =  IT66321_Write(appData.wrBuffer[0], nTxBytes-1, &appData.wrBuffer[1]);
							
	                        appData.state = APP_STATE_WAIT_ERASE_COMMAND_TRANSFER_COMPLETE;
	                    }
	                }
	                else
	                {
	                    /* Firmware programming complete. Now verify CRC by sending verify command */
	                    appData.state = APP_STATE_SEND_VERIFY_COMMAND;
	                }
	                break;

	            case APP_STATE_WAIT_ERASE_COMMAND_TRANSFER_COMPLETE:
	                if (appData.trasnferStatus == TRUE)
	                {
		
	                    /* Read the status of the erase command */
	                    appData.wrBuffer[0] = APP_BL_COMMAND_READ_STATUS;
						
				for(i=0; i<10; i++)
	                    	{
	                    		delay_1ms(100);
					appData.status = 0xff;
	                    		appData.trasnferStatus = IT66321_Read(appData.wrBuffer[0], 1, &appData.status);

					LOGDT("3: HDMI MCU Get erase flash OK flag [%d] : %d\n", i, appData.status);
								
					 if (appData.trasnferStatus == TRUE)
					 {
						 if (appData.status == 0)
				                {
				                    	  appData.state = APP_STATE_SEND_WRITE_COMMAND;
				                        break;
				                }
					 }
					 else
					 {
						appData.state = APP_STATE_ERROR;
						break;
					 }
					 	
				}

				 if (appData.status != 0)
	                 	   appData.state = APP_STATE_ERROR;
				 
	                    
	                }
	                else 
	                {
	                    appData.state = APP_STATE_ERROR;
	                }

	                break;

	            case APP_STATE_SEND_WRITE_COMMAND:
	                if (appData.trasnferStatus == TRUE)
	                {
	                    nTxBytes = APP_ImageDataWrite(fd, (appData.appMemStartAddr + appData.nBytesWritten), APP_PROGRAM_PAGE_SIZE);

				LOGDT("4: HDMI MCU Send upg data, packnum %d, %d\n", appData.nBytesWritten, nTxBytes);

			     appData.trasnferStatus =  IT66321_Write(appData.wrBuffer[0], nTxBytes-1, &appData.wrBuffer[1]);
						
	                    appData.state = APP_STATE_WAIT_WRITE_COMMAND_TRANSFER_COMPLETE;

					cur_vol = (100 * appData.nBytesWritten)/(APP_MCU_ROM_SIZE-APP_IMAGE_START_ADDR);
					if((last_vol != cur_vol) )
					{
						last_vol = cur_vol;
						//HdmiEvt *e = Q_NEW(HdmiEvt, HDMI_UPG_SIG);
						//e->vol = cur_vol;
		 				//QACTIVE_POST(UserApp_get(), (QEvt *)e, (void*)0);
		 				ITE66321_UPG_DISPLAY(cur_vol);
					} 
	                }
	                else 
	                {
	                    appData.state = APP_STATE_ERROR;
	                }
	                break;

	            case APP_STATE_WAIT_WRITE_COMMAND_TRANSFER_COMPLETE:
	                if (appData.trasnferStatus == TRUE)
	                {
	                    appData.nBytesWritten += APP_PROGRAM_PAGE_SIZE;
	                    appData.nBytesWrittenInErasedPage += APP_PROGRAM_PAGE_SIZE;

	                    /* Read the status of the write command */
	                    appData.wrBuffer[0] = APP_BL_COMMAND_READ_STATUS;
						
	                     for(i=0; i<10; i++)
	                    	{
	                    		delay_1ms(100);
					appData.status = 0xff;
	                    		appData.trasnferStatus = IT66321_Read(appData.wrBuffer[0], 1, &appData.status);

					LOGDT("5: HDMI MCU get upg data ok flag[%d] : %d\n", i, appData.status);
								
					 if (appData.trasnferStatus == TRUE)
					 {
						 if (appData.status == 0)
				                {
				                    	  appData.state = APP_STATE_SEND_WRITE_COMMAND;
				                        break;
				                }
					 }
					 else
					 {
						appData.state = APP_STATE_ERROR;
						break;
					 }
					 	
				}

			      if (appData.status != 0)
	                    { 
	                 	  appData.state = APP_STATE_ERROR;
			     }		
	                    else if (appData.nBytesWrittenInErasedPage == APP_ERASE_PAGE_SIZE)
	                    {
	                        /* All the pages of the Erased Row are written. Erase next row. */
	                        appData.state = APP_STATE_SEND_ERASE_COMMAND;
	                    }
	                    else
	                    {
	                        /* Continue to write to the Erased Page */
	                        appData.state = APP_STATE_SEND_WRITE_COMMAND;
	                    }
	                }
	                else if (appData.trasnferStatus == APP_TRANSFER_STATUS_ERROR)
	                {
	                    appData.state = APP_STATE_ERROR;
	                }
	                break;

	            case APP_STATE_SEND_VERIFY_COMMAND:
	                crc = APP_CRCGenerate();
	                nTxBytes = APP_VerifyCommandSend(crc);
	               appData.trasnferStatus =  IT66321_Write(appData.wrBuffer[0], nTxBytes-1, &appData.wrBuffer[1]);

			LOGDT("6: HDMI MCU send upg data checksum : 0x%x\n", crc);
				   
				   
	                appData.state = APP_STATE_WAIT_VERIFY_COMMAND_TRANSFER_COMPLETE;
	                break;

	            case APP_STATE_WAIT_VERIFY_COMMAND_TRANSFER_COMPLETE:
	                if (appData.trasnferStatus == TRUE)
	                {
	                    /* Read the status of the verify command */
	                    appData.wrBuffer[0] = APP_BL_COMMAND_READ_STATUS;
						
	                     for(i=0; i<10; i++)
	                    	{
	                    		delay_1ms(100);
					appData.status = 0xff;
	                    		appData.trasnferStatus = IT66321_Read(appData.wrBuffer[0], 1, &appData.status);

					LOGD("7: HDMI MCU get data checksum OK flag[%d]: %d\n", i, appData.status);
								
					 if (appData.trasnferStatus == TRUE)
					 {
						 if (appData.status == 0)
				                {
				                    	  appData.state = APP_STATE_SEND_RESET_COMMAND;
				                        break;
				                }
					 }
					 else
					 {
						appData.state = APP_STATE_ERROR;
						break;
					 }
					 	
				}

			     if (appData.status != 0)
	                    {	 
	                 	  appData.state = APP_STATE_ERROR;
			     }		

	                }
	                else 
	                {
	                    appData.state = APP_STATE_ERROR;
	                }
	                break;

	         

	            case APP_STATE_SEND_RESET_COMMAND:

			LOGDT("8: HDMI MCU upg complete, reset MCU\n");
			
	                appData.wrBuffer[0] = APP_BL_COMMAND_RESET;
	                appData.trasnferStatus =  IT66321_Write(appData.wrBuffer[0], 0, NULL);
	                appData.state = APP_STATE_WAIT_RESET_COMMAND_TRANSFER_COMPLETE;
	                break;

	            case APP_STATE_WAIT_RESET_COMMAND_TRANSFER_COMPLETE:
	                if (appData.trasnferStatus == TRUE)
	                {
	                    appData.state = APP_STATE_SUCCESSFUL;
					LOGDT("9: HDMI MCU upg successful\n");
	                }
	                else
	                {
	                    appData.state = APP_STATE_ERROR;
					LOGDT("9: HDMI MCU upg error\n");
	                }
	                break;

	            case APP_STATE_SUCCESSFUL:{

	                appData.state = APP_STATE_IDLE;


				ITE66321_UPG_DISPLAY(HDMI_UPG_OK);

					//HdmiEvt *e = Q_NEW(HdmiEvt, HDMI_UPG_SIG);
					//e->vol = HDMI_UPG_OK;
		 		//	QACTIVE_POST(UserApp_get(), (QEvt *)e, (void*)0);
					
	                break;
	            	}
	            case APP_STATE_ERROR:{
	                appData.state = APP_STATE_IDLE;

					ITE66321_UPG_DISPLAY(HDMI_UPG_NG);
					
				//	HdmiEvt *e = Q_NEW(HdmiEvt, HDMI_UPG_SIG);
				//	e->vol = HDMI_UPG_NG;
		 		//	QACTIVE_POST(UserApp_get(), (QEvt *)e, (void*)0);
	                break;
	            	}
	            case APP_STATE_IDLE:
					goto UPG_DONE;
	                break;
		}
	}

UPG_DONE:

	free(appData.wrBuffer);
	close(fd);
	delay_1ms(1000);
	LOGD("10: HDMI MCU upg exit\n");

	ite.upg_flag = 0;

	return 0;
	
}




HDMI_FUNC ite_rep = 
{
	IT66321_Cfg,
	NULL,
	ITE66321_GetStatus, //SB_I2cIrq,
	ITE66321_CmdHandle, //SB_HostControl,
	ITE66321_MCU_UPG,
	ITE66321_CheckEARC,
	ITE66321_CheckADOType,
	ITE66321_GetVersion,
};

