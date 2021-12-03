/******************************************************************************/
/**
 *
 * \file	pinmux.h
 *
 * \brief	This file provides the SPA100 pin mux config \n
 *
 * \note	Copyright (c) 2020 Sunplus Technology Co., Ltd. \n
 *			All rights reserved.
 *
 *\author	email@sunplus.com
 *\version	v0.01
 *\date		2020/04/15
 ******************************************************************************/
#ifndef __POSIX_PINMUX_H__
#define __POSIX_PINMUX_H__

#define GPIO_PROHIBITED_USE	(-1)

/** io functioni mode*/
#define IO_MODULEMODE	(0)
#define IO_GPIO			(1)
#define IO_OTHERUSE		(2)
/** io master mode*/
#define	IO_RISC			(1)
#define IO_IOP			(0)
/** io dir */
#define IO_INPUT		(0)
#define IO_OUTPUT		(1)
/** io level */
#define IO_HIGH			(1)
#define IO_LOW			(0)
/** io opendrain */
#define IO_OD_ENABLE	(1)
#define IO_OD_DISABLE	(0)
/** io inverse */
#define IO_INVERSE_ENABLE	(1)
#define IO_INVERSE_DISABLE	(0)


#define GMX_MAX_PIN		(53)
#define IVMX_MAX_PIN	(9)
#define IC_TOTAL_PIN	(66)


/**
*\brief  SPA100 pinmux conifg header
*/
struct _HEADDEF
{
	int		sdHeadID;	/*!< pinmux header ID  Defaule: 0xc75a*/
	int		sdIC;		/*!< SPA100 IC model*/
	int		sdOffset;	/*!< header offset value*/
	int		sdLength;	/*!< pinmux conifg table size */
	int		sdDSLength;
	int		sdModListOffset;	/*!< Module list offset value*/
	int		sdModListLength;	/*!< Module list size */
};

/**
*\brief  SPA100 pin conifg struct
*/
struct _PINDEF
{
	char bModuleID;		/*!<  the module ID white defined in SPA100 Module list */
	char bPinName;		/*!<  the pin ID white defined in Module pin name emun */
	char bPinFunc: 2,	/*!<  GPIO Function 0: mudile mode ,1: gpio mode , 2: reuse pin mode */
		 bPinStatus: 6;	/*!<  pin status. 0: no one use, 1: gpio ,other: other Module  */
	char bReuseModuleID;/*!< the reuse pin module ID white defined in SPA100 Module list  */
};

typedef union
{
	int sdPinIndex;
	struct _PINDEF stPinDef;
} PINCFG;

/**
*\brief  SPA100 Module list
*/

#define PINMUX_TABLE    \
	X(PIN_RESERVE = 0	,"nouse")			\
	X(GPIO = 1			,"gpio")			\
	X(SPIFLASH			,"spiflash")		\
	X(SPIFLASH_QUAD		,"spiflash_quad")	\
	X(UART0				,"uart0")			\
	X(UART1				,"uart1")			\
	X(BT				,"bt")				\
	X(UART3				,"uart3")			\
	X(PWM0				,"pwm0")			\
	X(PWM1				,"pwm1")			\
	X(PWM2				,"pwm2")			\
	X(CARD				,"card")			\
	X(SPI_MASTER		,"spi_master")		\
	X(I2S_OUT_8CH		,"i2sout_8ch")		\
	X(I2S_OUT_16CH		,"i2sout_16ch")		\
	X(I2S_IN_2CH		,"i2sin_2ch")		\
	X(I2S_IN0_8CH		,"i2sin_8ch")		\
	X(SPDIF_OUT			,"spdifout")		\
	X(SPDIF_IN0			,"spdifin0")		\
	X(SPDIF_IN1			,"spdifin1")		\
	X(SPDIF_IN2			,"spdifin2")		\
	X(MST_I2C			,"mst_i2c")			\
	X(SLA_I2C			,"sla_i2c")			\
	X(ADC0_PIN			,"adc0")			\
	X(ADC1_PIN			,"adc1")			\
	X(ADC2_PIN			,"adc2")			\
	X(ADC3_PIN			,"adc3")			\
	X(HDMI_REPEATER		,"hdmi_repeater")	\
	X(HDMI_PIN			,"hdmi")			\
	X(TDMTX				,"tdmtx")			\
	X(PDM_RX			,"pdm_rx")			\
	X(INTR0_GPIO		,"intr0_gpio")		\
	X(INTR1_GPIO		,"intr1_gpio")		\
	X(INTR2_GPIO		,"intr2_gpio")		\
	X(INTR3_GPIO		,"intr3_gpio")		\
	X(IR				,"ir")				\
	X(POWERKEY			,"powerkey")		\
	X(I2C1				,"i2c1_sw")			\
	X(I2C2				,"i2c2_sw")			\
	X(I2C3				,"i2c3_sw")			\
	X(I2C4				,"i2c4_sw")			\
	X(I2C5				,"i2c5_sw")			\
	X(AMP_PIN			,"amp_pin")			\
	X(VFD_TM16312		,"vfd_tm16312")		\
	X(VFD_PT6315		,"vfd_pt6315")		\
	X(EXT_POWER			,"ext_power")		\
	X(AUDIO_MUTE		,"audio_mute")		\
	X(VBUS_EN			,"vbus_en")			\
	X(MAX_MODULE		,"max_module")

// SPA100 Module list definition
#define X(pin, d)	pin,
typedef enum
{
	PINMUX_TABLE
} ModuleList;
#undef X

/**
*\brief  SPA100 Module pin name emun
*/
#define SPIFLASHPIN (4)
enum _eMod_SPIFLASH
{
	SPIFLASH_D0 = 0,
	SPIFLASH_CLK,
	SPIFLASH_D1,
	SPIFLASH_CE,
};

#define SPIFLASHQUADPIN (2)
enum _eMod_SPIFLASH_QUAD
{
	SPIFLASH_QUAD_D2,
	SPIFLASH_QUAD_D3,
};

#define UART0PIN (2)
enum _eMod_UART0
{
	UART0_TX = 0,
	UART0_RX,
};

#define UART1PIN (2)
enum _eMod_UART1
{
	UART1_TX = 0,
	UART1_RX,
};

#define BTPIN (5)
enum _eMod_BT
{
	UART2_TX = 0,
	UART2_RX,
	UART2_CTS,
	UART2_RTS,
	BT_EN,
};


#define UART3PIN (2)
enum _eMod_UART3
{
	UART3_TX = 0,
	UART3_RX,
};



#define CARDPIN (7)
enum _eMod_CARD
{
	SD_D3 = 0,
	SD_D2,
	SD_D1,
	SD_D0,
	SD_CLK,
	SD_CMD,
	SD_SENCE,
};

#define SPIMASTERPIN (4)

enum _eMod_SPI_MASTER
{
	SPI_DI = 0,
	SPI_DO,
	SPI_CLK,
	SPI_EN,
};

#define I2SOUT8CHPIN (7)
enum _eMod_I2SOUT8CH
{
	I2S_OUT_LRCK = 0,
	I2S_OUT_MCK,
	I2S_OUT_BCK,
	I2S_OUT_D0,
	I2S_OUT_D1,
	I2S_OUT_D2,
	I2S_OUT_D3,
};

#define I2SOUT16CHPIN (4)
enum _eMod_I2SOUT16CH
{
	I2S_OUT_D4 = 0,
	I2S_OUT_D5,
	I2S_OUT_D6,
	I2S_OUT_D7,
};

#define I2SIN2CHPIN (4)
enum _eMod_I2SIN2CH
{
	I2S_IN_2CH_MCK = 0,
	I2S_IN_2CH_BCK,
	I2S_IN_2CH_LRCK,
	I2S_IN_2CH_D0,
};

#define I2SIN8CHPIN (7)
enum _eMod_I2SIN8CH
{
	I2S_IN0_8CH_LRCK = 0,
	I2S_IN0_8CH_MCK,
	I2S_IN0_8CH_BCK,
	I2S_IN0_8CH_D0,
	I2S_IN0_8CH_D1,
	I2S_IN0_8CH_D2,
	I2S_IN0_8CH_D3,
};


#define MSTI2CPIN	(2)
enum _eMod_MST_I2C
{
	MST_I2C_SCL = 0,
	MST_I2C_SDA,
};

#define SLAI2CPIN	(2)
enum _eMod_SLA_I2C
{
	SLA_I2C_SCL = 0,
	SLA_I2C_SDA,
};


#define HDMIREPEATERPIN	(6)
enum _eMod_HDMI_REPEATER
{
	HDMIRX_LRCK = 0,
	HDMIRX_BCK,
	HDMIRX_D0,
	HDMIRX_D1,
	HDMIRX_D2,
	HDMIRX_D3,
};

#define HDMIPIN	(2)
enum _eMod_HDMI
{
	HDMI_HPD = 0,
	HDMI_CEC,
};


#define TDMTXPIN	(7)
enum _eMod_TDMTX
{
	TDMTX_XCK = 0,
	TDMTX_BCK,
	TDMTX_SYNC,
	TDMTX_D16,
	TDMTX_D4A,
	TDMTX_D4B,
	TDMTX_D8,
};

#define PDMRXPIN	(3)
enum _eMod_PDMRX
{
	PDMRX_BCK = 0,
	PDMRX_D0,
	PDMRX_D1,
};

#define I2C1PIN	(2)
enum _eMod_I2C1
{
	I2C1_SCL = 0,
	I2C1_SDA,
};

#define I2C2PIN	(2)
enum _eMod_I2C2
{
	I2C2_SCL = 0,
	I2C2_SDA,
};

#define I2C3PIN	(2)
enum _eMod_I2C3
{
	I2C3_SCL = 0,
	I2C3_SDA,
};

#define I2C4PIN	(2)
enum _eMod_I2C4
{
	I2C4_SCL = 0,
	I2C4_SDA,
};

#define I2C5PIN	(2)
enum _eMod_I2C5
{
	I2C5_SCL = 0,
	I2C5_SDA,
};

#define AMPPIN	(3)
enum _eMod_AMP_PIN
{
	RST = 0,
	PD,
	OTW,
};


#define VFD_TM16312_PIN	(3)
enum _eMod_VFD_TM16312
{
	VFD_TM16312_STB = 0,
	VFD_TM16312_CLK,
	VFD_TM16312_DIO,
};

#define VFD_PT6315_PIN	(4)
enum _eMod_VFD_PT6315
{
	VFD_PT6315_STB = 0,
	VFD_PT6315_CLK,
	VFD_PT6315_DI,
	VFD_PT6315_DO,
};

#define EXTPOWERCTLPIN	(1)
enum _eMod_EXTPOWERCTL
{
	EXT_POWER_CTL = 0,
};

#define AUDIOMUTECTLPIN	(1)
enum _eMod_AUDIOMUTECTL
{
	AUDIO_MUTE_CTL = 0,
};

#define VBUSENCTLPIN	(1)
enum _eMod_VBUSENCTL
{
	VBUS_EN_CTL = 0,
};

/**
*\brief  SPA100 Special Module pinmux emun
*/

enum _Pinmux_SPIFLASH
{
	SPIFLASH_X1 = 0x1,
	SPIFLASH_X2,
};

enum _Pinmux_SPIFLASH_QUAD
{
	SPIFLASH_QUAD_DISABLE = 0x0,
	SPIFLASH_QUAD_X1,
	SPIFLASH_QUAD_X2,
};

enum _Pinmux_UART0
{
	UART0_X1 = 0x1,
	UART0_X2,
	UART0_X3,
	UART0_X4,
};

enum _Pinmux_UART3
{
	UART3_X1 = 0x1,
	UART3_X2,
	UART3_X3,
	UART3_X4,
};


/******************************************************************************************/
/**
 * \fn		int Pinmux_Init(int table_addr)
 *
 * \brief		initializes SPA100 pinmux .
 *
 * \param		sdTable_addr : (Input) The pinmux config pin.
 *
 * \return		\e 0 on success. \n
 *			\e other on fail. \n
 *
 ******************************************************************************************/
int Pinmux_Init(int sdTable_addr);

/******************************************************************************************/
/**
 * \fn		int Pinmux_UnInit(void)
 *
 * \brief		uninitializes SPA100 pinmux .
 *
 * \param		void
 *
 * \return		\e 0 on success. \n
 *			\e other on fail. \n
 *
 ******************************************************************************************/
int Pinmux_UnInit(void);

/******************************************************************************************/
/**
 * \fn		int Pinmux_PinFunc_Get(int sdPinNum)
 *
 * \brief		get SPA100 pin function mode .
 *
 * \param		sdPinNum  : (input)		Pin Number
 * \param		eModuleID : (input)		Module ID
 *
 * \return	\e SPA100 functoin mode\n
 *			\e -1 this pin is not config. \n
 *
 ******************************************************************************************/

int Pinmux_PinFunc_Get(int sdPinNum, int eModuleID);

/******************************************************************************************/
/**
 * \fn			static int Pinmux_ModulePin_Find(ModuleList eModuleID, int sdPinID)
 *
 * \brief		get SPA100 module pin index .
 *
 * \param		eModuleID  : (input)	SPA100 Module ID
 * \param		sdPinID    : (input)	SPA100 Module Pin Name
 *
 * \return		\e SPA100 Pin index\n
 *			\e -1 this module pin is not config. \n
 *
 ******************************************************************************************/
int Pinmux_ModulePin_Find(int eModuleID, int sdPinID);

/******************************************************************************************/
/**
 * \fn			static int Pinmux_ModulePin_Get(ModuleList eModuleID)
 *
 * \brief		get SPA100 module pin number .
 *
 * \param		eModuleID  : (input)	SPA100 Module ID
 *
 * \return		\e SPA100 Pin total number\n
 *			\e -1 this module pin is not config. \n
 *
 ******************************************************************************************/
int Pinmux_ModulePin_Get(int eModuleID);

/******************************************************************************************/
/**
 * \fn		int Pinmux_Module_Alloc(int sdModuleID)
 *
 * \brief		Alloc the SPA100 module pin
 *
 * \param		sdModuleID  : (input)  SPA100 Module ID
 *
 * \return		\e 0 on success. \n
 *				\e other  same  pin is used by other module. \n
 *
 ******************************************************************************************/
int Pinmux_Module_Alloc(int sdModuleID);

/******************************************************************************************/
/**
 * \fn			int Pinmux_Module_Free(int sdModuleID)
 *
 * \brief		Alloc the SPA100 pin to GPIO use
 *
 * \param		sdModuleID  : (input)  SPA100 pin number
 *
 * \return		\e 0 on success. \n
 *			\e other  same  pin is used by other module. \n
 *
 ******************************************************************************************/
int Pinmux_Module_Free(int sdModuleID);

/******************************************************************************************/
/**
 * \fn			int Pinmux_GPIO_Alloc(int sdPinNum, int sdGPIOFunc, int sdGPIOMaster)
 *
 * \brief		Alloc the SPA100 pin to GPIO use
 *
 * \param		ePinNum  : (input)  SPA100 pin number
 * \param		sdGPIOFunc  : (input)  SPA100 pin GPIO Function mode
 * \param		sdGPIOMaster  : (input)  SPA100 pin GPIO Master mode
 *
 * \return		\e 0 on success. \n
 *			\e other  same  pin is used by other module. \n
 *
 ******************************************************************************************/
int Pinmux_GPIO_Alloc(int sdPinNum, int sdGPIOFunc, int sdGPIOMaster);

/******************************************************************************************/
/**
 * \fn		int Pinmux_GPIO_Free(int sdPinNum))
 *
 * \brief		Alloc the SPA100 pin to GPIO use
 *
 * \param		ePinNum  : (input)  SPA100 pin number
 *
 * \return		\e 0 on success. \n
 *			\e other  same  pin is used by other module. \n
 *
 ******************************************************************************************/
int Pinmux_GPIO_Free(int sdPinNum);

/******************************************************************************************/
/**
 * \fn		int Pinmux_GPIO_CheckStatus(int sdPinNum)
 *
 * \brief		Check the SPA100 pin status
 *
 * \param		ePinNum  : (input)  SPA100 pin number
 *
 * \return		\e 0 on success. \n
 *
 ******************************************************************************************/
int Pinmux_GPIO_CheckStatus(int sdPinNum);

/******************************************************************************************/
/**
 * \fn			int GPIO_Funtion_Set(int sdPinNum, int sdGPIOFunc)
 *
 * \brief		Set GPIO Function mode
 *
 * \param		sdPinNum : (input)  SPA100 pin number
 * \param		sdGPIOFunc : (intput) function mode
 *
 * \return		\e 0 on success. \n
 *				\e other  same  pin is used by other module. \n
 *
 ******************************************************************************************/
int GPIO_Funtion_Set(int sdPinNum, int sdGPIOFunc);

/******************************************************************************************/
/**
 * \fn			int GPIO_Funtion_Get(int sdPinNum)
 *
 * \brief		Get GPIO Function mode
 *
 * \param		sdPinNum : (input)  SPA100 pin number
 *
 * \return		\e pin function mode on success. \n
 *				\e other  false. \n
 *
 ******************************************************************************************/
int GPIO_Funtion_Get(int sdPinNum);

/******************************************************************************************/
/**
 * \fn			int GPIO_Master_Set(int sdPinNum, int sdGPIOFunc)
 *
 * \brief		Set GPIO master mode
 *
 * \param		sdPinNum : (input)  SPA100 pin number
 * \param		sdGPIOMaster : (intput) master mode
 *
 * \return		\e 0 on success. \n
 *				\e other  same  pin is used by other module. \n
 *
 ******************************************************************************************/
int GPIO_Master_Set(int sdPinNum, int sdGPIOMaster);

/******************************************************************************************/
/**
 * \fn			int GPIO_Master_Get(int sdPinNum)
 *
 * \brief		Get GPIO Master mode
 *
 * \param		sdPinNum : (input)  SPA100 pin number
 *
 * \return		\e pin Master mode on success. \n
 *				\e other  false. \n
 *
 ******************************************************************************************/
int GPIO_Master_Get(int sdPinNum);

/******************************************************************************************/
/**
 * \fn			int GPIO_OE_Set(int sdPinNum, int sdGPIOOE)
 *
 * \brief		Set GPIO input / output mode
 *
 * \param		sdPinNum : (input)  SPA100 pin number
 * \param		sdGPIOOE : (intput) OE mode
 *
 * \return		\e 0 on success. \n
 *				\e other  same  pin is used by other module. \n
 *
 ******************************************************************************************/
int GPIO_OE_Set(int sdPinNum, int sdGPIOOE);

/******************************************************************************************/
/**
 * \fn			int GPIO_OE_Read(int sdPinNum)
 *
 * \brief		Read GPIO output enable / disable
 *
 * \param		sdPinNum : (input) pin number
 *
 * \return		\e 1  gpio output enable. \n
 *				\e 0  gpio output disable. \n
 *
 ******************************************************************************************/
int GPIO_OE_Read(int sdPinNum);

/******************************************************************************************/
/**
 * \fn			int GPIO_OpenDrain_Set(int sdPinNum, int sdGPIOOD)
 *
 * \brief		Set GPIO opendrain mode
 *
 * \param		sdPinNum : (input)  SPA100 pin number
 * \param		sdGPIOOD : (intput) opendrain mode
 *
 * \return		\e 0 on success. \n
 *				\e other  same  pin is used by other module. \n
 *
 ******************************************************************************************/
int GPIO_OpenDrain_Set(int sdPinNum, int sdGPIOOD);

/******************************************************************************************/
/**
 * \fn			int GPIO_Input_Inverse_Set(int sdPinNum, int sdGPIOIV)
 *
 * \brief		Set GPIO Input Inverse mode
 *
 * \param		sdPinNum : (input)  SPA100 pin number
 * \param		sdGPIOIV : (input) GPIO Input Inverse
 *
 * \return		\e 0 on success. \n
 *				\e other  same  pin is used by other module. \n
 *
 ******************************************************************************************/
int GPIO_Input_Inverse_Set(int sdPinNum, int sdGPIOIV);


/******************************************************************************************/
/**
 * \fn			int GPIO_Output_Inverse_Set(int sdPinNum, int sdGPIOIV)
 *
 * \brief		Set GPIO Output Inverse mode
 *
 * \param		sdPinNum : (input)  SPA100 pin number
 * \param		sdGPIOIV : (input) GPIO Output Inverse
 *
 * \return		\e 0 on success. \n
 *				\e other  same  pin is used by other module. \n
 *
 ******************************************************************************************/
int GPIO_Output_Inverse_Set(int sdPinNum, int sdGPIOIV);


/******************************************************************************************/
/**
 * \fn			int GPIO_Driver_Strength_Set(int sdPinNum, int value)
 *
 * \brief		Set GPIO Driver Strength
 *
 * \param		sdPinNum : (input)  SPA100 pin number
 * \param		value : (input) Driver Strength value 0~3
 *
 * \return		\e 0 on success. \n
 *				\e other  same  pin is used by other module. \n
 *
 ******************************************************************************************/
int GPIO_Driver_Strength_Set(int sdPinNum, int value);

/******************************************************************************************/
/**
 * \fn			int GPIO_Driver_Strength_Read(int sdPinNum)
 *
 * \brief		GPIO_Driver_Strength_Read
 *
 * \param		sdPinNum : (input) pin number
 *
 * \return		\e pin value. \n
 *				\e -1 error . \n
 *
 ******************************************************************************************/
int GPIO_Driver_Strength_Read(int sdPinNum);

/******************************************************************************************/
/**
 * \fn			int GPIO_OE_Toggle(int sdPinNum, int sdGPIOOE)
 *
 * \brief		first, set GPIO input / output mode to the opposite of sdGPIOOE, then set to sdGPIOOE
 *
 * \param		sdPinNum : (input)  SPA100 pin number
 * \param		sdGPIOOE : (intput) OE mode
 *
 * \return		\e 0 on success. \n
 *				\e other  same  pin is used by other module. \n
 *
 ******************************************************************************************/
int GPIO_OE_Toggle(int sdPinNum, int sdGPIOOE);

/******************************************************************************************/
/**
 * \fn			int GPIO_Output_Write(int sdPinNum, int sdGPIOValue)
 *
 * \brief		Set GPIO high / low level
 *
 * \param		sdPinNum : (input)  SPA100 pin number
 * \param		sdGPIOValue : (intput) high / low
 *
 * \return		\e 0 on success. \n
 *				\e other  same  pin is used by other module. \n
 *
 ******************************************************************************************/
int GPIO_Output_Write(int sdPinNum, int sdGPIOValue);

/******************************************************************************************/
/**
 * \fn			int GPIO_Intput_Read(int sdPinNum)
 *
 * \brief		Set GPIO master mode
 *
 * \param		sdPinNum : (input)  SPA100 pin number
 *
 * \return		\e pin high / low levwl. \n
 *				\e -1 error . \n
 *
 ******************************************************************************************/
int GPIO_Intput_Read(int sdPinNum);

#endif //__POSIX_PINMUX_H__

