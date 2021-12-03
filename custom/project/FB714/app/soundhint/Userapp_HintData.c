#include "dsp_hint_sound.h"
#include "AudDspService.h"

#include "USB_option_3.inc"
#include "Optical_in.inc"
#include "HDMI.inc"
#include "Auxiliary_in.inc"
#include "Bluetooth.inc"

#include "Bluetooth_connected.inc"
#include "Bluetooth_pairing.inc"

#include "Movie_mode.inc"
#include "Music_mode.inc"
#include "Talk_mode.inc"
#include "Standard_mode.inc"

#include "Power_Off.inc"
#include "Power_On.inc"

#include "Volume_Max.inc"

#include "arc.inc"
#include "usc.inc"



DSP_HINT_SOUND_P hint_bt_connect = 
{
	.HINT_DATA = Bluetooth_connected,
	.SIZE = sizeof(Bluetooth_connected),

	.duck_vol = DEFAULT_DUCK_VOL,
	.xef_vol = DEFAULT_XEF_VOL,
	.L_to_out_ch_num = DEFAULT_L_OUT_CH,
	.R_to_out_ch_num = DEFAULT_R_OUT_CH,
};


DSP_HINT_SOUND_P hint_bt_disconnect = 
{
	.HINT_DATA = Bluetooth_pairing,
	.SIZE = sizeof(Bluetooth_pairing),

	.duck_vol = DEFAULT_DUCK_VOL,
	.xef_vol = DEFAULT_XEF_VOL,
	.L_to_out_ch_num = DEFAULT_L_OUT_CH,
	.R_to_out_ch_num = DEFAULT_R_OUT_CH,
};



DSP_HINT_SOUND_P hint_usb = 
{
	.HINT_DATA = USB_option_3,
	.SIZE = sizeof(USB_option_3),

	.duck_vol = DEFAULT_DUCK_VOL,
	.xef_vol = DEFAULT_XEF_VOL,
	.L_to_out_ch_num = DEFAULT_L_OUT_CH,
	.R_to_out_ch_num = DEFAULT_R_OUT_CH,
};


DSP_HINT_SOUND_P hint_hdmi = 
{
	.HINT_DATA = HDMI,
	.SIZE = sizeof(HDMI),

	.duck_vol = DEFAULT_DUCK_VOL,
	.xef_vol = DEFAULT_XEF_VOL,
	.L_to_out_ch_num = DEFAULT_L_OUT_CH,
	.R_to_out_ch_num = DEFAULT_R_OUT_CH,
};


DSP_HINT_SOUND_P hint_optical = 
{
	.HINT_DATA = Optical_in,
	.SIZE = sizeof(Optical_in),

	.duck_vol = DEFAULT_DUCK_VOL,
	.xef_vol = DEFAULT_XEF_VOL,
	.L_to_out_ch_num = DEFAULT_L_OUT_CH,
	.R_to_out_ch_num = DEFAULT_R_OUT_CH,
};

DSP_HINT_SOUND_P hint_bt = 
{
	.HINT_DATA = Bluetooth,
	.SIZE = sizeof(Bluetooth),

	.duck_vol = DEFAULT_DUCK_VOL,
	.xef_vol = DEFAULT_XEF_VOL,
	.L_to_out_ch_num = DEFAULT_L_OUT_CH,
	.R_to_out_ch_num = DEFAULT_R_OUT_CH,
};

#if 1 //def SB714
DSP_HINT_SOUND_P hint_usc = 
{
	.HINT_DATA = USB_sound_card,
	.SIZE = sizeof(USB_sound_card),

	.duck_vol = DEFAULT_DUCK_VOL,
	.xef_vol = DEFAULT_XEF_VOL,
	.L_to_out_ch_num = DEFAULT_L_OUT_CH,
	.R_to_out_ch_num = DEFAULT_R_OUT_CH,
};
#endif

DSP_HINT_SOUND_P hint_aux = 
{
	.HINT_DATA = Auxiliary_in,
	.SIZE = sizeof(Auxiliary_in),

	.duck_vol = DEFAULT_DUCK_VOL,
	.xef_vol = DEFAULT_XEF_VOL,
	.L_to_out_ch_num = DEFAULT_L_OUT_CH,
	.R_to_out_ch_num = DEFAULT_R_OUT_CH,
};

DSP_HINT_SOUND_P hint_arc = 
{
	.HINT_DATA = src_arc,
	.SIZE = sizeof(src_arc),

	.duck_vol = DEFAULT_DUCK_VOL,
	.xef_vol = DEFAULT_XEF_VOL,
	.L_to_out_ch_num = DEFAULT_L_OUT_CH,
	.R_to_out_ch_num = DEFAULT_R_OUT_CH,
};



DSP_HINT_SOUND_P hint_music= 
{
	.HINT_DATA = Music_mode,
	.SIZE = sizeof(Music_mode),

	.duck_vol = DEFAULT_DUCK_VOL,
	.xef_vol = DEFAULT_XEF_VOL,
	.L_to_out_ch_num = DEFAULT_L_OUT_CH,
	.R_to_out_ch_num = DEFAULT_R_OUT_CH,
};

DSP_HINT_SOUND_P hint_movie = 
{
	.HINT_DATA = Movie_mode,
	.SIZE = sizeof(Movie_mode),

	.duck_vol = DEFAULT_DUCK_VOL,
	.xef_vol = DEFAULT_XEF_VOL,
	.L_to_out_ch_num = DEFAULT_L_OUT_CH,
	.R_to_out_ch_num = DEFAULT_R_OUT_CH,
};


DSP_HINT_SOUND_P hint_talk = 
{
	.HINT_DATA = Talk_mode,
	.SIZE = sizeof(Talk_mode),

	.duck_vol = DEFAULT_DUCK_VOL,
	.xef_vol = DEFAULT_XEF_VOL,
	.L_to_out_ch_num = DEFAULT_L_OUT_CH,
	.R_to_out_ch_num = DEFAULT_R_OUT_CH,
	
};


DSP_HINT_SOUND_P hint_power_on= 
{
	.HINT_DATA = Power_On,
	.SIZE = sizeof(Power_On),

	.duck_vol = DEFAULT_DUCK_VOL,
	.xef_vol = DEFAULT_XEF_VOL,
	.L_to_out_ch_num = DEFAULT_L_OUT_CH,
	.R_to_out_ch_num = DEFAULT_R_OUT_CH,
};

DSP_HINT_SOUND_P hint_power_off = 
{
	.HINT_DATA = Power_Off,
	.SIZE = sizeof(Power_Off),

	.duck_vol = DEFAULT_DUCK_VOL,
	.xef_vol = DEFAULT_XEF_VOL,
	.L_to_out_ch_num = DEFAULT_L_OUT_CH,
	.R_to_out_ch_num = DEFAULT_R_OUT_CH,
};


DSP_HINT_SOUND_P hint_volmax = 
{
	.HINT_DATA = Volume_Max,
	.SIZE = sizeof(Volume_Max),

	.duck_vol = DEFAULT_DUCK_VOL,
	.xef_vol = DEFAULT_XEF_VOL,
	.L_to_out_ch_num = DEFAULT_L_OUT_CH,
	.R_to_out_ch_num = DEFAULT_R_OUT_CH,
};


