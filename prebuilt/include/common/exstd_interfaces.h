#ifndef __EXSTD_INTERFACES_H__
#define __EXSTD_INTERFACES_H__

extern const char bCodeVersion[];
#define GetCodeVersion()	bCodeVersion
extern const char bCurrBranch[];
#define GetCurrBranch()		bCurrBranch

unsigned int msleep(unsigned int mseconds);

unsigned int Convert2HexNumber(char *bpHexString, unsigned int *dpNumber);

unsigned int Convert2DecimalNumer(char *bpString, unsigned int *dpNumber);

int Convert2DecimalNumer2(char *bpString, int *dpNumber);

int DumpRegs(unsigned int const *address, const int count);

int DumpMem(const char *address, const int count);

void DumpTaskBacktrace(void);

extern void ReStartSystem(void);

/******************************************************************************************/
/**
 * \fn		void FullEnvDelayMs(unsigned int sdMSec)
 *
 * \brief		Delay current task sdMSec in all the Envirment which in ISR or task and no matter the OS is ready
 *
 * \param		sdMSec:the mSec of delay
 *
 * \return	none
 *
 ******************************************************************************************/
extern void FullEnvDelayMs(unsigned int sdMSec);

#endif

