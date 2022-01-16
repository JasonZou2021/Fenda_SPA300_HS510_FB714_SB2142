/*${.::UserApp.h} ..............................................................*/

#ifndef AMP_CFG_h
#define AMP_CFG_h

#include "types.h"

typedef enum _eAmpModule
{
	eTas5711 = 0,
	eAD83584D = 1,
	eAD85050 = 2,
	eTas5825M = 3,
	ePCM5242 = 4,
	MAX_AMP,
} eAmpModule;

typedef enum _eAmpIdx
{
	eAmp1,
	eAmp2,
	eAmp3,
	eAmp4,
	eAmp5,
	eAmp6,
	eAmp7,
	eAmp8,
	eAmpMax,
} eAmpIdx;

typedef enum eAmpSignal
{
	eAmpSignal_NULL,
	eAmpSignal_ANALOG,
	eAmpSignal_I2S,
	eAmpSignal_Max,
} eAmpSignal_t;

typedef struct
{
	eAmpModule AmpModule;					//功放模块
	BYTE AmpNum;							//功放的数量				
	BYTE SignalModeAmp1;					//信号模式					
	BYTE SignalModeAmp2;
	BYTE SignalModeAmp3;
	BYTE SignalModeAmp4;
	BYTE SignalModeAmp5;
	BYTE SignalModeAmp6;
	BYTE SignalModeAmp7;
	BYTE SignalModeAmp8;
	BYTE SpeakerCh;
	BYTE MainGainStepNum;
	BYTE *MainGainTbl;
	BYTE Ch1GainStepNum;
	BYTE *Ch1_GainTbl;
	BYTE Ch2GainStepNum;
	BYTE *Ch2_GainTbl;
	BYTE Ch3GainStepNum;
	BYTE *Ch3_GainTbl;
	BYTE Ch4GainStepNum;
	BYTE *Ch4_GainTbl;
	BYTE Ch5GainStepNum;
	BYTE *Ch5_GainTbl;
	BYTE Ch6GainStepNum;
	BYTE *Ch6_GainTbl;
	BYTE Ch7GainStepNum;
	BYTE *Ch7_GainTbl;
	BYTE Ch8GainStepNum;
	BYTE *Ch8_GainTbl;
	BYTE AnalogAmpGainStepNum;
	BYTE *AnalogAmp_GainTbl; // to do: it is provided by DSP team
} CFG_AMP_T;

typedef struct _AmpModuleConf
{
	eAmpIdx     eAmpID;				//功放的ID号(编号)
	char        *pbI2cModule;		//功放的I2C模块名
	UINT8 		SlaveAddr;			//从机地址
	int         sdI2cHandle;		//功放的I2C句柄
} AmpModuleConf;

extern CFG_AMP_T stAmpCfg;
extern AmpModuleConf stAmp[];

#endif /* UserApp_h */
