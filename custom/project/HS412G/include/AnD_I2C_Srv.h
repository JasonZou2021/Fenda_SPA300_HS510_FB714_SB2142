#ifndef __AnD_I2C_Srv__
#define __AnD_I2C_Srv__

#include "types.h"

typedef enum
{
	I2C_HIGH_V_OUTPUT,
	I2C_HIGH_V_INPUT,	

}I2C_HIGH_V;

typedef enum
{
	I2C_BYTE_HIGH_TO_LOW,
	I2C_BYTE_LOW_TO_HIGH,	
	
}I2C_BYTE_ENDIAN;

typedef enum
{	
	I2C_START_SIG_CONTINUE_UNSUPPORT,
	I2C_START_SIG_CONTINUE_SUPPORT,
	
}I2C_START_SIG;

typedef enum
{	
	I2C_READ_HAVE_REG,
	I2C_READ_NO_REG,
	
}I2C_READ_METHOD;

typedef enum
{	
	I2C_WR_BYTE_NORMAL,
	I2C_WR_BYTE_WAIT,
	
}I2C_WR_BYTE_METHOD;

typedef struct _i2c_cfg
{
	int clk;
	int dat;
	int delay; 
	int ack_delay; 
	I2C_HIGH_V high_voltage;
	I2C_BYTE_ENDIAN byte_type;
	I2C_START_SIG continue_start;
	I2C_READ_METHOD read_method;
	I2C_WR_BYTE_METHOD wr_method;
	
}I2C_CFG;

typedef enum _eI2CSWOperatorStatus
{
	eI2COK,
	eI2CFail,
}eOpStat;


int I2c_IO_Init(I2C_CFG *cfg);
int I2c_IO_ReInit(I2C_CFG *cfg);
I2C_CFG *I2C_GetSrv(int i);
int I2c_IO_Get_Cfg(int fd, I2C_CFG *cfg);
int I2c_IO_Check_Used(I2C_CFG *cfg);

eOpStat AnD_I2CSWWrite(int fd, int iDeviceAddr, int iAddr, BYTE *bBuffer, int len);
eOpStat AnD_I2CSWRead(int fd, int iDeviceAddr, int iAddr, UINT8 *bData, int len);

#endif

