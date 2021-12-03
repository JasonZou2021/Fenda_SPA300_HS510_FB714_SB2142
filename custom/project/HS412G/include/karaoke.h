
#ifndef _Karaoke_h
#define _Karaoke_h

/*---------------------------------------------------------------------------*
 *    MACRO DECLARATIONS
 *---------------------------------------------------------------------------*/

#define KOK_BGM_VOL_MIN     (0)
#define KOK_BGM_VOL_MAX     (5)

#define KOK_MIC_VOL_MIN     (0)
#define KOK_MIC_VOL_MAX     (40)

#define KOK_KEY_SHIFT_MIN   (1)
#define KOK_KEY_SHIFT_MAX   (15)

#define KOK_ECHO_MIN        (0)
#define KOK_ECHO_MAX        (13)

#define KOK_VC_LVL_MIN      (0)
#define KOK_VC_LVL_MAX      (3)



/*............................... KOK param .......................................*/

typedef enum
{
	KOK_NR_OFF,
	KOK_NR_ON,
} KOK_NR_STATE;

//MIC INPUT (ADC 3 to 1 MUX)
enum
{
	ADC_0,
	ADC_1,
	ADC_2,
};

enum
{
	USERAPP_KOK_EN = 0,
	USERAPP_KOK_ECHO,
	USERAPP_KOK_KEY_SHIFT,
	USERAPP_KOK_MIC1_VOL,
	USERAPP_KOK_MIC2_VOL,
	USERAPP_KOK_BGM_VOL,
	USERAPP_KOK_THRESHOLD,
	USERAPP_KOK_NR,
	USERAPP_KOK_VC_LEVEL,
};


typedef struct
{
	QHsm super;
	int Kok_Bgm_Vol;
	int Kok_Mic1_Vol;
	int Kok_Mic2_Vol;
	int Kok_KeyShift;
	int Kok_Echo;
	int Kok_Threshold;
	KOK_NR_STATE Kok_Nr;
	int Kok_Vc_Level;
} Karaoke_Effect;


void Karaoke_ctor(Karaoke_Effect *const me);
void Karaoke_Usersetting_init(Karaoke_Effect *const me);


#endif
