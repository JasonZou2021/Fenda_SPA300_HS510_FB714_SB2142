
/**********************************************************************/
/**
 *
 * \file 	mf_player.h
 *
 * \brief media player
 * 		detail standard header, player define
 *
 * \note Copyright (c) 2016 Sunplus Technology Co., Ltd. \n
 * 		All rights reserved.
 *
 * \author lu.zhou@sunplus.com.cn
 *
 * \version v0.01
 *
 * \date 2016-05-23
 *
 ***********************************************************************/

#ifndef __MF_PLAYER_H__
#define __MF_PLAYER_H__

/*---------------------------------------------------------------------*
 *                                          INCLUDE DECLARATIONS                                       *
 *---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*
 *                                                MACRO DECLARATIONS                                    *
 *---------------------------------------------------------------------*/

/**
 * \brief play commad declaration
 */
typedef enum mfpCmd_e
{
	MFP_SET_PLAY_MIN = 0, /**< set play parameter min command */
	MFP_SET_URL = 0, /**< set play url, parameter type is char* */
	MFP_SET_FD, /**< set play file descripation, parameter type is fd */
	MFP_SET_BOOKMARK, /**< set play bookmark, parameter type is int, unit in ms */
	MFP_SET_RESFMT, /**< set play resource format, parameter type is mfResFmt_t */
	MFP_SET_CODEC_INFO,	/**< set codec info, parameter type is mfpOpaque_t*/
	MFP_SET_LISTENER, /**< set play notify listener, parameter type is mfpListener_t */
	MFP_SET_DESTORY_LISTENER,
	MFP_SET_NEXT, /**< play next , should here??? */
	MFP_SET_PLAY_MODE, /**< play next , should here??? */
	MFP_GET_PLAY_MODE, /**< play next , should here??? */
	MFP_SET_PLAY_REPEAT, /**< play next , should here??? */
	MFP_GET_PLAY_REPEAT, /**< play next , should here??? */
	MFP_SET_SWSRC_FLAG,
	MFP_SET_CLEAN_RESUME,
	MFP_SET_PLAY_MAX, /**< set play parameter  max command*/

	MFP_GET_PLAY_MIN = 100, /**< get play parameter min command */
	MFP_GET_STATE, /**< get play state */
	MFP_GET_DURATION, /**< get play duration */
	MFP_GET_PLAY_TIME, /**< get play time */
	MFP_GET_PLAY_MAX, /**< get play parameter  max command*/

	MFP_SET_VIDEO_MIN = 200, /**< set video parameter min command */
	MFP_SET_VIDEO_MAX, /**< set video parameter  max command*/

	MFP_GET_VIDEO_MIN = 300, /**< get video parameter min command */
	MFP_GET_VIDEO_MAX, /**< get video parameter  max command*/

	MFP_SET_AUDIO_MIN = 400, /**< set audio parameter min command */
	MFP_SET_VOLUME, 		/**< mfpVolume_t type */
	MFP_SET_MUTE,			   /**< 1: enable/0: disable, int type */
	MFP_SET_DEMUTE, 		   /**< 1: enable/0: disable, int type */
	MFP_SET_BYPASS_MODE,	   /**< 1: enable/0: disable, int type */
	MFP_SET_EQ, 			   /**< 1: enable/0: disable, int type */
	MFP_SET_EQ_MC_CTL,		   /**< mfpEQMCCTL_t type */
	MFP_SET_SURROUND,		   /**< mfpSurround_t type */
	MFP_SET_TRU_SURR,		   /**< mfpSurround_t type */
	MFP_SET_PROLOGIC,		   /**< mode,  int type */
	MFP_SET_GM5PL2, 		   /**< mode,  int type */
	MFP_SET_DVS,			   /**< mode,  mfpDVS_t type */
	MFP_SET_SPEAKER_OUT_DELAY, /**< x,	int type */
	MFP_SET_DRC,			   /**< mode,  mfpDRC_t type */
	MFP_SET_AVL_CTL,		   /**< mode,  mfpAVLCTL_t type */
	MFP_SET_AO, /**< set user AO handle to player */
	MFP_WRITE_AUD_DATA,	/**< write data to player. only for bitstream without container*/
	MFP_SET_AUDIO_MAX, /**< set audio parameter  max command*/

	MFP_GET_AUDIO_MIN = 500, /**< get audio parameter min command */
	MFP_GET_AUDIO_MAX, /**< get audio parameter  max command*/

	MFP_SET_SUB_MIN = 600, /**< set subtitle parameter min command */
	MFP_SET_SUB_MAX, /**< set subtitle parameter  max command*/

	MFP_GET_SUB_MIN = 700, /**< get subtitle parameter min command */
	MFP_GET_SUB_MAX, /**< get subtitle parameter  max command*/

	MFP_CMD_MAX, /**< max command*/
} mfpCmd_t;

/**
 * \brief audio player tricky speed declaration
 */
typedef enum mfpSpeed_e
{
	MFP_SPEED_1X = 0,
	MFP_SPEED_FF2X,
	MFP_SPEED_FF4X,
	MFP_SPEED_FF8X,
	MFP_SPEED_FB2X,
	MFP_SPEED_FB4X,
	MFP_SPEED_FB8X,
	MFP_SPEED_MAX,
} mfpSpeed_t;

/**
 *\brief media player error code declaration
 */
typedef enum mfpRet_e
{
	MFP_ERR_NO = 0, /**< no error */
	MFP_ERR_UNKNOWN, /**< unknown error */
	MFP_ERR_PARAM, /**< parameter invalid */
	MFP_ERR_NOMEM, /**< no memory */
	MFP_ERR_RES, /**< resource open error */
	MFP_ERR_UNSUPPORT, /**< current operation unsupport */
	MFP_ERR_ASYNC, /**< async finish  */
	MFP_ERR_STATE, /**< state error for current operation */
	MFP_ERR_PARSE_UNSUPPORT, /**< parse unsupport error */
	MFP_ERR_DECODE_UNSUPPORT, /**< decode unsupport */
	MFP_ERR_SEEK_UNSUPPORT, /**< seek unsupport */
	MFP_ERR_READ_FAIL, /**< read resource fail */
	MFP_ERR_PARSE_FAIL, /**< parse fail */
	MFP_ERR_PARSE_END, /**< parse end */
	MFP_ERR_DECODE_FAIL, /**< decode fail */
	MFP_ERR_NO_DEVICE, /**< no device */
	MFP_ERR_DEVICE, /**< error device */
	MFP_MSG_NO_FILE,

} mfpRet_t;

/**
 *\brief media player notify message declaration
 */

typedef enum mfpMsg_e
{
	MFP_MSG_ERR = 0, /**< play error message, error code defined in mfpRet_t */
	MFP_MSG_PREPARE_FINISH, /**< async prepare finish message*/
	MFP_MSG_SEEK_FINISH, /**< async seek finish message*/
	MFP_MSG_STOP_FINISH, /**< async stop end message */
	MFP_MSG_PLAY_START, /**< play start message, audio/video start presentation */
	MFP_MSG_PARSE_END, /**< parse to the tail of resource message */
	MFP_MSG_PLAY_END, /**< play to the tail of resource message */
	MFP_MSG_TRICKY_END, /**< tricky to the head/tail of resource message */
	MFP_MSG_SWITCH_NEXT, /**< switch to next file of resource message */
	MFP_MSG_SWITCH_PLAY, /**< switch to next file of resource message */
	MFP_MSG_SWITCH_STOP, /**< switch to next file of resource message */
	MFP_MSG_STOP_TO_PLAY, /**< switch to  file of resource message */
	MFP_MSG_STATE_CHANGE,
	MFP_MSG_UPDATE_FILE,
	MFP_MSG_UPDATE_FOLDER,
	MFP_MSG_AUTO_PLAY,
	MFP_MSG_ERR_DEVICE,

} mfpMsg_t;

/**
 * \brief media player state declaration
 */
typedef enum mfpState_e
{
	MFP_STATE_STOP = 0,
	MFP_STATE_PERPARE,
	MFP_STATE_PLAY,
	MFP_STATE_PAUSE,
	MFP_STATE_SEEK,
	MFP_STATE_TRICKY,
} mfpState_t;

/**
 * \brief media player capability function declaration
 */
typedef enum mfpCap_e
{
	/**< prepare, seek, stop API, will be notify finish by message of listener*/
	MFP_CAP_ASYNC = (1 << 0),
} mfpCap_t;

/**
* \brief codec declaration for user interface
*/
typedef enum
{
	CDC_UNKNOWN = 0,
	CDC_AUD_S,
	CDC_AUD_ALAC,
	CDC_AUD_MP3,
	CDC_AUD_AAC,
	CDC_AUD_PCM,
	CDC_AUD_EAC3,
	CDC_AUD_E,
} mfpCdc_t;

enum
{
	PLAY_MODE_ORDER = 0,
	PLAY_MODE_SHUFFLE,
	PLAY_MODE_RANDOM,
	PLAY_MODE_UNKNOW,

	PLAY_MODE_NUM,
};

enum
{
	REPEAT_MODE_NOTHING = 0,
	REPEAT_MODE_ALL,
	REPEAT_MODE_SINGLE_NOREPEAT,
	REPEAT_MODE_SINGLE,
#if (SUPPORT_FLODER)
	REPEAT_MODE_DIR,
#endif
	REPEAT_MODE_USER,

	REPEAT_MODE_NUM,
};

enum
{
	PLAY_NOTHING = 0x0,
	PLAY_NEXT_FILE = 0x1,
	PLAY_PREV_FILE = -0x1,
	PLAY_CUR_FILE = 0x2,
	PLAY_NEXT_FOLDER = 0x10,
	PLAY_PREV_FOLDER = -0x10,
	PLAY_NEXT_PAGE = 0x100,
	PLAY_PREV_PAGE = -0x100,
};

enum
{
	PLAY_SRC_USB = 0,
	PLAY_SRC_CARD,
	PLAY_SRC_AUX,
	PLAY_SRC_LINE,
	PLAY_SRC_SPDIF_Optical,
	PLAY_SRC_SPDIF_Coaxial,
	PLAY_SRC_ARC,
	PLAY_SRC_Bt,
	PLAY_SRC_Dlna,
	PLAY_SRC_Airplay,
	PLAY_SRC_RAWDATA,
};

enum
{
	SRC_NOTHING = 0x0,
	SRC_USB_IN = 0x1,
	SRC_USB_OUT = 0x2,
	SRC_USB_READY = 0x4,
	SRC_USB_MASK = 0xF,
	SRC_CARD_IN = 0x10,
	SRC_CARD_OUT = 0x20,
	SRC_CARD_READY = 0x40,
	SRC_CARD_MASK = 0xF0,
};

typedef enum mfResFmt_e
{
	MF_RES_FMT_UNKNOWN = 0,
	// for movie container format
	MF_RES_FMT_AVI,
	MF_RES_FMT_ASF, /*!< @brief for asf/wmv/wma */
	MF_RES_FMT_FLV,
	MF_RES_FMT_MKV, /*!< @brief for mka/mkv */
	MF_RES_FMT_MP4, /*!< @brief for mp4/3gp/mov/m4a */
	MF_RES_FMT_OGV, /*!< @brief for ogv/ogg */
	MF_RES_FMT_RMVB, /*!< @brief for rm/rmvb/ra */
	MF_RES_FMT_VOB, /*!< @brief for true vob */
	MF_RES_FMT_PS, /*!< @brief for mpg/dat/vob */
	MF_RES_FMT_TS,
	MF_RES_FMT_SWF,

	MF_RES_FMT_DVB,
	MF_RES_FMT_HXS,
	MF_RES_FMT_ASX,
	MF_RES_FMT_RTSP,

	MF_RES_FMT_CSFF, /*!< @brief for ff container => avformat */
	MF_RES_FMT_CSRGB,
	MF_RES_FMT_CSYUV,
	MF_RES_FMT_MOVIE_MAX,

	// for music container format
	MF_RES_FMT_MUSIC = 0x20,
	MF_RES_FMT_CSAAC,
	MF_RES_FMT_CSAC3,
	MF_RES_FMT_CSDTS,
	MF_RES_FMT_CSAMR,
	MF_RES_FMT_CSAPE,
	MF_RES_FMT_CSFLAC,
	MF_RES_FMT_CSMP3,
	MF_RES_FMT_CSPCM,
	MF_RES_FMT_WAV,
	MF_RES_FMT_M4A,
	MF_RES_FMT_MKA,
	MF_RES_FMT_WMA,
	MF_RES_FMT_OGG,
	MF_RES_FMT_APPEND,
	MF_RES_FMT_AIFF,
	MF_RES_FMT_DSD,
	MF_RES_FMT_MUSIC_MAX,

	// for subtitle container format
	MF_RES_FMT_SUBTITLE = 0x40,
	MF_RES_FMT_ASS,
	MF_RES_FMT_SMI,
	MF_RES_FMT_SRT,
	MF_RES_FMT_SSA,
	MF_RES_FMT_SUBMD, /*!< @brief for external macro dvd subtitle */
	MF_RES_FMT_SUBV, /*!< @brief for external SubViewer subtitle */
	MF_RES_FMT_IDX, /*!< @brief for external picture subtitle */
	MF_RES_FMT_SUBTITLE_MAX,

	// for picture container format
	MF_RES_FMT_PICTURE = 0x60,
	MF_RES_FMT_BMP,
	MF_RES_FMT_CSJPG,
	MF_RES_FMT_GIF,
	MF_RES_FMT_PNG,
	MF_RES_FMT_TIF,
	MF_RES_FMT_PICTURE_MAX,

	// for video codec format
	MF_RES_FMT_ESVIDEO = 0x100,
	MF_RES_FMT_MPEG1,
	MF_RES_FMT_MPEG2,
	MF_RES_FMT_MPEG4,		// mpeg4
	MF_RES_FMT_MPEG4_SP,		// mpeg4 sp
	MF_RES_FMT_MPEG4_ASP,	    // mpeg4 asp
	MF_RES_FMT_MPEG4_HD,
	MF_RES_FMT_JPEG,
	MF_RES_FMT_MJPEG,
	MF_RES_FMT_H263,
	MF_RES_FMT_H263_SORENSON, /*for Sorenson H.263*/
	MF_RES_FMT_H263_PLUS,
	MF_RES_FMT_H264,
	MF_RES_FMT_H264_BP,
	MF_RES_FMT_H264_MP,
	MF_RES_FMT_H264_HP,
	MF_RES_FMT_H265,
	MF_RES_FMT_WMV7,
	MF_RES_FMT_WMV8,
	MF_RES_FMT_WMV9,
	MF_RES_FMT_VC1,
	MF_RES_FMT_VC1_SP,         /*for VC1 SP */
	MF_RES_FMT_VC1_AP,         /*for VC1 MP and AP */
	MF_RES_FMT_VC1_CP,		   /*for VC1 CP */
	MF_RES_FMT_RV10,           //15:
	MF_RES_FMT_RV20,           /* RVG2 */
	MF_RES_FMT_RV30,           /* RV8 */
	MF_RES_FMT_RV40,           /* RV9 and RV10 */
	MF_RES_FMT_AVS,
	MF_RES_FMT_DIVX3,
	MF_RES_FMT_XVID,
	MF_RES_FMT_VP6,
	MF_RES_FMT_VP7,
	MF_RES_FMT_VP8,
	MF_RES_FMT_MS_MP41,
	MF_RES_FMT_MS_MP42,
	MF_RES_FMT_MS_MP43,
	MF_RES_FMT_THEORA,
	MF_RES_FMT_CINEPACK,
	MF_RES_FMT_ESVIDEO_MAX,

	// for video raw format
	MF_RES_FMT_RSVIDEO = 0x200,
	MF_RES_FMT_RGB565,
	MF_RES_FMT_RGAB5515,
	MF_RES_FMT_ARGB1555,
	MF_RES_FMT_ARGB4444,
	MF_RES_FMT_RGB888,
	MF_RES_FMT_ARGB8888,
	MF_RES_FMT_RGBA8888,       // SP_BITMAP_RGBA8888
	MF_RES_FMT_BGR565,
	MF_RES_FMT_RGB555,
	MF_RES_FMT_BGAR5515,
	MF_RES_FMT_ABGR1555,
	MF_RES_FMT_ABGR4444,
	MF_RES_FMT_BGR888,
	MF_RES_FMT_ABGR8888,
	MF_RES_FMT_BGRA8888,	   // SP_BITMAP_BGRA8888
	MF_RES_FMT_BGRG,
	MF_RES_FMT_GBGR,
	MF_RES_FMT_RGBG,
	MF_RES_FMT_GRGB,
	MF_RES_FMT_1BPP,
	MF_RES_FMT_2BPP,
	MF_RES_FMT_4BPP,
	MF_RES_FMT_8BPP,
	MF_RES_FMT_YCbCr,
	MF_RES_FMT_YUV,
	MF_RES_FMT_YCbYCr,
	MF_RES_FMT_YUYV,
	MF_RES_FMT_YVYU,
	MF_RES_FMT_UYVY,
	MF_RES_FMT_VYUY,
	MF_RES_FMT_4Y4U4Y4V,
	MF_RES_FMT_4Y4Cb4Y4Cr,
	MF_RES_FMT_YCbCr400,
	MF_RES_FMT_YUV400,
	MF_RES_FMT_YCbCr422,
	MF_RES_FMT_YUV422,
	MF_RES_FMT_YCbCr444,
	MF_RES_FMT_YUV444,
	MF_RES_FMT_YCbCr420,       /*!< @brief CDSP decoder output format */
	MF_RES_FMT_YUV420,         /*!< @brief CDSP decoder output format */
	MF_RES_FMT_SEMI400,        //  SP_BITMAP_SEMI400
	MF_RES_FMT_SEMI420, 	   //  SP_BITMAP_SEMI420
	MF_RES_FMT_SEMI422,        // SP_BITMAP_SEMI422
	MF_RES_FMT_SEMI444,        // SP_BITMAP_SEMI444
	MF_RES_FMT_PAL8,	   // FF subtitle color index picture

	MF_RES_FMT_RSVIDEO_MAX,

	// for audio codec format
	MF_RES_FMT_ESAUDIO = 0x300,
	MF_RES_FMT_AC2,              //= CODEC_ID_AC2,
	MF_RES_FMT_AC3, 			 //= CODEC_ID_AC3,
	MF_RES_FMT_EAC3,             //= CODEC_ID_EAC3,
	MF_RES_FMT_TRUEHD,           //= CODEC_ID_TRUEHD,
	MF_RES_FMT_ATRAC3,           //= CODEC_ID_ATRAC3,
	MF_RES_FMT_A52,              //= CODEC_ID_A52,
	MF_RES_FMT_MP2,              //= CODEC_ID_MP2,
	MF_RES_FMT_MP3,              //= CODEC_ID_MP3,
	MF_RES_FMT_MP2A,             //= CODEC_ID_MP2A,
	MF_RES_FMT_MP4A,             //= CODEC_ID_MP4A,
	MF_RES_FMT_AAC,              //= CODEC_ID_AAC,
	MF_RES_FMT_NEC_AAC, 		  //= CODEC_ID_NEC_AAC,
	MF_RES_FMT_DIVIO_AAC, 		  //= CODEC_ID_NEC_AAC,
	MF_RES_FMT_AAC_PLUS,         //= CODEC_ID_AAC_PLUS,
	MF_RES_FMT_AAC_LATM,         //= CODEC_ID_AAC_LATM, //mm.xie add code
	MF_RES_FMT_AMR_NB,           //= CODEC_ID_AMR_NB,
	MF_RES_FMT_AMR_WB,           //= CODEC_ID_AMR_WB,
	MF_RES_FMT_FLAC,             //= CODEC_ID_FLAC,
	MF_RES_FMT_APE,              //= CODEC_ID_APE,
	MF_RES_FMT_ALAC,             //= CODEC_ID_ALAC,
	MF_RES_FMT_COOK,             //= CODEC_ID_COOK,
	MF_RES_FMT_WMA1,              //= CODEC_ID_WMAV1,
	MF_RES_FMT_WMA2,             //= CODEC_ID_WMAV2,
	MF_RES_FMT_WMAPRO,           //= CODEC_ID_WMAPRO,
	MF_RES_FMT_WMALOSSLESS,      //=CODEC_ID_WMALOSSLESS
	MF_RES_FMT_WMAVOICE, 	 //=CODEC_ID_WMALOSSLESS
	MF_RES_FMT_PCM,              //= CODEC_ID_PCM_S16LE, // need fix
	MF_RES_FMT_PCM_BLURAY,       //= CODEC_ID_PCM_BLURAY,
	MF_RES_FMT_ADPCM,			 //= CODEC_ID_ADPCM_MS,
	MF_RES_FMT_ADPCM_MS,         //= CODEC_ID_ADPCM_MS,
	MF_RES_FMT_ADPCM_IMA,        //= CODEC_ID_ADPCM_IMA_WAV,
	MF_RES_FMT_ADPCM_IMA_QT,     //= CODEC_ID_ADPCM_IMA_QT,
	MF_RES_FMT_ADPCM_G721,		 //= CODEC_ID_ADPCM_G721,
	MF_RES_FMT_ADPCM_G723,		 //= CODEC_ID_ADPCM_G723,
	MF_RES_FMT_ADPCM_G726,       //= CODEC_ID_ADPCM_G726,
	MF_RES_FMT_ADPCM_G728,		 //= CODEC_ID_ADPCM_G728,
	MF_RES_FMT_ADPCM_ALAW,       //= CODEC_ID_PCM_ALAW,
	MF_RES_FMT_ADPCM_MULAW,      //= CODEC_ID_PCM_MULAW,
	MF_RES_FMT_ADPCM_SWF,        //= AV_CODEC_ID_ADPCM_SWF,
	MF_RES_FMT_PCM_ZORK,         //= CODEC_ID_PCM_ZORK,
	MF_RES_FMT_VORBIS,           //= CODEC_ID_VORBIS,
	MF_RES_FMT_DTS,              //= CODEC_ID_DTS,
	MF_RES_FMT_SDDS,             //= ,
	MF_RES_FMT_SIPR,             //= AV_CODEC_ID_SIPR,
	MF_RES_FMT_RT29,
	MF_RES_FMT_IEEE_FLOAT,
	MF_RES_FMT_ESAUDIO_MAX,

	// for audio raw format
	MF_RES_FMT_RSAUDIO = 0x400,
	MF_RES_FMT_PCMU8,
	MF_RES_FMT_PCMS8,

	MF_RES_FMT_PCMS16L,//PCM signed 16-bit little-endian
	MF_RES_FMT_PCMS16B,//PCM signed 16-bit bit-endian
	MF_RES_FMT_PCMU16L,//PCM unsigned 16-bit little-endian
	MF_RES_FMT_PCMU16B,//PCM unsigned 16-bit big-endian

	MF_RES_FMT_PCMS24L,//PCM signed 24-bit little-endian
	MF_RES_FMT_PCMS24B,//PCM signed 24-bit big-endian
	MF_RES_FMT_PCMU24L,//PCM unsigned 24-bit little-endian
	MF_RES_FMT_PCMU24B,//PCM unsigned 24-bit big-endian

	MF_RES_FMT_PCMS32L,//PCM signed 32-bit little-endian
	MF_RES_FMT_PCMS32B,//PCM signed 32-bit big-endian
	MF_RES_FMT_PCMU32L,//PCM unsigned 32-bit little-endian
	MF_RES_FMT_PCMU32B,//PCM unsigned 32-bit big-endian

	MF_RES_FMT_PCMF32L,//PCM 32-bit floating-point little-endian
	MF_RES_FMT_PCMF32B,//PCM 32-bit floating-point big-endian
	MF_RES_FMT_PCMF64L,//PCM 64-bit floating-point little-endian
	MF_RES_FMT_PCMF64B,//PCM 32-bit floating-point big-endian

	MF_RES_FMT_PCMU8P, //PCM unsigned 8-bit planar
	MF_RES_FMT_PCMS8P, //PCM signed 8-bit planar
	MF_RES_FMT_PCMS16P,//PCM signed 16-bit little-endian planar
	MF_RES_FMT_PCMS24P,//PCM signed 24-bit little-endian planar
	MF_RES_FMT_PCMS32P,//PCM signed 32-bit little-endian planar
	MF_RES_FMT_PCMF32P,//PCM 32-bit floating-point little-endian planar
	MF_RES_FMT_PCMF64P,//PCM 64-bit floating-point little-endian planar

	MF_RES_FMT_PCM_MONO  = 0x480, 		//AV_CH_LAYOUT_MONO
	MF_RES_FMT_PCM_STEREO, 				//AV_CH_LAYOUT_STEREO
	MF_RES_FMT_PCM_2POINT1, 			//AV_CH_LAYOUT_2POINT1
	MF_RES_FMT_PCM_2_1, 					//AV_CH_LAYOUT_2_1
	MF_RES_FMT_PCM_SURROUND, 			//AV_CH_LAYOUT_SURROUND
	MF_RES_FMT_PCM_3POINT1, 			//AV_CH_LAYOUT_3POINT1
	MF_RES_FMT_PCM_4POINT0, 			//AV_CH_LAYOUT_4POINT0
	MF_RES_FMT_PCM_4POINT1, 			//AV_CH_LAYOUT_4POINT1
	MF_RES_FMT_PCM_2_2, 					//AV_CH_LAYOUT_2_2
	MF_RES_FMT_PCM_QUAD, 				//AV_CH_LAYOUT_QUAD
	MF_RES_FMT_PCM_5POINT0, 			//AV_CH_LAYOUT_5POINT0
	MF_RES_FMT_PCM_5POINT1,				//AV_CH_LAYOUT_5POINT1
	MF_RES_FMT_PCM_5POINT0_BACK, 		//AV_CH_LAYOUT_5POINT0_BACK
	MF_RES_FMT_PCM_5POINT1_BACK, 		//AV_CH_LAYOUT_5POINT1_BACK
	MF_RES_FMT_PCM_6POINT0,				//AV_CH_LAYOUT_6POINT0
	MF_RES_FMT_PCM_6POINT0_FRONT, 		//AV_CH_LAYOUT_6POINT0_FRONT
	MF_RES_FMT_PCM_HEXAGONAL, 		//AV_CH_LAYOUT_HEXAGONAL
	MF_RES_FMT_PCM_6POINT1, 			//AV_CH_LAYOUT_6POINT1
	MF_RES_FMT_PCM_6POINT1_BACK, 		//AV_CH_LAYOUT_6POINT1_BACK
	MF_RES_FMT_PCM_6POINT1_FRONT, 		//AV_CH_LAYOUT_6POINT1_FRONT
	MF_RES_FMT_PCM_7POINT0, 			//AV_CH_LAYOUT_7POINT0
	MF_RES_FMT_PCM_7POINT0_FRONT, 		//AV_CH_LAYOUT_7POINT0_FRONT
	MF_RES_FMT_PCM_7POINT1, 			//AV_CH_LAYOUT_7POINT1
	MF_RES_FMT_PCM_7POINT1_WIDE,		//AV_CH_LAYOUT_7POINT1_WIDE
	MF_RES_FMT_PCM_7POINT1_WIDE_BACK, //AV_CH_LAYOUT_7POINT1_WIDE_BACK
	MF_RES_FMT_PCM_OCTAGONAL,			//AV_CH_LAYOUT_OCTAGONAL
	MF_RES_FMT_PCM_STEREO_DOWNMIX,	//AV_CH_LAYOUT_STEREO_DOWNMIX
	MF_RES_FMT_RSAUDIO_MAX,

	// subtitle codec format
	MF_RES_FMT_ESSUBTITLE = 0x500,
	MF_RES_FMT_TXT,
	MF_RES_FMT_SUBVOB,
	MF_RES_FMT_DVB_TELETEXT,
	MF_RES_FMT_DVB_SUBTITLE,
	MF_RES_FMT_HDMV_PGS_SUBTITLE,
	MF_RES_FMT_XSUB,
	MF_RES_FMT_XSUB_PLUS,
	MF_RES_FMT_ESSUBTITLE_END,
} mfResFmt_t;

/*---------------------------------------------------------------------*
 *                                                DATE TYPES                                                   *
 *---------------------------------------------------------------------*/
/**
 *\brief media player notify listener struct declaration
 */
typedef struct mfpListener_s
{
	void *user;
	void (*notify)(void *user, mfpMsg_t msg, int param);
} mfpListener_t;

/**
 * \brief volume parameter struct declaration
 */
typedef struct mfpVolume_s
{
	int channel;
	int value;
} mfpVolume_t;

/**
 * \brief EQ MC CTL parameter struct declaration
 */
typedef struct mfpEQMCCTL_s
{
	int cmd;
	int band;
	int val;
	int ch;
} mfpEQMCCTL_t;

/**
 * \brief Surround/TrueSurround parameter event struct declaration
 */
typedef struct mfpSurround_s
{
	int param;
	int val;
} mfpSurround_t;

/**
 * \brief DVS parameter struct declaration
 */
typedef struct mfpDVS_s
{
	int enable;
	int layout;
	int mode;
} mfpDVS_t;

/**
 * \brief DRC parameter struct declaration
 */
typedef struct mfpDRC_s
{
	int param;
	int val;;
} mfpDRC_t;

/**
 * \brief AVLCTL parameter struct declaration
 */
typedef struct mfpAVLCTL_s
{
	int cmd;
	int val;;
} mfpAVLCTL_t;

/**
* \brief audio codec info structure for user interface
*/
typedef struct
{
	unsigned short fmt;	/*reference mfpCdc_t*/
	unsigned short channels;
	unsigned int sample_rate;
	unsigned short block_align;	/*same as sample_size*/
	unsigned short sample_size;	/*bits per sample*/
} mfpAudInfo_t;

typedef struct
{
	unsigned int size;	/*size of data in opaque*/
	void *opaque;	/*codec specific data, mfpAudInfo_t for audio*/
} mfpOpaque_t;

/*---------------------------------------------------------------------*
 *                                              GLOBAL DATA                                                  *
 *---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*
 *                                         EXTERNAL DECLARATIONS                                      *
 *---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*
 *                                         FUNCTION DECLARATIONS                                      *
 *---------------------------------------------------------------------*/


/***********************************************************************
  *
  * \fn  mfpRet_t MFP_Create(void **hd, char *name, int source)
  *
  * \brief	 create player
  * 	             create media framework player
  *
  * \param [in] hd: the address of player instance
  * \param [in] name: player name
  *
  * \param [out] *hd: player instance handle
  *
  * \return ok retrun MFP_ERR_NO, or error code
  *
  * \note
  *
  ***********************************************************************/
mfpRet_t MFP_Create(void **hd, char *name, int source, void *caller);

/***********************************************************************
  *
  * \fn  void MFP_Destroy(void *hd)
  *
  * \brief	 Destroy player
  * 	             Destroy player
  *
  * \param [in] hd: player instance handle
  *
  * \param [out]
  *
  * \return MFP_ERR_NO is success, or error code
  *
  * \note the player instance created with MFP_Create()
  *
  ***********************************************************************/
mfpRet_t MFP_Destroy(void *hd);

/***********************************************************************
  *
  * \fn  mfpRet_t MFP_Ctrl(void *hd, mfpCmd_t cmd, void *param)
  *
  * \brief	 player control
  * 	             set/get play parameter, or control play action
  *
  * \param [in] hd: player instance handle
  * \param [in] cmd: control command
  * \param [in] param: set/get/control parameter address
  *
  * \param [out] *param: get pamater
  *
  * \return MFP_ERR_NO is success, or error code
  *
  * \note
  *
  ***********************************************************************/
mfpRet_t MFP_Ctrl(void *hd, mfpCmd_t cmd, void *param);
/***********************************************************************
  *
  * \fn  mfpRet_t MFP_Perpare(void *hd)
  *
  * \brief	 prepare player
  * 	             prepare player from stop state
  *
  * \param [in] hd: player instance handle
  *
  * \param [out]
  *
  * \return MFP_ERR_NO is success, or error code
  *
  * \note
  *
  ***********************************************************************/
mfpRet_t MFP_Perpare(void *hd);

/***********************************************************************
  *
  * \fn  mfpRet_t MFP_Play(void *hd)
  *
  * \brief	 start play
  * 	             prepare and play start
  *
  * \param [in] hd: player instance handle
  *
  * \param [out]
  *
  * \return MFP_ERR_NO is success, or error code
  *
  * \note
  *
  ***********************************************************************/
mfpRet_t MFP_Play(void *hd);

/***********************************************************************
  *
  * \fn  mfpRet_t MFP_PlayModeSet(void *hd, int8_t playMode)
  *
  * \brief	 start play
  * 	             prepare and play start
  *
  * \param [in] hd: player instance handle
  * \param [in] playMode: playmode to set PLAY_MODE_ORDER,PLAY_MODE_RANDOM,PLAY_MODE_SHUFFLR
  *
  * \param [out]
  *
  * \return MFP_ERR_NO is success, or error code
  *
  * \note
  *
  ***********************************************************************/
mfpRet_t MFP_PlayModeSet(void *hd, int playMode);

/***********************************************************************
  *
  * \fn  mfpRet_t MFP_PlayModeSet(void *hd, int8_t playMode)
  *
  * \brief	 start play
  * 	             prepare and play start
  *
  * \param [in] hd: player instance handle
  * \param [in] repeat: repeat 1 for repeat, 0 for not repeat
  *
  * \param [out]
  *
  * \return MFP_ERR_NO is success, or error code
  *
  * \note
  *
  ***********************************************************************/
mfpRet_t MFP_PlayRepeatSet(void *hd, int repeat);

/***********************************************************************
  *
  * \fn  mfpRet_t MFP_PlayNext(void *hd, int direction)
  *
  * \brief	 start play
  * 	             prepare and play start
  *
  * \param [in] hd: player instance handle
  * \param [in] direction: direction 1 for next, -1 for prev
  *
  * \param [out]
  *
  * \return MFP_ERR_NO is success, or error code
  *
  * \note
  *
  ***********************************************************************/
mfpRet_t MFP_PlayNext(void *hd, int direction);


/***********************************************************************
  *
  * \fn  mfpRet_t MFP_Stop(void *hd)
  *
  * \brief	 stop player
  * 	             stop player, will be released the memory that malloc by prepare and play
  *
  * \param [in] hd: player instance handle
  *
  * \param [out]
  *
  * \return MFP_ERR_NO is success, or error code
  *
  * \note
  *
  ***********************************************************************/
mfpRet_t MFP_Stop(void *hd);

/***********************************************************************
  *
  * \fn  mfpRet_t MFP_Pause(void *hd)
  *
  * \brief	 pause player
  * 	             pause player from play or tricky state
  *
  * \param [in] hd: player instance handle
  *
  * \param [out]
  *
  * \return MFP_ERR_NO is success, or error code
  *
  * \note
  *
  ***********************************************************************/

mfpRet_t MFP_Pause(void *hd);
/***********************************************************************
  *
  * \fn  mfpRet_t MFP_Seek(void *hd)
  *
  * \brief	 seek player
  * 	             seek to the specified timestamp to play
  *
  * \param [in] hd: player instance handle
  * \param [in] ms: the specified timestamp, unit in ms
  *
  * \param [out]
  *
  * \return MFP_ERR_NO is success, or error code
  *
  * \note
  *
  ***********************************************************************/

mfpRet_t MFP_Seek(void *hd, int ms);
/***********************************************************************
  *
  * \fn  mfpRet_t MFP_Tricky(void *hd, mfpSpeed_t speed)
  *
  * \brief	 tricky player
  * 	             tricky play with the specified speed
  *
  * \param [in] hd: player instance handle
  * \param [in] speed: the specified speed
  *
  * \param [out]
  *
  * \return MFP_ERR_NO is success, or error code
  *
  * \note
  *
  ***********************************************************************/
mfpRet_t MFP_Tricky(void *hd, mfpSpeed_t speed);
/***********************************************************************
  *
  * \fn   mfpRet_t MFP_Fetch(void *hd, int ms, void *frame)
  *
  * \brief	 fetch thumbnail
  * 	             fetch the thumbnail of the specified timestamp
  *
  * \param [in] hd: player instance handle
  * \param [in] ms: the specified timestamp, unit in ms
  * \param [in] mbuf: the thumbnail media bufffer
  *
  * \param [out]
  *
  * \return MFP_ERR_NO is success, or error code
  *
  * \note
  *
  ***********************************************************************/
mfpRet_t MFP_Fetch(void *hd, int ms, void *mbuf);

/********************************************************************
* \fn  int MFP_Write(void *hd, unsigned char *src, int size)

* \brief  push data to a given player

* \param [in] hd: player instance
* \param [in] src: data source pointer
* \param [in] size: data size

* \return  data lenght pushed to player

* \note  only used in stream without container
********************************************************************/
int MFP_Write(void *hd, const unsigned char *src, int size);
#if defined(MF_DEBUG)
	mfpRet_t MFP_Test(void);
#endif

#endif /* __MF_PLAYER_H__ */
