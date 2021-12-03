#ifndef __WAVE_FMT_H
#define __WAVE_FMT_H


// wFormatTag definitions in mmreg.h
#define WAVE_FORMAT_UNKNOWN                     0x0000
#define WAVE_FORMAT_PCM                         0x0001
#define WAVE_FORMAT_ADPCM                       0x0002
#define WAVE_FORMAT_IEEE_FLOAT          0x0003
#define WAVE_FORMAT_APE                       0x0004
#define WAVE_FORMAT_FLAC                       0x0005
#define WAVE_FORMAT_ALAW            0x0006    //guohao add for u-law dsp
#define WAVE_FORMAT_MULAW           0x0007    //guohao add for u-law dsp
#define WAVE_FORMAT_PCM_S8          0x000D    //l.hua add 21-3-10 for aiff MF_RES_FMT_PCMS8 and MF_RES_FMT_PCMS24B
#define WAVE_FORMAT_G723_ADPCM      0x0014
#define WAVE_FORMAT_DSD				0x001E
#define WAVE_FORMAT_G721_ADPCM          0x0040
#define WAVE_FORMAT_G728_CELP           0x0041
#define WAVE_FORMAT_G726_ADPCM          0x0064
#define WAVE_FORMAT_DOLBY_AC2           0x0030
#define WAVE_FORMAT_DOLBY_AC3           0x2000
#define WAVE_FORMAT_DOLBY_TRUEHD        0x2003
#define WAVE_FORMAT_DOLBY_EC3           0x2002
#define WAVE_FORMAT_DOLBY_DTS           0x2001
#define WAVE_FORMAT_MPEG                        0x0050
#define WAVE_FORMAT_MPEGLAYER3          0x0055
#define WAVE_FORMAT_IMA_ADPCM           0x0011 //add by Roger 06-04-18 for support ASF IMA ADPCM

#define WAVE_FORMAT_WMA_OLD                     0x0160          // WMA version type 1, not supported any more ->ycchou
#define WAVE_FORMAT_WMA_STEREO          0x0161          // could be supported, version 7, 8, 9
#define WAVE_FORMAT_WMA_PRO                     0x0162
#define WAVE_FORMAT_WMA_LOSSLESS        0x0163
#define WAVE_FORMAT_WMA_VOICE           0x000A

#define WAVE_FORMAT_NEC_AAC                     0x00B0          // NEC AAC
#define WAVE_FORMAT_MPEG2AAC            0x0180          // MP2 AAC
#define WAVE_FORMAT_DIVIO_AAC           0x4143          // DIVIO AAC
#define WAVE_FORMAT_AAC                         0xa106          // MP4 AAC

#define WAVE_FORMAT_REAL_AUDIO          0x3040      // reference 8203D
//-----------------------------------APE DEINFE
#define APE_DSP_VER_H			0
#define APE_DSP_VER_L			1
#define APE_DSP_COMPRESS_H		2
#define APE_DSP_COMPRESS_L		3
#define APE_DSP_TOTAL_FRAME_H	4
#define APE_DSP_TOTAL_FRAME_L	5

// proprietary wma header
typedef struct _wmaheader
{
	UINT16 wFormatTag;
	UINT32 dwSampRate;
	UINT32 dwBytePerSec;
	UINT16 wBlockAlign;
	UINT16 wReserve1;       // nChannels+nBitsPerSample
	UINT32 dwSampPerBlk;
	UINT16 wEncOption;
	UINT16 wTag;
} WMAHEADER;

typedef struct tWAVEFORMATEX
{
	UINT16 wFormatTag       __attribute__ ((packed)); /* format type */
	UINT16 nChannels        __attribute__ ((packed)); /* number of channels (i.e. mono, stereo...) */
	UINT32 nSamplesPerSec   __attribute__ ((packed)); /* sample rate */
	UINT32 nAvgBytesPerSec  __attribute__ ((packed)); /* for buffer estimation */
	UINT16 nBlockAlign      __attribute__ ((packed)); /* block size of data */
	UINT16 wBitsPerSample   __attribute__ ((packed)); /* Number of bits per sample of mono data */
	UINT16 cbSize           __attribute__ ((packed)); /* The count in bytes of the size of extra information (after cbSize) */
} WAVEFORMATEX;

typedef struct tWMAUDIO2WAVEFORMAT
{
	WAVEFORMATEX wfx                __attribute__ ((packed));
	UINT32 dwSamplesPerBlock   __attribute__ ((packed));
	UINT16 wEncodeOptions      __attribute__ ((packed));
	UINT32 dwSuperBlockAlign   __attribute__ ((packed));
}   WMAUDIO2WAVEFORMAT;

typedef struct mpeg1waveformat_tag
{
	WAVEFORMATEX wfx                         __attribute__ ((packed));
	UINT16 fwHeadLayer               __attribute__ ((packed));
	UINT32 dwHeadBitrate      __attribute__ ((packed));
	UINT16 fwHeadMode                __attribute__ ((packed));
	UINT16 fwHeadModeExt     __attribute__ ((packed));
	UINT16 wHeadEmphasis     __attribute__ ((packed));
	UINT16 fwHeadFlags               __attribute__ ((packed));
	UINT32 dwPTSLow           __attribute__ ((packed));
	UINT32 dwPTSHigh          __attribute__ ((packed));
} MPEG1WAVEFORMAT;

typedef struct mpeglayer3waveformat_tag
{
	WAVEFORMATEX wfx                                     __attribute__ ((packed));
	UINT16 wID                                   __attribute__ ((packed));
	UINT32 fdwFlags                       __attribute__ ((packed));
	UINT16 nBlockSize                    __attribute__ ((packed));
	UINT16 nFramesPerBlock               __attribute__ ((packed));
	UINT16 nCodecDelay                   __attribute__ ((packed));
} MPEGLAYER3WAVEFORMAT;

typedef struct tWAVFORMATMACRO
{
	WAVEFORMATEX wfx             __attribute__ ((packed));
	UINT8 reserve[30]     __attribute__ ((packed));
}   WAVFORMATMACRO;

typedef struct tRWAVFORMATMACRO //rmf
{
	int nFrameBits;
	UINT16 nSamples;
	UINT16 nRegions;
	UINT16 cplStart;
	UINT16 cplQbits;
	WAVEFORMATEX wfx             __attribute__ ((packed));
}   RAWAVEFORMAT;

// yangwu 2013.12.24 [8288 SPEECH] ++{
typedef struct tSPEECHFORMAT
{
	UINT16 addr_HIGH;
	UINT16 addr_LOW;
	UINT16 buffer_len;
	UINT16 mic_data_conf;
}   SPEECHFORMAT;
// yangwu 2013.12.24 [8288 SPEECH] ++}
























/////////////////////////////////////////////////////////8368-u
/////////////////////////////////////////////////////////8368-u
/////////////////////////////////////////////////////////8368-u
#define WAVE_FORMAT_SWF_ADPCM		0x500A
#define WAVE_FORMAT_CD_LPCM			0x5001
#define WAVE_FORMAT_DVD_LPCM			0x5002
#define WAVE_FORMAT_BD_LPCM			0x5003
#define WAVE_FORMAT_ULaw_LPCM		0x5005
#define WAVE_FORMAT_ADPCM_U_LAW		0x0007
#define WAVE_FORMAT_BigEndian_LPCM	0x5006
#define WAVE_FORMAT_PCM_ALAW		0x0006
#define WAVE_FORMAT_HEAAC			0x1610
#define WAVE_FORMAT_AAC_LATM			0x1602	/* AAC LATM/LOAS */
#define WAVE_FORMAT_ADTS_AAC			0x1600
#define WAVE_FORMAT_DOLBY_DIGITAL_PLUS  0x3010	/* EAC3 */
#define WAVE_FORMAT_DTSHD				0x3000
#define WAVE_FORMAT_DTSHD_MASTER_AUDIO		0x3001
#define WAVE_FORMAT_DTSLBR              	0x3008
#define WAVE_FORMAT_TRUEHD			0x3020
#define WAVE_FORMAT_OGG_VORBIS		0x3030
#define WAVE_FORMAT_AVI_VORBIS			0x566F
#define WAVE_FORMAT_AVI_VORBIS_MODE1       0x674F  // Original stream compatible
#define WAVE_FORMAT_AVI_VORBIS_MODE2       0x6750  // Have independent header
#define WAVE_FORMAT_AVI_VORBIS_MODE3       0x6751  // Have no codebook header
#define WAVE_FORMAT_AVI_VORBIS_MODE1_PLUS  0x676F  // Original stream compatible
#define WAVE_FORMAT_AVI_VORBIS_MODE2_PLUS  0x6770  // Have independent header
#define WAVE_FORMAT_AVI_VORBIS_MODE3_PLUS  0x6771  // Have no codebook header
#define WAVE_FORMAT_NOISE_GEN		0x3050
#define WAVE_FORMAT_AVS				0x3080
#define WAVE_FORMAT_SBC				0x3090
#define WAVE_FORMAT_ALAC				0x30A0
#define WAVE_FORMAT_Encode_MPEG		0x6001
#define WAVE_FORMAT_Encode_AC3		0x6002
#define WAVE_FORMAT_Encode_DTS		0x6003
#define WAVE_FORMAT_Encode_AUTO		0x6004
/***********************************
*			ADPCM
************************************/
typedef struct tPCMwaveformat_tag
{
	WAVEFORMATEX  wfx;
	UINT16	nSamplesPerBlock;
	UINT8    reserve[28];
} __attribute__ ((packed)) PCMWAVEFORMAT;

/***********************************
*	BD LPCM
************************************/
typedef struct tBDLPCMWAVEFORMAT
{
	WAVEFORMATEX  wfx;
	UINT16	channel_cfg;
	UINT8    reserve[28];
} __attribute__ ((packed)) BDLPCMWAVEFORMAT;

/***********************************
*			AAC
************************************/
typedef struct tAACWAVEFORMAT
{
	WAVEFORMATEX wfx;
	UINT16      BSType;
	UINT8        reserve[28];
}   __attribute__ ((packed)) AACWAVEFORMAT;

/***********************************
*			WMA Pro
************************************/
typedef struct tWMAPROWAVEFORMAT
{
	WAVEFORMATEX wfx;
	UINT16      cbsize;
	UINT16      nChannelMask;
	UINT8       reserve0[10];
	UINT16      nEncodeOpt;
	UINT8       reserve[14];
}   __attribute__ ((packed)) WMAPROWAVEFORMAT;

/***********************************
*			SBC
************************************/
typedef struct tSBCwaveformat_tag
{
	WAVEFORMATEX  wfx;
	UINT16   nBsType;		//0 for A2DP(SBC decode), 1 for HFP WB(mSBC decode)
	UINT16   nSupportPLC;	//0 for not support, 1 for support
	UINT8    reserve[26];
} __attribute__ ((packed)) SBCWAVEFORMAT;














#endif      // __WAVE_FMT_H

