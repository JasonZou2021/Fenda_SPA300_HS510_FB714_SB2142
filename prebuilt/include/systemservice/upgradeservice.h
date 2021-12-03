#ifndef UPGRADESERVICE_H
#define UPGRADESERVICE_H
#include "qp_pub.h"
#include "datarammap.h"
//#include "apupgrade.h"
//#include "pipe.h"

#ifdef BUILT4ISP2
#define upg_printf1(f, a...)  LOGD(f, ##a)
#define upg_printf2(f, a...)  LOGD(f, ##a)
#else
#define upg_printf1(f, a...)  LOGD(f, ##a)
#define upg_printf2(f, a...)  LOGDT(f, ##a)
#endif
#define upg_printf_E(f, a...) LOGE(f, ##a)

#define ISP_UPGRADE_NONE	(0x00000000)
#define ISP_UPGRADE_UART1	(0x11111111)
#define ISP_UPGRADE_USB		(0x22222222)

// enum
// {
// 	eUART_BR_115200 = 0,
// 	eUART_BR_921600,
// 	eUART_BR_1843200,
// 	eUART_BR_3686400,
// 	eUART_BR_CUSTOM,
// 	eUART_BR_max,
// };

enum UPGRRADEMODE
{
	BTUPGRADEMODE,
	USBUPGRADEMODE,
	UARTUPGRADMODE,
	DEFALETMODE,
};

enum ERASEUSERSETTING
{
	USERSETTING_ERASE_ENABLE = 0,
	USERSETTING_ERASE_DISABLE = 1,
};

typedef enum eUpgradeFileName
{
	eSpUpdateFileName = 0,
	eAllFileName,
	eDsptoolFileName,
#ifdef BUILT4ISP2
	eIspboootFileName,
#endif
	eUpgradeFileNum,
} eUpgradeFileName_t;

typedef struct
{
	QEvt super;
	int version_check;
} UPGRADE_CheckVer_Evt_t;

typedef struct
{
	QEvt super;
	QActive *pstSender;
	UINT8 *data;
	UINT16 data_len;
} UPGRADE_Evt_t;

typedef struct
{
	int error_code;
	int offset;
	UINT32 size;
} UPGRADE_CBK_Param_t;

typedef struct
{
	UINT32 partitionAddr;
	UINT32 sdBinLen;
	UINT32 sdprogress;
	UINT32 dPos;
	UINT32 sdWritten;
	int checksum;
	int sdChkSum;
} PARTUPGRADE;

typedef struct
{
	QHsm super;
	QTimeEvt stUpgradeTimeEvt;
	// enum UPGRRADEMODE eUpgradeMode;
	// VOLUMEPARAM pstUpgradeVolParam;
	// int SelUpgFlie;
	// char *pbUpgFileName[eUpgradeFileNum];
	// char *pbUpgFilePath[eUpgradeFileNum];
	// QActive *pstUpgScanFinishCBK;
	QActive *pstCBKResponder;
	UINT32 partition_list;
	UINT8 upgrade_usersetting_erase_flag;
	UINT32 app_header_addr;
	PARTUPGRADE part_upgrd;
	UINT32 runcodeaddr;
	UPGRADE_CBK_Param_t *pCbkParam;
	unsigned int start_time;
	unsigned int end_time;
	int version_check;
	int upgrd_status;
} UPGRADESERVICE;

typedef struct
{
	int date;
	int time;
	char vendor_name[24];
	unsigned int product_id;
	union {
		struct {
			unsigned short sub_version;
			unsigned short main_version;
		};
		unsigned int code_version;
	};
	union {
		struct {
			unsigned char factory_mode;
			unsigned char forceupdate_mode;
		};
		unsigned short mode;
	};
} RUNCODEINFO_T;

typedef struct _stBinFile
{
	//PIPE_T* spBinPipe;
	INT32 sbLastProgress;
	UINT32 dStartAddr;
	UINT32 dFileLenth;
	UINT32 dWritePos;
	INT32 sdChkSum;
	INT32 sdbinReadChkSum;
	INT32 sdprogress;
	INT32 sdbuffpos;
} STBINFILE;

/*---------------------------------------------------------------------------*
 *                         FUNCTION   DECLARATIONS                           *
 *---------------------------------------------------------------------------*/
int UpgradeService_SetVersionCheck(void const *pSender, int version_check);
void UpgradeService_Ctor(UPGRADESERVICE *const me, QActive *superAO);
void UpgradeService_Start(void const *pSender, ARFunc_t pfFunc);
void UpgradeService_Send_Data(void const *pSender, UINT8 volatile *data, UINT16 volatile data_len, ARFunc_t pfFunc);
int IsAcodeRunning(void);
int getRunCodeInfo(RUNCODEINFO_T *);

#endif

