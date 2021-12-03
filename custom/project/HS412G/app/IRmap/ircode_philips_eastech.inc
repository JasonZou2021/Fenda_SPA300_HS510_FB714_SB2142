
const BYTE ir_mapcode[] =
{
#if 0//ndef DVDRELEASE
	IRC_GOTO,  IRC_HOME,    IRC_KOK_REC,           IRC_REPEAT, //		IRC_9, //0x00, 0x01 ,0x02, 0x03
	IRC_4,     IRC_5,          IRC_6,               IRC_7, //0x04, 0x05 ,0x06, 0x07
	IRC_8,     IRC_9,          IRC_NULL,                    IRC_NULL, //0x08, 0x09 ,0x0a, 0x0b
#else
	IRC_0,  IRC_1,    IRC_2,           IRC_3, //		IRC_9, //0x00, 0x01 ,0x02, 0x03
	IRC_4,     IRC_5,          IRC_6,               IRC_7, //0x04, 0x05 ,0x06, 0x07
	IRC_8,     IRC_9,          IRC_NULL,                    IRC_NULL, //0x08, 0x09 ,0x0a, 0x0b
#endif
	IRC_NULL,    IRC_NULL,  IRC_NULL,  IRC_NULL, //0x0c, 0x0d ,0x0e, 0x0f

	IRC_NULL,    IRC_NULL,        IRC_NULL,         IRC_NULL, //0x10, 0x11 ,0x12, 0x13
	IRC_NULL,     IRC_NULL,    IRC_NULL, IRC_NULL,  //0x14, 0x15 ,0x16, 0x17
	IRC_NULL,             IRC_NULL,         IRC_NULL,             IRC_NULL,   //0x18, 0x19 ,0x1a, 0x1b
	IRC_NULL,   IRC_REPEAT,       IRC_NULL,           IRC_NULL,   //0x1c, 0x1d ,0x1e, 0x1f

	IRC_NEXT,       IRC_PREV,               IRC_NULL,               IRC_NULL, //0x20, 0x21 ,0x22, 0x23
	IRC_NULL,     IRC_NULL,               IRC_NULL,               IRC_NULL, //0x24, 0x25 ,0x26, 0x27
	IRC_NULL,     IRC_NULL,               IRC_NULL,               IRC_NULL, //0x28, 0x29 ,0x2a, 0x2b
	IRC_PLAY_PAUSE,     IRC_NULL,          IRC_NULL,               IRC_NULL, //0x2c, 0x2d ,0x2e, 0x2f

	IRC_NULL,     IRC_STOP,               IRC_NULL,               IRC_NULL, //0x30, 0x31 ,0x32, 0x33
	IRC_NULL,     IRC_NULL,               IRC_NULL,               IRC_NULL, //0x34, 0x35 ,0x36, 0x37
	IRC_NULL,     IRC_NULL,               IRC_NULL,               IRC_NULL, //0x38, 0x39 ,0x3a, 0x3b
	IRC_NULL,     IRC_NULL,               IRC_NULL,               IRC_NULL, //0x3c, 0x3d ,0x3e, 0x3f

	IRC_NULL,         IRC_NULL,     IRC_EJECT,            IRC_NULL, //0x40, 0x41 ,0x42, 0x43
	IRC_NULL,       IRC_NULL,             IRC_NULL,     IRC_NULL, //0x44, 0x45 ,0x46, 0x47
	IRC_NULL,     IRC_NULL,               IRC_NULL,   IRC_SUBTITLE, //0x48, 0x49 ,0x4a, 0x4b
	IRC_NULL,   IRC_NULL,      IRC_AUDIO,               IRC_NULL, //0x4c, 0x4d ,0x4e, 0x4f

	IRC_NULL,     IRC_NULL,       IRC_NULL,       IRC_NULL, //0x50, 0x51 ,0x52, 0x53
	IRC_NULL, IRC_NULL,   IRC_NULL,          IRC_NULL, //0x54, 0x55 ,0x56, 0x57
	IRC_UP,     IRC_DOWN,         IRC_LEFT,               IRC_RIGHT, //0x58, 0x59 ,0x5a, 0x5b
	IRC_SELECT,  IRC_NULL,                  IRC_NULL,             IRC_NULL, //0x5c, 0x5d ,0x5e, 0x5f

	IRC_NULL,     IRC_NULL,                   IRC_NULL,           IRC_NULL, //0x60, 0x61 ,0x62, 0x63
	IRC_NULL,     IRC_FACTROY_SWITCH_VIDEO,    IRC_FACTROY_SWITCH_16943,     IRC_FACTROY_SWITCH_PN, //0x64, 0x65 ,0x66, 0x67
	IRC_FACTROY_SHOW_VERSION,     IRC_FACTROY_RESET,             IRC_NULL,       IRC_NULL, //0x68, 0x69 ,0x6a, 0x6b
	IRC_NULL,     IRC_NULL,                  IRC_NULL,           IRC_NULL, //0x6c, 0x6d ,0x6e, 0x6f

	IRC_NULL,     IRC_NULL,               IRC_NULL,                 IRC_NULL, //0x70, 0x71 ,0x72, 0x73
	IRC_NULL,     IRC_NULL,       IRC_NULL,         IRC_NULL, //0x74, 0x75 ,0x76, 0x77
	IRC_NULL,     IRC_NULL,       IRC_NULL,         IRC_NULL, //0x78, 0x79 ,0x7a, 0x7b
	IRC_NULL,     IRC_NULL,               IRC_NULL,         IRC_NULL, //0x7c, 0x7d ,0x7e, 0x7f

	IRC_NULL,     IRC_NULL,                   IRC_OPTION,                IRC_RETURN, //0x80, 0x81 ,0x82, 0x83
	IRC_NULL,     IRC_NULL,           IRC_NULL,             IRC_NULL, //0x84, 0x85 ,0x86, 0x87
	IRC_NULL,       IRC_NULL,             IRC_NULL,       IRC_NULL, //0x88, 0x89 ,0x8a, 0x8b
	IRC_NULL,     IRC_NULL,                   IRC_NULL,           IRC_NULL, //0x8c, 0x8d ,0x8e, 0x8f

	IRC_NULL,     IRC_NULL,                   IRC_HOME,              IRC_NULL, //0x90, 0x91 ,0x92, 0x93
	IRC_NULL,     IRC_NULL,       IRC_NULL,     IRC_NULL, //0x94, 0x95 ,0x96, 0x97
	IRC_NULL,       IRC_NULL,             IRC_NULL,       IRC_NULL, //0x98, 0x99 ,0x9a, 0x9b
	IRC_NULL,     IRC_NULL,                   IRC_NULL,           IRC_NULL, //0x9c, 0x9d ,0x9e, 0x9f

	IRC_VOLUME,           IRC_NULL,                   IRC_NULL,           IRC_NULL, //0xa0, 0xa1 ,0xa2, 0xa3
	IRC_NULL,     IRC_NULL,       IRC_NULL,     IRC_NULL, //0xa4, 0xa5 ,0xa6, 0xa7
	IRC_NULL,       IRC_NULL,             IRC_NULL,       IRC_NULL, //0xa8, 0xa9 ,0xaa, 0xab
	IRC_NULL,     IRC_NULL,                   IRC_NULL,           IRC_NULL, //0xac, 0xad ,0xae, 0xaf

	IRC_NULL,     IRC_NULL,                   IRC_NULL,           IRC_NULL, //0xb0, 0xb1 ,0xb2, 0xb3
	IRC_NULL,     IRC_NULL,       IRC_NULL,     IRC_NULL, //0xb4, 0xb5 ,0xb6, 0xb7
	IRC_NULL,       IRC_NULL,             IRC_NULL,       IRC_NULL, //0xb8, 0xb9 ,0xba, 0xbb
	IRC_NULL,     IRC_NULL,                   IRC_NULL,           IRC_NULL, //0xbc, 0xbd ,0xbe, 0xbf

	IRC_NULL,     IRC_NULL,                   IRC_NULL,           IRC_NULL, //0xc0, 0xc1 ,0xc2, 0xc3
	IRC_NULL,     IRC_NULL,       IRC_NULL,     IRC_POWER, //0xc4, 0xc5 ,0xc6, 0xc7
	IRC_NULL,       IRC_NULL,             IRC_NULL,       IRC_NULL, //0xc8, 0xc9 ,0xca, 0xcb
	IRC_NULL,     IRC_NULL,                   IRC_NULL,           IRC_NULL, //0xcc, 0xcd ,0xce, 0xcf

	IRC_GOTO,     IRC_MENU,                    IRC_NULL,           IRC_NULL, //0xd0, 0xd1 ,0xd2, 0xd3
	IRC_NULL,     IRC_NULL,           IRC_NULL,       IRC_NULL, //0xd4, 0xd5 ,0xd6, 0xd7
	IRC_NULL,     IRC_NULL,           IRC_NULL,           IRC_NULL, //0xd8, 0xd9 ,0xda, 0xdb
	IRC_NULL,     IRC_NULL,                   IRC_NULL,           IRC_NULL, //0xdc, 0xdd ,0xde, 0xdf

	IRC_NULL,     IRC_NULL,                   IRC_NULL,           IRC_NULL, //0xe0, 0xe1 ,0xe2, 0xe3
	IRC_NULL,     IRC_NULL,       IRC_NULL,     IRC_NULL, //0xe4, 0xe5 ,0xe6, 0xe7
	IRC_KOK_REC,            IRC_NULL,             IRC_NULL,       IRC_NULL, //0xe8, 0xe9 ,0xea, 0xeb
	IRC_NULL,     IRC_NULL,                   IRC_NULL,           IRC_NULL, //0xec, 0xed ,0xee, 0xef

	IRC_NULL,     IRC_NULL,                   IRC_NULL,           IRC_NULL, //0xf0, 0xf1 ,0xf2, 0xf3
	IRC_NULL,     IRC_NULL,           IRC_NULL,       IRC_ZOOM, //0xf4, 0xf5 ,0xf6, 0xf7
	IRC_NULL,       IRC_NULL,             IRC_NULL,       IRC_NULL, //0xf8, 0xf9 ,0xfa, 0xfb
	IRC_NULL,     IRC_NULL,                   IRC_NULL,           IRC_NULL, //0xfc, 0xfd ,0xfe, 0xff
};
