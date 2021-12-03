
#include "VoiceAssistService.h"

void VoiceAssistService_VAStatus(QActive    * pSender, uint8_t auxva_status, uint8_t btva_status)
{
	VAStatusEvt *e = Q_NEW(VAStatusEvt, VA_STATUS_SIG);
	(void)pSender;
	e->auxva_status = auxva_status;
	e->btva_status = btva_status;
	QACTIVE_POST((QActive *)VoiceAssist_get(), (QEvt *)e, (void *)pSender);
}

void VoiceAssistService_APSource(QActive    * pSender, uint8_t src)
{
	VAStatusEvt *e = Q_NEW(VAStatusEvt, VA_SOURCE_SIG);
	(void)pSender;
	e->src = src;
	QACTIVE_POST((QActive *)VoiceAssist_get(), (QEvt *)e, (void *)pSender);
}

void VoiceAssistService_VAStatusReset(QActive    * pSender, uint8_t auxva_status, uint8_t btva_status)
{
	(void)pSender;
	if(auxva_status || btva_status)
	{
		VAStatusEvt *e = Q_NEW(VAStatusEvt, VA_STATUS_RESET_SIG);
		e->auxva_status = auxva_status;
		e->btva_status = btva_status;
		QACTIVE_POST((QActive *)VoiceAssist_get(), (QEvt *)e, (void *)pSender);
	}
}

void VoiceAssistService_SetAuxMute(QActive    * pSender)
{
	(void)pSender;
	QACTIVE_POST((QActive *)VoiceAssist_get(), Q_NEW(QEvt, VA_SET_AUX_MUTE_SIG), (void *)pSender);
}

void VoiceAssistService_SetAuxResume(QActive    * pSender)
{
	(void)pSender;
	QACTIVE_POST((QActive *)VoiceAssist_get(), Q_NEW(QEvt, VA_SET_AUX_RESUME_SIG), (void *)pSender);
}

void VoiceAssistService_SetBTMute(QActive    * pSender)
{
	(void)pSender;
	QACTIVE_POST((QActive *)VoiceAssist_get(), Q_NEW(QEvt, VA_SET_BT_MUTE_SIG), (void *)pSender);
}

void VoiceAssistService_SetBTResume(QActive    * pSender)
{
	(void)pSender;
	QACTIVE_POST((QActive *)VoiceAssist_get(), Q_NEW(QEvt, VA_SET_BT_RESUME_SIG), (void *)pSender);
}

