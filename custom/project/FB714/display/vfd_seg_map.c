#include "types.h"
#include "vfd_seg_map.h"
#include "cfg_sdk.h"
#include <string.h>

extern VFD_CFG vfd_cfg;
UINT16 _VFD_SEG[14];

/*
** Definition for 7-seg display
**
** we assume 7-seg are defined as traditional 7-segment LED.
**
**	 AAAA			
**	C    B				 
**	C    B				 
**	 DDDD				 
**	F    E				 
**	F    E				 
**	 GGGG				 
**					 
**					 

*/
static UINT16 vfd_seg_digit_table[10];
static UINT16 vfd_seg_char_table[26];
static UINT16 vfd_seg_char_lowercase_table[26]; 

static void vfd_7seg_set_digit_table(void)
{
	
/*0*/	vfd_seg_digit_table[0] = (VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SG|VFD_7SEG_SC|VFD_7SEG_SE|VFD_7SEG_SF);
/*1*/	vfd_seg_digit_table[1] = (VFD_7SEG_SB|VFD_7SEG_SE);
/*2*/	vfd_seg_digit_table[2] = (VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SD|VFD_7SEG_SF|VFD_7SEG_SG);
/*3*/	vfd_seg_digit_table[3] = (VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SG);
/*4*/	vfd_seg_digit_table[4] = (VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SB|VFD_7SEG_SE);
/*5*/	vfd_seg_digit_table[5] = (VFD_7SEG_SA|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SG);
/*6*/	vfd_seg_digit_table[6] = (VFD_7SEG_SA|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SF|VFD_7SEG_SE|VFD_7SEG_SG);
/*7*/	vfd_seg_digit_table[7] = (VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SE);
/*8*/	vfd_seg_digit_table[8] = (VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SG);
/*9*/	vfd_seg_digit_table[9] = (VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SG);
};



static void vfd_7seg_set_char_table(void)
{
/*A*/	vfd_seg_char_table[0] = (VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SD);
/*B*/	vfd_seg_char_table[1] = (VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SG);
/*C*/	vfd_seg_char_table[2] = (VFD_7SEG_SA|VFD_7SEG_SC|VFD_7SEG_SF|VFD_7SEG_SG);
/*D*/	vfd_seg_char_table[3] = (VFD_7SEG_SB|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SG);
/*E*/	vfd_seg_char_table[4] = (VFD_7SEG_SA|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SF|VFD_7SEG_SG);
/*F*/	vfd_seg_char_table[5] = (VFD_7SEG_SA|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SF);
/*G*/	vfd_seg_char_table[6] = (VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SG);
/*H*/	vfd_seg_char_table[7] = (VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SF|VFD_7SEG_SE);
/*I*/	vfd_seg_char_table[8] = (VFD_7SEG_SB|VFD_7SEG_SE);
/*J*/	vfd_seg_char_table[9] = (VFD_7SEG_SB|VFD_7SEG_SE|VFD_7SEG_SG|VFD_7SEG_SF);
/*K*/	vfd_seg_char_table[10] = (VFD_7SEG_SB|VFD_7SEG_SE|VFD_7SEG_SG|VFD_7SEG_SF);
/*L*/	vfd_seg_char_table[11] = (VFD_7SEG_SC|VFD_7SEG_SF|VFD_7SEG_SG);
/*M*/	vfd_seg_char_table[12] = (VFD_7SEG_SF|VFD_7SEG_SD|VFD_7SEG_SE);
/*N*/	vfd_seg_char_table[13] = (VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SE|VFD_7SEG_SF);
/*O*/	vfd_seg_char_table[14] = (VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SG|VFD_7SEG_SC|VFD_7SEG_SE|VFD_7SEG_SF);
/*P*/	vfd_seg_char_table[15] = (VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SF);
/*Q*/	vfd_seg_char_table[16] = (VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SF);
/*R*/	vfd_seg_char_table[17] = (VFD_7SEG_SD|VFD_7SEG_SF);
/*S*/	vfd_seg_char_table[18] = (VFD_7SEG_SA|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SG);
/*T*/	vfd_seg_char_table[19] = (VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SF|VFD_7SEG_SG);
/*U*/	vfd_seg_char_table[20] = (VFD_7SEG_SC|VFD_7SEG_SB|VFD_7SEG_SF|VFD_7SEG_SE|VFD_7SEG_SG);
/*V*/	vfd_seg_char_table[21] = (VFD_7SEG_SC|VFD_7SEG_SB|VFD_7SEG_SF|VFD_7SEG_SE|VFD_7SEG_SG);
/*W*/	vfd_seg_char_table[22] = (VFD_7SEG_SC|VFD_7SEG_SB|VFD_7SEG_SF|VFD_7SEG_SE|VFD_7SEG_SG);
/*X*/	vfd_seg_char_table[23] = (VFD_7SEG_SC|VFD_7SEG_SB|VFD_7SEG_SF|VFD_7SEG_SE|VFD_7SEG_SG);
/*Y*/	vfd_seg_char_table[24] = (VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SB|VFD_7SEG_SE|VFD_7SEG_SG);
/*Z*/	vfd_seg_char_table[25] = (VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SD|VFD_7SEG_SF|VFD_7SEG_SG);

};


static void vfd_7seg_set_char_lowercase_table(void)
{
/*a*/	vfd_seg_char_lowercase_table[0] = (VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SD);
/*b*/	vfd_seg_char_lowercase_table[1] = (VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SG);
/*c*/	vfd_seg_char_lowercase_table[2] = (VFD_7SEG_SA|VFD_7SEG_SC|VFD_7SEG_SF|VFD_7SEG_SG);
/*d*/	vfd_seg_char_lowercase_table[3] = (VFD_7SEG_SB|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SG);
/*e*/	vfd_seg_char_lowercase_table[4] = (VFD_7SEG_SA|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SF|VFD_7SEG_SG);
/*f*/ 	vfd_seg_char_lowercase_table[5] = (VFD_7SEG_SA|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SF);
/*g*/	vfd_seg_char_lowercase_table[6] = (VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SG);
/*h*/	vfd_seg_char_lowercase_table[7] = (VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SF|VFD_7SEG_SE);
/*i*/	vfd_seg_char_lowercase_table[8] = (VFD_7SEG_SE);
/*j*/ 	vfd_seg_char_lowercase_table[9] = (VFD_7SEG_SB|VFD_7SEG_SE|VFD_7SEG_SG|VFD_7SEG_SF);
/*k*/	vfd_seg_char_lowercase_table[10] =(VFD_7SEG_SB|VFD_7SEG_SE|VFD_7SEG_SG|VFD_7SEG_SF);
/*l*/ 	vfd_seg_char_lowercase_table[11] =(VFD_7SEG_SC|VFD_7SEG_SF|VFD_7SEG_SG);
/*m*/	vfd_seg_char_lowercase_table[12] =(VFD_7SEG_SF|VFD_7SEG_SD|VFD_7SEG_SE);
/*n*/	vfd_seg_char_lowercase_table[13] =(VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SE|VFD_7SEG_SF);
/*o*/	vfd_seg_char_lowercase_table[14] =(VFD_7SEG_SF|VFD_7SEG_SD|VFD_7SEG_SG|VFD_7SEG_SE);
/*p*/	vfd_seg_char_lowercase_table[15] =(VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SF);
/*q*/	vfd_seg_char_lowercase_table[16] =(VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SF);
/*r*/	vfd_seg_char_lowercase_table[17] =(VFD_7SEG_SD|VFD_7SEG_SF);
/*s*/	vfd_seg_char_lowercase_table[18] =(VFD_7SEG_SA|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SG);
/*t*/ 	vfd_seg_char_lowercase_table[19] =(VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SF|VFD_7SEG_SG);
/*u*/	vfd_seg_char_lowercase_table[20] =(VFD_7SEG_SC|VFD_7SEG_SB|VFD_7SEG_SF|VFD_7SEG_SE|VFD_7SEG_SG);
/*v*/	vfd_seg_char_lowercase_table[21] =(VFD_7SEG_SC|VFD_7SEG_SB|VFD_7SEG_SF|VFD_7SEG_SE|VFD_7SEG_SG);
/*w*/	vfd_seg_char_lowercase_table[22] =(VFD_7SEG_SC|VFD_7SEG_SB|VFD_7SEG_SF|VFD_7SEG_SE|VFD_7SEG_SG);
/*x*/	vfd_seg_char_lowercase_table[23] =(VFD_7SEG_SC|VFD_7SEG_SB|VFD_7SEG_SF|VFD_7SEG_SE|VFD_7SEG_SG);
/*y*/	vfd_seg_char_lowercase_table[24] =(VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SB|VFD_7SEG_SE|VFD_7SEG_SG);
/*z*/	vfd_seg_char_lowercase_table[25] =(VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SD|VFD_7SEG_SF|VFD_7SEG_SG);

};



/*
** Definition for 14-seg display
**
** we assume 7-seg are defined as traditional 14-segment LED.
**
**	   AAAAAAAAA			
**	F  H     I     J    B				 
**	F    H   I  J       B				 
**	   GGG    KKKK			 
**	E     N  M  L     C				 
**	E  N     M     L  C				 
**     DDDDDDDDD				 
**					 
**	
*/


static void vfd_14seg_set_digit_table(void)
{
/*0*/	vfd_seg_digit_table[0] =(VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SF);
/*1*/	vfd_seg_digit_table[1] =(VFD_7SEG_SB|VFD_7SEG_SC);
/*2*/	vfd_seg_digit_table[2] =(VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SG|VFD_7SEG_SE|VFD_7SEG_SD|VFD_7SEG_SK);
/*3*/	vfd_seg_digit_table[3] =(VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SG|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SK);
/*4*/	vfd_seg_digit_table[4] =(VFD_7SEG_SF|VFD_7SEG_SG|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SK);
/*5*/	vfd_seg_digit_table[5] =(VFD_7SEG_SA|VFD_7SEG_SF|VFD_7SEG_SG|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SK);
/*6*/	vfd_seg_digit_table[6] =(VFD_7SEG_SA|VFD_7SEG_SF|VFD_7SEG_SG|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SK);		
/*7*/	vfd_seg_digit_table[7] =(VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC);
/*8*/	vfd_seg_digit_table[8] =(VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SG|VFD_7SEG_SK);
/*9*/	vfd_seg_digit_table[9] =(VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SF|VFD_7SEG_SG|VFD_7SEG_SK);
};

static void vfd_14seg_set_char_table(void)
{
/*A*/	vfd_seg_char_table[0] = (VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SG|VFD_7SEG_SK);
/*B*/	vfd_seg_char_table[1] = (VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SI|VFD_7SEG_SK|VFD_7SEG_SM);
/*C*/	vfd_seg_char_table[2] = (VFD_7SEG_SA|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SF);
/*D*/	vfd_seg_char_table[3] = (VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SI|VFD_7SEG_SM);
/*E*/	vfd_seg_char_table[4] = (VFD_7SEG_SA|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SG|VFD_7SEG_SK);
/*F*/	vfd_seg_char_table[5] = (VFD_7SEG_SA|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SG|VFD_7SEG_SK);
/*G*/	vfd_seg_char_table[6] = (VFD_7SEG_SA|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SK);
/*H*/	vfd_seg_char_table[7] = (VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SG|VFD_7SEG_SK);
/*I*/	vfd_seg_char_table[8] = (VFD_7SEG_SA|VFD_7SEG_SD|VFD_7SEG_SI|VFD_7SEG_SM);
/*J*/	vfd_seg_char_table[9] = (VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SD);
/*K*/	vfd_seg_char_table[10] =(VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SG|VFD_7SEG_SJ|VFD_7SEG_SL);
/*L*/	vfd_seg_char_table[11] =(VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SF);
/*M*/	vfd_seg_char_table[12] =(VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SH|VFD_7SEG_SJ);
/*N*/	vfd_seg_char_table[13] =(VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SH|VFD_7SEG_SL);
/*O*/	vfd_seg_char_table[14] =(VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SF);
/*P*/	vfd_seg_char_table[15] =(VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SG|VFD_7SEG_SK);
/*Q*/	vfd_seg_char_table[16] =(VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SL);
/*R*/	vfd_seg_char_table[17] =(VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SG|VFD_7SEG_SK|VFD_7SEG_SL);
/*S*/	vfd_seg_char_table[18] =(VFD_7SEG_SA|VFD_7SEG_SG|VFD_7SEG_SF/*VFD_7SEG_SH*/|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SK);
/*T*/	vfd_seg_char_table[19] =(VFD_7SEG_SA|VFD_7SEG_SI|VFD_7SEG_SM);
/*U*/	vfd_seg_char_table[20] =(VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SF);
/*V*/	vfd_seg_char_table[21] =(VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SN|VFD_7SEG_SJ);
/*W*/	vfd_seg_char_table[22] =(VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SN|VFD_7SEG_SL);
/*X*/	vfd_seg_char_table[23] =(VFD_7SEG_SH|VFD_7SEG_SJ|VFD_7SEG_SL|VFD_7SEG_SN);
/*Y*/	vfd_seg_char_table[24] =(VFD_7SEG_SH|VFD_7SEG_SJ|VFD_7SEG_SM);
/*Z*/	vfd_seg_char_table[25] =(VFD_7SEG_SA|VFD_7SEG_SD|VFD_7SEG_SJ|VFD_7SEG_SN);
};


static void vfd_14seg_set_char_lowercase_table(void)
{
/*a*/   	vfd_seg_char_lowercase_table[0] = (VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SG|VFD_7SEG_SK);
/*b*/   	vfd_seg_char_lowercase_table[1] = (VFD_7SEG_SF|VFD_7SEG_SG|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SE);
/*c*/	vfd_seg_char_lowercase_table[2] = (VFD_7SEG_SG|VFD_7SEG_SE|VFD_7SEG_SD);
/*d*/   	vfd_seg_char_lowercase_table[3] = (VFD_7SEG_SB|VFD_7SEG_SG|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SE);
/*e*/   	vfd_seg_char_lowercase_table[4] = (VFD_7SEG_SA|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SG|VFD_7SEG_SK);
/*f*/   	vfd_seg_char_lowercase_table[5] = (VFD_7SEG_SA|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SG|VFD_7SEG_SK);
/*g*/   	vfd_seg_char_lowercase_table[6] = (VFD_7SEG_SA|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SK);
/*h*/   	vfd_seg_char_lowercase_table[7] = (VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SG|VFD_7SEG_SK);
/*i*/	vfd_seg_char_lowercase_table[8] = (VFD_7SEG_SC),
/*j*/  	vfd_seg_char_lowercase_table[9] = (VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SD);
/*k*/   	vfd_seg_char_lowercase_table[10] =(VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SG|VFD_7SEG_SJ|VFD_7SEG_SL);
/*l*/ 	vfd_seg_char_lowercase_table[11] =(VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SF);
/*m*/   	vfd_seg_char_lowercase_table[12] =(VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SH|VFD_7SEG_SJ);
/*n*/	vfd_seg_char_lowercase_table[13] =(VFD_7SEG_SG|VFD_7SEG_SC|VFD_7SEG_SE);
/*o*/   	vfd_seg_char_lowercase_table[14] =(VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SG|VFD_7SEG_SK);
/*p*/   	vfd_seg_char_lowercase_table[15] =(VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SG|VFD_7SEG_SK);
/*q*/   	vfd_seg_char_lowercase_table[16] =(VFD_7SEG_SA|VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SL);
/*r*/  	vfd_seg_char_lowercase_table[17] =(VFD_7SEG_SE|VFD_7SEG_SN|VFD_7SEG_SK);
/*s*/   	vfd_seg_char_lowercase_table[18] =(VFD_7SEG_SA|VFD_7SEG_SG|VFD_7SEG_SF/*VFD_7SEG_SH*/|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SK);
/*t*/  	vfd_seg_char_lowercase_table[19] =(VFD_7SEG_SE|VFD_7SEG_SD|VFD_7SEG_SF|VFD_7SEG_SG);
/*u*/   	vfd_seg_char_lowercase_table[20] =(VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SD|VFD_7SEG_SE|VFD_7SEG_SF);
/*v*/   	vfd_seg_char_lowercase_table[21] =(VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SN|VFD_7SEG_SJ);
/*w*/    	vfd_seg_char_lowercase_table[22] =(VFD_7SEG_SB|VFD_7SEG_SC|VFD_7SEG_SE|VFD_7SEG_SF|VFD_7SEG_SN|VFD_7SEG_SL);
/*x*/   	vfd_seg_char_lowercase_table[23] =(VFD_7SEG_SH|VFD_7SEG_SJ|VFD_7SEG_SL|VFD_7SEG_SN);
/*y*/    	vfd_seg_char_lowercase_table[24] =(VFD_7SEG_SH|VFD_7SEG_SJ|VFD_7SEG_SM);
/*z*/   	vfd_seg_char_lowercase_table[25] =(VFD_7SEG_SA|VFD_7SEG_SD|VFD_7SEG_SJ|VFD_7SEG_SN);
};


static VFD_SEG_MAP seg_map = 
{
	.digit_tbl = vfd_seg_digit_table,
	.char_tbl = vfd_seg_char_table,
	.char_lowercase_tbl = vfd_seg_char_lowercase_table,
};



void vfd_set_seg_table(void)
{
	VFD_CFG *cust_cfg = &vfd_cfg;

	if(cust_cfg->vfd_func == NULL)
		return;
	
	memcpy(_VFD_SEG, cust_cfg->vfd_func->seg_bit_tbl, cust_cfg->vfd_func->seg_bit_tbl_size);

	if(cust_cfg->seg_tbl == VFD_7SEG_TBL)
	{
		vfd_7seg_set_digit_table();
		vfd_7seg_set_char_table();
		vfd_7seg_set_char_lowercase_table();
	}
	else
	{
		vfd_14seg_set_digit_table();
		vfd_14seg_set_char_table();
		vfd_14seg_set_char_lowercase_table();
	}
}


VFD_SEG_MAP *vfd_get_seg_table(void)
{
	return &seg_map;
}


