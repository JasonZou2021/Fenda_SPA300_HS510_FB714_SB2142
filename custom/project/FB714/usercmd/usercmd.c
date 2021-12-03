/**************************************************************************/
/**
*
* \file	user_cli.c
*
* \brief	user cli command \n
* \note  	Copyright (c) 2016 Sunplus Technology Co., Ltd. \n
*		All rights reserved.
*
*
* \author   Daniel@sunplus.com
* \version	v0.01
* \date 	 2019/10/07
***************************************************************************/

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include "types.h"
#define LOG_MODULE_ID ID_CMD
#include "log_utils.h"
#include "user_cli.h"
#include "initcall.h"
#include "userapp.h"
#include "mcu_if_sp.h"
#include "exstd_interfaces.h"
#include "AudDspService.h"

/*---------------------------------------------------------------------------*
 *                            GLOBAL   VARIABLES                             *
 *---------------------------------------------------------------------------*/
static const char* const strParamError = "Parameter Error\n";

/*---------------------------------------------------------------------------*
 *                          Command func                                     *
 *---------------------------------------------------------------------------*/
static void _APP_test(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    int i;
    LOGI("paramNum = %d\n", paramNum);

    for(i = 0; i < paramNum; i++)
    {
        LOGI("Param = %s\n", pParam[i]);
    }
}

static void _APP_sig(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    // Check Parameter
    unsigned int dP0 = 0;
    if(paramNum < 1 || Convert2DecimalNumer(pParam[0], &dP0) == 0)
    {
        LOGI("%s", strParamError);
        return;
    }

    // Check signal range
    if((dP0 < CUSTOMER_PUB_START_SIG || dP0 > CUSTOMER_PRIVATE_SIG_MAX) ||
       (dP0 > CUSTOMER_PUB_SIG_MAX && dP0 < CUSTOMER_PRIVATE_SIG_START))
    {
        LOGI("Parameter is out of custom sinal range\n");
        LOGI("Custom public sinal range is between %d and %d\n", CUSTOMER_PUB_START_SIG, CUSTOMER_PUB_SIG_MAX);
        LOGI("Custom private sinal range is between %d and %d\n", CUSTOMER_PRIVATE_SIG_START, CUSTOMER_PRIVATE_SIG_MAX);
        return;
    }

    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, dP0), (void*)0);
}

static void _APP_card(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, USERAPP_CLI_SRC_CARD_SIG), (void*)0);
}

static void _APP_stop(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, STOP_RELEASE_SIG), (void*)0);
}

static void _APP_play(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, PLAY_RELEASE_SIG), (void*)0);
}

static void _APP_next(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, NEXT_RELEASE_SIG), (void*)0);
}

static void _APP_pre(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, PRE_RELEASE_SIG), (void*)0);
}

static void _APP_pause(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, PAUSE_RELEASE_SIG), (void*)0);
}

static void _APP_one(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, ONE_RELEASE_SIG), (void*)0);
}

static void _APP_six(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, SIX_RELEASE_SIG), (void*)0);
}

static void _APP_usb(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, USERAPP_CLI_SRC_USB_SIG), (void*)0);
}

static void _APP_aux(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, USERAPP_CLI_SRC_AUX_SIG), (void*)0);
}

static void _APP_line(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, USERAPP_CLI_SRC_LINE_SIG), (void*)0);
}

static void _APP_optical(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, USERAPP_CLI_SRC_OPTICAL_SIG), (void*)0);
}

static void _APP_coax(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, USERAPP_CLI_SRC_COAXIAL_SIG), (void*)0);
}

static void _APP_arc(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, USERAPP_CLI_SRC_ARC_SIG), (void*)0);
}

static void _APP_hdmi0(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, USERAPP_CLI_SRC_HDMI0_SIG), (void*)0);
}

static void _APP_hdmi1(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, USERAPP_CLI_SRC_HDMI1_SIG), (void*)0);
}

static void _APP_hdmi2(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, USERAPP_CLI_SRC_HDMI2_SIG), (void*)0);
}

static void _APP_bt(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, USERAPP_CLI_SRC_BT_SIG), (void*)0);
}

static void _APP_clearbt(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, USERAPP_CLI_BT_CLEAR_ALLBONDDEV_SIG), (void*)0);
}

static void _APP_dut(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, USERAPP_CLI_BT_ENTER_DUT_SIG), (void*)0);
}

static void _APP_i2s(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, USERAPP_CLI_SRC_I2S_SIG), (void*)0);
}

static void _APP_earc(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, USERAPP_CLI_SRC_EARC_SIG), (void*)0);
}

static void _APP_demo(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, USERAPP_CLI_SRC_RAW_DATA_SIG), (void*)0);
}

static void _APP_volup(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, VOL_UP_RELEASE_SIG), (void*)0);
}

static void _APP_voldw(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, VOL_DOWN_RELEASE_SIG), (void*)0);
}

static void _APP_shuffle(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, SHUFFLE_RELEASE_SIG), (void*)0);
}

static void _APP_repeat(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, REPEAT_RELEASE_SIG), (void*)0);
}

static void _APP_pwr(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, POWER_RELEASE_SIG), (void*)0);
}

static void _APP_ff(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, FF_RELEASE_SIG), (void*)0);
}

static void _APP_fb(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, FB_RELEASE_SIG), (void*)0);
}

static void _APP_search(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, BT_START_SEARCH_RELEASE_SIG), (void*)0);
}

static void _APP_modesw(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, BT_TWS_MODE_SWITCH_RELEASE_SIG), (void*)0);
}

static void _APP_btdisc(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, BT_SOURCE_DISCONNECT_SIG), (void*)0);
}

/*          USB command func          */
static void _APP_devlist(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, USERAPP_CLI_DEV_LIST_SIG), (void*)0);
}

static void _APP_play1(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, USERAPP_CLI_PLAY_DEV_NUM1_SIG), (void*)0);
}

static void _APP_play2(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, USERAPP_CLI_PLAY_DEV_NUM2_SIG), (void*)0);
}

static void _APP_play3(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, USERAPP_CLI_PLAY_DEV_NUM3_SIG), (void*)0);
}

static void _APP_play4(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, USERAPP_CLI_PLAY_DEV_NUM4_SIG), (void*)0);
}

static void _APP_auxva(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, AUXVA_SW_SIG), (void*)0);
}

static void _APP_btva(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, BTVA_SW_SIG), (void*)0);
}

static void _APP_vaon(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, ALL_VAON_SIG), (void*)0);
}

static void _APP_vaoff(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, ALL_VAOFF_SIG), (void*)0);
}

static void _APP_energy(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, GET_ENERGY_STATE_SIG), (void*)0);
}

static void _APP_auxgain(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    // Check Parameter
    unsigned int dP0 = 0;
    if(paramNum < 1 || Convert2DecimalNumer(pParam[0], &dP0) == 0)
    {
        LOGI("%s", strParamError);
        return;
    }

    Servicer_VA_Param_t gServicer_VA_Param_t;
    gServicer_VA_Param_t.Ch_L_bitmark_MIXER0 = 0x1;
    gServicer_VA_Param_t.Ch_R_bitmark_MIXER0 = 0x2;
    gServicer_VA_Param_t.Ch_L_bitmark_MIXER1 = 0x0;
    gServicer_VA_Param_t.Ch_R_bitmark_MIXER1 = 0x0;
    gServicer_VA_Param_t.VASource_Gain = dP0;
    gServicer_VA_Param_t.VASource_Ramp_step = 0x00020000;
    gServicer_VA_Param_t.VASource_Duck_Gain = 0x08000000;
    AudDspService_VoiceAssist_Contrl(NULL, VA_Set_VAGain, 0, dP0);
}

static void _APP_btgain(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    // Check Parameter
    unsigned int dP0 = 0;
    if(paramNum < 1 || Convert2DecimalNumer(pParam[0], &dP0) == 0)
    {
        LOGI("%s", strParamError);
        return;
    }

    Servicer_VA_Param_t gServicer_VA_Param_t;
    gServicer_VA_Param_t.Ch_L_bitmark_MIXER0 = 0x1;
    gServicer_VA_Param_t.Ch_R_bitmark_MIXER0 = 0x2;
    gServicer_VA_Param_t.Ch_L_bitmark_MIXER1 = 0x0;
    gServicer_VA_Param_t.Ch_R_bitmark_MIXER1 = 0x0;
    gServicer_VA_Param_t.VASource_Gain = dP0;
    gServicer_VA_Param_t.VASource_Ramp_step = 0x00020000;
    gServicer_VA_Param_t.VASource_Duck_Gain = 0x08000000;
    AudDspService_VoiceAssist_Contrl(NULL, VA_Set_VAGain, 1, dP0);
}

static void _APP_bgmgain(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    // Check Parameter
    unsigned int dP0 = 0;
    if(paramNum < 1 || Convert2DecimalNumer(pParam[0], &dP0) == 0)
    {
        LOGI("%s", strParamError);
        return;
    }

    AudDspService_VoiceAssist_Contrl(NULL, VA_Set_BGMGain, 0, dP0);
}

static void _APP_conn(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, HINT_CONNECT_SIG), (void*)0);
}

static void _APP_hintdisc(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, HINT_DISCONNECT_SIG), (void*)0);
}

static void _APP_speaker(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, HINT_SPEAKER_TEST_SIG), (void*)0);
}

static void _APP_fw(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, HINT_FW_VERSION_SIG), (void*)0);
}

static void _APP_info(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, HINT_INFO_SIG), (void*)0);
}

static void _APP_hstop(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, HINT_STOP_SIG), (void*)0);
}

static void _APP_btsearch(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, HINT_BT_SEARCH_SIG), (void*)0);
}

static void _APP_detect(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, HINT_DETECT_SEARCH_SIG), (void*)0);
}

static void _APP_eqmv(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, HINT_EQMOCIE_SIG), (void*)0);
}

static void _APP_eqms(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, HINT_EQMUSIC_SIG), (void*)0);
}

static void _APP_eqgm(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, HINT_EQGAME_SIG), (void*)0);
}

static void _APP_baudrate(char **pParam __UNUSED, const char paramNum __UNUSED)
{
    LOGI("CMD: set baud rate\n");

    // Check Parameter
    if(paramNum < 1)
    {
        LOGI("%s", strParamError);
        return;
    }

    // Run command
    if(!strcmp(pParam[0],"115200") || !strcmp(pParam[0],"0"))
    {
        QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, MCU_IF_USER_BAUDSET0_SIG), (void *)0);
        LOGI("Baud rate = 115200\n");
    }
    else if(!strcmp(pParam[0],"921600") || !strcmp(pParam[0],"1"))
    {
        QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, MCU_IF_USER_BAUDSET1_SIG), (void *)0);
        LOGI("Baud rate = 921600\n");
    }
    else if(!strcmp(pParam[0],"1843200") || !strcmp(pParam[0],"2"))
    {
        QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, MCU_IF_USER_BAUDSET2_SIG), (void *)0);
        LOGI("Baud rate = 1843200\n");
    }
    else if(!strcmp(pParam[0],"3686400") || !strcmp(pParam[0],"3"))
    {
        QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, MCU_IF_USER_BAUDSET3_SIG), (void *)0);
        LOGI("Baud rate = 3686400\n");
    }
    else
    {
        LOGI("%s", strParamError);
    }
}

/*---------------------------------------------------------------------------*
 *                          Command func cmd list control                    *
 *---------------------------------------------------------------------------*/
 typedef struct
{
	char cmd[16];
	void (*func_ptr)(char **pParam, const char paramNum);
	char desription[32];
} APPCMD_LIST;

static APPCMD_LIST appcmd_list[] =
{
    {"test",        _APP_test,      "Test user cmd"},
    {"sig",         _APP_sig,       "Post specific signal"},
    {"card",        _APP_card,      ""},
    {"stop",        _APP_stop,      ""},
    {"play",        _APP_play,      ""},
    {"next",        _APP_next,      ""},
    {"pre",         _APP_pre,       ""},
    {"pause",       _APP_pause,     ""},
    {"one",         _APP_one,       ""},
    {"six",         _APP_six,       ""},
    {"usb",         _APP_usb,       ""},
    {"aux",         _APP_aux,       ""},
    {"line",        _APP_line,      ""},
    {"optical",     _APP_optical,   ""},
    {"coax",        _APP_coax,      ""},
    {"arc",         _APP_arc,       ""},
    {"hdmi0",       _APP_hdmi0,     ""},
    {"hdmi1",       _APP_hdmi1,     ""},
    {"hdmi2",       _APP_hdmi2,     ""},
    {"bt",          _APP_bt,        ""},
    {"clearbt",     _APP_clearbt,   ""},
    {"dut",         _APP_dut,       ""},
    {"i2s",         _APP_i2s,       ""},
    {"earc",        _APP_earc,      ""},
    {"demo",        _APP_demo,      ""},
    {"volup",       _APP_volup,     ""},
    {"voldw",       _APP_voldw,     ""},
    {"shuffle",     _APP_shuffle,   ""},
    {"repeat",      _APP_repeat,    ""},
    {"pwr",         _APP_pwr,       ""},
    {"ff",          _APP_ff,        ""},
    {"fb",          _APP_fb,        ""},
    {"search",      _APP_search,    ""},
    {"modesw",      _APP_modesw,    ""},
    {"btdisc",      _APP_btdisc,    ""},
    {"devlist",     _APP_devlist,   ""},
    {"play1",       _APP_play1,     ""},
    {"play2",       _APP_play2,     ""},
    {"play3",       _APP_play3,     ""},
    {"play4",       _APP_play4,     ""},
    {"auxva",       _APP_auxva,     ""},
    {"btva",        _APP_btva,      ""},
    {"vaon",        _APP_vaon,      ""},
    {"vaoff",       _APP_vaoff,     ""},
    {"energy",      _APP_energy,    ""},
    {"auxgain",     _APP_auxgain,   ""},
    {"btgain",      _APP_btgain,    ""},
    {"bgmgain",     _APP_bgmgain,   ""},
    {"conn",        _APP_conn,      ""},
    {"hintdisc",    _APP_hintdisc,  ""},
    {"speaker",     _APP_speaker,   ""},
    {"fw",          _APP_fw,        ""},
    {"info",        _APP_info,      ""},
    {"hstop",       _APP_hstop,     ""},
    {"btsearch",    _APP_btsearch,  ""},
    {"detect",      _APP_detect,    ""},
    {"eqmv",        _APP_eqmv,      ""},
    {"eqms",        _APP_eqms,      ""},
    {"eqgm",        _APP_eqgm,      ""},
    {"baud",        _APP_baudrate,  ""},
};

/******************************************************************************************/
/**
 * \fn		int UserApp_CliCmd_cb(const char *const pCmd, const char *const *const pParam, const char paramNum)
 *
 * \brief		user cli cmd callback function, get the cli cmd info from user.
 *				the content in this function is just a sample for reference, modify custom user cmd here by yourself
 * \param		pCmd:(Input): command
 * \param		pParam:(Input): parameters of command
 * \param		paramNum:(Input): the number of parameters
 *
 * \return	    0: There is no more data
 *
 ******************************************************************************************/
int UserApp_CliCmd_cb(const char *const pCmd, char **pParam, const char paramNum)
{
    int i, cmd_func_count = sizeof(appcmd_list) / sizeof(APPCMD_LIST);

    //LOGI("CMD = %s\n", pCmd);

    // match command function
    for (i = 0; i < cmd_func_count; i++)
    {
        if (strcmp(pCmd, appcmd_list[i].cmd) == 0)
        {
            appcmd_list[i].func_ptr(pParam, paramNum);
            break;
        }
    }

    return 0;
}

void Usercmd_register_cb(void)
{
	Cli_RegisterUserCmd(UserApp_CliCmd_cb);
}

// this is
DRIVER_INIT(Usercmd_register_cb);


