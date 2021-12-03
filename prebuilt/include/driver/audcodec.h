/**************************************************************************/
/**
*
* \file audcodec.h
*
* \brief This file is all about dsp coding mode index define for Audio I/F
*
* \note Copyright (c) 2016 Sunplus Technology Co., Ltd. \n
* All rights reserved.
*
*
* \author email@sunplus.com
* \version v0.01
* \date 2016/04/15
***************************************************************************/
#ifndef __AUD_CODEC_H__
#define __AUD_CODEC_H__

/*---------------------------------------------------------------------------*
 *    INCLUDE DECLARATIONS
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 *    MACRO DECLARATIONS
 *---------------------------------------------------------------------------*/
typedef enum _CODECID
{
	ID_MAIN 			= 0x00,
	ID_DECODER_PCM		= 0x01,
	ID_DECODER_DDP		= 0x02,
	ID_DECODER_TRUEHD	= 0x03,
	ID_DECODER_DTSLBR	= 0x04,
	ID_DECODER_DTSHD	= 0x05,
	ID_DECODER_MPEG		= 0x06,
	ID_DECODER_AAC		= 0x07,
	ID_DECODER_WMA		= 0x08,
	ID_DECODER_RA		= 0x09,
	ID_DECODER_OGG		= 0x0A,
	ID_DECODER_EMU		= 0x0B,
	ID_DECODER_WMAPRO   = 0x0C,
	ID_DECODER_FLAC     = 0x0D,
	ID_DECODER_AMR      = 0x0E,
	ID_DECODER_MIDI     = 0x0F,

	ID_DECODER_APE     =  0x13,
	ID_DECODER_ALAC    =  0x14,
	ID_DECODER_AVS     =  0x15,
    ID_DECODER_HEAAC   =  0x16,
    ID_DECODER_DRA     =  0x17,
    ID_DECODER_SBC     =  0x18,
    ID_DECODER_DSD     =  0x1E,

	ID_ENCODER_MPEG		= 0x10,
	ID_ENCODER_AC3		= 0x11,
	ID_ENCODER_DTS		= 0x12,
	ID_ENCODER_SBC		= 0x1B,

	ID_CODEC_HANDSFREE = 0x1C,
	ID_ENCODER_AAC    = 0x1D,

	ID_MAX_ENCID        = 0x1D,


	ID_MAX_DECID        = 0x1E,  // note should refine
	ID_MAX_CODECID      = 0x1E,
	ID_DEC_ROM			= 0xFF,
}CODECID;

#define CODING_MODE_SYSTEM  	        ID_MAIN 			   // 0x00
#define CODING_MODE_PCM	                ID_DECODER_PCM		   // 0x01
#define CODING_MODE_LPCM                ID_DECODER_PCM
#define CODING_MODE_SPDIFINCD           ID_DECODER_PCM
#define CODING_MODE_DDP	                ID_DECODER_DDP		   // 0x02
#define CODING_MODE_AC3                 ID_DECODER_DDP
#define CODING_MODE_SPDIFINAC3          ID_DECODER_DDP
#define CODING_MODE_TRUEHD              ID_DECODER_TRUEHD	   // 0x03
#define CODING_MODE_PPCM                ID_DECODER_TRUEHD
#define CODING_MODE_DTSLBR              ID_DECODER_DTSLBR	   // 0x04
#define CODING_MODE_DTSHD               ID_DECODER_DTSHD	   // 0x05(not ready)
#define CODING_MODE_DTS                 ID_DECODER_DTSHD
#define CODING_MODE_DTSCD               ID_DECODER_DTSHD
#define CODING_MODE_SPDIFINDTS          ID_DECODER_DTSHD
#define CODING_MODE_MPEG                ID_DECODER_MPEG		   // 0x06
#define CODING_MODE_MP1                 ID_DECODER_MPEG
#define CODING_MODE_MP2                 ID_DECODER_MPEG
#define CODING_MODE_MP3                 ID_DECODER_MPEG
#define CODING_MODE_MP3_6CH             ID_DECODER_MPEG
#define CODING_MODE_AAC                 ID_DECODER_AAC		   // 0x07
#define CODING_MODE_WMA                 ID_DECODER_WMA		   // 0x08
#define CODING_MODE_RA                  ID_DECODER_RA		   // 0x09
#define CODING_MODE_OGG                 ID_DECODER_OGG		   // 0x0a
#define CODING_MODE_EMU                 ID_DECODER_EMU		   // 0x0b
#define CODING_MODE_WMAPRO              ID_DECODER_WMAPRO      // 0x0c
#define CODING_MODE_FLAC                ID_DECODER_FLAC        // 0x0d
#define CODING_MODE_AMR                 ID_DECODER_AMR        // 0x0e
#define CODING_MODE_MIDI				ID_DECODER_MIDI        // 0x0f
#define CODING_MODE_APE  				ID_DECODER_APE         // 0x13
#define CODING_MODE_ALAC 				ID_DECODER_ALAC        // 0x14
#define CODING_MODE_AVS					ID_DECODER_AVS         //0x15
#define CODING_MODE_HEAAC				ID_DECODER_HEAAC       // 0x16
#define CODING_MODE_SBC			        ID_DECODER_SBC		// 0x18
#define CODING_MODE_DSD			        ID_DECODER_DSD		// 0x1E


#define CODING_MODE_MPEG_ENC            ID_ENCODER_MPEG		   // 0x10(not ready)
#define CODING_MODE_AC3_ENC             ID_ENCODER_AC3		   // 0x11(not ready)
#define CODING_MODE_DTS_ENC             ID_ENCODER_DTS		   // 0x12(not ready)

#define FILE_TYPE_AAC 0
#define FILE_TYPE_MP4 1
/*---------------------------------------------------------------------------*
 *    DATA TYPES
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 *    GLOBAL DATA
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 *    EXTERNAL DECLARATIONS
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 *    FUNCTION DECLARATIONS
 *---------------------------------------------------------------------------*/

#endif  // __AUD_CODEC_H__

