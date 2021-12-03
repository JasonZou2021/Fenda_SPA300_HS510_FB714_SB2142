/******************************************************************************/
/**
*
* \file	 VFD_Drv.c
*
* \brief	This file provides the vfd driver layer function \n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*\author       email@sunplus.com
*\version	 v0.01
*\date 	 2016/08/9
******************************************************************************/
#include <string.h>
#define LOG_MODULE_ID ID_VFD
#include "log_utils.h"
#include "busy_delay.h"
#include "VFD.h"
#include "VFD_Pt6315_Drv.h"
#include "pinmux.h"
#include "hw_gpio_list.h"

/*----------------------------------------------*
  *             VFD info
  *----------------------------------------------*/

#define VFD_GPIO_STB  eHW_IO_24
#define VFD_GPIO_CLK  eHW_IO_23
#define VFD_GPIO_DIO  eHW_IO_22
#define VFD_GPIO_DI   eHW_IO_14

#define	VFD_MEM_LEN_PT6315			36

#define PT6315_SEVEN_SEQUENCE_LENGTH    8


#define DISPLAYMODE_SETTING_COMMAND_BASE (0x0 << 6)

#define ADDRESS_SETTING_COMMAND_BASE (0x03 << 6)

#define EIGHT_BITS_PER_BYTE 8
#define PT6315_DATA_SIZE  3

static BYTE vfd_mem[VFD_MEM_LEN_PT6315] = {0};

/*----------------------------------------------*
  *             Cmd
  *----------------------------------------------*/

//cmd 1 : Display Mode
#define PT6315_CMD1 (0)
typedef union
{
	UINT8 CmdByte;
	struct
	{
		unsigned DispPlaySet: 4;
		unsigned Reserved: 2;
		unsigned Cmd: 2;
	};
} unDispPlayModeCmd1_t;
typedef enum _ePT6315DisplayMode
{
	e4Digits24Segments = 0,
	e5Digits23Segments,
	e6Digits22Segments,
	e7Digits21Segments,
	e8Digits20Segments,
	e9Digits19Segments,
	e10Digits18Segments,
	e11Digits17Segments,
	e12Digits16Segments,
} ePt6315DisPlayMode;

// cmd 2 : data setting
#define PT6315_CMD2 (1)
typedef union
{
	UINT8 CmdByte;
	struct
	{
		unsigned ReadWriteMode: 2;
		unsigned AddressMode: 1;
		unsigned ModeSetting: 1;
		unsigned Reserved: 2;
		unsigned Cmd: 2;
	};
} unDataSettingCmd2_t;

enum
{
	eVFD_DispMode = 0,
	eVFD_LEDMode,
	eVFD_ReadMode,
};

enum
{
	eVFD_IncAddr = 0,
	eVFD_FixAddr,
};

enum
{
	eVFD_NormalMode = 0,
	eVFD_TestMode,
};


// cmd 4 : display control
#define PT6315_CMD4 (2)
typedef union
{
	UINT8 CmdByte;
	struct
	{
		unsigned DimmingSetting: 3;
		unsigned DispSetting: 1;
		unsigned Reserved: 2;
		unsigned Cmd: 2;
	};
} unDispCtrlCmd4_t;

enum
{
	ePulseWidth_1_16 = 0,
	ePulseWidth_2_16,
	ePulseWidth_4_16,
	ePulseWidth_10_16,
	ePulseWidth_11_16,
	ePulseWidth_12_16,
	ePulseWidth_13_16,
	ePulseWidth_14_16,
};

/*----------------------------------------------*
  * DATA TYPE  DECLARATIONS
  *----------------------------------------------*/

int giVFD_PT6315_STB_PinNum = 0;
int giVFD_PT6315_CLK_PinNum = 0;
int giVFD_PT6315_DIO_PinNum = 0;
int giVFD_PT6315_DI_PinNum = 0;

static const UINT32 vfd_char[26] =
{
	VFD_7SEG_A,
	VFD_7SEG_B,
	VFD_7SEG_C,
	VFD_7SEG_D,
	VFD_7SEG_E,
	VFD_7SEG_F,
	VFD_7SEG_G,
	VFD_7SEG_H,
	VFD_7SEG_I,
	VFD_7SEG_J,
	VFD_7SEG_K,
	VFD_7SEG_L,
	VFD_7SEG_M,
	VFD_7SEG_N,
	VFD_7SEG_O,
	VFD_7SEG_P,
	VFD_7SEG_Q,
	VFD_7SEG_R,
	VFD_7SEG_S,
	VFD_7SEG_T,
	VFD_7SEG_U,
	VFD_7SEG_V,
	VFD_7SEG_W,
	VFD_7SEG_X,
	VFD_7SEG_Y,
	VFD_7SEG_Z
};

static const  UINT32  vfd_digit[] =
{
	VFD_7SEG_0,
	VFD_7SEG_1,
	VFD_7SEG_2,
	VFD_7SEG_3,
	VFD_7SEG_4,
	VFD_7SEG_5,
	VFD_7SEG_6,
	VFD_7SEG_7,
	VFD_7SEG_8,
	VFD_7SEG_9
};

#define DOT_MEM_SHIFT_1 (1) // Dp and USB
#define DOT_MEM_SHIFT_2 (2) // others

#define VFD_DOT_ADDR_1G (PT6315_DATA_SIZE * 0)
#define VFD_DOT_ADDR_2G (PT6315_DATA_SIZE * 1)
#define VFD_DOT_ADDR_3G (PT6315_DATA_SIZE * 2)
#define VFD_DOT_ADDR_4G (PT6315_DATA_SIZE * 3)
#define VFD_DOT_ADDR_5G (PT6315_DATA_SIZE * 4)
#define VFD_DOT_ADDR_6G (PT6315_DATA_SIZE * 5)
#define VFD_DOT_ADDR_7G (PT6315_DATA_SIZE * 6)
#define VFD_DOT_ADDR_8G (PT6315_DATA_SIZE * 7)

#define MONEY_BIT (1<<2)
#define DP_BIT (1<<7)
#define USB_BIT (1<<7)
#define PWR_BIT (1<<7)

#define VFD_P16 (1<<7)
#define VFD_P17 (1<<0)
#define VFD_P18 (1<<1)
#define VFD_P19 (1<<2)
#define VFD_P20 (1<<3)

typedef enum _ePT6315DigitsNum
{
	eDigits_1G = 0,
	eDigits_2G = 1,
	eDigits_3G = 2,
	eDigits_4G = 3,
	eDigits_5G = 4,
	eDigits_6G = 5,
	eDigits_7G = 6,
	eDigits_8G = 7,

} ePt6315DigitsNum;

typedef enum _ePT6315DisplayOnOff
{
	eDisplay_Off,
	eDisplay_On,
} ePt6315DisplayOnOff;


int InitVfd_Pt6315(void);
int vfd_dot_Pt6315(UINT32 p, UINT32 method);
int vfd_set_str_Pt6315(const char *s);
int VFD_Get_Seven_Seq_Len_Pt6315(void);
UINT32 ReadVFDKeys_Pt6315(void);
static void Deinit_Pt6315(void);

VFDOP_T	 g_stPt6315 =
{
	InitVfd_Pt6315,
	vfd_dot_Pt6315,
	vfd_set_str_Pt6315,
	VFD_Get_Seven_Seq_Len_Pt6315,
	ReadVFDKeys_Pt6315,
    Deinit_Pt6315
};

static void SetGpio(UINT32 GpioIdx, UINT8 Data)
{
	GPIO_Output_Write(GpioIdx, Data);
}


static void SetGpioSTB(UINT8 bGpioLvl)
{

	SetGpio(giVFD_PT6315_STB_PinNum, bGpioLvl);
}

static void SetGpioClk(UINT8 bGpioLvl)
{
	SetGpio(giVFD_PT6315_CLK_PinNum, bGpioLvl);
}

static void SetGpioDIO(UINT8 bGpioLvl)
{
	SetGpio(giVFD_PT6315_DIO_PinNum, bGpioLvl);
}

static UINT8 GetGpio(UINT32 GpioIdx)
{
	return GPIO_Intput_Read(GpioIdx);
}

/****************************************************
Function:SendAByteToPT6315
Description:
Input:const UINT8 bData
Output: None
Return Value:void
Calls:
Called By: SendBufToPt6315 only

History:
Date					  Modification		  Author
2014/06/19			 Created			 chaofa.yan
Others:
do not call SendAByteToPT6315 directly,
call SendBufToTm16312 instead of SendAByteToPT6315
SendBufToTm16312 can call it only.
*****************************************************/
static void SendAByteToPT6315(const UINT8 bData)
{
	//LOGD("SendAByteToPT6315 : bData = %d\n",bData);

	UINT32 i = 0;

	for (i = 0; i < (sizeof(bData)*EIGHT_BITS_PER_BYTE); i++)
	{
		SetGpioClk(0);
		SetGpioDIO((bData >> i) & 0x01);
		SetGpioClk(1);
	}
	SetGpioDIO(1);
}

static void SendBufToPt6315(const UINT8 *const pBuf,
							const UINT32 dLen)
{
	UINT32 i = 0;

	for (i = 0; i < dLen; i++)
	{
		SendAByteToPT6315(pBuf[i]);
	}
}

static void DisplayModeCmd(unDispPlayModeCmd1_t unCmd)
{
	unCmd.Cmd = PT6315_CMD1;

	LOGDT("%s bDisplayMode = %x\n", __func__, unCmd.CmdByte);
	SetGpioSTB(0);
	SendBufToPt6315(&unCmd.CmdByte, sizeof(unCmd.CmdByte));
	SetGpioSTB(1);
}

static void DataRdWrCmd(unDataSettingCmd2_t unCmd)
{
	unCmd.Cmd = PT6315_CMD2;

	LOGDT("%s bDisplayMode = %x\n", __func__, unCmd.CmdByte);
	SetGpioSTB(0);
	SendBufToPt6315(&unCmd.CmdByte, sizeof(unCmd.CmdByte));
	SetGpioSTB(1);
}

static void DisplayData(const UINT8 bAddr, const UINT8 bData)
{
	//LOGD("DisplayData\n");

	const UINT8 bPt6315RegAddr = (bAddr & 0x3f) | ADDRESS_SETTING_COMMAND_BASE;
	SetGpioSTB(0);

	SendBufToPt6315(&bPt6315RegAddr, 1);
	SendBufToPt6315(&bData, 1);
	SetGpioSTB(1);
}

static void DisplayCtlCmd(unDispCtrlCmd4_t unCmd)
{
	unCmd.Cmd = PT6315_CMD4;

	LOGDT("%s bDisplayMode = %x\n", __func__, unCmd.CmdByte);
	SetGpioSTB(0);
	SendBufToPt6315(&unCmd.CmdByte, sizeof(unCmd.CmdByte));
	SetGpioSTB(1);
}

static UINT8 ReadAByte(void)
{
	//LOGDT("ReadAByte\n");

	UINT32 i = 0;
	UINT8 bData = 0;

	for (i = 0; i < (sizeof(bData)*EIGHT_BITS_PER_BYTE); i++)
	{
		SetGpioClk(0);

		SetGpioClk(1);
		bData |= GetGpio(giVFD_PT6315_DI_PinNum) << i;
	}

	return (bData);
}

static UINT32 Read4Byte(void)
{
	//LOGDT("Read4Byte\n");

	UINT32 dData = 0;
	UINT32 i = 0;

	for (i = 0; i < 4; i++)
	{
		dData |= ReadAByte() << (i * EIGHT_BITS_PER_BYTE);
	}

	return dData;
}

/******************************************************************************************/
/**
 * \fn			UINT32 ReadVFDKeys_Pt6315(void)
 *
 * \brief		to read vfd key
 *
 * \param
 *
 * \return		\e dData on success. \n
 *			    \e -1 on error. \n
 *
 ******************************************************************************************/
UINT32 ReadVFDKeys_Pt6315(void)
{
	//LOGD("ReadVFDKeys_Pt6315\n");

	UINT32 dData = 0;

	// write data
	//vfd_show();
	//delay_1us(1);

	// read key
	unDataSettingCmd2_t unDataSettingCmd2;
	unDataSettingCmd2.AddressMode = eVFD_IncAddr;
	unDataSettingCmd2.ModeSetting = eVFD_NormalMode;
	unDataSettingCmd2.ReadWriteMode = eVFD_ReadMode;
	unDataSettingCmd2.Cmd = PT6315_CMD2;

	SetGpioSTB(0);
	SendBufToPt6315(&unDataSettingCmd2.CmdByte, sizeof(unDataSettingCmd2.CmdByte));
	delay_1us(1);

	dData = Read4Byte();
	SetGpioSTB(1);

	//delay_1us(1);

	//SetGpioSTB(0);
	//SendAByteToPT6315(0xC0); // cmd 3 , set address to 0
	//SetGpioSTB(1);
	//delay_1us(1);

	//DisplayModeCmd(e8Digits20Segments); //cmd 1
	//delay_1us(1);

	//DisplayCtlCmd(ePulseWidth_14_16); // cmd 4

	return (dData);
}


/******************************************************************************************/
/**
 * \fn			int vfd_show()
 *
 * \brief		show vfd string by IOP
 *
 * \param		BYTE *vfd_mem :Vfd Write Data Address
 *
 * \return		\e 0 on success. \n
 *			    \e -1 on error. \n
 *
 ******************************************************************************************/
static int vfd_show()
{
	LOGDT("vfd_show in!!!!!!!\n");

	UINT32 i = 0;
	unDataSettingCmd2_t unDataSettingCmd2;

	unDataSettingCmd2.ReadWriteMode = eVFD_DispMode;
	unDataSettingCmd2.AddressMode = eVFD_FixAddr;
	unDataSettingCmd2.ModeSetting = eVFD_NormalMode;

	DataRdWrCmd(unDataSettingCmd2);
	delay_1us(1);

	for (i = 0; i < VFD_MEM_LEN_PT6315; i++) // update all the memory
	{
		if ((i % 3) != 2)
		{
			DisplayData(i, vfd_mem[i]);
			delay_1us(1);
			LOGDT("vfd_mem[%d] = 0x%x\n", i, vfd_mem[i]);
		}
	}
	SetGpioDIO(1);
	return 0;
}

/******************************************************************************************/
/**
 * \fn			int vfd_set_str_Pt6315(const char *s)
 *
 * \brief		convert string to vfd_char table
 *
 * \param		const char *s
 *
 * \return		\e 0 on success. \n
 *			    \e -1 on vfd_show error. \n
 *
 *
 ******************************************************************************************/
int vfd_set_str_Pt6315(const char *s)
{
	LOGDT("vfd_set_str in : string = %s\n", s);

	BYTE    i, bStrLen;
	//UINT32 	vfd_buf[TOTAL_SEG];
	UINT32  c = 0;
	int ivfd = 0;

	bStrLen = strlen(s);
	for (i = 0; i < TOTAL_SEG; i++)
	{

		if (i < bStrLen)
		{
			if ((s[i] >= 'a') && (s[i] <= 'z'))
				c = vfd_char[s[i] - 'a'];
			else if ( (s[i] >= 'A') && (s[i] <= 'Z') )
				c = vfd_char[s[i] - 'A'];
			else if ( (s[i] >= '0') && (s[i] <= '9') )
				c = vfd_digit[s[i] - '0'];
			else
			{
				c = 0;
			}
		}
		else
		{
			c = 0;
		}
		//vfd_buf[i] = c & 0xffffffff;

		vfd_mem[i * 3] = c & 0xFF; // 1st byte string data
		vfd_mem[i * 3 + 1] = ((c & 0xFF00) >> 8) | (vfd_mem[i * 3 + 1] & (1 << 7)); // 2nd byte string data with dot data
		vfd_mem[i * 3 + 2] = ((c & 0xFF0000) >> 16) | (vfd_mem[i * 3 + 2]); // 3rd byte string data with dot data

	}

	ivfd = vfd_show();
	return ivfd;

}

int vfd_dot_Pt6315(UINT32 p, UINT32 method)
{
	UINT8 *pVfdBuf;
	int Addr = 0;
	UINT8 DataBit = 0;

	switch (p)
	{
		case VFD_DOT_PWR:
			Addr = VFD_DOT_ADDR_1G + DOT_MEM_SHIFT_2;
			DataBit = VFD_P18;
			break;
		case VFD_DOT_CIRCLE1:
			Addr = VFD_DOT_ADDR_2G + DOT_MEM_SHIFT_2;
			DataBit = VFD_P18;
			break;
		case VFD_DOT_CIRCLE2:
			Addr = VFD_DOT_ADDR_3G + DOT_MEM_SHIFT_2;
			DataBit = VFD_P18;
			break;
		case VFD_DOT_PLAY:
			Addr = VFD_DOT_ADDR_4G + DOT_MEM_SHIFT_2;
			DataBit = VFD_P18;
			break;
		case VFD_DOT_PAUSE:
			Addr = VFD_DOT_ADDR_5G + DOT_MEM_SHIFT_2;
			DataBit = VFD_P18;
			break;
		case VFD_DOT_DOLBY:
			Addr = VFD_DOT_ADDR_7G + DOT_MEM_SHIFT_2;
			DataBit = VFD_P18;
			break;
		case VFD_DOT_HD:
			Addr = VFD_DOT_ADDR_8G + DOT_MEM_SHIFT_2;
			DataBit = VFD_P18;
			break;
		case VFD_DOT_CLOCK:
			Addr = VFD_DOT_ADDR_5G + DOT_MEM_SHIFT_2;
			DataBit = VFD_P19;
			break;
		case VFD_DOT_MONEY:
			Addr = VFD_DOT_ADDR_6G + DOT_MEM_SHIFT_2;
			DataBit = VFD_P19;
			break;
		case VFD_DOT_EPG:
			Addr = VFD_DOT_ADDR_8G + DOT_MEM_SHIFT_2;
			DataBit = VFD_P19;
			break;
		case VFD_DOT_SUB:
			Addr = VFD_DOT_ADDR_8G + DOT_MEM_SHIFT_2;
			DataBit = VFD_P20;
			break;
		case VFD_DOT_DOT1:
			Addr = VFD_DOT_ADDR_1G + DOT_MEM_SHIFT_1;
			DataBit = VFD_P16;
			break;
		case VFD_DOT_DOT2:
			Addr = VFD_DOT_ADDR_2G + DOT_MEM_SHIFT_1;
			DataBit = VFD_P16;
			break;
		case VFD_DOT_DOT3:
			Addr = VFD_DOT_ADDR_3G + DOT_MEM_SHIFT_1;
			DataBit = VFD_P16;
			break;
		case VFD_DOT_DOT4:
			Addr = VFD_DOT_ADDR_4G + DOT_MEM_SHIFT_1;
			DataBit = VFD_P16;
			break;
		case VFD_DOT_DOT5:
			Addr = VFD_DOT_ADDR_5G + DOT_MEM_SHIFT_1;
			DataBit = VFD_P16;
			break;
		case VFD_DOT_DOT6:
			Addr = VFD_DOT_ADDR_6G + DOT_MEM_SHIFT_1;
			DataBit = VFD_P16;
			break;
		case VFD_DOT_DOT7:
			Addr = VFD_DOT_ADDR_7G + DOT_MEM_SHIFT_1;
			DataBit = VFD_P16;
			break;
		case VFD_DOT_USB:
			Addr = VFD_DOT_ADDR_8G + DOT_MEM_SHIFT_1;
			DataBit = VFD_P16;
			break;
		case VFD_DOT_COLON1:
			Addr = VFD_DOT_ADDR_4G + DOT_MEM_SHIFT_2;
			DataBit = VFD_P17;
			break;
		case VFD_DOT_COLON2:
			Addr = VFD_DOT_ADDR_6G + DOT_MEM_SHIFT_2;
			DataBit = VFD_P17;
			break;
		default:
			LOGE("vfd_dot p not support = %d...\n", p);
			return -1;
			break;

	}

	pVfdBuf = vfd_mem + Addr;

	if (method == VFD_DOT_OP_SET)
	{
		*pVfdBuf |= DataBit;
	}
	else if (method == VFD_DOT_OP_CLR)
	{
		*pVfdBuf &= ~DataBit;
	}
	else
	{
		LOGE("vfd_dot method fail = %d...\n", method);
		return -1;
	}

	DisplayData(Addr, *pVfdBuf);

	return 0;
}


int VFD_Get_Seven_Seq_Len_Pt6315(void)
{
	int seven_seq_length = -1;

	seven_seq_length = PT6315_SEVEN_SEQUENCE_LENGTH;

	return seven_seq_length;

}



/******************************************************************************************/
/**
 * \fn			int InitPt6315Gpio(void)
 *
 * \brief		VFD GPIO init
 *
 * \param
 *
 *
 *
 * \return
 *
 *
 ******************************************************************************************/
int InitPt6315Gpio(void)
{
	//WriteGpio(TM16312_STB_GPIO, 1);
	//WriteGpio(TM16312_CLK_GPIO, 1);
	//WriteGpio(TM16312_DIO_GPIO, 1);
	LOGD("InitPt6315Gpio in : vfd without IOP\n");
	int iRes = 0;

	giVFD_PT6315_STB_PinNum = VFD_GPIO_STB;
	giVFD_PT6315_CLK_PinNum = VFD_GPIO_CLK;
	giVFD_PT6315_DIO_PinNum = VFD_GPIO_DIO;
	giVFD_PT6315_DI_PinNum = VFD_GPIO_DI;
	LOGD("Pinmux_ModulePin_Find STB: %d\n", giVFD_PT6315_STB_PinNum);
	LOGD("Pinmux_ModulePin_Find CLK: %d\n", giVFD_PT6315_CLK_PinNum);
	LOGD("Pinmux_ModulePin_Find DIO: %d\n", giVFD_PT6315_DIO_PinNum);
	LOGD("Pinmux_ModulePin_Find DI: %d\n", giVFD_PT6315_DI_PinNum);

	iRes |= Pinmux_GPIO_Alloc(giVFD_PT6315_STB_PinNum, IO_GPIO, IO_RISC);
	iRes |= Pinmux_GPIO_Alloc(giVFD_PT6315_CLK_PinNum, IO_GPIO, IO_RISC);
	iRes |= Pinmux_GPIO_Alloc(giVFD_PT6315_DIO_PinNum, IO_GPIO, IO_RISC);
	iRes |= Pinmux_GPIO_Alloc(giVFD_PT6315_DI_PinNum, IO_GPIO, IO_RISC);

	LOGD("Pinmux_GPIO_Alloc: %d\n", iRes);

	if (iRes != 0)
	{
		LOGE("Pt6315 VFD Pin Find Fail: %d\n", iRes);
		return -1;
	}

	iRes = 0;

	iRes |= GPIO_OE_Set(giVFD_PT6315_STB_PinNum, IO_OUTPUT);
	iRes |= GPIO_OE_Set(giVFD_PT6315_CLK_PinNum, IO_OUTPUT);
	iRes |= GPIO_OE_Set(giVFD_PT6315_DIO_PinNum, IO_OUTPUT);
	iRes |= GPIO_OE_Set(giVFD_PT6315_DI_PinNum, IO_INPUT);
	LOGD("GPIO_OE_Set: %d\n", iRes);

	iRes |= GPIO_Output_Write(giVFD_PT6315_STB_PinNum, IO_HIGH);
	iRes |= GPIO_Output_Write(giVFD_PT6315_CLK_PinNum, IO_HIGH);
	iRes |= GPIO_Output_Write(giVFD_PT6315_DIO_PinNum, IO_HIGH);
	iRes |= GPIO_Output_Write(giVFD_PT6315_DI_PinNum, IO_HIGH);

	LOGD("GPIO_Output_Write: %d\n", iRes);

	if (iRes != 0)
	{
		LOGE("driver: Pt6315 VFD init Fail!!!\n");
		return -1;
	}
	else
	{
		LOGD("driver: Pt6315 VFD  init OK!!!\n");
		return 0;
	}
}


/******************************************************************************************/
/**
 * \fn			int InitVfd_Pt6315(void)
 *
 * \brief		VFD Pt6315 init
 *
 * \param
 *
 *
 * \return
 *
 *
 ******************************************************************************************/
int InitVfd_Pt6315(void)
{
	int iRinit = 0;
	LOGD("@@@@@@ InitVfd PT6315 @@@@@@\n");

	iRinit = InitPt6315Gpio();
	if (iRinit != 0)
	{
		LOGD("Init VFD PT6315 GPIO Fail!!!\n");
		return -1;

	}

	vfd_show();// cmd 3 // clear memory
	delay_1us(1);

	unDispPlayModeCmd1_t unDispPlayModeCmd;
	unDispPlayModeCmd.DispPlaySet = e8Digits20Segments;
	DisplayModeCmd(unDispPlayModeCmd); //cmd 1
	delay_1us(1);

	unDispCtrlCmd4_t unDispCtrlCmd4;
	unDispCtrlCmd4.DimmingSetting = ePulseWidth_14_16;
	unDispCtrlCmd4.DispSetting = 1;
	DisplayCtlCmd(unDispCtrlCmd4); // cmd 4

	return 0;
}

static void Deinit_Pt6315(void)
{
    GPIO_OE_Set(VFD_GPIO_STB, IO_OUTPUT);
    GPIO_Output_Write(VFD_GPIO_STB, IO_LOW);

    GPIO_OE_Set(VFD_GPIO_CLK, IO_OUTPUT);
    GPIO_Output_Write(VFD_GPIO_CLK, IO_LOW);

    GPIO_OE_Set(VFD_GPIO_DIO, IO_OUTPUT);
    GPIO_Output_Write(VFD_GPIO_DIO, IO_LOW);

    GPIO_OE_Set(VFD_GPIO_DI, IO_OUTPUT);
    GPIO_Output_Write(VFD_GPIO_DI, IO_LOW);
}
