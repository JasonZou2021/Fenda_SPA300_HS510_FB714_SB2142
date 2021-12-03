
#ifndef _KEY_Srv
#include "qp_pub.h"
#include "cfg_sdk.h"
#include "iop_scanner.h"
#include "Display_cfg.h"
#include "Display_Srv.h"


#define _KEY_Srv


typedef struct       /* the Key active object */
{
	QHsm super;
	QTimeEvt timeEvtKEY; /* private time event generator */
	hCircBuf_t *hKeyBuf; // ring buf handler (key)
	stKeyInfo_t *stDisplayKeyData; /* Display Key info structure */
	CFG_KEY_T *pstKEY_cfg;
	VFD_CFG_T *pstVFD_cfg;
	KEY_GPIO_CFG_T *pstKEY_GPIO_cfg;
	int iDisplayTickCnt;
} KeySrv;

void Key_ctor(KeySrv *const me);

#endif
