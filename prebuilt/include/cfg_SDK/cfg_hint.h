
#ifndef CFG_HINT_H
#define CFG_HINT_H

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include "qp_pub.h"
/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/

#define HINT_PROMPTLY_PLAY (1 << 16)
#define HINT_SMOOTH_PLAY (0 << 16)
#define XEFF_BUF_SIZE 100	//unit Kb

typedef struct
{
	unsigned int singlePlayMode;
	UINT8 xeffBufSize;
} CFG_HINT_T;

#endif

