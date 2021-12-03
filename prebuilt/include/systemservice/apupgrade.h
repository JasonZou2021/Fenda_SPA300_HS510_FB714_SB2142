#ifndef __APUPGRADE_H__
#define __APUPGRADE_H__
#include "types.h"
#include "mbase.h"
#include "upgradeservice.h"
#include "pipe.h"

#define BIN_FILE_NAME			 "USB:/rom.bin"
#define SUPPORT_USB_UPGRADE

#define UPGRADE_FLAG			"SPHE"
#define UPGRADE_FLAG_SIZE		(4)

#define UPGRADE_FLAG_ADDR		(40)

#define PARTITION_MAP_SIZE             (0x1000)
#define HEADER_SIZE				(0x80)


typedef enum
{
	FLASH_BIN_TYPE_VALID = 0,
	FLASH_BIN_TYPE_INVALID,
} FLASH_BIN_TYPE_E;

typedef enum
{
	BIN_TYPE_INVALIDE = -1,
	BIN_TYPE_APP_BIN = 0,
	BIN_TYPE_ALL_BIN = 1,
	BIN_TYPE_DSP_BIN,
#ifdef BUILT4ISP2
	BIN_TYPE_ISPBOOOT_BIN,
#endif
	BIN_TYPE_MAX,
} BIN_TYPE_E;

typedef enum
{
	USB_OPEN_FILE = 0,
	USB_UPGRADE_START = 1,
	USB_UPGRADE_FAILE_RETURN,
	USB_UPGRADE_FAIL,
	USB_UPGRADE_OK,
} USB_UPGRADE_STATE_E;

typedef enum
{
	MOUNT_OK = 0,
	MOUNT_FAIL = 1,
	UNMOUNT_OK,
	UNMOUNT_FAIL,
} MOUNT_STATE_E;

INT32 AppBinUpgrade(BIN_TYPE_E BinType, char *pbUpgFilePath, UINT8 UsersettingEraseFlag);
INT32 CheckFlashBin(UINT32 sdaddr);
UINT8 GetUpgradeBin (UINT8 *dbintype, char *DiscName, char **pbBinFileName);
void UpgradeWorkFree(void *pParam);
int GetCheckSum(UINT8 *pbBinBuff, UINT32 dOffSet, UINT32 dBuffLen);
int TimeMjdToYmd(UINT32 diMjd, UINT16 *pwYear, UINT8 *pbMonth, UINT8 *pbDay);

USB_UPGRADE_STATE_E UpdateSettingFlash();
PIPE_T *CreatBinPipe(void);
BIN_TYPE_E CheckSunplusBinType(UINT8 *pbUpgradeFile, UINT32 dFileLength __attribute__((unused)));
MOUNT_STATE_E MountUSB(VOLUMEPARAM pstUpgradeVolParam);

UINT32 GetBinLength(UINT8 *pbUpgradeFile, UINT32 dLength);
void Get_Flash_Size_Info(UINT32 *SIZE);
UINT32 get_setting_start(void);

typedef BIN_TYPE_E (*pFunComCheckBinType)(UINT8 *pu8UpgradeFile, UINT32 u32FileLength);
/*****************************************************************************
*
 * \fn		 static UINT32 GetUpgradeAddr ( BIN_TYPE_E mode )
 *
 * \brief	get the upgrade Addr
 * \param  	mode the mode of bin
 * \return	UINT32
*
 * \note		none.
 * ******************************************************************************/
UINT32 GetUpgradeAddr ( BIN_TYPE_E mode );

int get_flash_sector_size(void);
#define READ_SIZE			(get_flash_sector_size())
#define WRITE_SIZE			(get_flash_sector_size())

#define O_RDONLY     			(1<<0)   /* Open for reading only */
#define O_WRONLY     			(1<<1)   /* Open for writing only */

#endif  /*__APUPGRADE_H__*/

