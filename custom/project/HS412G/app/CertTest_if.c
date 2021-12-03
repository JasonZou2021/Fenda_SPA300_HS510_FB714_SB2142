/******************************************************************************/
/**
*
* \file	 CertTest_if.c
*
* \brief	This file provides the vfd middleware state machine framework \n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*\author       email@sunplus.com
*\version	 v0.01
*\date 	 2016/08/8
******************************************************************************/
#include <string.h>
#include "qpc.h"
#include "userapp.h"
#include "custom_sig.h"
#include "CertTest_if.h"
#include "CertTest_vfd_str_tbl.inc"
#include "Display_Srv.h"
#include "Usersetting.h"
#define LOG_MODULE_ID ID_APUSR
#include "log_utils.h"

#define ap_atmos_cert_printf(f, a ...)  LOGD("[ATMOSCERT]"f, ## a)
#define ap_ddp_cert_printf(f, a ...)    LOGD("[DDPCERT]"f, ## a)
#define ap_dts_cert_printf(f, a ...)    LOGD("[DTSCERT]"f, ## a)
#define ap_dtsx_cert_printf(f, a ...)   LOGD("[DTSXCERT]"f, ## a)

Q_DEFINE_THIS_FILE


QState CertTest_IF_initial(stCertTest_IF_t *const me, QEvt const *const e);
QState CertTest_IF_Active(stCertTest_IF_t *const me, QEvt const *const e);
QState CertTest_IF_Active_ATMOS(stCertTest_IF_t *const me, QEvt const *const e);
QState CertTest_IF_Active_DTSX(stCertTest_IF_t *const me, QEvt const *const e);
QState CertTest_IF_Active_AMP(stCertTest_IF_t *const me, QEvt const *const e);

QState CertTest_IF_Active_DDP(stCertTest_IF_t *const me, QEvt const *const e);
QState CertTest_IF_Active_DTS(stCertTest_IF_t *const me, QEvt const *const e);
QState CertTest_IF_Active_DTSVirtualX(stCertTest_IF_t *const me, QEvt const *const e);




extern void vTaskDelay();
extern void AUD_Set_Channelcopy();
extern void AUDIF_Set_DSP_AdcInSampleRate(UINT16 SF_ID);



void CertTest_IF_ctor(stCertTest_IF_t *const me)
{

	LOGD("CertTest_IF_ctor ENTRY; \n");

	QHsm_ctor(&me->super, Q_STATE_CAST(&CertTest_IF_initial));
}




QState CertTest_IF_initial(stCertTest_IF_t *const me, QEvt const *const e)
{
	(void)e; /* avoid compiler warning about unused parameter */


	me->atmos_opt.dap_enable = 0;
	me->atmos_opt.direct = 0;
	me->atmos_opt.tuning_mode = 0;
	me->atmos_opt.config_file = 0;
	me->atmos_opt.config_virt_enable = 0;
	me->atmos_opt.virt_mode = 0;
	me->atmos_opt.virt_frontangle = 0;
	me->atmos_opt.virt_surrangle = 0;
	me->atmos_opt.virt_heightangle = 0;
	me->atmos_opt.virt_rearsurrangle = 0;

	me->atmos_opt.b_postproc= 1;
	me->atmos_opt.profile_id = 0;
	me->atmos_opt.b_vlamp= 1;
	me->atmos_opt.b_enable_upmixer= 1;
	me->atmos_opt.b_center_spread= 0;
	me->atmos_opt.b_height_filter= 0;
	me->atmos_opt.b_enable_loudness_mgmt= 1;
	me->atmos_opt.drc_mode = 0;
	me->atmos_opt.drc_cut = 100;
	me->atmos_opt.drc_boost = 100;

	//ddp
	//default line mode on
	me->ddp_opt.drc_mode = 1;
	me->ddp_opt.lfe_mode = 1;
	//default auto
	me->ddp_opt.stereo_dnmix_mode = 0;
	me->ddp_opt.output_mode = 7;

	//dts
	//fixme,todo
	me->dts_opt.output_mode = 0;
	me->dts_opt.stereo_dnmix_mode = 0;

	//default normal listening test
	me->dspmode = 0;

	//dtsx
	me->dtsx_opt.speaker_config= 47;		// 5.1.2
	me->dtsx_opt.Nerualx = 0;				// 0: disable, 1: enable
	me->dtsx_opt.analog_compensation = 0;	// 0: disable, 1: enable
	me->dtsx_opt.Decode_96_from_192 = 0;	// 0: disable, 1: enable

	me->dtsx_opt.DRC = 0;					// [0:100] %
	me->dtsx_opt.direct_mode = 1;			// 0: disable (enable upmix), 1: enable (disable upmix)
	me->dtsx_opt.dialog_control = 0;		// [0:6] dB

	return Q_TRAN(&CertTest_IF_Active);
}

QState CertTest_IF_Active(stCertTest_IF_t *const me, QEvt const *const e)
{
	QState status;

#if 0
	UINT16 isrc = 0;
	UserSetting_ReadSrc(&isrc);
    LOGD("CertTest_IF_Active isrc = %d; \n",isrc);
    if((isrc != eAp_ARC_Src)&&(isrc != eAp_HDMI0_Src)&&(isrc != eAp_HDMI1_Src)&&(isrc != eAp_HDMI2_Src))
    {
        //status = Q_SUPER(&QHsm_top);
        return status;
    }
#endif
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGD("CertTest_IF_Active ENTRY; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGD("CertTest_IF_Active EXIT; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			LOGD("CertTest_IF_Active INIT; \n");
			status = Q_HANDLED();
			break;
		}
		case SIX_RELEASE_SIG:
		{
			LOGD("CertTest_IF_Active SIX_RELEASE_SIG; \n");
			status = Q_HANDLED();
			break;
		}
		case FOUR_LONG_RELEASE_SIG:
		{

			LOGD("CertTest_IF_Active FOUR_LONG_RELEASE_SIG; \n");
            status = Q_TRAN(&CertTest_IF_Active_DDP);
			break;

		}
		case FIVE_LONG_RELEASE_SIG:
		{

			LOGD("CertTest_IF_Active FIVE_LONG_RELEASE_SIG; \n");
            status = Q_TRAN(&CertTest_IF_Active_DTS);
			break;

		}
		case SIX_LONG_RELEASE_SIG:
		{

			LOGD("CertTest_IF_Active SIX_LONG_RELEASE_SIG; \n");
            status = Q_TRAN(&CertTest_IF_Active_DTSVirtualX);
			break;

		}
		case SEVEN_LONG_RELEASE_SIG:
		{

			LOGD("CertTest_IF_Active SEVEN_LONG_RELEASE_SIG; \n");
            status = Q_TRAN(&CertTest_IF_Active_ATMOS);

			break;

		}
		case EIGHT_LONG_RELEASE_SIG:
		{

			LOGD("CertTest_IF_Active EIGHT_LONG_RELEASE_SIG; \n");
            status = Q_TRAN(&CertTest_IF_Active_DTSX);
			break;

		}
		case NINE_LONG_RELEASE_SIG:
		{

			LOGD("CertTest_IF_Active NINE_LONG_RELEASE_SIG; \n");
            status = Q_TRAN(&CertTest_IF_Active_AMP);
			break;

		}
		case ONE_RELEASE_SIG:
		{
        	ap_ddp_cert_printf("CertTest_IF_Active ONE_RELEASE_SIG\n");
            VFD_Service_POP(&me->super, CertTestvfd_ddp_str[STR_DSP_MODE], 1, CENTER);
            me->dspmode += 1;
            if(me->dspmode > 3)
            {
                me->dspmode = 0;
            }
		#ifdef SPA100_DEC_CERT
			CFG_AUD_T *pstAUD_Cfg = GET_SDK_CFG(CFG_AUD_T);
			pstAUD_Cfg->DSP_Mode = me->dspmode;
			if (me->dspmode == 1 ||
				me->dspmode == 2)
			{
				spAudapi_ramp(0x8000, 0);
			}
			else
			{
				spAudapi_ramp(0x8000, 1);
			}
		#endif
            ap_ddp_cert_printf("dsp mode %s\n",CertTestvfd_ddp_str[STR_DSP_MODE_NORMAL + me->dspmode]);
			spAudapi_dspmode_change(me->dspmode);
            vTaskDelay(1000);
            VFD_Service_POP(&me->super, CertTestvfd_ddp_str[STR_DSP_MODE_NORMAL + me->dspmode], 3, RIGHT);
            status = Q_HANDLED();
            break;
        }
		default:
			status = Q_SUPER(&QHsm_top);
			break;

	}
	return status;
}


QState CertTest_IF_Active_ATMOS(stCertTest_IF_t *const me, QEvt const *const e)
{
	QState status;

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGD("CertTest_IF_Active_ATMOS ENTRY; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGD("CertTest_IF_Active_ATMOS EXIT; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{

			LOGD("CertTest_IF_Active_ATMOS INIT; \n");
			status = Q_HANDLED();
			break;
		}

		case SEVEN_LONG_RELEASE_SIG:
		{

			LOGD("CertTest_IF_Active_ATMOS SIX_RELEASE_SIG; \n");
            status = Q_TRAN(&CertTest_IF_Active);
			break;

		}


#if 1//def ATMOS_CERT
        case ONE_RELEASE_SIG:
        {

            VFD_Service_POP(&me->super, CertTestvfd_str[STR_ATMOS_DIRECT], 1, CENTER);
            me->atmos_opt.direct += 1;
            if(me->atmos_opt.direct > 1)
            {
                me->atmos_opt.direct = 0;
            }
            ap_atmos_cert_printf("Direct %s\n",CertTestvfd_str[STR_OFF + me->atmos_opt.direct]);
            SpHdsp_setAtmos(NULL,SP_ATMOS_DIRECT,me->atmos_opt.direct,0);
            SpHdsp_Reset(NULL,0);
            vTaskDelay(1000);
            VFD_Service_POP(&me->super, CertTestvfd_str[STR_OFF + me->atmos_opt.direct], 3, RIGHT);


            status = Q_HANDLED();
            break;
        }
        case TWO_RELEASE_SIG:
        {
            VFD_Service_POP(&me->super, CertTestvfd_str[STR_ATMOS_HFT], 1, CENTER);
            me->atmos_opt.b_height_filter += 1;
            if(me->atmos_opt.b_height_filter > 1)
            {
                me->atmos_opt.b_height_filter = 0;
            }
            ap_atmos_cert_printf("Height Cut Filter %s\n",CertTestvfd_str[STR_OFF + me->atmos_opt.b_height_filter]);
            SpHdsp_setAtmos(NULL,SP_ATMOS_HEIGHT_FILTER,me->atmos_opt.b_height_filter,0);
            vTaskDelay(1000);
            VFD_Service_POP(&me->super, CertTestvfd_str[STR_OFF + me->atmos_opt.b_height_filter], 3, RIGHT);
            status = Q_HANDLED();
            break;
        }
        case THREE_RELEASE_SIG:
        {
            VFD_Service_POP(&me->super, CertTestvfd_str[STR_ATMOS_LM], 1, CENTER);
            me->atmos_opt.b_enable_loudness_mgmt += 1;
            if(me->atmos_opt.b_enable_loudness_mgmt > 1)
            {
                me->atmos_opt.b_enable_loudness_mgmt = 0;
            }
            ap_atmos_cert_printf("Loudness managemant %s\n",CertTestvfd_str[STR_OFF + me->atmos_opt.b_enable_loudness_mgmt]);
            SpHdsp_setAtmos(NULL,SP_ATMOS_LM,me->atmos_opt.b_enable_loudness_mgmt,0);
            vTaskDelay(1000);
            VFD_Service_POP(&me->super, CertTestvfd_str[STR_OFF + me->atmos_opt.b_enable_loudness_mgmt], 3, RIGHT);

            status = Q_HANDLED();
            break;
        }
        case FOUR_RELEASE_SIG:
        {
            VFD_Service_POP(&me->super, CertTestvfd_str[STR_ATMOS_VLAMP], 1, CENTER);
            me->atmos_opt.b_vlamp += 1;
            if(me->atmos_opt.b_vlamp > 1)
            {
                me->atmos_opt.b_vlamp = 0;
            }
            ap_atmos_cert_printf("Volume Leveler %s\n",CertTestvfd_str[STR_OFF + me->atmos_opt.b_vlamp]);
            SpHdsp_setAtmos(NULL,SP_ATMOS_VLAMP,me->atmos_opt.b_vlamp,0);
            vTaskDelay(1000);
            VFD_Service_POP(&me->super, CertTestvfd_str[STR_OFF + me->atmos_opt.b_vlamp], 3, RIGHT);

            status = Q_HANDLED();
            break;
        }
        case FIVE_RELEASE_SIG:
        {
            VFD_Service_POP(&me->super, CertTestvfd_str[STR_ATMOS_UPMIX], 1, CENTER);
            me->atmos_opt.b_enable_upmixer += 1;
            if(me->atmos_opt.b_enable_upmixer > 1)
            {
                me->atmos_opt.b_enable_upmixer = 0;
            }
            ap_atmos_cert_printf("Surround Decoder %s\n",CertTestvfd_str[STR_OFF + me->atmos_opt.b_enable_upmixer]);
            SpHdsp_setAtmos(NULL,SP_ATMOS_UPMIX,me->atmos_opt.b_enable_upmixer,0);
            vTaskDelay(1000);
            VFD_Service_POP(&me->super, CertTestvfd_str[STR_OFF + me->atmos_opt.b_enable_upmixer], 3, RIGHT);

            status = Q_HANDLED();
            break;
        }
        case SIX_RELEASE_SIG:
        {
            VFD_Service_POP(&me->super, CertTestvfd_str[STR_ATMOS_CNTSPD], 1, CENTER);
            me->atmos_opt.b_center_spread += 1;
            if(me->atmos_opt.b_center_spread > 1)
            {
                me->atmos_opt.b_center_spread = 0;
            }
            ap_atmos_cert_printf("Center Spread %s\n",CertTestvfd_str[STR_OFF + me->atmos_opt.b_center_spread]);
            SpHdsp_setAtmos(NULL,SP_ATMOS_CSPREAD,me->atmos_opt.b_center_spread,0);
            vTaskDelay(1000);
            VFD_Service_POP(&me->super, CertTestvfd_str[STR_OFF + me->atmos_opt.b_center_spread], 3, RIGHT);


            status = Q_HANDLED();
            break;
        }
        case SEVEN_RELEASE_SIG:
        {
            VFD_Service_POP(&me->super, CertTestvfd_str[STR_ATMOS_POST], 1, CENTER);
            me->atmos_opt.b_postproc += 1;
            if(me->atmos_opt.b_postproc > 1)
            {
                me->atmos_opt.b_postproc = 0;
            }
            ap_atmos_cert_printf("Post Processing %s\n",CertTestvfd_str[STR_OFF + me->atmos_opt.b_postproc]);
            SpHdsp_setAtmos(NULL,SP_ATMOS_POST,me->atmos_opt.b_postproc,0);
            vTaskDelay(1000);
            VFD_Service_POP(&me->super, CertTestvfd_str[STR_OFF + me->atmos_opt.b_postproc], 3, RIGHT);

            status = Q_HANDLED();
            break;
        }
        case EIGHT_RELEASE_SIG:
        {
            VFD_Service_POP(&me->super, CertTestvfd_str[STR_ATMOS_VIRT], 1, CENTER);
            me->atmos_opt.virt_mode += 1;
            if(me->atmos_opt.virt_mode > 1)
            {
                me->atmos_opt.virt_mode = 0;
            }
            ap_atmos_cert_printf("Virtualization %s\n",CertTestvfd_str[STR_OFF + me->atmos_opt.virt_mode]);
            SpHdsp_setAtmos(NULL,SP_ATMOS_VIRT,0,me->atmos_opt.virt_mode);
            SpHdsp_Reset(NULL,0);
            vTaskDelay(1000);
            VFD_Service_POP(&me->super, CertTestvfd_str[STR_OFF + me->atmos_opt.virt_mode], 3, RIGHT);

            status = Q_HANDLED();
            break;
        }
        case NINE_RELEASE_SIG:
        {
            VFD_Service_POP(&me->super, CertTestvfd_str[STR_ATMOS_XML], 1, CENTER);
            me->atmos_opt.config_file += 1;
            if(me->atmos_opt.config_file > 1)
            {
                me->atmos_opt.config_file = 0;
            }
            ap_atmos_cert_printf("XML %s\n",CertTestvfd_str[STR_OFF + me->atmos_opt.config_file]);
            SpHdsp_setAtmos(NULL,SP_ATMOS_CONFIG,0,me->atmos_opt.config_file);
            SpHdsp_Reset(NULL,0);
            vTaskDelay(1000);
            VFD_Service_POP(&me->super, CertTestvfd_str[STR_OFF + me->atmos_opt.config_file], 3, RIGHT);

            status = Q_HANDLED();
            break;
        }
        case ZERO_RELEASE_SIG:
        {
            VFD_Service_POP(&me->super, CertTestvfd_str[STR_ATMOS_DRC], 1, CENTER);
            me->atmos_opt.drc_mode += 1;
            if(me->atmos_opt.drc_mode > 2)
            {
                me->atmos_opt.drc_mode = 0;
            }
            ap_atmos_cert_printf("DRC %s\n",CertTestvfd_str[STR_ATMOS_DRC_OFF + me->atmos_opt.drc_mode]);
            SpHdsp_setAtmos(NULL,SP_ATMOS_DRC,0,me->atmos_opt.drc_mode);
            vTaskDelay(1000);
            VFD_Service_POP(&me->super, CertTestvfd_str[STR_ATMOS_DRC_OFF + me->atmos_opt.drc_mode], 3, RIGHT);

            status = Q_HANDLED();
            break;
        }
        case KOK_ECHO_DN_RELEASE_SIG:
        case KOK_ECHO_DN_TICK_SIG:
        {
            char str[14];
            me->atmos_opt.drc_cut += 5;
            if(me->atmos_opt.drc_cut > 100)
            {
                me->atmos_opt.drc_cut = 0;
            }
            ap_atmos_cert_printf("DRC Cut %d\n",me->atmos_opt.drc_cut);
            SpHdsp_setAtmos(NULL,SP_ATMOS_DRC,1,me->atmos_opt.drc_cut);
            UserAppNum2Str(str,me->atmos_opt.drc_cut);
            VFD_Service_POP(&me->super,str, 3, RIGHT);

            status = Q_HANDLED();
            break;
        }
        case KOK_KEY_SHIFT_DN_RELEASE_SIG:
        case KOK_KEY_SHIFT_DN_TICK_SIG:
        {
            char str[14];
            me->atmos_opt.drc_boost += 5;
            if(me->atmos_opt.drc_boost > 100)
            {
                me->atmos_opt.drc_boost = 0;
            }
            ap_atmos_cert_printf("DRC Boost %d\n",me->atmos_opt.drc_boost);
            SpHdsp_setAtmos(NULL,SP_ATMOS_DRC,2,me->atmos_opt.drc_boost);
            UserAppNum2Str(str,me->atmos_opt.drc_boost);
            VFD_Service_POP(&me->super,str, 3, RIGHT);

            status = Q_HANDLED();
            break;
        }
        case PRE_RELEASE_SIG:
        {
            char str[14];
            me->atmos_opt.virt_frontangle += 1;
            if(me->atmos_opt.virt_frontangle > 30)
            {
                me->atmos_opt.virt_frontangle = 0;
            }
            ap_atmos_cert_printf("Virtualizer Front Angle %d\n",me->atmos_opt.virt_frontangle);
            SpHdsp_setAtmos(NULL,SP_ATMOS_VIRT,1,me->atmos_opt.virt_frontangle);
            SpHdsp_Reset(NULL,0);
            UserAppNum2Str(str,me->atmos_opt.virt_frontangle);
            VFD_Service_POP(&me->super,str, 3, RIGHT);

            status = Q_HANDLED();
            break;
        }
        case NEXT_RELEASE_SIG:

        {
            char str[14];
            me->atmos_opt.virt_surrangle += 1;
            if(me->atmos_opt.virt_surrangle >30)
            {
                me->atmos_opt.virt_surrangle = 0;
            }
            ap_atmos_cert_printf("Virtualizer Surround Angle %d\n",me->atmos_opt.virt_surrangle);
            SpHdsp_setAtmos(NULL,SP_ATMOS_VIRT,2,me->atmos_opt.virt_surrangle);
            SpHdsp_Reset(NULL,0);
            UserAppNum2Str(str,me->atmos_opt.virt_surrangle);
            VFD_Service_POP(&me->super,str, 3, RIGHT);

            status = Q_HANDLED();
            break;
        }
        case FB_RELEASE_SIG:
        {
            char str[14];
            me->atmos_opt.virt_heightangle += 1;
            if(me->atmos_opt.virt_heightangle >30)
            {
                me->atmos_opt.virt_heightangle = 0;
            }
            ap_atmos_cert_printf("Virtualizer Height Angle %d\n",me->atmos_opt.virt_heightangle);
            SpHdsp_setAtmos(NULL,SP_ATMOS_VIRT,3,me->atmos_opt.virt_heightangle);
            SpHdsp_Reset(NULL,0);
            UserAppNum2Str(str,me->atmos_opt.virt_heightangle);
            VFD_Service_POP(&me->super,str, 3, RIGHT);

            status = Q_HANDLED();
            break;
        }
        case FF_RELEASE_SIG:
        {
            char str[14];
            me->atmos_opt.virt_rearsurrangle += 1;
            if(me->atmos_opt.virt_rearsurrangle >30)
            {
                me->atmos_opt.virt_rearsurrangle = 0;
            }
            ap_atmos_cert_printf("Virtualizer Rear Sourround Angle %d\n",me->atmos_opt.virt_rearsurrangle);
            SpHdsp_setAtmos(NULL,SP_ATMOS_VIRT,4,me->atmos_opt.virt_rearsurrangle);
            SpHdsp_Reset(NULL,0);
            UserAppNum2Str(str,me->atmos_opt.virt_rearsurrangle);
            VFD_Service_POP(&me->super,str, 3, RIGHT);

            status = Q_HANDLED();
            break;
        }
        case STOP_RELEASE_SIG:
        {
            int profile_max = 3;
            me->atmos_opt.profile_id += 1;
            if(me->atmos_opt.config_file)
            {
                profile_max = 11;
            }

            if(me->atmos_opt.profile_id > profile_max)
            {
                me->atmos_opt.profile_id = 0;
            }

            ap_atmos_cert_printf("Profile %s\n",CertTestvfd_str[STR_ATMOS_PROFILE_OFF + me->atmos_opt.profile_id]);
            SpHdsp_setAtmos(NULL,SP_ATMOS_PROFILE,me->atmos_opt.profile_id,0);
            VFD_Service_POP(&me->super, CertTestvfd_str[STR_ATMOS_PROFILE_OFF + me->atmos_opt.profile_id], 3, LEFT);

            status = Q_HANDLED();
            break;
        }
#endif
		default:
			status = Q_SUPER(&QHsm_top);
			break;

	}
	return status;
}

QState CertTest_IF_Active_DTSX(stCertTest_IF_t *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGD("CertTest_IF_Active_DTSX ENTRY; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGD("CertTest_IF_Active_DTSX EXIT; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			LOGD("CertTest_IF_Active_DTSX INIT; \n");
			status = Q_HANDLED();
			break;
		}
		case EIGHT_LONG_RELEASE_SIG:
		{

			LOGD("CertTest_IF_Active_DTSX EIGHT_LONG_RELEASE_SIG; \n");
            status = Q_TRAN(&CertTest_IF_Active);
			break;

		}

		case ONE_RELEASE_SIG:
		{
			VFD_Service_POP(&me->super, CertTestvfd_dtsx_str[STR_DTSX_NEURALX], 1, CENTER);
			me->dtsx_opt.Nerualx += 1;
			if(me->dtsx_opt.Nerualx > 1)
			{
				me->dtsx_opt.Nerualx = 0;
			}
			ap_dtsx_cert_printf("Neural:X %s\n",CertTestvfd_dtsx_str[STR_DTSX_OFF + me->dtsx_opt.Nerualx]);
			SpHdsp_setDtsX(NULL,SP_DTSX_NERULAX,me->dtsx_opt.Nerualx);
			SpHdsp_Reset(NULL,0);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, CertTestvfd_dtsx_str[STR_DTSX_OFF + me->dtsx_opt.Nerualx], 3, RIGHT);

			status = Q_HANDLED();
			break;
		}

		case TWO_RELEASE_SIG:
		{
			VFD_Service_POP(&me->super, CertTestvfd_dtsx_str[STR_DTSX_ANALOGCOMP], 1, CENTER);
			me->dtsx_opt.analog_compensation += 1;
			if(me->dtsx_opt.analog_compensation > 1)
			{
				me->dtsx_opt.analog_compensation = 0;
			}
			ap_dtsx_cert_printf("Analog Compensation %s\n",CertTestvfd_dtsx_str[STR_DTSX_OFF + me->dtsx_opt.analog_compensation]);
			SpHdsp_setDtsX(NULL,SP_DTSX_ANALOGCOMP,me->dtsx_opt.analog_compensation);
			SpHdsp_Reset(NULL,0);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, CertTestvfd_dtsx_str[STR_DTSX_OFF + me->dtsx_opt.analog_compensation], 3, RIGHT);

			status = Q_HANDLED();
			break;
		}

		case THREE_RELEASE_SIG:
		{
			VFD_Service_POP(&me->super, CertTestvfd_dtsx_str[STR_DTSX_DECODE96FROM192], 1, CENTER);
			me->dtsx_opt.Decode_96_from_192+= 1;
			if(me->dtsx_opt.Decode_96_from_192 > 1)
			{
				me->dtsx_opt.Decode_96_from_192 = 0;
			}
			ap_dtsx_cert_printf("Decode 96 from 192 %s\n",CertTestvfd_dtsx_str[STR_DTSX_OFF + me->dtsx_opt.Decode_96_from_192]);
			SpHdsp_setDtsX(NULL,SP_DTSX_DECODE96FROM192,me->dtsx_opt.Decode_96_from_192);
			SpHdsp_Reset(NULL,0);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, CertTestvfd_dtsx_str[STR_DTSX_OFF + me->dtsx_opt.Decode_96_from_192], 3, RIGHT);

			status = Q_HANDLED();
			break;
		}

		case FOUR_RELEASE_SIG:
		{
			char str[14];
			VFD_Service_POP(&me->super, CertTestvfd_dtsx_str[STR_DTSX_DRC], 1, CENTER);
			me->dtsx_opt.DRC+= 66;
			if(((me->dtsx_opt.DRC) > 100) && ((me->dtsx_opt.DRC) <= 132))
			{
				me->dtsx_opt.DRC = 100;
			}
			else if(((me->dtsx_opt.DRC) > 0) && ((me->dtsx_opt.DRC) <= 100))
			{
				me->dtsx_opt.DRC = 66;
			}
			else
			{
				me->dtsx_opt.DRC = 0;
			}
			ap_dtsx_cert_printf("DRC %d \n",me->dtsx_opt.DRC);
			SpHdsp_setDtsX(NULL,SP_DTSX_DRC,me->dtsx_opt.DRC);
			vTaskDelay(1000);
			UserAppNum2Str(str,me->dtsx_opt.DRC);
            VFD_Service_POP(&me->super,str, 3, RIGHT);

			status = Q_HANDLED();
			break;
		}

		case FIVE_RELEASE_SIG:
        {
			VFD_Service_POP(&me->super, CertTestvfd_dtsx_str[STR_DTSX_DIRECT_DECODE], 1, CENTER);
			me->dtsx_opt.direct_mode += 1;
			if(me->dtsx_opt.direct_mode > 1)
			{
				me->dtsx_opt.direct_mode = 0;
			}
			ap_dtsx_cert_printf("Direct Mode %s\n",CertTestvfd_dtsx_str[STR_DTSX_OFF + me->dtsx_opt.direct_mode]);
			SpHdsp_setDtsX(NULL,SP_DTSX_DIRECT_DECODE,me->dtsx_opt.direct_mode);
			vTaskDelay(1000);
			VFD_Service_POP(&me->super, CertTestvfd_dtsx_str[STR_DTSX_OFF + me->dtsx_opt.direct_mode], 3, RIGHT);

			status = Q_HANDLED();
			break;
		}

		case SIX_RELEASE_SIG:
		{
			char str[14];
			VFD_Service_POP(&me->super, CertTestvfd_dtsx_str[STR_DTSX_DIALOG_CONTROL], 1, CENTER);
			me->dtsx_opt.dialog_control+= 3;
			if(me->dtsx_opt.dialog_control > 6)
			{
				me->dtsx_opt.dialog_control = 0;
			}
			ap_dtsx_cert_printf("Dialog Control %d db\n",me->dtsx_opt.dialog_control);
			SpHdsp_setDtsX(NULL,SP_DTSX_DIALOG_CONTROL,me->dtsx_opt.dialog_control);
			vTaskDelay(1000);
			UserAppNum2Str(str,me->dtsx_opt.dialog_control);
            VFD_Service_POP(&me->super,str, 3, RIGHT);

			status = Q_HANDLED();
			break;
		}
		case SEVEN_RELEASE_SIG:
		{
			me->dtsx_opt.speaker_config+= 1;
			uint8_t value =0;
			char str[14];
			VFD_Service_POP(&me->super, CertTestvfd_dtsx_str[STR_DTSX_SPEAKER_OUT], 1, CENTER);

			if (me->dtsx_opt.speaker_config>20)
			{
				me->dtsx_opt.speaker_config=1;
			}
			switch (me->dtsx_opt.speaker_config)
			{
				//2.0
				case 1:
					value = 1;
					UserAppNum2Str(str,200);
					ap_dtsx_cert_printf("speaker output 2.0\n");
					break;
				//2.0.2
				case 2:
					value = 33;
					UserAppNum2Str(str,202);
					ap_dtsx_cert_printf("speaker output 2.0.2\n");
					break;
				//2.1
				case 3:
					value = 5;
					UserAppNum2Str(str,210);
					ap_dtsx_cert_printf("speaker output 2.1\n");
					break;

				//2.1.2
				case 4:
					value = 37;
					UserAppNum2Str(str,212);
					ap_dtsx_cert_printf("speaker output 2.1.2\n");
					break;
				//3.0
				case 5:
					value = 9;
					UserAppNum2Str(str,300);
					ap_dtsx_cert_printf("speaker output 3.0\n");
					break;
				//3.0.2
				case 6:
					value = 41;
					UserAppNum2Str(str,302);
					ap_dtsx_cert_printf("speaker output 3.0.2\n");
					break;
				//3.1
				case 7:
					value = 13;
					UserAppNum2Str(str,310);
					ap_dtsx_cert_printf("speaker output 3.1\n");
					break;
				//3.1.2
				case 8:
					value = 45;
					UserAppNum2Str(str,312);
					ap_dtsx_cert_printf("speaker output 3.1.2\n");
					break;
				//4.0
				case 9:
					value = 3;
					UserAppNum2Str(str,400);
					ap_dtsx_cert_printf("speaker output 4.0\n");
					break;
				//4.1
				case 10:
					value = 7;
					UserAppNum2Str(str,410);
					ap_dtsx_cert_printf("speaker output 4.1\n");
					break;
				//5.0.2
				case 11:
					value = 43;
					UserAppNum2Str(str,502);
					ap_dtsx_cert_printf("speaker output 5.0.2\n");
					break;
				//5.0.4
				case 12:
					value = 107;
					UserAppNum2Str(str,504);
					ap_dtsx_cert_printf("speaker output 5.0.4\n");
					break;
				//5.1
				case 13:
					value = 15;
					UserAppNum2Str(str,510);
					ap_dtsx_cert_printf("speaker output 5.1\n");
					break;
				//5.1.2
				case 14:
					value = 47;
					UserAppNum2Str(str,512);
					ap_dtsx_cert_printf("speaker output 5.1.2\n");
					break;
				//5.1.4
				case 15:
					value = 111;
					UserAppNum2Str(str,514);
					ap_dtsx_cert_printf("speaker output 5.1.4\n");
					break;
				//7.0.2
				case 16:
					value = 59;
					UserAppNum2Str(str,702);
					ap_dtsx_cert_printf("speaker output 7.0.2\n");
					break;
				//7.0.4
				case 17:
					value = 123;
					UserAppNum2Str(str,704);
					ap_dtsx_cert_printf("speaker output 7.0.4\n");
					break;
				//7.1
				case 18:
					value = 31;
					UserAppNum2Str(str,710);
					ap_dtsx_cert_printf("speaker output 7.1\n");
					break;
				//7.1.2
				case 19:
					value = 63;
					UserAppNum2Str(str,712);
					ap_dtsx_cert_printf("speaker output 7.1.2\n");
					break;
				//7.1.4
				case 20:
					value = 127;
					UserAppNum2Str(str,714);
					ap_dtsx_cert_printf("speaker output 7.1.4\n");
					break;
			}
			SpHdsp_Set_DownmixConfig(NULL,value);
			SpHdsp_Reset(NULL,0);
			vTaskDelay(300);
			VFD_Service_POP(&me->super,str, 3, RIGHT);


			status = Q_HANDLED();
			break;
		}
		case EIGHT_RELEASE_SIG:
		{
			char str[14];
			VFD_Service_POP(&me->super, CertTestvfd_dtsx_str[STR_DTSX_DISABLE_T1], 1, CENTER);
			me->dtsx_opt.disableT1relabel+=1;
			if(me->dtsx_opt.disableT1relabel>1)
			{
				me->dtsx_opt.disableT1relabel=0;
			}
			ap_dtsx_cert_printf("disable T1 relabel %d \n",me->dtsx_opt.disableT1relabel);
			SpHdsp_setDtsX(NULL,SP_DTSX_DISABLET1RELABEL,me->dtsx_opt.disableT1relabel);
			vTaskDelay(1000);
			UserAppNum2Str(str,me->dtsx_opt.disableT1relabel);
			VFD_Service_POP(&me->super,str, 3, RIGHT);
			status = Q_HANDLED();
			break;
		}
		default:
			status = Q_SUPER(&QHsm_top);
			break;

	}
	return status;
}


QState CertTest_IF_Active_AMP(stCertTest_IF_t *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGD("CertTest_IF_Active_AMP ENTRY; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGD("CertTest_IF_Active_AMP EXIT; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			LOGD("CertTest_IF_Active_AMP INIT; \n");
			status = Q_HANDLED();
			break;
		}
		case ZERO_RELEASE_SIG:
		{
			LOGD("CertTest_IF_Active_AMP ZERO_RELEASE_SIG; \n");
			status = Q_HANDLED();
			break;
		}
		case NINE_LONG_RELEASE_SIG:
		{

			LOGD("CertTest_IF_Active_AMP NINE_LONG_RELEASE_SIG; \n");
            status = Q_TRAN(&CertTest_IF_Active);
			break;

		}

		case ONE_RELEASE_SIG:
        {

			// grm setting copy LR to other channels
			LOGD("Copy LR to other channels \n");
			AUD_Set_Channelcopy();
            status = Q_HANDLED();
            break;
        }
		case TWO_RELEASE_SIG:
        {

			//freq_hopping function enable
			LOGD("AdcIn Bypass 32K\n");
			AUDIF_Set_DSP_AdcInSampleRate(0x0020);

            status = Q_HANDLED();
            break;
        }
		case THREE_RELEASE_SIG:
        {
			LOGD("AdcIn Bypass 44.1K\n");
			AUDIF_Set_DSP_AdcInSampleRate(0x0000);
            status = Q_HANDLED();
            break;
        }

		case FOUR_RELEASE_SIG:
        {
			LOGD("AdcIn Bypass 48K\n");
			AUDIF_Set_DSP_AdcInSampleRate(0x0010);
            status = Q_HANDLED();
            break;
        }

		case FIVE_RELEASE_SIG:
        {
			LOGD("AdcIn Bypass 88.2K\n");
			AUDIF_Set_DSP_AdcInSampleRate(0x0001);
            status = Q_HANDLED();
            break;
        }

		case SIX_RELEASE_SIG:
        {
			LOGD("AdcIn Bypass 96K\n");
			AUDIF_Set_DSP_AdcInSampleRate(0x0011);
            status = Q_HANDLED();
            break;
        }

		case SEVEN_RELEASE_SIG:
        {
			LOGD("AdcIn Bypass 176.4K\n");
			AUDIF_Set_DSP_AdcInSampleRate(0x0003);
            status = Q_HANDLED();
            break;
        }

		case EIGHT_RELEASE_SIG:
        {
			LOGD("AdcIn Bypass 192K\n");
			AUDIF_Set_DSP_AdcInSampleRate(0x0013);
            status = Q_HANDLED();
            break;
        }


		default:
			status = Q_SUPER(&QHsm_top);
			break;

	}
	return status;
}

QState CertTest_IF_Active_DDP(stCertTest_IF_t *const me, QEvt const *const e)
{
	QState status;

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			ap_ddp_cert_printf("CertTest_IF_Active_DDP ENTRY; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			ap_ddp_cert_printf("CertTest_IF_Active_DDP EXIT; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{

			ap_ddp_cert_printf("CertTest_IF_Active_DDP INIT; \n");
			status = Q_HANDLED();
			break;
		}
		case FOUR_LONG_RELEASE_SIG:
		{

			ap_ddp_cert_printf("CertTest_IF_Active_DDP FOUR_LONG_RELEASE_SIG; \n");
            status = Q_TRAN(&CertTest_IF_Active);
			break;

		}
		case ONE_RELEASE_SIG:
        {
        	ap_ddp_cert_printf("CertTest_IF_Active_DDP ONE_RELEASE_SIG\n");
            VFD_Service_POP(&me->super, CertTestvfd_ddp_str[STR_DSP_MODE], 1, CENTER);
            me->dspmode += 1;
            if(me->dspmode > 3)
            {
                me->dspmode = 0;
            }
		#ifdef SPA100_DEC_CERT
			CFG_AUD_T *pstAUD_Cfg = GET_SDK_CFG(CFG_AUD_T);
			pstAUD_Cfg->DSP_Mode = me->dspmode;
			if (me->dspmode == 1 ||
				me->dspmode == 2)
			{
				spAudapi_ramp(0x8000, 0);
			}
			else
			{
				spAudapi_ramp(0x8000, 1);
			}

			if((me->dspmode == 2) && (me->ddp_opt.drc_mode == 0))
			{
				//RF mode need to reduce 10.5DB
				AudDspService_Set_SourceGain(NULL, -10.5);
			}
			else
			{
				AudDspService_Set_SourceGain(NULL, 0);
			}
		#endif

            ap_ddp_cert_printf("dsp mode %s\n",CertTestvfd_ddp_str[STR_DSP_MODE_NORMAL + me->dspmode]);
			spAudapi_dspmode_change(me->dspmode);
            vTaskDelay(1000);
            VFD_Service_POP(&me->super, CertTestvfd_ddp_str[STR_DSP_MODE_NORMAL + me->dspmode], 3, RIGHT);
            status = Q_HANDLED();
            break;
        }
        case TWO_RELEASE_SIG:
        {
			ap_ddp_cert_printf("CertTest_IF_Active_DDP TWO_RELEASE_SIG\n");
            VFD_Service_POP(&me->super, CertTestvfd_ddp_str[STR_DDP_DRC], 1, CENTER);
            me->ddp_opt.drc_mode += 1;
            if(me->ddp_opt.drc_mode > 2)
            {
                me->ddp_opt.drc_mode = 0;
            }
            ap_ddp_cert_printf("drc mode %s\n",CertTestvfd_ddp_str[STR_DDP_DRC_RF + me->ddp_opt.drc_mode]);
		#ifdef SPA100_DEC_CERT
			if((me->dspmode == 2) && (me->ddp_opt.drc_mode == 0))
			{
				//RF mode need to reduce 10.5DB
				AudDspService_Set_SourceGain(NULL, -10.5);
			}
			else
			{
				AudDspService_Set_SourceGain(NULL, 0);
			}
		#endif
			spAudapi_ddpdrc(me->ddp_opt.drc_mode);
            vTaskDelay(1000);
            VFD_Service_POP(&me->super, CertTestvfd_ddp_str[STR_DDP_DRC_RF + me->ddp_opt.drc_mode], 3, RIGHT);
            status = Q_HANDLED();
            break;
        }
        case THREE_RELEASE_SIG:
        {
			ap_ddp_cert_printf("CertTest_IF_Active_DDP THREE_RELEASE_SIG\n");
            VFD_Service_POP(&me->super, CertTestvfd_ddp_str[STR_DDP_LFE], 1, CENTER);
            me->ddp_opt.lfe_mode += 1;
            if(me->ddp_opt.lfe_mode > 1)
            {
                me->ddp_opt.lfe_mode = 0;
            }
            ap_ddp_cert_printf("lfe mode %s\n",CertTestvfd_str[STR_OFF + me->ddp_opt.lfe_mode]);
			spAudapi_ddplfe(me->ddp_opt.lfe_mode);
            vTaskDelay(1000);
            VFD_Service_POP(&me->super, CertTestvfd_str[STR_OFF + me->ddp_opt.lfe_mode], 3, RIGHT);

            status = Q_HANDLED();
            break;
        }
        case FOUR_RELEASE_SIG:
        {
        	ap_ddp_cert_printf("CertTest_IF_Active_DDP FOUR_RELEASE_SIG\n");
            VFD_Service_POP(&me->super, CertTestvfd_ddp_str[STR_DDP_STEREO_DOWNMIX], 1, CENTER);
            me->ddp_opt.stereo_dnmix_mode += 1;
            if(me->ddp_opt.stereo_dnmix_mode > 2)
            {
                me->ddp_opt.stereo_dnmix_mode = 0;
            }
            ap_ddp_cert_printf("stereo downmix mode %s\n",CertTestvfd_ddp_str[STR_DDP_STEREO_DOWNMIX_AUTO + me->ddp_opt.stereo_dnmix_mode]);
			spAudapi_ddpstereo(me->ddp_opt.stereo_dnmix_mode);
            vTaskDelay(1000);
            VFD_Service_POP(&me->super, CertTestvfd_ddp_str[STR_DDP_STEREO_DOWNMIX_AUTO + me->ddp_opt.stereo_dnmix_mode], 3, RIGHT);

            status = Q_HANDLED();
            break;
        }
		case FIVE_RELEASE_SIG:
        {
        	char str[14];
			ap_ddp_cert_printf("CertTest_IF_Active_DDP FIVE_RELEASE_SIG\n");
            VFD_Service_POP(&me->super, CertTestvfd_ddp_str[STR_DDP_OUTPUT], 1, CENTER);
            me->ddp_opt.output_mode += 1;
            if(me->ddp_opt.output_mode > 7)
            {
                me->ddp_opt.output_mode = 1;
            }

            ap_ddp_cert_printf("output mode %d\n",me->ddp_opt.output_mode);
			spAudapi_ddpout(me->ddp_opt.output_mode);
			UserAppNum2Str(str,me->ddp_opt.output_mode);
            vTaskDelay(1000);
            VFD_Service_POP(&me->super, str, 3, RIGHT);

            status = Q_HANDLED();
            break;
        }
		default:
		{
			ap_ddp_cert_printf("CertTest_IF_Active_DDP default\n");
			status = Q_SUPER(&QHsm_top);
			break;
		}
	}

	return status;
}

QState CertTest_IF_Active_DTS(stCertTest_IF_t *const me, QEvt const *const e)
{
	QState status;

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGD("CertTest_IF_Active_DTS ENTRY; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGD("CertTest_IF_Active_DTS EXIT; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{

			LOGD("CertTest_IF_Active_DTS INIT; \n");
			status = Q_HANDLED();
			break;
		}
		case FIVE_LONG_RELEASE_SIG:
		{

			LOGD("CertTest_IF_Active_DTS FIVE_LONG_RELEASE_SIG; \n");
			status = Q_TRAN(&CertTest_IF_Active);
			break;

		}
		default:
		{
			status = Q_SUPER(&QHsm_top);
			break;
		}

	}

	return status;
}

QState CertTest_IF_Active_DTSVirtualX(stCertTest_IF_t *const me, QEvt const *const e)
{
	QState status;

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGD("CertTest_IF_Active_DTSVirtualX ENTRY; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGD("CertTest_IF_Active_DTSVirtualX EXIT; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{

			LOGD("CertTest_IF_Active_DTSVirtualX INIT; \n");
			status = Q_HANDLED();
			break;
		}
		case SIX_LONG_RELEASE_SIG:
		{

			LOGD("CertTest_IF_Active_DTSVirtualX SIX_LONG_RELEASE_SIG; \n");
			status = Q_TRAN(&CertTest_IF_Active);
			break;

		}
		default:
		{
			status = Q_SUPER(&QHsm_top);
			break;
		}
	}

	return status;
}
