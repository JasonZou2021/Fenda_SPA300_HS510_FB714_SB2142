#ifndef __WIRELESS_H__
#define __WIRELESS_H__

#include "qp_pub.h"

typedef enum
{
	WIRELESS_UPG_TX,
	WIRELESS_UPG_RX_SURROUND,	
	WIRELESS_UPG_EEPROM_SURROUND,
	WIRELESS_UPG_RX_SUBWOOFER,	
	WIRELESS_UPG_EEPROM_SUBWOOFER,
	WIRELESS_UPG_MAX,
}eWirelessUpg;

#define WL_UPG_OK 0XFF
#define WL_UPG_NG 0XFE

typedef struct _wireless_func
{
	int (*wl_Init)(void);
	int (*wl_pair)(unsigned char);
	int (*wl_vol)(unsigned char);
	int (*wl_mute)(unsigned char);
	int (*wl_woofer_vol)(unsigned char);
	int (*wl_surround_vol)(unsigned char);
	int (*wl_bass_vol)(unsigned char);
	int (*wl_eq)(unsigned char);
	int (*wl_version)(UINT8 *);
	int (*wl_poll)(void);
	int (*wl_get_syn)(void);
	int (*wl_upg)(QActive* , ARFunc_t, eWirelessUpg);
	int (*wl_standby)(void);

}WIRELESS_FUNC; 

int wireless_init(void);
int wireless_pair(BYTE status);
int wireless_volume(unsigned char gain);
int wireless_woofer_vol(unsigned char gain);
int wireless_surround_vol(unsigned char gain);
int wireless_bass_vol(unsigned char gain);
int wireless_eq(unsigned char eq);
int wireless_mute(unsigned char mute);
int wireless_get_version(UINT8 *type);
int wireless_polling(void);
int wireless_get_syn(void);
// type   0: RX    1:  TX
int wireless_upg(QActive* me, ARFunc_t func, eWirelessUpg type);

int wireless_standby(void);
	

#endif
