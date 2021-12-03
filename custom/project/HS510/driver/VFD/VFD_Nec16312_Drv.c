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
#include "VFD_Nec16312_Drv.h"
#include "pinmux.h"
#include "hw_gpio_list.h"

#if 1
#define VFD_GPIO_STB	eHW_IO_24
#define VFD_GPIO_CLK	eHW_IO_23
#define VFD_GPIO_DIO	eHW_IO_22
#define VFD_GPIO_DI		eHW_IO_14


#define	VFD_MEM_LEN_NEC16312			14

#define TM1642_SEVEN_SEQUENCE_LENGTH    4


#define TM16312_WRITE (0<<1)
#define TM16312_READ  (1<<1)
#define ADDR_INC (0<<2)
#define ADDR_FIXATION (1<<2)


#define DATA_READ_WRITE_BASE (0x01<<6)
#define ADDR_SETTING_BASE (0x03<<6)
#define DISPLAY_CTL_BASE (0x02<<6)

#define EIGHT_BITS_PER_BYTE 8

#define DISPLAY_CTL_CMD (DISPLAY_CTL_BASE | 0x03 | 0x08)


static BYTE vfd_mem[VFD_MEM_LEN_NEC16312] = {0};


int giVFD_TM16312_STB_PinNum = 0;
int giVFD_TM16312_CLK_PinNum = 0;
int giVFD_TM16312_DIO_PinNum = 0;


VFD_DISP vfd_disp =
{
	{0, 2, 4, 6},
	{
		DOT_DISC, DOT_USB, DOT_CARD, DOT_IPOD,
		DOT_BT, DOT_FM, DOT_STEREO, DOT_MUTE,
		DOT_REPEAT, DOT_COLON1, DOT_COLON2, DOT_DOT1,
		DOT_DOT2, DOT_DOT3, DOT_MHZ
	}
};

static const UINT16 vfd_char[26] =
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

static const  UINT16  vfd_digit[] =
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


/*----------------------------------------------*
  * DATA TYPE  DECLARATIONS
  *----------------------------------------------*/
typedef enum _eTM16312DisplayMode
{
	e4Grid13Seg = 0x00,
	e5Grid12Seg = 0x01,
	e6Grid11Seg = 0x02,
	e7Grid10Seg = 0x03,
} eDisPlayMode;

typedef enum _eTm16312DataReadWriteCmd
{
	eAddrIncWrite = DATA_READ_WRITE_BASE | ADDR_INC | TM16312_WRITE,
	eAddrFixationWrite = DATA_READ_WRITE_BASE | ADDR_FIXATION | TM16312_WRITE,
	eAddrIncRead = DATA_READ_WRITE_BASE | ADDR_INC | TM16312_READ,
} eRdWrCmd;

int InitVfd_Nec16312(void);
int vfd_dot_Nec16312(UINT32 p, UINT32 method);
int vfd_set_str_Nec16312(const char *s);
int VFD_Get_Seven_Seq_Len_Nec16312(void);
UINT32 ReadVFDKeys_Nec16312(void);
static void Deinit_Nec16312(void);

VFDOP_T	 g_stNec16312 =
{
	InitVfd_Nec16312,
	vfd_dot_Nec16312,
	vfd_set_str_Nec16312,
	VFD_Get_Seven_Seq_Len_Nec16312,
	ReadVFDKeys_Nec16312,
    Deinit_Nec16312
};

static void WriteVFDGpio(UINT32 GpioIdx, UINT8 Data)
{
	//GPIO_M_SET(GpioIdx, 1);
	//GPIO_E_SET(GpioIdx, 1);

	//GPIO_O_SET(GpioIdx, Data);
	int iRes = 0;

	iRes |= GPIO_OE_Set(GpioIdx, IO_OUTPUT);

	//LOGDT("WriteGpio:GPIO_OE_Set: %d\n", iRes);

	iRes |= GPIO_Output_Write(GpioIdx, Data);

	//LOGDT("WriteGpio:GPIO_Output_Write: %d\n", iRes);

}


static void SetGpio(UINT32 GpioIdx, UINT8 Data)
{
	//GPIO_O_SET(GpioIdx, Data);
	//GPIO_OE_Set(GpioIdx, IO_OUTPUT);
	GPIO_Output_Write(GpioIdx, Data);
}


static void SetGpioSTB(UINT8 bGpioLvl)
{

	SetGpio(giVFD_TM16312_STB_PinNum, bGpioLvl);
}

static void SetGpioClk(UINT8 bGpioLvl)
{
	SetGpio(giVFD_TM16312_CLK_PinNum, bGpioLvl);
}

static void SetGpioDIO(UINT8 bGpioLvl)
{
	SetGpio(giVFD_TM16312_DIO_PinNum, bGpioLvl);
}


static UINT8 ReadVFDGpio(UINT32 GpioIdx)
{

	//GPIO_F_SET(GpioIdx, 1);
	//GPIO_M_SET(GpioIdx, 1);
	//GPIO_E_SET(GpioIdx, 0);
	//return GPIO_I_GET(GpioIdx);
	int iRes = 0;

	iRes |= GPIO_OE_Set(GpioIdx, IO_INPUT);

	//LOGDT("ReadGpio:GPIO_OE_Set: %d\n", iRes);

	return GPIO_Intput_Read(GpioIdx);

}


static UINT8 GetGpio(UINT32 GpioIdx)
{

	//return GPIO_I_GET(GpioIdx);
	return GPIO_Intput_Read(GpioIdx);
}





/****************************************************
Function:SendAByteToTM16312
Description:
Input:const UINT8 bData
Output: None
Return Value:void
Calls:
Called By: SendBufToTm16312 only

History:
Date					  Modification		  Author
2014/06/19			 Created			 chaofa.yan
Others:
do not call SendAByteToTM16312 directly,
call SendBufToTm16312 instead of SendAByteToTM16312
SendBufToTm16312 can call it only.
*****************************************************/
static void SendAByteToTM16312(const UINT8 bData)
{
	//LOGDT("SendAByteToTM16312 : bData = %d\n",bData);

	UINT32 i = 0;

	for (i = 0; i < (sizeof(bData)*EIGHT_BITS_PER_BYTE); i++)
	{
		SetGpioClk(0);
		SetGpioDIO((bData >> i) & 0x01);
		//delay_1us(1);

		SetGpioClk(1);
		//delay_1us(1);
	}
}

static void SendBufToTm16312(const UINT8 *const pBuf,
							 const UINT32 dLen)
{
	UINT32 i = 0;

	SetGpioSTB(0);
	//delay_1us(2);

	for (i = 0; i < dLen; i++)
	{
		SendAByteToTM16312(pBuf[i]);
	}

	//delay_1us(2);
	SetGpioSTB(1);
	//delay_1us(2);
}






static void DisplayModeCmd(const eDisPlayMode eDisMode)
{
	LOGDT("DisplayModeCmd in\n");

	const UINT8 bDisplayMode = eDisMode;

	LOGDT("%s bDisplayMode = %x\n", __func__, bDisplayMode);

	SendBufToTm16312(&bDisplayMode, sizeof(bDisplayMode));
}

static void DataRdWrCmd(const eRdWrCmd eCmd)
{
	//LOGD("DataRdWrCmd\n");

	const UINT8 bRdWrCmd = eCmd;

	LOGDT("%s bRdWrCmd = %x\n", __func__, bRdWrCmd);

	SendBufToTm16312(&bRdWrCmd, sizeof(bRdWrCmd));
}




static void DisplayData(const UINT8 bAddr, const UINT8 bData)
{
	//LOGD("DisplayData\n");

	const UINT8 bTm16312RegAddr = (bAddr & 0x0f)
								  | ADDR_SETTING_BASE;

	const UINT8 baAddrData[] = {bTm16312RegAddr, bData};

	SendBufToTm16312(&baAddrData[0], sizeof(baAddrData));
}

static void DisplayCtlCmd(void)
{
	LOGDT("DisplayCtlCmd\n");

	const UINT8 bDisplayCtl = DISPLAY_CTL_CMD;

	LOGDT("%s bDisplayCtl = %x\n", __func__, bDisplayCtl);

	SendBufToTm16312(&bDisplayCtl, sizeof(bDisplayCtl));
}



static UINT8 ReadAByte(void)
{
	//LOGDT("ReadAByte\n");

	UINT32 i = 0;
	UINT8 bData = 0;

	for (i = 0; i < (sizeof(bData)*EIGHT_BITS_PER_BYTE); i++)
	{
		SetGpioClk(0);
		//SetGpioDIO((bData >> i) & 0x01);
		//delay_1us(1);

		SetGpioClk(1);
		//delay_1us(1);
		bData |= GetGpio(giVFD_TM16312_DIO_PinNum) << i;
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
 * \fn			UINT32 ReadVFDKeys_Nec16312(void)
 *
 * \brief		to read vfd key
 *
 * \param
 *
 * \return		\e dData on success. \n
 *			    \e -1 on error. \n
 *
 ******************************************************************************************/
UINT32 ReadVFDKeys_Nec16312(void)
{
	//LOGD("ReadVFDKeys_Nec16312\n");

	UINT32 dData = 0;

	SetGpioSTB(0);
	//delay_1us(1);

	SendAByteToTM16312(eAddrIncRead);
	//delay_1us(1);

	ReadVFDGpio(giVFD_TM16312_DIO_PinNum);

	dData = Read4Byte();

	//delay_1us(1);
	SetGpioSTB(1);
	//delay_1us(1);

	WriteVFDGpio(giVFD_TM16312_DIO_PinNum, 1);

	return (dData);
}

/******************************************************************************************/
/**
 * \fn			int vfd_show(BYTE *vfd_mem)
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
	//LOGDT("vfd_show in!!!!!!!\n");

	UINT32 i = 0;

	for (i = 0; i < VFD_MEM_LEN_NEC16312; i++)
	{
		LOGDT("vfd_mem[%d] = 0x%x\n", i, vfd_mem[i]);
		DisplayData(i, vfd_mem[i]);

	}
	return 0;
}

/******************************************************************************************/
/**
 * \fn			int vfd_set_str_Nec16312(const char *s)
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
int vfd_set_str_Nec16312(const char *s)
{
	LOGDT("vfd_set_str_Nec16312 in!!!!!!!\n");

	BYTE    i, bStrLen;
	//BYTE    var_data;//tingwei mark
	UINT16 	vfd_buf[TOTAL_SEG];
	UINT16  c = 0;
	int ivfd = 0;
	int cnt = 0;

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
		vfd_buf[i] = c & 0xff;

	}



	for (i = 0; i < TOTAL_SEG; i++)
	{
		/* //tingwei mark
			var_data = vfd_disp.seg_p[i];

			vfd_mem[var_data] = (vfd_buf[i])&0xff;

			vfd_mem[var_data+1] = 0;
		*/
		vfd_mem[i * 2] = (vfd_buf[i]) & 0xff;
		if ((i != 1) && (i != 2)) //tingwei : not clear VFD_DOT_COLON1 & VFD_DOT_COLON2
		{
			vfd_mem[(i * 2) + 1] = 0;
		}
	}

	do
	{
		ivfd = vfd_show();
		if (ivfd != 0)
		{
			delay_1ms(1);
			cnt++;
		}
	}
	while ((ivfd != 0) && (cnt < 2));
	if (cnt >= 2)
	{
		LOGD("show string error..\n");
	}

	return ivfd;

}


int VFD_Get_Seven_Seq_Len_Nec16312(void)
{
	int seven_seq_length = -1;

	seven_seq_length = TM1642_SEVEN_SEQUENCE_LENGTH;

	return seven_seq_length;

}



static void Vfd_Set_Memory_Clear()
{
	LOGDT("Vfd_Set_Memory_Clear in ..\n");

	int i = 0;

	for (i = 0; i < VFD_MEM_LEN_NEC16312; i++) // update all the memory
	{
		vfd_mem[i] = 0x0;

	}

	vfd_show();

	return ;
}


/******************************************************************************************/
/**
 * \fn			int vfd_dot_Nec16312(UINT32 p, UINT32 method)
 *
 * \brief		write the dot
 *
 * \param		p: the dot addr \n
 *              method : 0 -> set the dot,\n
 *              1 -> clear the dot bit,\n
 *              2 -> toggle the dot bit.\n
 * \return		\e 0 on success. \n
 *			    \e -1 on vfd_show error. \n
 *
 *
 ******************************************************************************************/
int vfd_dot_Nec16312(UINT32 p, UINT32 method)
{
	LOGDT("vfd_dot_Nec16312 in!!!!!!!\n");

	if ((p !=
			VFD_DOT_COLON1) && (p !=
								VFD_DOT_COLON2))
	{
		LOGE("in valid dot !!!!!!!\n");
		return -1;
	}

	UINT32	A = vfd_disp.dot_num[p] >> 3;
	UINT32	tag = 1 << (vfd_disp.dot_num[p] & 0x07);

	LOGDT("vfd_dot_Nec16312 : A = %d ; tag = 0x%x \n", A, tag);

	BYTE	*ptr, x;
	int cnt = 0;
	int idot = 0;

	if (A >= VFD_MEM_LEN_NEC16312)
		return -1;
	ptr = &vfd_mem[A];

	x = *ptr;

	if (method == 0)
		x |= tag;
	else if (method == 1)
		x &= ~tag;
	else if (method == 2)
		x ^= tag;
	*ptr = x;

	do
	{
		idot = vfd_show();
		if (idot != 0)
		{
			delay_1ms(1);
			cnt++;
		}
	}
	while ((idot != 0) && (cnt < 2));
	if (cnt >= 2)
	{
		LOGD("show dot error..\n");
	}
	return idot;

}

/******************************************************************************************/
/**
 * \fn			int InitTm16312Gpio(void)
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

int InitTm16312Gpio(void)
{
	//WriteGpio(TM16312_STB_GPIO, 1);
	//WriteGpio(TM16312_CLK_GPIO, 1);
	//WriteGpio(TM16312_DIO_GPIO, 1);
	LOGD("InitTm16312Gpio\n");
	int iRes = 0;

	giVFD_TM16312_STB_PinNum = VFD_GPIO_STB;
	giVFD_TM16312_CLK_PinNum = VFD_GPIO_CLK;
	giVFD_TM16312_DIO_PinNum = VFD_GPIO_DIO;
	LOGD("Pinmux_ModulePin_Find STB: %d\n", giVFD_TM16312_STB_PinNum);
	LOGD("Pinmux_ModulePin_Find CLK: %d\n", giVFD_TM16312_CLK_PinNum);
	LOGD("Pinmux_ModulePin_Find DIO: %d\n", giVFD_TM16312_DIO_PinNum);

	iRes |= Pinmux_GPIO_Alloc(giVFD_TM16312_STB_PinNum, IO_GPIO, IO_RISC);
	iRes |= Pinmux_GPIO_Alloc(giVFD_TM16312_CLK_PinNum, IO_GPIO, IO_RISC);
	iRes |= Pinmux_GPIO_Alloc(giVFD_TM16312_DIO_PinNum, IO_GPIO, IO_RISC);

	if (iRes != 0)
	{
		LOGE("Nec16312 VFD Pin Find Fail: %d\n", iRes);
		return -1;
	}

	iRes = 0;

	iRes |= GPIO_OE_Set(giVFD_TM16312_STB_PinNum, IO_OUTPUT);
	iRes |= GPIO_OE_Set(giVFD_TM16312_CLK_PinNum, IO_OUTPUT);
	iRes |= GPIO_OE_Set(giVFD_TM16312_DIO_PinNum, IO_OUTPUT);
	LOGDT("GPIO_OE_Set: %d\n", iRes);

	iRes |= GPIO_Output_Write(giVFD_TM16312_STB_PinNum, IO_HIGH);
	iRes |= GPIO_Output_Write(giVFD_TM16312_CLK_PinNum, IO_HIGH);
	iRes |= GPIO_Output_Write(giVFD_TM16312_DIO_PinNum, IO_HIGH);
	LOGDT("GPIO_Output_Write: %d\n", iRes);

	if (iRes != 0)
	{
		LOGE("driver: VFD init Fail!!!\n");
		return -1;
	}
	else
	{
		LOGD("driver: VFD  init OK!!!\n");
		return 0;
	}
}


/******************************************************************************************/
/**
 * \fn			int InitVfd_Nec16312(void)
 *
 * \brief		VFD Nec16312 init
 *
 * \param
 *
 *
 * \return
 *
 *
 ******************************************************************************************/
int InitVfd_Nec16312(void)
{
	int iRinit = 0;

	iRinit = InitTm16312Gpio();
	if (iRinit != 0)
	{
		LOGD("Init VFD GPIO Fail!!!\n");
		return -1;

	}
	DisplayModeCmd(e4Grid13Seg);
	DataRdWrCmd(eAddrFixationWrite);
	vfd_show();
	DisplayCtlCmd();
	Vfd_Set_Memory_Clear();
	return 0;
}
#endif

static void Deinit_Nec16312(void)
{
    GPIO_OE_Set(VFD_GPIO_STB, IO_OUTPUT);
    GPIO_Output_Write(VFD_GPIO_STB, IO_LOW);

    GPIO_OE_Set(VFD_GPIO_CLK, IO_OUTPUT);
    GPIO_Output_Write(VFD_GPIO_CLK, IO_LOW);

    GPIO_OE_Set(VFD_GPIO_DIO, IO_OUTPUT);
    GPIO_Output_Write(VFD_GPIO_DIO, IO_LOW);
}
