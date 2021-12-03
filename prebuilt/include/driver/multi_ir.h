/******************************************************************************

   Copyright (c) 2016 Sunplus technology co. Ltd.All right reserved!

******************************************************************************/
#ifndef _IR_MULTI_H
#define _IR_MULTI_H

#define REPEAT 2 //for return repeat flag
#define IR_FORMAT_MONKEY_TEST (0xff)
typedef struct
{
	int IR_Fmt;
	int repeat_flag;
	int ready_flag;
	UINT32 CustomerCode;
	UINT16 KeyCode;
	UINT8 product_data;
} stIR_Info_t;

typedef enum
{
	IR_FORMAT_TC9012 = 0,
	IR_FORMAT_UPD6121,
	IR_FORMAT_M50560,
	IR_FORMAT_MATC6D6,
	IR_FORMAT_SONYD7C5,
	IR_FORMAT_SONYD7C8,
	IR_FORMAT_AHEA48,
	IR_FORMAT_MATC5D6,
	IR_FORMAT_RC6,
	IR_FORMAT_SHARP,
	IR_FORMAT_SAMSUNG,

	IR_FORMAT_MAX,
} enum_Ir_Format;


int CheckMultiIR(UINT8 MultiIrFmt, stIR_Info_t *pstIR_Info);

int CheckMultiIR_WithRawData(UINT8 MultiIrFmt, stIR_Info_t *pstIR_Info, UINT8* pIR_array_temp);

#endif


