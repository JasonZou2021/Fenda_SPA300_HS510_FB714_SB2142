#ifndef _IR_KEY_H
#define _IR_KEY_H

#define IR_KEY_NUM 255

typedef enum _IR_KEY_LIST
{
	// 00-0F
	IRC_0,                          //0x00
	IRC_1,                          //0x01
	IRC_2,                          //0x02
	IRC_3,                          //0x03
	IRC_4,                          //0x04
	IRC_5,                          //0x05
	IRC_6,                          //0x06
	IRC_7,                          //0x07
	IRC_8,                          //0x08
	IRC_9,                          //0x09
	IRC_10,                         //0x0a
	IRC_P10,                        //0x0b
	IRC_EJECT,                      //0x0c
	IRC_MUTE,                       //0x0d  T:(靜音)
	IRC_KEY_UP,                     //0x0e
	IRC_KEY_RESET,                  //0x0f

	//10-1F
	IRC_KEY_DN,                     //0x10  T:key-
	IRC_VOLUME_UP,                  //0x11  T:(音量+)
	IRC_VOLUME_DN,                  //0x12  T:(音量-)
	IRC_SLOW,                       //0x13
	IRC_STEP,                       //0x14
	IRC_PAUSE,                      //0x15
	IRC_PLAY,                       //0x16
	IRC_PLAY_RESUME,                //0x17
	IRC_RETURN,                     //0x18
	IRC_STOP,                       //0x19
	IRC_LANGUAGE,                   //0x1a
	IRC_BACKWARD,                   //0x1b  Picture- T:(畫面-)
	IRC_FORWARD,                    //0x1c  Picture+ T:(畫面+)
	IRC_PREV,                       //0x1d
	IRC_NEXT,                       //0x1e
	IRC_DISPLAY,                    //0x1f  Display T:(顯示)

	//20-2F
	IRC_AB,                         //0x20  B
	IRC_REPEAT,                     //0x21  A
	IRC_PBC,                        //0x22  Manu T:(選單)
	IRC_GOTO,                       //0x23  SetTime T:(選時)
	IRC_RANDOM,                     //0x24
	IRC_RESUME,                     //0x25
	IRC_TIME,                       //0x26
	IRC_POWER,                      //0x27  Power T:(電源)
	IRC_ZOOM,                       //0x28
	IRC_UP,                         //0x29
	IRC_DOWN,                       //0x2a
	IRC_LEFT,                       //0x2b
	IRC_RIGHT,                      //0x2c
	IRC_SRC,                        //0x2d
	IRC_FREEZE,                     //0x2e
	IRC_CHANNEL,                    //0x2f

	//30-3F
	IRC_CHANNEL_DN,                 //0x30
	IRC_EFFECT,                     //0x31 sound mode
	IRC_PREVIEW1,                       //0x32
	IRC_9FRAMES,                    //0x33
	IRC_PROGRAM,                    //0x34
	IRC_FORMAT,                     //0x35
	IRC_INTRO,                      //0x36
	IRC_KEY_DISC,                    //0x37
	IRC_KEY_USB,                     //0x38
	IRC_KEY_CARD,                    //0x39
	IRC_GAME_DN,                    //0x3a
	IRC_GAME_TN,                    //0x3b
	IRC_GAME_UL,                    //0x3c
	IRC_GAME,                       //0x3d
	IRC_VIRTUAL_KEYBOARD,           //0x3e, // alan, 2003/8/16 05:02AM
	IRC_DISC1,                      //0x3f

	//40-4F
	IRC_DISC2,                      //0x40
	IRC_DISC3,                      //0x41
	IRC_PAUSEPLAY,                  //0x42
	IRC_QUALIFY,                    //0x43
	IRC_RESET_DSP,                  //0x44
	IRC_SUBP,                       //0x45
	IRC_ANGLE,                      //0x46
	IRC_AUD_LANGUAGE,               //0x47
	IRC_SELECT,                     //0x48
	IRC_TITLEMENU,                  //0x49
	IRC_CLEAR,                      //0x4a
	IRC_SETUP,                      //0x4b
	IRC_KARAOKE,                    //0x4c
	IRC_3D,                         //0x4d
	IRC_BK_SLOW,                    //0x4e
	IRC_READING,                    //0x4f

	//50-5F
	IRC_MENU,                  		//0x50
	IRC_VOLUME,                     //0x51
	IRC_AUDIO,                      //0x52
	IRC_PLAY_PAUSE,                 //0x53
	IRC_HOME,                       //0x54
	IRC_PLAY_SELECT,                //0x55
	IRC_MIX_FUNC,                   //0x56
	IRC_SLOW_BAK_FORD,              //0x57
	IRC_BOOK,                       //0x58
	IRC_SCORE,                      //0x59
	IRC_TITLE_RETURN,               //0x5a
	IRC_ECHO_UP,                    //0x5b
	IRC_ECHO_DOWN,                  //0x5c
	IRC_MIC_UP,                     //0x5d
	IRC_MIC_DOWN,                   //0x5e
	IRC_P_SCAN,                     //0x5f

	IRC_CHG_DAC,                    //0x60
	IRC_NULL5d,                     //0x61
	IRC_DEFAULT_SETUP,              //0x62
	IRC_NULL5f,                     //0x63
	IRC_ONLY_PAUSE,                 //0x64
	IRC_EXIT_SETUP,                 //0x65
	IRC_NULL,                       //0x66
	IRC_OPTION,                     //0x67
	IRC_SUBTITLE,                   //0x68
	IRC_KOK_REC,                    //0x69

	IRC_FACTROY_SWITCH_VIDEO,       //0x6a
	IRC_FACTROY_SWITCH_16943,       //0x6b
	IRC_FACTROY_SWITCH_PN,          //0x6c
	IRC_FACTROY_SHOW_VERSION,       //0x6d
	IRC_FACTROY_RESET,              //0x6e

	// VMCD use
	IRC_DELETE,						//0x6f
	IRC_ENCODE,						//0x70

	IRC_PN,                        //0x71
	IRC_PROG,                      //0x72
	IRC_PREVIEW,                   //0x73
	IRC_TV_RADIO,                  //0x74
	IRC_MIXER,                     //0x75
	IRC_RIPPING,                   //0x76
	IRC_REC,                       //0X77
	IRC_HD,                        //0X78
	IRC_BT_SOURCE_DISCONNECT,		//0x79
	IRC_WSS_BT_PAIR,				//0x7a
	IRC_BT,							//0x7b
	IRC_USB,						//0x7c
	IRC_WOOFER_VOL_UP,				//0x7d
	IRC_WOOFER_VOL_DOWN,			//0x7e

	IRC_MUTE_ON,					//0x7f
	IRC_MUTE_OFF,					//0x80
	IRC_POWER_OFF,					//0x81
	IRC_POWER_ON,					//0x82
	IRC_DOLBY,

	IRC_TREBLE,
	IRC_BASS,
	IRC_DIM,
	IRC_OPTICAL,
	IRC_AUX,
	IRC_ARC,
	IRC_HDMI,

	IRC_BASS_DN,
	IRC_BASS_UP,
	IRC_TREBLE_UP,
	IRC_TREBLE_DN,

	IRC_MUSIC,			
	IRC_MOVIE,       	
	IRC_NEWS,
	IRC_NIGHT,
	IRC_SPORT,

	IRC_DIM_UP,
	IRC_DIM_DN,
  		
	IRC_COAX,
	
	IR_CODE_MAX_NUM,       //Just remark the maxium IR COMAMND Numbers. No func mapped to it!!
} _IR_KEY_LIST;



#endif //_IR_KEY_H

