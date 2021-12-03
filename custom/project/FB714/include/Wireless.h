#ifndef __WIRELESS_H__
#define __WIRELESS_H__

#include "qp_pub.h"

typedef enum
{
	WIRELESS_Type_TX,
	WIRELESS_Type_RX_SURROUND,	
	WIRELESS_Type_EEPROM_SURROUND,
	WIRELESS_Type_RX_SUBWOOFER,	
	WIRELESS_Type_EEPROM_SUBWOOFER,
	WIRELESS_Type_MAX,
	
}eWirelessType;



typedef enum
{
	WLE_UPG_TX,
	WLE_UPG_RX,	
	WLE_UPG_EE_SUR,
	WLE_UPG_EE_SWF,

}eWirelessUpg;


typedef enum
{
	WLE_UPG_METHOD_ONE_BIN,
	WLE_UPG_METHOD_ALL_BIN,

}eWLEUpgMethod;

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
	int (*wl_upg)(QActive* , ARFunc_t, eWirelessUpg, eWLEUpgMethod);
	int (*wl_standby)(void);
	int (*wl_plug_status)(void);

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
int wireless_upg(QActive* me, ARFunc_t func, eWirelessUpg type, eWLEUpgMethod method);

int wireless_standby(void);
int wireless_get_plug_status(void);

enum
{
	eWirelessEQ_20W,	 // plug in
	eWirelessEQ_25W,	 // bypass
	eWirelessEQ_10W, // no plug in
	eWirelessEQ_MAX,
};

	

#endif

