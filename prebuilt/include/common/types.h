/**************************************************************************/
/**
*
* \file	types.h
*
* \brief	 define types
*
* \note  	Copyright (c) 2016 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   email@sunplus.com
* \version	 v0.01
* \date 	 2016/04/15
***************************************************************************/
#ifndef __TYPES_H
#define __TYPES_H


#ifndef __GNUC__
	#define __attribute__(x)
	#define __alignof__(x)          sizeof(x)
#endif

//
// standard types
//
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short HWORD;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;

#define W64     long long
#define W32     int
#define W16     short
#define W8      char

typedef W64 INT64;
typedef W32 INT32;
typedef W16 INT16;
typedef signed W8 INT8;

typedef unsigned W64 UINT64;
typedef unsigned W32 UINT32;
typedef unsigned W16 UINT16;
typedef unsigned W8 UINT8;

//typedef UINT32 uint;

//FreeRTOS used typedef
/* Exact-width types. WG14/N843 C99 Standard, Section 7.18.1.1 */
typedef signed char        int8_t;    /*!< exact-width  8-bit signed   int  */
typedef signed short       int16_t;   /*!< exact-width 16-bit signed   int  */
typedef signed long        int32_t;   /*!< exact-width 32-bit signed   int  */
typedef signed long long   int64_t;   /*!< exact-width 64-bit signed   int  */

typedef unsigned char      uint8_t;   /*!< exact-width  8-bit unsigned int  */
typedef unsigned short     uint16_t;  /*!< exact-width 16-bit unsigned int  */
typedef unsigned long      uint32_t;  /*!< exact-width 32-bit unsigned int  */
typedef unsigned long long uint64_t;  /*!< exact-width 64-bit unsigned int  */

/* Fastest minimum-width types. WG14/N843 C99 Standard, Section 7.18.1.3 */
typedef signed   int  int_fast8_t;   /*!< fast at-least  8-bit signed   int */
typedef unsigned int  uint_fast8_t;  /*!< fast at-least  8-bit unsigned int */
typedef signed   int  int_fast16_t;  /*!< fast at-least 16-bit signed   int */
typedef unsigned int  uint_fast16_t; /*!< fast at-least 16-bit unsigned int */
typedef signed   long int_fast32_t;  /*!< fast at-least 32-bit signed   int */
typedef unsigned long uint_fast32_t; /*!< fast at-least 32-bit unsigned int */

typedef void *PVOID;
typedef void (*VFUNC)();

#ifndef __cplusplus

	typedef int   bool;     /*!< standard Boolean data type */
	#define true  ((bool)1) /*!< standard 'true' constant */
	#define false ((bool)0) /*!< standard 'false' constant */

#endif /* !__cplusplus */

#define __bool_true_false_are_defined 1

#define CPPDECLARE __cplusplus
#define CPPBEGIN \
	#ifdef __cplusplus \
	extern "C" { \
	#endif

#define CPPEND \
	#ifdef __cplusplus \
	} \
	#endif


// attribute
#define __UNUSED        __attribute__((unused))
#define __PACKED        __attribute__((packed))
#define __ALIGNED(n)    __attribute__((aligned(n)))
#define __WEAK          __attribute__((weak))

/* bcd */
#define getBCD(p)       (((((UINT8 *)(p))[0]) >> 4) * 10 + ((((UINT8 *)(p))[0]) & 0x0f))

//
// ...
#define getUINT16_bi(p) ((UINT16) (((UINT8 *)(p))[1] | ((UINT16)((UINT8 *)(p))[0] << 8)))
#define getUINT16_li(p) ((UINT16) (((UINT8 *)(p))[0] | ((UINT16)((UINT8 *)(p))[1] << 8)))

#define getUINT24_bi(p) ((UINT32) (((UINT8 *)(p))[2] | ((UINT32)((UINT8 *)(p))[1] << 8) | ((UINT32)((UINT8 *)(p))[0] << 16)))

#define getUINT32_bi(p) ((UINT32) (((UINT8 *)(p))[3] | ((UINT32)((UINT8 *)(p))[2] << 8) | ((UINT32)((UINT8 *)(p))[1] << 16) | ((UINT32)((UINT8 *)(p))[0] << 24)))
#define getUINT32_li(p) ((UINT32) (((UINT8 *)(p))[0] | ((UINT32)((UINT8 *)(p))[1] << 8) | ((UINT32)((UINT8 *)(p))[2] << 16) | ((UINT32)((UINT8 *)(p))[3] << 24)))

//
// endian support
//
//#include "hw\endian.h"

#undef  getUINT16_li
#undef  getUINT16_bi
#undef  getUINT32_li
#undef  getUINT32_bi

#define getUINT16_li    lle16
#define getUINT16_bi    lbe16
#define getUINT32_li    lle32
#define getUINT32_bi    lbe32

#define getUINT64_bi(p) ((((UINT64) getUINT32_bi(p)) << 32) | ((UINT64) getUINT32_bi(p + 4)))
#define getUINT64_li(p) ((((UINT64) getUINT32_li(p)) << 32) | ((UINT64) getUINT32_li(p + 4)))

//#define BIG_ENDIAN

/* endian-less taste */
#ifdef  BIG_ENDIAN
	#define getUINT16(p)    getUINT16_bi((BYTE *)p)
	#define getUINT32(p)    getUINT32_bi((BYTE *)p)
#else
	#define getUINT16(p)    getUINT16_li((BYTE *)p)
	#define getUINT32(p)    getUINT32_li((BYTE *)p)
#endif

#define getBiUINT32(p)  getUINT32_li((BYTE *)p)
#define getBiUINT16(p)  getUINT16_li((BYTE *)p)

#ifndef  TRUE
	#define  TRUE  1
#endif

#ifndef  FALSE
	#define  FALSE 0
#endif

#ifndef  SUCCESS
	#define SUCCESS (0)
#endif

#ifndef FAIL
	#define FAIL (-1)
#endif


#define IS_B7_ONE(p)        ( ((p) & 0x80) == 0x80)
#define IS_B7_ZERO(p)       ( ((p) & 0x80) == 0x00)

#define GET_LOW_NIBBLE(p)   ((p) & 0x0F)

#ifndef NULL
	#define NULL    ((void *)0)
#endif

#ifndef MIN
#define MIN(a, b)\
	({__typeof__ (a) _a = (a);\
	__typeof__ (b) _b = (b);\
	_a < _b? _a: _b;})
#endif

#ifndef MAX
#define MAX(a, b)\
	({__typeof__ (a) _a = (a);\
		__typeof__ (b) _b = (b);\
		_a > _b? _a: _b;})
#endif


#define BITS_PER_BYTE 8
#define CONTAIN_BITS(type) (sizeof(type)*BITS_PER_BYTE)

#define MAX_NUM(type) ((type)(((type)-1)>(type)0 ? \
							  (type)-1: \
							  ((1<<(CONTAIN_BITS(type)-2))-1+(1<<(CONTAIN_BITS(type)-2)))))

#define MIN_NUM(type)  ((type)(((type)-1)>0 ? \
							   0: \
							   (type)(-((UINT32)(1<<(CONTAIN_BITS(type)-1))))))


#define WORD_MINUS(a,b)		((a) > (b) ? ((a) - (b)) : ((0xffffffff - (b)) + (a) +1))

typedef void *(*ThreadFunction_t)( void * );

#endif /*__TYPES_H*/

