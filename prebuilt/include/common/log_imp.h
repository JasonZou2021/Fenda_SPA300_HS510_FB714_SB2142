#ifndef __LOG_IMPL_H__
#define __LOG_IMPL_H__

#define ID_ADC_STR    "ADC"
#define ID_APAUD_STR  "AUD"
#define ID_APL_STR    "APL"
#define ID_APUSR_STR  "USR"
#define ID_APVA_STR   "USR"
#define ID_AUD_STR    "AUD"
#define ID_BT_STR     "BT "
#define ID_CEC_STR    "CEC"
#define ID_CMD_STR    "CMD"
#define ID_CTR_STR    "CTR"
#define ID_DISP_STR   "DIS"
#define ID_DLNA_STR   "DLN"
#define ID_DMX_STR    "DMX"
#define ID_DPS_STR    "DPS"
#define ID_DSP_STR    "DSP"
#define ID_FL_STR     "FL "
#define ID_FS_STR     "FS "
#define ID_I2C_STR    "I2C"
#define ID_IOP_STR    "IOP"
#define ID_IR_STR     "IR "
#define ID_KEY_STR    "KEY"
#define ID_KOK_STR    "KOK"
#define ID_LIC_STR    "LIC"
#define ID_MDNS_STR   "MDN"
#define ID_OS_STR     "OS "
#define ID_PSX_STR    "PSX"
#define ID_PWR_STR    "PWR"
#define ID_QP_STR     "QP "
#define ID_SD_STR     "SD "
#define ID_SET_STR    "SET"
#define ID_SOCK_STR   "SCK"
#define ID_SPI_STR    "SPI"
#define ID_SRC_STR    "SRC"
#define ID_SYS_STR    "SYS"
#define ID_TEST_STR   "TST"
#define ID_UKN_STR    "UKN"
#define ID_UPG_STR    "UPG"
#define ID_USB_STR    "USB"
#define ID_VFD_STR    "VFD"

#define ID_ADC_LVL    LV_D
#define ID_APAUD_LVL  LV_D
#define ID_APL_LVL    LV_D
#define ID_APUSR_LVL  LV_D
#define ID_APVA_LVL   LV_D
#define ID_AUD_LVL    LV_D
#define ID_BT_LVL     LV_D
#define ID_CEC_LVL    LV_D
#define ID_CMD_LVL    LV_D
#define ID_CTR_LVL    LV_D
#define ID_DISP_LVL   LV_D
#define ID_DLNA_LVL   LV_D
#define ID_DMX_LVL    LV_D
#define ID_DPS_LVL    LV_D
#define ID_DSP_LVL    LV_D
#define ID_FL_LVL     LV_D
#define ID_FS_LVL     LV_D
#define ID_I2C_LVL    LV_D
#define ID_IOP_LVL    LV_D
#define ID_IR_LVL     LV_D
#define ID_KEY_LVL    LV_D
#define ID_KOK_LVL    LV_D
#define ID_LIC_LVL    LV_D
#define ID_MDNS_LVL   LV_D
#define ID_OS_LVL     LV_D
#define ID_PSX_LVL    LV_D
#define ID_PWR_LVL    LV_D
#define ID_QP_LVL     LV_D
#define ID_SD_LVL     LV_D
#define ID_SET_LVL    LV_D
#define ID_SOCK_LVL   LV_D
#define ID_SPI_LVL    LV_D
#define ID_SRC_LVL    LV_D
#define ID_SYS_LVL    LV_D
#define ID_TEST_LVL   LV_D
#define ID_UKN_LVL    LV_D
#define ID_UPG_LVL    LV_D
#define ID_USB_LVL    LV_D
#define ID_VFD_LVL    LV_D

typedef enum
{
	LOG_STANDBY = 0,
	LOG_ENABLED,
	LOG_DISABLED,
	LOG_STATE_MAX
} STATE_e;

typedef enum {
    ID_ADC = 0,
    ID_APAUD,// AP for audio player
    ID_APL,  // Airplay protocol log
    ID_APUSR,// AP for all users
    ID_APVA,
    ID_AUD,  // Audio log
    ID_BT,
    ID_CEC,
    ID_CMD,  // CLI log in system
    ID_CTR,  // Controller log
    ID_DISP, // display
    ID_DLNA, // DLNA protocaol log
    ID_DMX,  // Demux log
    ID_DPS,  // AP for dps log
    ID_DSP,
    ID_FL,   // File list  log
    ID_FS,
    ID_I2C,
    ID_IOP,
    ID_IR,
    ID_KEY,
    ID_KOK,
    ID_LIC,  // LIC For DSP license,always print
    ID_MDNS,
    ID_OS,
    ID_PSX,  // Posix log in system
    ID_PWR,  // power log
    ID_QP,   // QP log in system
    ID_SD,
    ID_SET,  // Settings log in system
    ID_SOCK, // Socket log
    ID_SPI,
    ID_SRC,  // source log
    ID_SYS,
    ID_TEST,
    ID_UKN,
    ID_UPG,  // Upgrade log
    ID_USB,
    ID_VFD,  // VFD log
    MODULE_MAX
} MODULE_ID_e;

typedef enum
{
    LV_T,
	LV_D,
	LV_I,
	LV_W,
	LV_E,
	LV_MAX
} LEVEL_e;

typedef enum
{
	SWITCH_UNKOWN = 0,
	SWITCH_ON,
	SWITCH_OFF
} SWITCH_e;

void LogSytemInitialize(void);

char *GetLogSystemPassword(void);

unsigned IsLogSytstemUnlocked(void);

unsigned IsNeedPrintout(MODULE_ID_e mod, LEVEL_e lev);

void EnableSystemLog(void);

void StandbySystemLog(void);

void LOG_SetState(STATE_e state);

STATE_e LOG_GetState(void);

void SetLogLevel(MODULE_ID_e modId, LEVEL_e lv);

LEVEL_e GetLogLevel(MODULE_ID_e modId);

void PrintMpduleID(void);

// void LogImplemention(MODULE_ID_e module, LEVEL_e level, const char *format, ...) __attribute__((__format__(printf, 3, 4)));
void LogImplemention(MODULE_ID_e module, LEVEL_e level, const char *format, ...);

#endif

