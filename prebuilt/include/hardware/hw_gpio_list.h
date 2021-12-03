/******************************************************************************/
/**
 *
 * \file	hw_gpio_list.h
 *
 * \brief	This file provides the SPA100 pin list define \n
 *
 * \note	Copyright (c) 2020 Sunplus Technology Co., Ltd. \n
 *			All rights reserved.
 *
 *\author	email@sunplus.com
 *\version	v0.01
 *\date		2016/04/15
 ******************************************************************************/
#ifndef __HW_GPIO_LIST_H__
#define __HW_GPIO_LIST_H__

typedef enum
{
	//GPIO Number       GPIO_FIRST(0-1)	    Pin NO.
	eHW_IO_0 = 0,		//GPIO[0]			1
	eHW_IO_1,			//GPIO[1]			4
	eHW_IO_2,			//GPIO[2]			5
	eHW_IO_3,			//GPIO[3]			6
	eHW_IO_4,			//GPIO[4]			7
	eHW_IO_5,			//GPIO[5]			8
	eHW_IO_6,			//GPIO[6]			9
	eHW_IO_7,			//GPIO[7]			10
	eHW_IO_8,			//GPIO[8]			11
	eHW_IO_9,			//GPIO[9]			12
	eHW_IO_10,			//GPIO[10]			15
	eHW_IO_11,			//GPIO[11]			16
	eHW_IO_12,			//GPIO[12]			17
	eHW_IO_13,			//GPIO[13]			18
	eHW_IO_14,			//GPIO[14]			19
	eHW_IO_15,			//GPIO[15]			20
	eHW_IO_16,			//GPIO[16]			21
	eHW_IO_17,			//GPIO[17]			22
	eHW_IO_18,			//GPIO[18]			23
	eHW_IO_19,			//GPIO[19]			24
	eHW_IO_20,			//GPIO[20]			25
	eHW_IO_21,			//GPIO[21]			37
	eHW_IO_22,			//GPIO[22]			36
	eHW_IO_23,			//GPIO[23]			35
	eHW_IO_24,			//GPIO[24]			34
	eHW_IO_25,			//GPIO[25]			59
	eHW_IO_26,			//GPIO[26]			60
	eHW_IO_27,			//GPIO[27]			61
	eHW_IO_28,			//GPIO[28]			62
	eHW_IO_29,			//GPIO[29]			65
	eHW_IO_30,			//GPIO[30]			66
	eHW_IO_31,			//GPIO[31]			67
	eHW_IO_32,			//GPIO[32]			68
	eHW_IO_33,			//GPIO[33]			69
	eHW_IO_34,			//GPIO[34]			75
	eHW_IO_35,			//GPIO[35]			76
	eHW_IO_36,			//GPIO[36]			77
	eHW_IO_37,			//GPIO[37]			78
	eHW_IO_38,			//GPIO[38]			79
	eHW_IO_39,			//GPIO[39]			80
	eHW_IO_40,			//GPIO[40]			81
	eHW_IO_41,			//GPIO[41]			82
	eHW_IO_42,			//GPIO[42]			87
	eHW_IO_43,			//GPIO[43]			88
	eHW_IO_44,			//GPIO[44]			89
	eHW_IO_45,			//GPIO[45]			90
	eHW_IO_46,			//GPIO[46]			91
	eHW_IO_47,			//GPIO[47]			92
	eHW_IO_48,			//GPIO[48]			95
	eHW_IO_49,			//GPIO[49]			96
	eHW_IO_50,			//GPIO[50]			97
	eHW_IO_51,			//GPIO[51]			98
	eHW_IO_52,			//GPIO[52]			99
	eHW_IO_53,			//GPIO[53]			100
	eHW_IO_54,			//GPIO[54]			101
	eHW_IO_55,			//GPIO[55]			102
	eHW_IO_56,			//GPIO[56]			103
	eHW_IO_57,			//GPIO[57]			104
	eHW_IO_58,			//GPIO[58]			105
	eHW_IO_59,			//GPIO[59]			106
	eHW_IO_60,			//GPIO[60]			123
	eHW_IO_61,			//GPIO[61]			124
	eHW_IO_62,			//GPIO[62]			125
	eHW_IO_63,			//GPIO[63]			126
	eHW_IO_64,			//GPIO[64]			127
	eHW_IO_65,			//GPIO[65]			128
	eHW_IO_NULL
} eHW_IO_PIN_GPIO;

#endif // __HW_GPIO_LIST_H__

