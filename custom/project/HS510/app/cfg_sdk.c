/******************************************************************************/
/**
*
* \file	    cfg_sdk.c
*
* \brief	SDK configuration for customer.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   yangf.wang@sunplus.com.cn
* \version	 v0.01
* \date 	 2016/07/22
******************************************************************************/

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include "project_cfg.h"
#include "cfg_sdk.h"
#include "adc_KeyTab.inc"
#include "AudUserControl.h"
#include "log_imp.h"
#include "user_def.h"
#include "pinmux.h"
#include "iop_user.h"

/*---------------------------------------------------------------------------*
    IR config
*---------------------------------------------------------------------------*/
//#define IR_CUSTOMER_CODE   		0x40bf //for SUNPLUS_IR300
#define IR_CUSTOMER_CODE   CUST_CUSTOMER_CODE  //(0x007F) //for other sunplus IR controller
#define IR_POWER_KEY_CODE  CUST_POWER_KEY_CODE// (0x46)

/*---------------------------------------------------------------------------*
    io scanner AO config
*---------------------------------------------------------------------------*/
// KEY_SCAN_FREQ unit: 100ms, ex: 1->1 time/100ms, 2->1 time/200ms
#define KEY_SCAN_FREQ_100MS (1) // unit: 100ms
#define KEY_SCAN_FREQ_200MS (2) // unit: 100ms
#define KEY_SCAN_FREQ KEY_SCAN_FREQ_100MS //how long time we scan keys one time // unit: 100ms
#define KEY_TICK_TIME (2) //the interval of a tick to userapp. it should be a multiple of KEY_SCAN_FREQ // unit: 100ms
#define KEY_LONG_PRESS_TIME (10) // unit: 100ms

#if (KEY_LONG_PRESS_TIME % KEY_SCAN_FREQ)
	#error: KEY_LONG_PRESS_TIME should be a multiple of KEY_SCAN_FREQ
#endif

#if (KEY_TICK_TIME % KEY_SCAN_FREQ)
	#error: KEY_TICK_TIME should be a multiple of KEY_SCAN_FREQ
#endif

/*---------------------------------------------------------------------------*
    cfg structure
*---------------------------------------------------------------------------*/
static const char *BTHeadphoneDesName[BT_HEADPHONE_NUN] = {"ATH", "TCE", "HM", "AEGIS", "Nakamichi", "Newmine"};
static const char *BTSubDesName[BT_SUB_NUN] = {"subSUB"};

static CFG_SDK_T gstSDKConfig =
{
	// configure option for log system
	.CFG_LOG_TO =
	{
		.eLogState = LOG_ENABLED,
        .pcPassword = "+88635786005",
	},

	// configure option for AudioPlayer&FileList module
	.CFG_PLAYER_TO =
	{
		.dStartMode = 0,
		.dFmtFilter =
		CFG_FILTER_MP3 | CFG_FILTER_WAV | CFG_FILTER_FLAC
		| CFG_FILTER_WMA /*| CFG_FILTER_AAC | CFG_FILTER_AC3
		| CFG_FILTER_DTS | CFG_FILTER_APE | CFG_FILTER_MKA
		| CFG_FILTER_M4A | CFG_FILTER_OGG | CFG_FILTER_EC3
		| CFG_FILTER_TS | CFG_FILTER_ASF | CFG_FILTER_EAC3
		| CFG_FILTER_MLP | CFG_FILTER_AIF | CFG_FILTER_AIFF
		| CFG_FILTER_AIFC*/,
		.nAutoPlay = CFG_AUTOPLAY_USB_ENABLE | CFG_AUTOPLAY_CARD_ENABLE,
		.dMaxBitrate = SUPPORT_MAX_BITRATE,
		.dMaxDirCnt = SUPPORT_MAX_DIR_CNT,
		.dMaxFileCnt = SUPPORT_MAX_FILE_CNT,
		.nIsResume = SUPPORT_RESUME,
		.dUSBResumeTime = 0,// 0 : disable switch src & usb plug out ,play time resume to 0;  1 : enable  resume to 0
	},
	//configure option for ADC channel 0
	.CFG_ADC_TO[0] =
	{
		.Enable = 0,
		.Func = EN_ADC_KEY_FUNC,
		.LevelNum = ADC_LEVEL_NUM_MAX,
		.pLevelTab = ADC_DataLevelTab,
	},
	//configure option for ADC channel 1
	.CFG_ADC_TO[1] =
	{
		.Enable = 0,
		.Func = EN_ADC_USER_FUNC,
		.LevelNum = ADC_LEVEL_NUM_MAX,
		.pLevelTab = ADC_DataLevelTab,
	},
	//configure option for ADC channel 2
	.CFG_ADC_TO[2] =
	{
		.Enable = 0, //ADC2 and HPD ADC mode cannot work at the same time
		.Func = EN_ADC_KEY_FUNC,
		.LevelNum = ADC_LEVEL_NUM_MAX,
		.pLevelTab = ADC_DataLevelTab,
	},
	//configure option for ADC channel 3
	.CFG_ADC_TO[3] =
	{
		.Enable = 0,
		.Func = EN_ADC_KEY_FUNC,
		.LevelNum = ADC_LEVEL_NUM_MAX,
		.pLevelTab = ADC_DataLevelTab,
	},
	//configure option for ir
	.CFG_IR_TO =
	{
		.Cfg_Enable = 1,
		.IopWakeUp = 1,// which one wake up system from standby mode by checking IR iPowerKeycode // 1: IOP(recommand), 0: Risc
		.TV_Remote_Ready_Enable = 1,
		.MainIrFmt = 0, //which ir fmt is active in  standby mode (check its power key)//0 for UPD6121 (NEC), 1 for TC9012, 2 for RC6 (philip)
		.IR_Learning_Enable = 1,
		.lCustomerID = IR_CUSTOMER_CODE,// default is for IR fmt: NEC (sunplus IR:400)
		.iPowerKeycode = IR_POWER_KEY_CODE,// default is for IR fmt: NEC (sunplus IR:400)
	},
	//configure option for key polling process
	.CFG_KEY_TO =
	{
		.Cfg_Enable = 1,
		.iPollingTime = KEY_SCAN_FREQ,
		.iKeyTickFreq = KEY_TICK_TIME / KEY_SCAN_FREQ,
		.iLongPressTickNum = KEY_LONG_PRESS_TIME / KEY_SCAN_FREQ,
	},
	//configure option for CEC module
    .CFG_CEC_TO =
    {
        .CEC_HPD_WakeUp            = 0,// 1 to enable HPD wake up feature, 0 to disable
        .Cfg_CecAlwaysRun          = 0,
    #ifdef USERAPP_SUPPORT_HDMI_CEC
        .Cfg_Enable                = 1,
        .pbI2C_DevStr              = "i2c3_sw",
        .repeaterAddr              = 0x00,
        .repeaterIntrGpio        = 25,
    #else
        // ep repeater
        .Cfg_Enable                = 0,
        .pbI2C_DevStr              = "i2cm_hw",
        .repeaterAddr              = 0xC2,
        .repeaterIntrGpio        = 25,
    #endif

        .HPD_DetectFunc            = eHPD_By_GPIO,//SPA100 HPD  check by GPIO function
        .HPD_VolThreshold          = 500,// if voltage = 3.64V, ADC value = 1023. if voltage = 1.8V, ADC value = 500
        .CEC_VendorId              = 0x001805,//0x008045,panasonic VendorID
        .pbCEC_DevOsdName          = "SUNPLUS",//show device name in the TV
        .CEC_PwrOnOpcodeNum        = 5,// max: 8
        .CEC_PwrOnOpcodeTbl        = // Soudbar received the CEC CMD to power on
        {
            PWR_ON_CMD_SYS_AUDIO_MODE_REQUEST,
            PWR_ON_CMD_USR_CTL_PRESSED,
            PWR_ON_CMD_SET_STREAM_PATH,
            PWR_ON_CMD_ACTIVE_SOURCE,
            PWR_ON_CMD_ROUTING_CHANGE,
        },
        .CEC_ShortAudDescriptor    = // ARC announces supported audio formats  GEA-861-D
        {
            CEC_RESERVED,
            CEC_LPCM,
            CEC_AC3,
            CEC_MPEG1,
            CEC_MP3,
            CEC_MPEG2,
            CEC_AAC,
            CEC_DTS,
            CEC_DDP,
            CEC_UNUSE,
        },
        .CEC_ShortAudDescriptorNum  = 9,// max :15
        .CEC_ShortAudDescriptorData =
        {
            {0x00, 0x00, 0x00},
            {0x09, 0x1F, 0x07},// LPCM _2ch
            {0x15, 0x07, 0x50},// AC3 _ 6ch
            {0x1D, 0x07, 0x07},// MPEG1_6ch
            {0x25, 0x07, 0x07},// Mp3_6ch
            {0x2D, 0x07, 0x07},// MPE2G_6ch
            {0x35, 0x07, 0x30},// AAC _6ch
            {0x3D, 0x1F, 0xC0},// DTS _6ch
            {0x45, 0x1F, 0x00},// ATRAC_6ch
            {0x4D, 0x07, 0x00},//ONE_BIT_audio_6ch
            {0x57, 0x04, 0x03},// DDP_6ch
            {0x5F, 0x7F, 0x01},// DTS-HD_8ch  //{0x5F, 0x7F,0x07},//DTS:X
            {0x67, 0x7F, 0x01},//Dolby True-HD // {0x67, 0x7F, 0x03}//Dolby True-HD Atmos
            {0x00, 0x00, 0x00},
            {0x00, 0x00, 0x00},
        },

    },
	//configure option for HDMI module
	.CFG_HDMI_TO =
	{
		.Cfg_HdmiOutputMode = eHDMI_AUTO,
	},
	//configure option for PMC
    .CFG_PMC_TO =
    {
        .bFstPwrOnFlag     = 1,// 1: system need into standby mode ,when system first power on. 0: system running ,when system first power on
        .bStandbyMode      = 3,// 0: PMC 1: IOP 2: RISC 3:DRAM
        .wIRwakeupMode     = 1, // 1:iop  2:RISC
        .wRtcwakeupMode    = 0,// 1: Enable RTC wake up mode 0: Disable
        .wCECwakeupMode    = 0,// 1:Enable CEC wake up mode 0:Disable
        .wSPDIFwakeupMode  = 0,// 1: enable spdif detect 0: disable
        .wRtcIntervalTime  = 10,// if wRtcwakeupMode = 1, system will exit standby and reset system after wRtcIntervalTime seonds
        .riscSleepTime     = 200,// risc sleep time in dram standby, unit is ms
    },
    // GPIO25
    .CFG_STDBY_GPIO_TO[0] = {
        // gpioChkEnable: enable gpio check in 8051, gpioTrigLvl: gpio trigger level, functionEnable: function select
        .gpioChkEnable = 0, .gpioTrigLvl = 0, .functionEnable = SRC_HPD
    },
    // GPIO26
    .CFG_STDBY_GPIO_TO[1] = {
        .gpioChkEnable = 0, .gpioTrigLvl = 0, .functionEnable = SRC_CEC
    },
    // GPIO27
    .CFG_STDBY_GPIO_TO[2] = {
        .gpioChkEnable = 0, .gpioTrigLvl = 0, .functionEnable = 0
    },
    // GPIO28
    .CFG_STDBY_GPIO_TO[3] = {
        .gpioChkEnable = 0, .gpioTrigLvl = 0, .functionEnable = 0
    },
    // GPIO29
    .CFG_STDBY_GPIO_TO[4] = {
        .gpioChkEnable = 0, .gpioTrigLvl = 0, .functionEnable = SRC_IR
    },
    // GPIO30
    .CFG_STDBY_GPIO_TO[5] = {
        .gpioChkEnable = 0, .gpioTrigLvl = 0, .functionEnable = SRC_PWRKEY
    },
	//configure option for PMC
	.CFG_POWER_KEY_TO =
	{
		.Cfg_Enable = 0,
	},
	//configure option for dsp tool
	.CFG_AUD_TO =
	{
		//[Audio Hardware] I2S output channel switch for External(off chip)-DAC
		/* {d0_left, d0_right, d1_l, d1_r, d2_l, d2_r, d3_l, d3_r, d4_l, d4_r, d5_l, d5_r, d6_l, d6_r, d7_l, d7_r}
		   Channel_Map[0] : I2S out data0 left channel selection
		   Channel_Map[1] : I2S out data0 right channel selection.
		   Channel_Map[2] : I2S out data1 left channel selection
		   Channel_Map[3] : I2S out data1 right channel selection.
		   Channel_Map[4] : I2S out data2 left channel selection
		   Channel_Map[5] : I2S out data2 right channel selection.
		   Channel_Map[6] : I2S out data3 left channel selection
		   Channel_Map[7] : I2S out data3 right channel selection.
		   Channel_Map[8] : I2S out data4 left channel selection
		   Channel_Map[9] : I2S out data4 right channel selection.
		   Channel_Map[10] : I2S out data5 left channel selection
		   Channel_Map[11] : I2S out data5 right channel selection.
		   Channel_Map[12] : I2S out data6 left channel selection
		   Channel_Map[13] : I2S out data6 right channel selection.
		   Channel_Map[14] : I2S out data7 left channel selection
		   Channel_Map[15] : I2S out data7 right channel selection.
		   */
		
		.Channel_Map = {L, R, Ls, Rs, C, Sub, Ltf, Rtf, Lrs, Rrs, Ltr, Rtr, Lw, Rw, Ltm, Rtm},

		//configure option for DSP Support Function switch
		.DSP_SUPPORT = SUPPORT_DEC1 | SUPPORT_ATMOS | SUPPORT_APP |
		                SUPPORT_INTADC | SUPPORT_I2S | SUPPORT_MIXER0 |
		                SUPPORT_MIXER1 | SUPPORT_XEFF0 |SUPPORT_XEFF1,
#if defined(SUPPORT_HE_FLOW) && (SUPPORT_HE_FLOW > 0)
		.AUDIO_FLOW = AUDFLOW_HE,//Audio flow for High-End SOC
#else
		.AUDIO_FLOW = AUDFLOW_LC,//Audio flow for Low-Cost SOC
#endif
		.I2S_IN =
		{
			// [Audio Hardware] configure option for External_ADC_I2S_IN_2CH
			.CLOCK_EXT_ADC = 2, // 0:master mode,use CLKGENA i2s in clock, 1:master mode,use CLKGENA i2s out clock; 2: slave mode,use i2s in clock, 3:slave mode,use i2s out clock
			.MODE_EXT_ADC = 0, 	   // 0:left justity, 1: right justity
			.LRCK_CYCLE_EXT_ADC = 2,  // 0:32 1:48, 2:64, BCKs per LRCK
			.BCK_SYNC_EDGE_EXT_ADC = 0, // 0:data is valid on rising edge of BCK, 1 falling edge
			.DATA_WIDTH_EXT_ADC = 3,  // 0:16, 1:18, 2:20, 3:24 bits
			.DATA_PARITY_EXT_ADC = 0, // 0:Left Channel (LRCK is Low), 1:Left Channel (LRCK is High)
			.DATA_DELAY_EXT_ADC = 1, //  1:The MSB of PCM data to external DAC delays one BCK cycle after LRCK transition

			// [Audio Hardware] configure option for HDMI_Repeater_I2S_IN_8CH
			.CLOCK_HDMI_IN = 2,  // 0:master mode,use CLKGENA i2s in clock, 1:master mode,use CLKGENA i2s out clock; 2: slave mode,use i2s in clock, 3:slave mode,use i2s out clock
			.MODE_HDMI_IN = 0,		  // 0:left justity, 1: right justity
			.LRCK_CYCLE_HDMI_IN = 2,  // 0:32 1:48, 2:64, BCKs per LRCK
			.BCK_SYNC_EDGE_HDMI_IN = 0, // 0:data is valid on rising edge of BCK, 1 falling edge
			.DATA_WIDTH_HDMI_IN = 3,  // 0:16, 1:18, 2:20, 3:24 bits
			.DATA_PARITY_HDMI_IN = 0, // 0:Left Channel (LRCK is Low), 1:Left Channel (LRCK is High)
			.DATA_DELAY_HDMI_IN = 1, //  1:The MSB of PCM data to external DAC delays one BCK cycle after LRCK transition
		},
		.I2S_OUT =
		{
			// [Audio Hardware] configure option for I2S_OUT_16CH
			.CLOCK_I2S_OUT = 0,       // 0:master mode,use CLKGENA i2s out clock, 1:master mode,use CLKGENA i2s in clock; 2: slave mode,use i2s out clock, 3:slave mode,use i2s in clock
			.MODE = 0,        // 0:left justity, 1: right justity
			.LRCK_CYCLE = 2,  // 0:32 1:48, 2:64, BCKs per LRCK
			.BCK_SYNC_EDGE = 0, // 0:data is valid on rising edge of BCK, 1 falling edge
			.DATA_WIDTH = 3,  // 0:16, 1:18, 2:20, 3:24 bits
			.DATA_PARITY = 0, // 0:Left Channel (LRCK is Low), 1:Left Channel (LRCK is High)
			.DATA_DELAY = 1, //  1:The MSB of PCM data to external DAC delays one BCK cycle after LRCK transition
		},
		.DSP_Mode = 0,			// 0: normal mode, 1: bypass mode(bypass all app,close small signal mute mechanism), 2: decoder certification mode, 3: app certification		
		.KOK_ADC = 0,			// 0: Internal ADC, Mic 2->1 mix; 1: External ADC, Mic 2->1 mix; 2: Internal ADC, Stereo MIC; 3:External ADC, Stereo MIC		
		.DDPCfg =		
		{			
			.OutMode = 7,		// 1: 1.0(C) output, 2:2.0(L R) output, 3:3.0(L C R) output, 4:2/1(L R Mono S), 5:3/1(L C R Mode S), 6: 2/2(L R Ls Rs), 7:3/2(L C R Ls Rs)			
			.OutLfe = 1,		// 0: lfe out off, 1: lfe out enable			
			.CompMode = 1,		// 1: line mode(default),  0: RF mode			
			.StereoMode = 0,	// 0: auto , 1: LtRt downmix, 2: LoRo downmix		
		},		
		.DTSCfg =		
		{		    
			.OutMode = 0,		// 0: auto,  1: 1.0(C), 2:1.1(C/LFE), 3:2.0(L/R), 4:2.1(L/R/lfe), 5:(LT/RF), 6: 3.0(L/C/R), 7:3.1(L/C/R/lfe)		                       
								// 8: (L/R/CS),  9: (L/R/CS/LFE), A:(L/C/R/CS), B:(L/C/R/CS/LFE), C:(L/R/LS/RS), D:(L/R/LS/RS/LFE)		                        
								// E: (L/C/R/LS/RS/LFE),  F: (L/C/R/LS/RS/LFE/LHS/RHS), 0X10:(L/C/R/LFE/LSR/RSR/LSS/RSS), 0X11:(L/C/R/LS/RS/LFE/LH/RH)		                        
								// 0X12: (L/C/R/LS/RS/LFE/LSR/RSR),  0X13: (L/C/R/LS/RS/LFE/CS/CH), 0X14:(L/C/R/LS/RS/LFE/CS/OH), 0X15:(L/C/R/LS/RS/LFE/LW/RW)		                        
								// 0X16: (L/C/R/LS/RS/LFE/CS)            .StereoMode = 0,	// 0: auto , 1: LtRt downmix, 2: LoRo downmix		
		},		
		.DSP_Downmix = 1,       // 0x0: Bypass, 0x1: auto downmix, 0x3: 2.0 output, 0x23: 2.1 output, 0x13: 3.0 output;		                        
		// 0x33: 3.1 output; 0xf: 4.0 output; 0x3f: 5.1 output;
	},
	//configure option for MCU IF
	.CFG_MCUIF_TO =
	{
		.baudRate = eUART_BR_115200,	// Which is the baud of UART1 if MCU_IF is communication with MCU via UART1. It can be eUART_BR_115200, eUART_BR_921600, eUART_BR_1843200 or eUART_BR_3686400.
	},
	//configure option for Upgrade
	.CFG_UPGRADE_TO =
	{
		.psbDevName = "USB",					// It can be "USB" or "SD", the system will load the upgrade file from the device
		.psbSpUpdateFileName = "spupdate.bin",	// The default name of the upgrade file, the system will use this name first to find the upgrade bin
	},
	//configure option for BT
	.CFG_BT_TO =
	{
		.PlatformRole =  BT_MAIN, //UNUSED
#ifdef USERAPP_SUPPORT_BT
		.BtAutoWork = ON, //ON: config to BT auto run, OFF: config to BT user ctrl to run
#else
		.BtAutoWork = OFF, //ON: config to BT auto run, OFF: config to BT user ctrl to run
#endif
		.IsSurportTwsSwitch = FALSE, //UNUSED
		.BtSdkCfg.BtMainCfg =
		{
			.address = {0x02,0x00,0x10,0x83,0x15,0x00,}, //BT address setting
			.LocalName = "SUNPLUS_TEST", //BT name setting
			.IsIgnoreSwitchSrc = FALSE, //TRUT: do not disconnect BT connection when swtich out BT source; FALSE: disconnect BT connection when swtich out BT source
			.IsIgnoreDevTimeout = FALSE, //TRUE: do not support BT auto connect back when BT connection timeout; FALSE: support BT auto connect back when BT connection timeout
			.AutoRetryTimes = 0xff, // 0xff : BT auto connect always until BT device is connected; others: BT auto connect retry times
			.AutoLatency = 5, // BT auto connect interval
			.AutoWorkState = ON, // ON: support BT auto connect, OFF: do not suppot  BT auto connect
			.Autorole = AUTO_PHONE_TWS, //UNUSED

#ifdef USERAPP_SUPPORT_WSS
			.TwsWork = ON, //UNUSED
#else
			.TwsWork = OFF, //UNUSED
#endif

#ifdef USERAPP_SUPPORT_BT
			.SinkMode = WSS, //UNUSED
#else
			.SinkMode = SINK_NONE, //UNUSED
#endif
			.IsConnectedSetDelay = FALSE, //UNUSED
			.pHeadphoneListName = BTHeadphoneDesName, //UNUSED
			.pSubListName = BTSubDesName, //UNUSED
			.HintWorkState = ON, //ON: support BT sound hint, OFF: do not support BT sound hint
			.TransMode = OFF, //UNUSED
		},
	},

	.CFG_I2C_TO =
	{
		//the i2c for EDID reading must be low speed to fit EDID spec (default EDID i2c is i2c3)
		.Cfg_CLK_I2C0 = 96,// set hw i2c default freq(kHz)
       	.Cfg_CLK_I2C1 = CFG_I2C_SPEED_L,
		.Cfg_CLK_I2C2 = CFG_I2C_SPEED_H,
		.Cfg_CLK_I2C3 = CFG_I2C_SPEED_L,
		.Cfg_Mode_I2C1 = CFG_I2C_OPEN_DRAIN,
		.Cfg_Mode_I2C2 = CFG_I2C_OPEN_DRAIN,
		.Cfg_Mode_I2C3 = CFG_I2C_OPEN_DRAIN,
	},
	//configure option for Usersetting
	.CFG_SETTING_TO =
	{
		.wSaveDataRange = CFG_SETTING_SAVE_RANGE,		// The total size of the Setting Flash (in sectors). The total size need to include Setting, System and User.
		.wCustomerUseRange = CFG_SETTING_USER_RANGE,	// The number of sectors of the total flash that the customer wants to use. e.g. The maximum flash size used to store USER SETTING
	},

	.CFG_BTHEAP_TO =
	{
#ifdef USERAPP_SUPPORT_BT
		.nSize = 100 * 1024, //BT buffer size
#else
		.nSize = 0,
#endif
	},

	.CFG_VA_TO =
	{
		//configuration option for VA(Voice Assistant)
		.BGMMuteTime = BGM_MUTE_TIME,//Background-Music mute time while VA start playback,default:10s
		.BGMResumeTime = BGM_RESUME_TIME,//Background-Music resume time while VA exit playback,default:3s
	},

	.CFG_USB_TO =
	{
		//host   mode: eUSB_HOST_MODE   (play media files)
		//device mode: eUSB_DEVICE_MODE (connect tools)
		.DefaultMode = eUSB_HOST_MODE,
	},

	//configuration option for Hint sound
	.CFG_HINT_TO =
	{
		/* Play(response) mode of the hint sound type of single audio file.
		HINT_PROMPTLY_PLAY: Stop the previous hint immediately and play the new hint when a new hint is triggered;
		HINT_SMOOTH_PLAY: The new hint will be played after the previous one is finished when a new hint is triggered.*/
		.singlePlayMode = HINT_PROMPTLY_PLAY,
		.xeffBufSize = XEFF_BUF_SIZE,	//xeffect buffer size,maximum supported hint sound audio size.
	},

    .CFG_ROTARY_TO =
    {
        .rotary1Enable = 0, // 0: disable, 1: enable rotary encoder 1 (gpio5, 6)
        .rotary2Enable = 0,	// 0: disable, 1: enable rotary encoder 2 (gpio10, 11)
        .rotary3Enable = 0,	// 0: disable, 1: enable rotary encoder 3 (gpio15, 16)
        .rotary4Enable = 0,	// 0: disable, 1: enable rotary encoder 4 (gpio46, 47)
        .debounceTh = 1,	// 0: disable, 1: enable debounce action
    }
};

/*---------------------------------------------------------------------------*
 *                          FUNCTION   DECLARATIONS                          *
 *---------------------------------------------------------------------------*/

CFG_SDK_T const *GetSDKConfig(void)
{
	return &gstSDKConfig;
}

//device id
/*#define BT_LOCALEVICE_VENDORID			0x04E8
#define BT_LOCALDEVICE_PRODUCTID		0x9000
#define BT_LOCALDEVICE_VERSION			0x0000
#define BT_LOCALDEVICE_VERDORIDSOURCE 	0x0100*/

#define BT_LOCALEVICE_VENDORID			0x08E7 //Barrot
#define BT_LOCALDEVICE_PRODUCTID		0xF000
#define BT_LOCALDEVICE_VERSION			0xF000
#define BT_LOCALDEVICE_VERDORIDSOURCE 	0x0F00

#define LICENSE \
0x0a,0xca,0x94,0xa0,\
0x79,0x40,0x62,0xed,\
0x52,0x69,0x95,0xfb,\
0xbc,0x37,0xb8,0x76,\
0x40,0xbf,0x31,0x11,\
0x4c,0xf1,0x2f,0xcd,\
0x87,0x6b,0x81,0x22,\
0x80,0x1c,0x93,0xe5,\
0xf7,0x8a,0x81,0xb4,\
0xf8,0xad,0xe9,0xac,\
0xb5,0x90,0x2f,0xa4,\
0xc7,0xe1,0x4b,0xb6,\

static CFGPRIVATE_BT_T gstBTSDKPrivateConfig =
{
	.VerdorIdSource = BT_LOCALDEVICE_VERDORIDSOURCE,
	.vid = BT_LOCALEVICE_VENDORID,
	.pid = BT_LOCALDEVICE_PRODUCTID,
	.version = BT_LOCALDEVICE_VERSION,
	.avrcpCtServiceName = "Woofer & EQ\n2",
	.avrcpCtProviderName = "HW-SUNPLUS",
	.license = {LICENSE}, //i1188-se
};

CFGPRIVATE_BT_T const *GetBTSDKPrivateConfig(void)
{
	return &gstBTSDKPrivateConfig;
}

// Setting for vim, please keep these 2 lines at end of the file.
// vim: shiftwidth=4 softtabstop=4 tabstop=4 noexpandtab
