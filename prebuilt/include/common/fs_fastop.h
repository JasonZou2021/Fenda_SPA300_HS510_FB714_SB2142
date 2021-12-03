#ifndef __FS_FASTOP_H__
#define __FS_FASTOP_H__

#include "types.h"

//#define SCAN_CAL_DIR_FILE_SIZE  /* current only for fat not exfat*/
//#define SCAN_SUPPORT_LONG_NAME
#define SCAN_SKIP_EMPTY_DIR  /* only for fat current */
#define SUPPORT_MAX_FILE_DIR 999
#define MALLOC_ITEMS 40 // Malloc size per time

//#define SCAN_DIR_DEBUG //
#define DEBUG_SHORT_NAME  // MORE BYTE for string end(NULL)
#ifdef SCAN_SKIP_EMPTY_DIR
#define FF_FS_EXFAT 1
//#define DEBUG_SACN_EMPTY_DIR_NAME
#endif

#ifdef SCAN_CAL_DIR_FILE_SIZE
//#define SCAN_CAL_DIR_FILE_SIZE_DEBUG
#endif

#define FILEDEBUG
#define LONGNAME (256)
#define SHORTNAME (13)

#ifdef SCAN_SUPPORT_LONG_NAME
//add abel 20181120 for tcl maxjuke
#define USERLONGNAME (33)
#endif

typedef enum {
	SCAN_OK = 0,
	SCAN_STOP,
	SCAN_ERROR,
	FR_TOO_MANY_SCAN_FILES,
	FR_TOO_MANY_SCAN_DIRS
} SCANRESULT;

typedef struct
{
	unsigned int offset;
	unsigned int FoClust;
#ifdef SCAN_CAL_DIR_FILE_SIZE
	unsigned int dirSize; /* all support size by KB */
#endif
#ifdef FILEDEBUG
#ifdef DEBUG_SHORT_NAME
	unsigned char FileName[SHORTNAME+1]; /* one for string end*/
#else
	unsigned char FileName[SHORTNAME];
#endif
#ifdef SCAN_SUPPORT_LONG_NAME
	unsigned char LongFileName[USERLONGNAME];
#endif
#endif
} FastOpenInfo;

typedef struct
{
	char lfname[LONGNAME];
	char altname[SHORTNAME];
} NameInfo;


typedef struct
{
	unsigned char *path;
	int *PFileType;
	int TypeNum;
	int max_dir_cnt;
} SCANFILE;

typedef struct
{
	FastOpenInfo POpenIfo;
	unsigned int Clust;
} DirInfo;

typedef struct
{
	int DirNum;
#ifdef SCAN_SKIP_EMPTY_DIR
	int emptyDirNum;
#endif
	DirInfo *DifIfo;
} RETDIRIFO;

typedef void (*fsop_stop_scan_file)(void);
typedef SCANRESULT (*fsop_scan_dir)(SCANFILE *inifo, RETDIRIFO *PDir);
typedef SCANRESULT (*fsop_scan_dir_file)(SCANFILE *inifo, DirInfo *PDirIfo, FastOpenInfo *outifo, INT32 IfoNum, INT32 *num);

typedef struct FsScanFunC
{
	unsigned char usb_sd_flag;
	fsop_stop_scan_file pfnstopscanfile;
	fsop_scan_dir  pfnscandir;
	fsop_scan_dir_file pfnscandirfile;
} fsScanFc;



#ifdef SCAN_SKIP_EMPTY_DIR
#ifdef DEBUG_SACN_EMPTY_DIR
#define EMPTY_DEBUG(f, a...)	LOGD(f, ##a)
#else
#define EMPTY_DEBUG(f, a...)		do{}while(0)
#endif

typedef struct SpecDirInfo{
	DirInfo dir;
	struct SpecDirInfo *next;
}SpecDirInfo_t;
typedef struct DirInfoMem{
	void *membase;
	SpecDirInfo_t item[MALLOC_ITEMS];
	struct DirInfoMem *next;
}DirInfoMem_t;
typedef struct {
	SpecDirInfo_t *empty_list;
	SpecDirInfo_t *free_list;

	DirInfoMem_t *memlist;
	int cnt;
}EmptyDirInfo;
int empty_dir_init(EmptyDirInfo *empty);
void empty_mempool_free(EmptyDirInfo *empty);
SCANRESULT ScanDirValidFile(SCANFILE *inifo, DirInfo *PDirIfo, char needRec);
void PrintBlockx(BYTE * pBuffStar, DWORD uiBuffLen);
void dump_node_name(const BYTE *dir);

#endif //4 #ifdef SCAN_SKIP_EMPTY_DIR


extern void stop_scan_file();
extern SCANRESULT scan_dir(SCANFILE *inifo, RETDIRIFO *PDir);
extern SCANRESULT  scan_dir_file(SCANFILE *inifo, DirInfo *PDirIfo, FastOpenInfo *outifo, INT32 IfoNum, INT32 *num);

#endif

