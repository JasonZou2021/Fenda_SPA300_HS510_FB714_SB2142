
#ifndef VoiceAssist_Srv_h
#define VoiceAssist_Srv_h

#define DUCK_GAIN 0x02000000
#define MUTE_GAIN 0x1
#define NORMAL_GAIN 0x08000000

#define DUCK_VOL -12	//dB
#define MUTE_VOL -140	//dB
#define NORMAL_VOL 0	//dB

typedef enum
{
	SRC_AUX = 0,
	SRC_BT,	//I2S IN source
	SRC_OTHER,
}VA_Source_t;

QActive *VoiceAssist_get(void);

void VoiceAssistService_VAStatus(QActive* pSender, uint8_t auxva_status, uint8_t btva_status);
void VoiceAssistService_APSource(QActive    * pSender, uint8_t src);
void VoiceAssistService_VAStatusReset(QActive    * pSender, uint8_t auxva_status, uint8_t btva_status);
void VoiceAssistService_SetAuxMute(QActive    * pSender);
void VoiceAssistService_SetAuxResume(QActive    * pSender);
void VoiceAssistService_SetBTMute(QActive    * pSender);
void VoiceAssistService_SetBTResume(QActive    * pSender);

#endif
