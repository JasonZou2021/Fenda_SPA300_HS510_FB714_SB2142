/**************************************************************************/
/**
*
* \file	mcode.inc
*
* \brief	 for mcode make for load code addr
*
* \note  	Copyright (c) 2016 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   email@sunplus.com
* \version	 v0.01
* \date 	 2016/04/15
***************************************************************************/
//！需要用"Make Mcode"指令将些文件生成"mcode.i"来引用
//！第一个字符为'/',表示当前行为注释,不会被写到mcode.i文件中
//！第一个字符为'$',表示当前行存在变量,变量名在"()"内,
//！其名字一定要与ldp文件中的.drv_xxx中xxx部分一样,否则是无法匹配的
//

//code section

#define CODE_SECTION_ST		(0)

#define BASE_CODE_ST		(CODE_SECTION_ST)
$#define BASE_CODE_SIZE		(base)

#define FS_CODE_ST		(BASE_CODE_ST+BASE_CODE_SIZE)
$#define FS_CODE_SIZE		(fs)

#define AUD_CODE_ST		(FS_CODE_ST+FS_CODE_SIZE)
$#define AUD_CODE_SIZE		(aud)

#define AP_CODE_ST		(AUD_CODE_ST+AUD_CODE_SIZE)
$#define AP_CODE_SIZE		(ap)

#define AP_DEMUX_CODE_ST		(AP_CODE_ST+AP_CODE_SIZE)
$#define AP_DEMUX_CODE_SIZE		(ap_demux)

#define FL_CODE_ST		(AP_DEMUX_CODE_ST+AP_DEMUX_CODE_SIZE)
$#define FL_CODE_SIZE		(fl)

#define FL_ID3_CODE_ST		(FL_CODE_ST+FL_CODE_SIZE)
$#define FL_ID3_CODE_SIZE		(fl_id3)

#define BT_CODE_ST		(FL_ID3_CODE_ST+FL_ID3_CODE_SIZE)
$#define BT_CODE_SIZE		(bt)

#define NET_CODE_ST		(BT_CODE_ST+BT_CODE_SIZE)
$#define NET_CODE_SIZE		(net)

#define AIRPLAY_CODE_ST		(NET_CODE_ST+NET_CODE_SIZE)
$#define AIRPLAY_CODE_SIZE		(airplay)

#define DLNA_CODE_ST		(AIRPLAY_CODE_ST+AIRPLAY_CODE_SIZE)
$#define DLNA_CODE_SIZE		(dlna)

#define CLI_CODE_ST		(DLNA_CODE_ST+DLNA_CODE_SIZE)
$#define CLI_CODE_SIZE		(cli)

#define USER_CODE_ST		(CLI_CODE_ST+CLI_CODE_SIZE)
$#define USER_CODE_SIZE		(user)

#define OTHER_CODE_ST		(USER_CODE_ST+USER_CODE_SIZE)
$#define OTHER_CODE_SIZE		(other)


//total size. must place to lastly
$#define CODE_SECTION_SIZE   (total)


#define DATA_SECTION_ST	  	(CODE_SECTION_ST+CODE_SECTION_SIZE)
$#define DATA_SECTION_SIZE 	(data)
#define DATA_SECTION_END	(DATA_SECTION_ST + DATA_SECTION_SIZE)

