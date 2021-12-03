#ifndef _FSI_API_H
#define _FSI_API_H

#include "types.h"
#include "../common/fs_fastop.h"

/* File access modes used for open() and fnctl() */
#define O_RDONLY     (1<<0)   /* Open for reading only */
#define O_WRONLY     (1<<1)   /* Open for writing only */
#define O_RDWR       (O_RDONLY|O_WRONLY) /* Open for reading and writing */

/* File access mode mask */
#define O_ACCMODE    (O_RDONLY|O_RDWR|O_WRONLY)

/* open() mode flags */

#define O_CREAT      (1<<3)    /* Create file it it does not exist */
#define O_EXCL       (1<<4)    /* Exclusive use */
#define O_NOCTTY     (1<<5)    /* Do not assign a controlling terminal */
#define O_TRUNC      (1<<6)    /* Truncate */

/* File status flags used for open() and fcntl() */
#define O_APPEND     (1<<7)    /* Set append mode */
#define O_DSYNC      (1<<8)    /* Synchronized I/O data integrity writes */
#define O_NONBLOCK   (1<<9)    /* No delay */
#define O_RSYNC      (1<<10)   /* Synchronized read I/O */
#define O_SYNC       (1<<11)   /* Synchronized I/O file integrity writes */
#define O_SEEK       (1<<12)

//Modes for fsync()
#define FSYNC              1
#define FDATASYNC          2

#define PATH_MARK       0xFF55

//sTat mode
#define __stat_mode_DIR    (1<<0)
#define __stat_mode_CHR    (1<<1)
#define __stat_mode_BLK    (1<<2)
#define __stat_mode_REG    (1<<3)
#define __stat_mode_FIFO   (1<<4)
#define __stat_mode_MQ     (1<<5)
#define __stat_mode_SEM    (1<<6)
#define __stat_mode_SHM    (1<<7)

#define S_ISDIR(__mode)  ((__mode) & __stat_mode_DIR )
#define S_ISCHR(__mode)  ((__mode) & __stat_mode_CHR )
#define S_ISBLK(__mode)  ((__mode) & __stat_mode_BLK )
#define S_ISREG(__mode)  ((__mode) & __stat_mode_REG )
#define S_ISFIFO(__mode) ((__mode) & __stat_mode_FIFO )

#define S_TYPEISMQ(__buf)   ((__buf)->st_mode & __stat_mode_MQ )
#define S_TYPEISSEM(__buf)  ((__buf)->st_mode & __stat_mode_SEM )
#define S_TYPEISSHM(__buf)  ((__buf)->st_mode & __stat_mode_SHM )

#define S_IFDIR          (__stat_mode_DIR)
#define S_IFCHR          (__stat_mode_CHR)
#define S_IFBLK          (__stat_mode_BLK)
#define S_IFREG          (__stat_mode_REG)

//lseek cmd list:
#ifdef SEEK_SET
	#undef SEEK_SET
#endif
#ifdef SEEK_CUR
	#undef SEEK_CUR
#endif
#ifdef SEEK_END
	#undef SEEK_END
#endif
#define SEEK_SET			0x00
#define SEEK_CUR			0x01
#define SEEK_END			0x02

#define FREE_FAT_TABLE     0
#define ALLOC_FAT_TABLE    1

typedef UINT32 mDIR;   //just for fd value
typedef INT64  offset_t;
typedef struct _sTat
{
	UINT32  st_mode;     /* File mode */
	offset_t  st_size;     /* File size (regular files only) */
	UINT32  st_atime;    /* Last access time */
	UINT32  st_mtime;    /* Last data modification time */
	UINT32  st_ctime;    /* Last file status change time */
	UINT16  st_dev;      /* ID of device containing file */
} sTat;

#define DIREC_NAME_MAX              64
typedef struct _direct
{
	offset_t dSize;
	UINT32 dAddr;
	UINT32 dRecLba;
	UINT32 dRecOffset;
	char szName[DIREC_NAME_MAX];    /* record name */
	UINT16 wAttr;
	UINT8 bIsDir;
	UINT8 ucCrtTimeTenth;  //daijun add for mts 245630
	UINT32 dwCrtDateAndTime;  //daijun add for mts 245630
} Direct;

extern int fastopen ( FastOpenInfo *po, NameInfo *Ni, int flags, int mode );
extern mDIR *fastopendir ( FastOpenInfo *po, NameInfo *Ni);

extern int mount(const char *devname, const char *dir, const char *fsname);
extern int umount(const char *name);
extern mDIR *opendir( const char *dirname);
extern int closedir (mDIR *dirp );
extern int chdir( const char *path );
extern int mkdir( const char *path, UINT32 mode );
extern int rmdir(const char *path);
extern int open(const char *path, int flags, int mode);
extern int rename( const char *path1, const char *path2 );
extern int stat( const char *path, sTat *buf );
extern int creat( const char *path, int mode );
extern Direct *readdir(mDIR *dirp);
extern int read(int fd, void *buf, UINT32 len);
extern int write(int fd, void *buf, UINT32 len);
extern offset_t lseek( int fd, offset_t pos, int whence );
extern int fsync( int fd );
extern int fstat( int fd, sTat *buf );
extern int close( int fd );
extern int chroot( const char *path );
extern const char *get_mte_name(void);
extern int ioctl( int fd, int com, int data );

int32_t OpenDev(uint8_t *pIdStr);

#endif //_FSI_API_H

