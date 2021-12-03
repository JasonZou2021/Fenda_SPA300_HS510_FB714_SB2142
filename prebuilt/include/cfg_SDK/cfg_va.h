
#ifndef CFG_VA_H
#define CFG_VA_H

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include "qp_pub.h"
/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/

#define BGM_MUTE_TIME (10)
#define BGM_RESUME_TIME (3)

typedef struct
{
	unsigned int BGMMuteTime;
	unsigned int BGMResumeTime;
} CFG_VA_T;

#endif

