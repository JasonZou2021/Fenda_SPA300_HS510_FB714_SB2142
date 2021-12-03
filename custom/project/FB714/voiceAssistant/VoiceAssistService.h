#ifndef VoiceAssist_Service_h
#define VoiceAssist_Service_h

#include "qpc.h"
#include "qp_pub.h"
#include "VoiceAssist_Srv.h"

#define ENERGY_AUX_THD      0x4000
#define ENERGY_BT_THD       0x4000
#define TIME_10S            100
#define TIME_5S             50
#define TIME_4S             40
#define TIME_3S             30
#define TIME_2S             20
#define TIME_1S             10
#define TIME_05S            5

#define ActivDtectedTime   TIME_05S
#define CounterMaxTime      TIME_10S

#define AUX_AttackTime   TIME_10S
#define AUX_ReleaseTime  TIME_3S
#define BT_AttackTime   TIME_10S
#define BT_ReleaseTime  TIME_3S
#define BGM_AttackTime  TIME_1S
#define BGM_ReleaseTime  TIME_1S


/* struct */
typedef struct
{
	QEvt stEvt;
} VAEvt;

typedef struct
{
	VAEvt stType;
	uint8_t va_status;
	uint8_t btva_status;
	uint8_t auxva_status;
	uint8_t src;
} VAStatusEvt;

typedef struct
{
	UINT16 wEnergyCounter;
	UINT16 wActivCounter;
	UINT16 wInActivCounter;
	UINT8 bState;
} OAssistant;

typedef struct
{
	OAssistant Aux;
	OAssistant Bt;
} OVoiceAssistant;

/* enum */
typedef enum
{
	VA_AUX = 0,
	VA_BT,
}VoiceAssist_ID_t;

typedef enum
{
	AUXVA_OFF = 0,
	AUXVA_ON,
}AUXVA_State_t;

typedef enum
{
	BTVA_OFF = 0,
	BTVA_ON,
}BTVA_State_t;

#endif
