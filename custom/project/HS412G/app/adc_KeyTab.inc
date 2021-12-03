/**************************************************************************/
/**
*
* \file	adc_KeyTab.c
*
* \brief
*
* \note  	Copyright (c) 2016 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author
* \version	 v0.01
* \date 	 2016/09/09
***************************************************************************/
/*---------------------------------------------------------------------------*
*                            INCLUDE   DECLARATIONS                         *
*---------------------------------------------------------------------------*/
#ifndef adc_KeyTab_INC
#define adc_KeyTab_INC


#define ADC_LEVEL_NUM_MAX 	(7)
// 2,5  vol+    1.77 vol-    1.16 src

#define ADC_DATA_LEVEL0 	1800
#define ADC_DATA_LEVEL1 	2500
#define ADC_DATA_LEVEL2 	3600
#define ADC_DATA_LEVEL3 	3600
#define ADC_DATA_LEVEL4 	3600
#define ADC_DATA_LEVEL5 	3600
#define ADC_DATA_LEVEL6 	3600
#define ADC_DATA_LEVEL_MAX	ADC_DATA_LEVEL6


const unsigned int ADC_DataLevelTab[ADC_LEVEL_NUM_MAX] =
{
	ADC_DATA_LEVEL0,
	ADC_DATA_LEVEL1,
	ADC_DATA_LEVEL2,
	ADC_DATA_LEVEL3,
	ADC_DATA_LEVEL4,
	ADC_DATA_LEVEL5,
	ADC_DATA_LEVEL6,
};


#endif

