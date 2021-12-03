#ifndef __VFD_TM1640__H
#define __VFD_TM1640__H


#define TM1640_MEM_CACHE (0Xf)  // MAX has 0x10


#define VFD_7SEG_DOT1    	1 //VFD_7SEG_SM		
#define VFD_7SEG_DOT2   	7 //VFD_7SEG_SN	

typedef enum _eTM1640CmdMode
{
	// bit 7 & bit 6
	eDataCmdMode 			= (1<<6),
	eDisPlayControlMode 	= (1<<7),
	eAddressCmdSetMode 	= (1<<7) |(1<<6),
	
}_eCmdMode;


typedef enum _eTM1640DataCmdMode
{
	// bit 2

	eAddressAutoIncrease 	= (0<<2),
	eAddressfixation 		= (1<<2),

}_eDataCmdMode;


typedef enum _eTM1640AddressCmdSetMode
{
	// bit 3 ~ bit 0
	eAddr0 = 0,
	eAddr1,
	eAddr2,
	eAddr3,
	eAddr4,
	eAddr5,
	eAddr6,
	eAddr7,
	eAddr8,
	eAddr9,
	eAddrA,
	eAddrB,
	eAddrC,
	eAddrD,
	eAddrE,
	eAddrF,

}_eAddressCmdSetMode;



typedef enum _eTM1640DisPlayControlMode
{
	// bit 2~0
	ePulseWidth1 	= 0,
	ePulseWidth2 	= 1,
	ePulseWidth4 	= 2,
	ePulseWidth10	= 3,
	ePulseWidth11 	= 4,
	ePulseWidth12 	= 5,
	ePulseWidth13 	= 6,
	ePulseWidth14 	= 7,

	// bit 3
	eDisplayOn 	= (1<<3),
	eDisplayOff  	= (0<<3),

}_eDisPlayControlMode;


void vfd_test(void);

#endif

