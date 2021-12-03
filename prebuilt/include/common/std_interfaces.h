#ifndef __STD_INTERFACES_H__
#define __STD_INTERFACES_H__

#include <stdlib.h>
#include <stdarg.h>

#include "types.h"

#ifndef __RAND_MAX
	#define __RAND_MAX  0x7FFF
#endif

#define enable_print_counter (_REENT_PTR->_enable_print_counter)
#define max_print_counter (_REENT_PTR->_max_print_counter)
#define print_counter (_REENT_PTR->_print_counter)
#define no_str_tail	(_REENT_PTR->_no_str_tail)

extern const char	_ctype_[];
extern const union __dmath __infinity[];

struct timeval
{
	long tv_sec;
	long tv_usec;
};

struct timezone
{
	int tz_minuteswest;
	int tz_dsttime;
};

struct  itimerval
{
	struct  timeval it_interval;
	struct  timeval it_value;
};

void *mempcpy(void *out, const void *in, size_t n);

size_t strlcpy(char *dst, const char *src, size_t siz);

float strtof(const char *str, char **tail);

size_t strnlen(const char *str, size_t n);

unsigned int usleep(unsigned int useconds);

int gettimeofday(struct timeval *tv, struct timezone *tz);

void int64tostring(int64_t number, char *str);

void covertHex_withspec(int x, char s[], int spec);

#endif

