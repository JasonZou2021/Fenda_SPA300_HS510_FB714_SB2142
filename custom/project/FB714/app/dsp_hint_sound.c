/******************************************************************************/
/**
*
* \file	    dsp_hint_sound.c
*
* \brief	configuration for hint sound.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author
* \version	 v0.01
* \date 	 2016/11/10
******************************************************************************/

#include "dsp_hint_sound.h"
#include "AudDspService.h"

const BYTE TONE[] =
{
	0x00, 0x00, 0x01, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x01, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x60,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA0, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5E,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0xFF, 0xFF, 0xFF, 0xE4, 0x74, 0x60, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA0, 0x00, 0x00, 0x01, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5E,
	0x00, 0x00, 0x01, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA2, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5E,
	0x00, 0x00, 0x01, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0xFF, 0xFF, 0xFF, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x60,
	0xFF, 0xFF, 0xFF, 0x1B, 0x8B, 0xA0, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0xFF, 0xFF, 0xFF, 0x1B, 0x8B, 0xA0, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB5, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x01, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA2, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA2, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA2, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5E,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x60,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0xFF, 0xFF, 0xFF, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB5, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5E,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x01, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4B, 0xE4, 0x74, 0x5F,
	0xFF, 0xFF, 0xFF, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA1, 0xFF, 0xFF, 0xFF, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5E,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA0, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA0, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA2, 0xFF, 0xFF, 0xFF, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0xFF, 0xFF, 0xFF, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x60, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5E,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5E,
	0x00, 0x00, 0x01, 0x1B, 0x8B, 0xA0, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA0, 0xFF, 0xFF, 0xFF, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA0, 0xFF, 0xFF, 0xFF, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x60, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA2, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x01, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5E,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x01, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x60,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5E,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA0, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA0, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA2, 0x00, 0x00, 0x01, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5E,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA2, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5E,
	0xFF, 0xFF, 0xFF, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x60, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5E,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x01, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0xFF, 0xFF, 0xFF, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0xFF, 0xFF, 0xFF, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x01, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA0, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA2, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA0, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA2, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x60,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x01, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x01, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA2, 0xFF, 0xFF, 0xFF, 0xE4, 0x74, 0x60, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x01, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0xFF, 0xFF, 0xFF, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x60, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0xFF, 0xFF, 0xFF, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x60, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4B, 0xE4, 0x74, 0x5E,
	0x00, 0x00, 0x01, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5E,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA0, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x60, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x60,
	0xFF, 0xFF, 0xFF, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x01, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0xFF, 0xFF, 0xFF, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA2, 0x00, 0x00, 0x01, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4B, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5E,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA2, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0xFF, 0xFF, 0xFF, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5E,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA2, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5E,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA0, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x60,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x60, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA0, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0xFF, 0xFF, 0xFF, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA2, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x01, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA0, 0x00, 0x00, 0x01, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x01, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA0, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA2, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x60, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA0, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x49, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5E,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA0, 0x32, 0xE5, 0xB7, 0x42, 0x80, 0x23, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA2, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x35, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5E,
	0xFF, 0xFF, 0xFF, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB5, 0x1B, 0x8B, 0xA2, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x60,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCB, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA2, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x60, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x49, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDE, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5E,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x23, 0x32, 0xE5, 0xB6, 0x1B, 0x8B, 0xA1, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5F, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDE, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x5F,
	0x00, 0x00, 0x00, 0x1B, 0x8B, 0xA1, 0x32, 0xE5, 0xB6, 0x42, 0x80, 0x22, 0x47, 0xFA, 0xCA, 0x42,
	0x80, 0x22, 0x32, 0xE5, 0xB7, 0x1B, 0x8B, 0xA2, 0x00, 0x00, 0x00, 0xE4, 0x74, 0x5E, 0xCD, 0x1A,
	0x4A, 0xBD, 0x7F, 0xDD, 0xB8, 0x05, 0x36, 0xBD, 0x7F, 0xDD, 0xCD, 0x1A, 0x4A, 0xE4, 0x74, 0x60,
};

// SoundHint input PCM sample rate
// 0:48  kHz    |  3:24    kHz   | 4:12     kHz    |
// 8:44.1kHz   | 11:22.05 kHz |12:11.025 kHz |
// 16:32  kHz  | 19:16    kHz  |20: 8     kHz    |

DSP_HINT_SOUND_P HINT_TONE =
{
	.HINT_DATA = TONE,

	.SIZE = sizeof(TONE),
};


