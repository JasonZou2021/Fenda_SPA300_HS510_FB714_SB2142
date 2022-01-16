/**************************************************************************/
/**
*
* \file	CertTest_vfd_str_tbl.inc
*
* \brief
*
* \note  	Copyright (c) 2016 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author
* \version	 v0.01
* \date 	 2016/09/30
***************************************************************************/

enum _STR_ID_
{
	STR_OFF,
	STR_ON,
	STR_ATMOS_UPMIX,
	STR_ATMOS_TUNING,
	STR_ATMOS_XML,
	STR_ATMOS_VIRT,
	STR_ATMOS_HFT,
	STR_ATMOS_CNTSPD,
	STR_ATMOS_VLAMP,
	STR_ATMOS_LM,
	STR_ATMOS_DIRECT,
	STR_ATMOS_POST,
	STR_ATMOS_DRC,
	STR_ATMOS_DRC_OFF,
	STR_ATMOS_DRC_ON,
	STR_ATMOS_DRC_AUTO,
	STR_ATMOS_PROFILE_OFF,
	STR_ATMOS_PROFILE_MOVIE,
	STR_ATMOS_PROFILE_MUSIC,
	STR_ATMOS_PROFILE_NIGHT,
	STR_ATMOS_PROFILE_DYNAMIC,
	STR_ATMOS_PROFILE_VOICE,
	STR_ATMOS_PROFILE_GAME,
	STR_ATMOS_PROFILE_PERSONALIZE,
	STR_ATMOS_PROFILE_CUSTO_A,
	STR_ATMOS_PROFILE_CUSTO_B,
	STR_ATMOS_PROFILE_CUSTO_C,
	STR_ATMOS_PROFILE_CUSTO_D,
};

//ddp
enum _STR_ID_DDP_
{
	STR_DSP_MODE,
	STR_DSP_MODE_NORMAL,
	STR_DSP_MODE_BYPASS,
	STR_DSP_MODE_DECODER,
	STR_DSP_MODE_VIRTUALX,
	STR_DDP_DRC,
	STR_DDP_DRC_RF,
	STR_DDP_DRC_LINE_ON,
	STR_DDP_DRC_LINE_OFF,
	STR_DDP_LFE,
	STR_DDP_STEREO_DOWNMIX,
	STR_DDP_STEREO_DOWNMIX_AUTO,
	STR_DDP_STEREO_DOWNMIX_LTRT,
	STR_DDP_STEREO_DOWNMIX_LORO,
	STR_DDP_OUTPUT,

}ddp_str_id_e;

//dtsx
enum _STR_ID_DTSX_
{
	STR_DTSX_OFF,
	STR_DTSX_ON,
	STR_DTSX_NEURALX,
	STR_DTSX_ANALOGCOMP,
	STR_DTSX_DECODE96FROM192,
	STR_DTSX_DRC,
	STR_DTSX_DIRECT_DECODE,
	STR_DTSX_DIALOG_CONTROL,
	STR_DTSX_SPEAKER_OUT,
	STR_DTSX_DISABLE_T1,
};

char *CertTestvfd_str[] =  //pointer to string address
{
	"OFF",
	"ON",
	"UPMIX",
	"TUNE",
	"XML",
	"VIRT",
	"HFT",
	"CSPD",
	"VAMP",
	"LM",
	"DIR",
	"POST",
	"DRC",
	"OFF",
	"ON",
	"AUTO",
	"OFF",
	"MOVIE",
	"MUSIC",
	"NIGHT",
	"DYN",
	"VOICE",
	"GAME",
	"PERSONALIZE",
	"CUS A",
	"CUS B",
	"CUS C",
	"CUS D",
};

char *CertTestvfd_ddp_str[] =
{
	"DSPM",
	"NORM",
	"PASS",
	"DEC",
	"VIRTX",
	"DRC",
	"RF",
	"LON",
	"LOFF",
	"LFE",
	"DNMX",
	"AUTO",
	"LTRT",
	"LORO",
	"OUT",
};

char *CertTestvfd_dtsx_str[] =
{
	"OFF",
	"ON",
	"NEUR",
	"ANAC",
	"DEC",
	"DRC",
	"DIR",
	"DIAC",
	"SPKO",
	"T1",
};
