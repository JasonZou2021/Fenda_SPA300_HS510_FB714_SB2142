#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "fsi_api.h"
#include "busy_delay.h"
#include "exstd_interfaces.h"
#define LOG_MODULE_ID  ID_APUSR
#define LOG_EXTRA_STR  "[ITE]"
#include "log_utils.h"
#include "hdmi_app.h"
//#include "../IO_IT6350.h"
//#include "global.h"

//#include "IAP_ControllerBase.h"
//#include "IAP_Controller_IteDp8051.h"

#define S_OK		0


static BYTE *bin;

#define IT8313_MAX_CODE_SIZE    0xFC00
#define IT8313_SECTOR_SIZE      0x400


typedef unsigned char UCHAR;
typedef unsigned char  ULONG;


BOOL IT6622_Write(BYTE addr, BYTE reg, int len, BYTE *bBuf);
BOOL IT6622_Read(BYTE addr, BYTE reg, int len, BYTE *buf);
int Vfd_Set_Str(const char *s);

#define i2c_write_byte(addr, u8Offset, u16Len, u8Buf)	IT6622_Write(addr, u8Offset, u16Len, u8Buf)
#define i2c_read_byte(addr, u8Offset, u16Len, u8Buf)	IT6622_Read(addr, u8Offset, u16Len, u8Buf)

typedef struct
{
	unsigned char m_eraseBootCode;    //=>是否要Erase boot code
	unsigned char m_fullyCompare;     //=>是否將Page讀出來重新compare一次
	unsigned char m_codeLength;       //=>寫進去的Size
	unsigned char *m_codeBuffer;       //=>Buffer的位置
	unsigned char m_chipRev;          // =>Chip的版本
}DEV_PRIVATE;

typedef struct
{
	unsigned char ISP_I2cAddr;
	unsigned char IAP_Length;
	unsigned short PageSize;
	unsigned char IAP_I2cAddr;
	unsigned char IAP_TagLength;
	unsigned char IAP_Tag[16];


}IAP_CONFIG;

//bool Isp_ReadPage( int PageOffset, int ByteOffset, void *Buffer, int Length );
//extern UCHAR i2c_block_dma_read(UCHAR ucCH, UCHAR ucSlaveID, UCHAR* pInputBuf, UCHAR ucWCount, UCHAR* pOutputBuf, UCHAR ucRCount);
//extern UCHAR i2c_block_dma_write(UCHAR ucCH,UCHAR ucSlaveID, UCHAR ucAddr, UCHAR* pInputBuf, UCHAR ucWCount);

bool Isp_ReadPage( int PageOffset, int ByteOffset, void *Buffer, int Length );

static int get_file_size(const char* file) 
{
    struct _sTat tbuf;
    stat(file, &tbuf);
    return tbuf.st_size;
}


int WriteI2CBuf( unsigned char dev, unsigned char address, unsigned char *p_data, unsigned char byteno, unsigned char flag)
//int i2c_write_byte( iTE_u8 address, iTE_u8 offset, iTE_u8 byteno, iTE_u8 *p_data, iTE_u8 device )
{
  //unsigned char device = 1;
	//if(i2c_block_dma_write(device, address, p_data[0], &p_data[1], byteno-1) )
	dev = dev;
  flag = flag;
  
		if(i2c_write_byte(address, p_data[0],  byteno-1 ,&p_data[1]))
		{
		//iTE_Msg(("***************ERROR: i2c_write_byte fail********************\r\r\n"));
		//iTE_Msg(("device = 0x%02X, addr=0x%02X, offset=0x%02X, num=0x%02X, data=0x%02X\r\n", device, address, offset, byteno, *p_data));
		return 1;
	 }else{
		return 0;
	 }
}

//result = ReadI2CBuf2( dev, i2cAddr, cmd[0], &reg0def[0], 3, flag ); // enable ISP mode
int ReadI2CBuf2( unsigned char dev, unsigned char address, unsigned char offset, unsigned char *p_data, unsigned char byteno, unsigned char flag)
//int i2c_write_byte( iTE_u8 address, iTE_u8 offset, iTE_u8 byteno, iTE_u8 *p_data, iTE_u8 device )
{
//    unsigned char device = 1;
	dev = dev;
  flag = flag;
	//if(i2c_block_dma_read(device, address, &offset, 1, p_data, byteno))
	if(i2c_read_byte(address, offset, byteno, p_data))
	{
		//iTE_Msg(("***************ERROR: i2c_read_byte fail********************\r\r\n"));
		//iTE_Msg(("device = 0x%02X, addr=0x%02X, offset=0x%02X, num=0x%02X\r\n", device, address, offset, byteno));
//		while(1);
		return 1;
	}else{
		return 0;
	}
}

static DEV_PRIVATE s_devPrv[1];
DEV_PRIVATE *devPrv = s_devPrv;

static IAP_CONFIG IAPConfig_IteDp8051[1] =
{{
    0xB8,  // SSP  address
    1,     // Max I/O data length
    256,   // Page size
    0xB8,  // IAP trigger I2c address
    6,     // SSP trigger tag length
    {0xff, 0xff, 0x96, 0x5A, 0xE7, 0x18, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15} // SSP tag
}};

IAP_CONFIG *gIAP = IAPConfig_IteDp8051;

static UCHAR ISPAddrs[6] = { 0xf2, 0xf2, 0x37, 0xf2, 0x5e, 0xd4 };
unsigned char DevIndex = 0;

int ITE6622_UPG_DISPLAY(BYTE vol)
{
	BYTE strBuf[10] = {0};

	if(vol == HDMI_UPG_OK)
	{
		sprintf(strBuf, "HD OK");
		//QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, POWER_RELEASE_SIG), NULL);
	}
	else if(vol == HDMI_UPG_NG)
	{
		sprintf(strBuf, "HD NG");
	}
	else if(vol == 100)
	{
		sprintf(strBuf, "HD%03d", vol);
	}
	else if(vol > 9)
	{
		sprintf(strBuf, "HD %02d", vol);
	}
	else
	{
		sprintf(strBuf, "HD  %01d", vol);
	}

	Vfd_Set_Str(strBuf);

	return 0;

}

#if 1
void Sleep(ULONG ms)
{
    delay_1ms(ms);
}

ULONG GetTickCount(void)
{
    return 0;
}


bool ClearReadFifo( void )
{
//////  reg16, bit6
#if 1
    unsigned char cmd[8];
    unsigned char i2cAddr;
    //unsigned char regTmp;
    unsigned long flag;
    unsigned long result;
    unsigned long dev;
    bool ret = false;

    //_TRACE(("%s++", __FUNC__);
    dev = DevIndex;
    flag = 0;
    i2cAddr = gIAP->IAP_I2cAddr;

    cmd[0]=0x16;
    cmd[1]=0x40;
    result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
    if ( S_OK == result )
    {
        cmd[0]=0x16;
        cmd[1]=0x0;
        result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
        if ( S_OK == result )
        {
        ret = true;
        }
    }
    else
    {
//        _TRACE(("%s : Fail (result=%d)\r\n", __FUNC__, result));
    }

    return ret;
#endif
/////
}

bool SetStartOffset( unsigned long Offset )
{
    unsigned char cmd[8];
    unsigned char i2cAddr;
    //unsigned char regTmp;
    unsigned long flag;
    unsigned long result;
    unsigned long dev;
    bool ret = false;
    int i;

    unsigned long offset;
    LOGD(" Offset %x  \n",Offset);
    //_TRACE(("%s++", __FUNC__);
    dev = DevIndex;
    flag = 0;
    i2cAddr = gIAP->IAP_I2cAddr;
    offset = Offset & 0x00FFFFFF;

    for( i=0 ; i<3 ; i++ )
    {
        cmd[0] = 0x0f-i;
        cmd[1] = (unsigned char)(offset & 0xFF);

        result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
        if ( S_OK == result )
        {
            ret = true;
            offset >>= 8;
            continue;
        }
        break;
    }

    return ret;
}
bool SetByteCount( int ByteCount )
{
    unsigned char cmd[8];
    unsigned char i2cAddr;
    //unsigned char regTmp;
    unsigned long flag;
    unsigned long result;
    unsigned long dev;
    bool ret = false;

    //unsigned long offset;

    //_TRACE(("%s++", __FUNC__);
    dev = DevIndex;
    flag = 0;
    i2cAddr = gIAP->IAP_I2cAddr;

    if ( ByteCount >=1 && ByteCount <=256 )
    {
        cmd[0] = 0x10;
        if ( ByteCount == 256 )
        {
            cmd[1] = 0;
        }
        else
        {
            cmd[1] = (unsigned char)ByteCount;
        }

        result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
        if ( S_OK == result )
        {
            ret = true;
        }
    }

    return ret;
}
bool SetWriteFifo( void *Buffer, int BufferLength )
{
    unsigned char cmd[128];
    unsigned char i2cAddr;
    unsigned long flag;
    unsigned long result;
    unsigned long dev;
    bool ret = false;

    unsigned char *data;

    //_TRACE(("%s++", __FUNC__);
    dev = DevIndex;
    flag = 0;
    i2cAddr = gIAP->IAP_I2cAddr;
    data = (unsigned char*)Buffer;

    if ( BufferLength >=1 && BufferLength <=256 )
    {
        int remainLength = BufferLength;
        int opLength = 16;
        int offset = 0;
        cmd[0] = 0x11;
        while( remainLength )
        {
            int i;
            //opLength = (opLength==7)? 9 : 7;
            //opLength = __get_op_length();
            //Sleep(20);
            if ( remainLength < opLength )
            {
                opLength = remainLength;
            }

            for( i=0 ; i<opLength ; i++ )
            {
                cmd[1+i] = (unsigned char)(data[offset] & 0xFF);
                offset++;
            }

            result = WriteI2CBuf( dev, i2cAddr, &cmd[0], opLength+1, flag );
            if ( S_OK == result )
            {
                remainLength -= opLength;
                ret = true;
                continue;
            }
            else
            {
//                _TRACE(("%s++ i2c write addr=%02x, off=%02x, len=%d\r\n", __FUNC__, i2cAddr, cmd[0], opLength+1));
            }
            //Sleep(150);
            ret = false;
            break;
        }
    }

    return ret;
}
bool EnableWriteProcess( void )
{
    unsigned char cmd[8];
    unsigned char regTmp;
    unsigned char i2cAddr;
    unsigned long flag;
    unsigned long result;
    unsigned long dev;
    bool ret = false;

    dev = DevIndex;
    flag = 0;
    i2cAddr = gIAP->IAP_I2cAddr;

    cmd[0] = 0x0c;

    result = S_OK;//ReadI2CBuf2( dev, i2cAddr, cmd[0], &regTmp, 1, flag );
    if ( S_OK == result )
    {
        regTmp = 0x04;
        cmd[1] = regTmp;

        result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
        if ( S_OK == result )
        {
            //Sleep(300); // todo : how to know if it finished ?
            ret = true;
        }
    }

    return ret;
}

bool GetReadFifo( void *Buffer, int BufferLength )
{
    unsigned char cmd[8];
    unsigned char i2cAddr;
    //unsigned char regTmp;
    unsigned long flag;
    unsigned long result;
    unsigned long dev;
    bool ret = false;

    unsigned char *data;
    int opLength = BufferLength; // todo: fit all kinds of input BufferLength!
    int retry;
    int retryMax;

    //_TRACE(("%s++", __FUNC__);
    dev = DevIndex;
    flag = 0;
    i2cAddr = gIAP->IAP_I2cAddr;
    data = (unsigned char*)Buffer;
    retry = 0;
    retryMax = 20;

    if ( opLength > 32 )
    {
        opLength = 32;
    }

    //int fifoReady = 0;
    while(1)
    {
        unsigned char reg18;
        result = ReadI2CBuf2( dev, i2cAddr, 0x18, &reg18, 1, flag );
        if ( S_OK == result )
        {
            if ( reg18 == opLength || reg18 >= 8 )
            {
                break;
            }
            else
            {
                retry++;
                LOGD("GetReadFifo reg18 = 0x%02x  (retry=%d)", reg18, retry);
                if ( retry > retryMax )
                {
                    break;
                }
                Sleep(10);
            }
        }
    }

    {
        unsigned char cnt=0;
		int offset = 0;
		int remainLength = BufferLength;
        retry = 0;
        retryMax = 3;
        opLength = 15;
        cmd[0] = 0x14;
		//printf("remainLength  %X opLength   %X \n",remainLength,opLength);
        while( remainLength )
        {
        	LOGD("remainLength  %X opLength   %X  cnt %d\n",remainLength,opLength,cnt);
			cnt++;
            if ( remainLength < opLength )
            {
                opLength = remainLength;
            }

            result = ReadI2CBuf2( dev, i2cAddr, cmd[0], &data[offset], opLength, flag );
            if ( S_OK == result )
            {
                offset+=opLength;
                remainLength -= opLength;
                ret = true;
                continue;
            }
            else
            {
                if ( retry < retryMax )
                {
//                  _WARN(("%s - retry %d, off=%d, opLen=%d", __FUNC__, retry, offset, opLength));
                    retry++;
                    ClearReadFifo();
                    continue;
                }
//                _ERROR(("%s - retry %d, off=%d, opLen=%d", __FUNC__, retry, offset, opLength));
                ret = false;
                break;
            }
        }
    }

    return ret;
}
bool EnableReadProcess( void )
{
    unsigned char cmd[8];
    unsigned char regTmp;
    unsigned char i2cAddr;
    unsigned long flag;
    unsigned long result;
    unsigned long dev;
    bool ret = false;

    dev = DevIndex;
    flag = 0;
    i2cAddr = gIAP->IAP_I2cAddr;

    cmd[0] = 0x0c;

    result = S_OK;//ReadI2CBuf2( dev, i2cAddr, cmd[0], &regTmp, 1, flag );
    if ( S_OK == result )
    {
        regTmp = 0x08;
        cmd[1] = regTmp;

        result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
        if ( S_OK == result )
        {
            //Sleep(300); // todo : how to know if it finished ?
            ret = true;
        }
    }

    return ret;
}

bool WaitForStatus( int RegOffset, int Mask, int Timeout )
{
    unsigned char cmd[8];
    unsigned char regTmp;
    unsigned char mask;
    unsigned char i2cAddr;
    unsigned long flag;
    unsigned long result;
    unsigned long dev;
    unsigned long t1, t2;
    bool ret = false;

    dev = DevIndex;
    flag = 0;
    i2cAddr = gIAP->IAP_I2cAddr;

    cmd[0] = (unsigned char)RegOffset;
    mask = (unsigned char)Mask;
    t2 = t1 = GetTickCount();
    //Timeout = 100;

__retry:
    result = ReadI2CBuf2( dev, i2cAddr, cmd[0], &regTmp, 1, flag );
    if ( S_OK == result )
    {
        //_TRACE(( "WaitForStatus offset %02X = %02X", RegOffset, regTmp );
        if ( regTmp & mask )
        {
            cmd[1] = mask;
            result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
            if ( S_OK == result )
            {
                ret = true;
            }
            else
            {
                ret = false;
            }
        }
    }
    else
    {
        LOGD( "WaitForStatus read failed" );
    }

    if ( ret == false )
    {
        if ( Timeout == 0 )
        {
            goto __retry;
        }
        else
        {
            if ( (t2-t1< (ULONG)Timeout) )
            {
                t2 = GetTickCount();
                goto __retry;
            }
        }

        LOGD( "WaitForStatus timeout failed %02X %02X %d", RegOffset, Mask, Timeout );
        //ret = true;
    }

    return ret;
}

bool TryEnterISP(  unsigned char WaitTime )
{
    unsigned char cmd[32];
    unsigned char regTmp;
    unsigned char i2cAddr;
    unsigned char i2cAddrRw;
    unsigned long flag;
    unsigned long result;
    unsigned long dev;
    //unsigned long t1, t2;
    bool ret = false;
    int retry_count;
    UCHAR m_chipRev;

	WaitTime = WaitTime;

    int redo_pw;
    redo_pw = 1;

//__REDO_PW:
//    _TRACE(( "%s++\r\n", __FUNC__ ));

    dev = DevIndex;
    flag = 0;
    i2cAddr = gIAP->IAP_I2cAddr;
    i2cAddrRw = gIAP->ISP_I2cAddr;
    memcpy( cmd, gIAP->IAP_Tag, gIAP->IAP_TagLength );

    //t1 = GetTickCount();
    //m_waitingTimeForEnterIsp = WaitTime;

    while(1)
    {
        result = S_OK;//ReadI2CBuf2( dev, i2cAddr, 0x16, &regTmp, 1, flag );
        if ( S_OK == result )
        {
            regTmp |= 0x80;
            cmd[0] = 0x16;
            cmd[1] = regTmp;
            result = S_OK;//WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag ); // enable ISP mode
            if ( S_OK == result )
            {
                int i;
                retry_count = 0;
__RETRY_PW:
                for( i=0 ; i<gIAP->IAP_TagLength ; i++ )
                {
                    cmd[0] = ISPAddrs[i];
                    cmd[1] = gIAP->IAP_Tag[i];
                    result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
                    if ( S_OK == result )
                    {
                        LOGD( "success pw[%d]=%02x, %02x\r\n", i, cmd[0], cmd[1] );
                        ret = true;
                    }
                    else
                    {
                        LOGD( "failed to write pw[%d]=%02x, %02x\r\n", i, cmd[0], cmd[1] );
                        ret = false;
                        break;
                    }
                }

                if (ret)
                {
                    UCHAR mcuState = 0;
                    ReadI2CBuf2( dev, i2cAddr, 0x50, &mcuState, 1, flag );
                    if ( (mcuState & 0x80)==0  )
                    {
                        retry_count++;
                        if ( retry_count < 20 )
                        {
                           LOGD( "retry password %d - 0x50=%02x\r\n", retry_count, mcuState );
                            goto __RETRY_PW;
                        }
                    }
                }
                ReadI2CBuf2( dev, i2cAddr, 0x0, &m_chipRev, 1, flag );

                if ( ret )
                {
                    LOGD( "password ok\r\n" );
                    //ShowMessage(L"password ok");

                    if ( redo_pw ) {
                        cmd[0] = 0x06;
                        cmd[1] = 0x01;
                        //result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
                        //_TRACE(( "set %02x => %02x, result = %d", cmd[0], cmd[1], result );
                        //Sleep(10);
                        cmd[0] = 0x06;
                        cmd[1] = 0x00;
                        //result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
                        //_TRACE(( "set %02x => %02x, result = %d", cmd[0], cmd[1], result );
                        redo_pw = 0;
                        //goto __REDO_PW;
                    }

                    cmd[0] = 0x04;
                    cmd[1] = 0x10;
                    //result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
                    cmd[0] = 0x05;
                    cmd[1] = 0x01;
                    //result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
                }
                else
                {
									  int wakeup_mcu = 20;
                    redo_pw = 1;

                    while( wakeup_mcu )
                    {
                        //_TRACE(( "wakeup_mcu %d", wakeup_mcu ));

                        cmd[0] = 0x0;
                        cmd[1] = 0x0;
                        WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag ); // make sda low

                        cmd[0] = ISPAddrs[0];
                        cmd[1] = gIAP->IAP_Tag[0];
                        result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
                        if ( S_OK == result )
                        {
                            break;
                        }

                        wakeup_mcu--;
                    }
                }

                if ( ret )
                {
                    // enable interrupt
                    retry_count = 0;
//__RETRY_INT_MASK:
                    cmd[0] = 0x0a;
                    cmd[1] = 0xfe;
                    result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag ); // enable ISP mode
                    if ( S_OK == result )
                    {
                        result = ReadI2CBuf2( dev, i2cAddr, cmd[0], &regTmp, 1, flag ); // enable ISP mode
                        if ( true == result )
                        {
                            if ( regTmp == cmd[1] )
                            {
                                ret = true;
                            }
                            else
                            {
                                ret = false;
                            }

                        }
                        else
                        {
                            LOGD( "failed to enable interrupt, read failed\r\n" );
                            ret = false;
                        }
                    }
                    else
                    {
                        LOGD( "failed to enable interrupt\r\n" );
                        ret = false;
                    }

                    /*
                    if ( false == ret )
                    {
                        retry_count++;
                        if ( retry_count < 10 )
                        {
                            _TRACE(( "retry int mask %d", retry_count );
                            goto __RETRY_INT_MASK;
                        }
                    }
                    */

                    cmd[0] = 0x0b;
                    cmd[1] = 0xff;
                    result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag ); // enable ISP mode
                    if ( S_OK == result )
                    {
                        result = ReadI2CBuf2( dev, i2cAddr, cmd[0], &regTmp, 1, flag ); // enable ISP mode
                        if ( true == result )
                        {
                            if ( 0x40 & cmd[1] )
                            {
                                ret = true;
                            }
                            else
                            {
                                ret = false;
                            }

                        }
                        else
                        {
                           LOGD( "failed to enable interrupt, read failed\r\n" );
                            ret = false;
                        }
                    }
                    else
                    {
                        LOGD( "failed to enable interrupt\r\n" );
                        ret = false;
                    }

                }
            }
            else
            {
                LOGD( "failed to write 0x16[7]\r\n" );
            }
        }
        else
        {
            LOGD( "Read 0x16 failed\r\n" );
        }

        if ( ret == true )
        {
            #if 0
            cmd[0] = 0x04;
            cmd[1] = 0x10;
            result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
            _TRACE(( "set %02x => %02x, result = %d", cmd[0], cmd[1], result );
            cmd[0] = 0x05;
            cmd[1] = 0x17;
            result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
            _TRACE(( "set %02x => %02x, result = %d", cmd[0], cmd[1], result );
            #endif



            #if 0
            cmd[0] = 0x06;
            cmd[1] = 0x00;
            result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
            _TRACE(( "set %02x => %02x, result = %d", cmd[0], cmd[1], result );
            Sleep(10);
            #endif
            break;
        }

        Sleep(80);

        if ( false == ret )
        {
            retry_count++;
            if ( retry_count < 10 )
            {
                LOGD( "### retry entering ISP %d\r\n", retry_count );
                goto __RETRY_PW;
            }
            else
            {
                LOGD( "### FAILED entering ISP %d\r\n", retry_count );
                break;
            }
        }

    }

    return ret;
}


bool ChipErase()
{
    unsigned char cmd[8];
    unsigned char regTmp;
    unsigned char i2cAddr;
    unsigned long flag;
    unsigned long result;
    unsigned long dev;
    bool ret = false;

    dev = DevIndex;
    flag = 0;
    i2cAddr = gIAP->IAP_I2cAddr;

    cmd[0] = 0x0c;
    //cmd[1] = 0x01;
    //result = ReadI2CBuf2( dev, i2cAddr, cmd[0], &regTmp, 1, flag );
    //result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );

//////
#if 0
    cmd[1]=0x01;
    result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
    if ( S_OK == result )
    {
        result = ReadI2CBuf2( dev, i2cAddr, 0x17, &regTmp, 1, flag );
        _TRACE(( "SR, Read 0x17 = %02x, (result=%d)", regTmp, result );
    }
    else
    {
        _TRACE(( "SR, Fail (result=%d)", result );
    }
#endif
/////

    #if 0
    if ( false == this->ClearReadFifo() )
    {
        return false;
    }
    #endif

    result = S_OK;
    if ( S_OK == result )
    {
        regTmp = 0x10;
        cmd[1] = regTmp;

        result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
        if ( S_OK == result )
        {
            DWORD tick1=GetTickCount();
            if ( WaitForStatus( 0x08, 0x80, 3500 ) )
            {
                DWORD tick2=GetTickCount();
                if ( tick2 - tick1 > 2000 )
                {
                     LOGD( "Erase takes %lu ticks", (tick2-tick1) );
                }
                ret = true;
            }
            else
            {
							  UCHAR reg17;
                //set_port(0x0B, 0xff, 0xff);
                ret = false;
                 LOGD( "WaitForStatus erase failed" );
                /////
               
                cmd[0]=0x17;

                result = ReadI2CBuf2( dev, i2cAddr, cmd[0], &reg17, 1, flag ); // enable ISP mode
                if ( S_OK == result )
                {
                     LOGD("ERASE Error WaitForStatus 0x17=%02X", reg17 );
                }
                else
                {
                    LOGD("ERASE Error WaitForStatus read 0x17 fail, result=%d", result );
                }
                //ShowMessage(L"STOPPED");
                /////
            }
        }
    }

    return ret;
}

//0x09 b4
bool SectorErase(unsigned long Offset)
{
    unsigned char cmd[8];
    unsigned char regTmp;
    unsigned char i2cAddr;
    unsigned long flag;
    unsigned long result;
    unsigned long dev;
    bool ret = false;

    dev = DevIndex;
    flag = 0;
    i2cAddr = gIAP->IAP_I2cAddr;

    cmd[0] = 0x0c;

//////
#if 0
    cmd[1]=0x01;
    result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
    if ( S_OK == result )
    {
        result = ReadI2CBuf2( dev, i2cAddr, 0x17, &regTmp, 1, flag );
        _TRACE(( "SR, Read 0x17 = %02x, (result=%d)", regTmp, result );
    }
    else
    {
        _TRACE(( "SR, Fail (result=%d)", result );
    }
#endif
/////

    //_TRACE(( "SectorErase, offset=%lu\r\n", Offset ));

    if ( SetStartOffset(Offset) )
    {
        regTmp = 0x20; //se command
        cmd[1] = regTmp;

        result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
        if ( S_OK == result )
        {
            DWORD tick1=GetTickCount();
            if ( WaitForStatus( 0x09, 0x40, 1500 ) )
            {
                DWORD tick2=GetTickCount();
                if ( tick2 - tick1 > 1000 )
                {
                    LOGD( "Sector Erase takes %lu ticks\r\n", (tick2-tick1) );
                }
                ret = true;
            }
            else
            {
							  UCHAR reg17;
							  UCHAR reg0def[8];
                //set_port(0x0B, 0xff, 0xff);
                ret = false;
                LOGD( "WaitForStatus sector erase failed, offset=%lu\r\n", Offset );
                /////
                
              
                cmd[0]=0x0d;

                result = ReadI2CBuf2( dev, i2cAddr, cmd[0], &reg0def[0], 3, flag ); // enable ISP mode
                if ( S_OK == result )
                {
                    LOGD("Sector ERASE Error WaitForStatus 0x0D=%02X %02X %02X\r\n", reg0def[0], reg0def[1], reg0def[2] );
                }
                else
                {
                    LOGD("Sector ERASE Error WaitForStatus read 0x0D fail, result=%d\r\n", result );
                }

                cmd[0]=0x17;
                result = ReadI2CBuf2( dev, i2cAddr, cmd[0], &reg17, 1, flag ); // enable ISP mode
                if ( S_OK == result )
                {
                    LOGD("Sector ERASE Error WaitForStatus 0x17=%02X\r\n", reg17 );
                }
                else
                {
                    LOGD("Sector ERASE Error WaitForStatus read 0x17 fail, result=%d\r\n", result );
                }
                //ShowMessage(L"STOPPED");
                /////
            }
        }
    }
    else
    {
        LOGD("SE failed to set offset\r\n");
    }

    return ret;
}

bool Erase()
{
    bool ret = false;
    int offset;

    // _TRACE(("%s++\r\n", __FUNC__));

    #if 0 // set to 0 for SE stress test
    ret = this->ChipErase();
    #else
    for( offset=0 ; offset <IT8313_MAX_CODE_SIZE ; offset+=IT8313_SECTOR_SIZE )
    {
        ret = SectorErase( offset );
        if ( false == ret )
        {
            LOGD( "Sector Erase failed offset=0x%04x\r\n", offset );
            goto __LEAVE;
        }
    }
    /*
    for( int offset=0 ; offset <IT8313_MAX_CODE_SIZE ; offset+=IT8313_SECTOR_SIZE )
    {
        ret = this->SectorErase( offset );
        if ( false == ret )
        {
            _ERROR(( "Sector Erase failed offset=0x%04x", offset );
            goto __LEAVE;
        }
    }
    */
    #endif

__LEAVE:
    return ret;
}


#if 1
bool Isp_WritePage( int PageOffset, int ByteOffset, void *Buffer, int Length )
{
    unsigned char cmd[8];
    //unsigned char data[256];
    unsigned char *dataBuffer;
    unsigned char i2cAddr;
    unsigned char i2cAddrRw;
    unsigned long flag;
    unsigned long result;
    unsigned long dev;
    unsigned long offset;
    unsigned long pageSize;
    //unsigned long t1, t2;
    int remainLength;
    int maxOpLength;
    int opLength;
    bool ret = false;

    unsigned char pageOffset = PageOffset;
    unsigned char byteOffset = ByteOffset & 0xFF;

    //_TRACE(("%s++\r\n", __FUNC__));

    dev = DevIndex;
    flag = 0;
    i2cAddr = gIAP->IAP_I2cAddr;
    i2cAddrRw = gIAP->ISP_I2cAddr;
    pageSize = gIAP->PageSize;

    dataBuffer = (unsigned char*)Buffer;
    maxOpLength = 128;
    remainLength = Length;
    opLength = maxOpLength;

    //_TRACE(( "%s() Buf=0x%08x, Len=%d, po=%d", __FUNC__, (int)Buffer, Length, (int)pageOffset, (int)byteOffset ));

    while( remainLength )
    {
        if ( remainLength < opLength )
        {
            opLength = remainLength;
        }

        offset = pageOffset * pageSize + byteOffset;

        #if 0
        if ( this->m_useSectorErase )
        {
            if ( (offset % 0x400)==0 ) {
                ret = this->SectorErase(offset);
                //Sleep(500);
            }
            else {
                ret = true;
            }
        }
        else
        #endif
        {
            ret = true;
        }

        if ( ret )
        {
            if ( SetStartOffset( offset ) )
            {
                if ( SetByteCount( opLength ) )
                {
                    if ( EnableWriteProcess() )
                    {
                        if ( SetWriteFifo( dataBuffer, opLength ) )
                        {
                            bool ret2 = WaitForStatus( 0x08, 0x08, 1500 );
                            //ret2 = true;
                            if ( ret2 )
                            {
                                remainLength -= opLength;
                                dataBuffer += opLength;
                                byteOffset += opLength;
                                ret = true;
                                continue;
                            }
                            else
                            {
								UCHAR reg17;
                                //set_port(0x0B, 0xff, 0xff);
                                LOGD("Isp_WritePage Error WaitForStatus\r\n" );
                                /////
                               
                                cmd[0]=0x17;

                                         //ReadI2CBuf2( dev, i2cAddr, cmd[0], &regTmp, 1, flag );
                                result = ReadI2CBuf2( dev, i2cAddr, cmd[0], &reg17, 1, flag ); // enable ISP mode
                                if ( S_OK == result )
                                {
                                    LOGD("Isp_WritePage Error WaitForStatus 0x17=%02X, offset=%d\r\n", reg17, offset );
                                }
                                else
                                {
                                    LOGD("Isp_WritePage Error WaitForStatus read 0x17 fail, result=%d\r\n", result );
                                }
                                //ShowMessage(L"STOPPED");
                                /////
                            }
                        }
                        else
                        {
                            LOGD("Isp_WritePage Error EnableWriteProcess\r\n");
                        }
                    }
                    else
                    {
                        LOGD("Isp_WritePage Error SetWriteFifo %p %d\r\n", dataBuffer, opLength );
                    }
                }
                else
                {
                    LOGD("Isp_WritePage Error SetByteCount %d\r\n", opLength );
                }
            }
            else
            {
                LOGD("Isp_WritePage Error SetStartOffset %d %d %d %d\r\n", pageOffset, pageSize, byteOffset, offset );
            }
        }
        else
        {
            LOGD("Isp_WritePage Error :: SectorErase\r\n" );
        }
        ret = false;
        break;
    }

    return ret;
}
#endif


static bool Write( const unsigned char *Buffer, int BufferLength )
{
    //unsigned long t1, t2;
    unsigned char readBuffer[256];
    //unsigned char cmd[8];
    //unsigned char data[32];
    unsigned char *opBuffer = (unsigned char*)Buffer;
    //unsigned char pageOffset;
    unsigned char byteOffset;
    bool ret = false;
    //int Offset=0; // write from offset 0
    int pageOffset;
    int pageCount;
    int pageSize = 256;//gIAP->PageSize;
    int opLength = pageSize;
    int retryMax = 5;
    int retry = retryMax;
    //int sec_length = 0x400;
    int position;

    //_TRACE(( "%s() ++ \r\n", __FUNC__ ));
    //_TRACE(( "%s() ++ \r\n", __FUNC__ ));

    pageCount = (BufferLength + (pageSize-1)) / pageSize;


    //_TRACE(( "%s() Buf=%p, Len=%lu, page count=%lu \r\n", __FUNC__, Buffer, BufferLength, pageCount ));

    // to make sure the original boot code position is not changed.
    // opBuffer[1] = m_bootCodePos;

    devPrv->m_codeLength = BufferLength;
    //memcpy( devPrv->m_codeBuffer, Buffer, devPrv->m_codeLength );
    devPrv->m_codeBuffer = (UCHAR*)Buffer;
    //this->m_codeBuffer[1] = 0x7B;

    if ( 1 )
    {
        UCHAR rbuf[256];
        int len = 16;
        //int i;

        Isp_ReadPage( (int)(0x8000/pageSize), 0, rbuf, len );

        //for( i=0 ; i<len; i++ )
            //_TRACE(( "read offset 0x8000 [%d] = 0x%02x", i, rbuf[i] ));
    }

    //for( pageOffset = pageCount-1 ; pageOffset >= 0 ; pageOffset-- )
    for( pageOffset = 0 ; pageOffset <pageCount ; pageOffset++ )
    {
        int skip_by_0xff = 1;
        int kk;


		ITE6622_UPG_DISPLAY(pageOffset*100/pageCount);

        opLength = pageSize;

         //_TRACE(( "%s() pageOffset=%d\r\n", pageOffset));

__RETRY_WRITE:
        position = pageOffset*pageSize;
        opBuffer = devPrv->m_codeBuffer + position;
        byteOffset = 0;
#if 0
        if ( (position) < MAIN_CODE_BEGIN_OFFSET )
        {
            if ( devPrv->m_eraseBootCode != TAG_ERASE_BOOT_CODE )
            {
                // the reserved area is not erased, skip write
                //__Log2( clTraceF3, clTraceB3, UnicodeString().sprintf(L"skip boot code page %d", pageOffset) );
                continue;
            }
        }
#endif
        
        for( kk=0 ; kk<pageSize ; kk++ )
        {
            if ( opBuffer[kk] != 0xff )
            {
                skip_by_0xff = 0;
                break;
            }
        }
        if ( skip_by_0xff )
        {
            //__Log2( clTraceF3, clTraceB3, UnicodeString().sprintf(L"skip 0xFF code at page %d", pageOffset) );
            ret = true;
            continue;
        }


        //t1=GetTickCount();
        ret = Isp_WritePage( pageOffset, byteOffset, opBuffer, opLength );
        //t2=GetTickCount();
        if ( ret )
        {
__RETRY_READ_COMPARE:
            ret = Isp_ReadPage( pageOffset, byteOffset, readBuffer, opLength );
            if ( ret )
            {
                bool compareResult;
                int currOffset;

                compareResult = memcmp( opBuffer, readBuffer, opLength );
                currOffset = pageOffset * pageSize;

                //if ( ( 0 != compareResult ) || GenRandomFailed(retry) )
                if ( ( 0 != compareResult ) )
                {
                    int ii;
                    LOGD("compare fail at page %d\r\n", pageOffset);
                    for( ii=0 ; ii<opLength ; ii++ )
                    {
                        if (opBuffer[ii] != readBuffer[ii])
                        {
                            LOGD("Wdata[%d]=%02X != Rdata[%d]=%02X\r\n", ii, opBuffer[ii], ii, readBuffer[ii]);
                        }
                        else
                        {
                           LOGD("data[%d]=%02X\r\n", ii, opBuffer[ii]);
                        }
                    }
                    LOGD("Verify failed\r\n");
                    //ShowMessage(L"Verify failed, STOP");

                    if ( retry > 0 )
                    {
                        retry--;

                        if ( retry == (retryMax-1) )
                        {
                            LOGD("Verify failed - retry read." );
                            goto __RETRY_READ_COMPARE;
                        }
                        else
                        {
                            // modify 'pageOffset' to make sector erase was called
                            // pageOffset must be 1k alignment

                            pageOffset = (pageOffset/4)*4;
                            ret = SectorErase( pageOffset*pageSize );
                            if ( false == ret )
                            {
                                LOGD( "Verify failed - SE failed." );
                            }
                            goto __RETRY_WRITE;
                        }
                    }
                    else
                    {
                        ret = false;
                        break;
                    }
                }
                else
                {
                    // compare ok
                    retry = retryMax;
                }
            }
            else
            {
               LOGD( "Isp_ReadPage failed" );
                break;
            }

            #if 0
            if ( t2 != t1 )
            {
                // log = UnicodeString().sprintf(L"Programming Page %d/%d with %lu ms", pageOffset, pageCount, t2-t1);
                //__Log( clTraceF, clTraceB, log );
                _TRACE(( AnsiString().sprintf("Programming Page %d/%d with %lu ms", pageOffset+1, pageCount, t2-t1).c_str() ));
            }
            else
            {
                //log = UnicodeString().sprintf(L"Programming Page %d/%d", pageOffset, pageCount);
                //__Log( clTraceF, clTraceB, log );
                _TRACE(( AnsiString().sprintf("Programming Page %d/%d", pageOffset+1, pageCount).c_str() ));
            }
            #endif
        }
        else
        {
            devPrv->m_codeLength = 0;
            LOGD( "Isp_WritePage failed\n" );
            ret = false;
            break;
        }
    }

    if ( 0 )
    {
        UCHAR rbuf[256];
        int len = 16;
        //int i;

        Isp_ReadPage( 0x8000/pageSize, 0, rbuf, len );

        //for( i=0 ; i<len; i++ )
            //_TRACE(( AnsiString().sprintf("read offset 0x8000 [%d] = 0x%02x", i, rbuf[i]).c_str() ));
    }


    if ( devPrv->m_fullyCompare )
    {
        UCHAR readbackBuffer[256] = {0};
        UCHAR *originalCode = devPrv->m_codeBuffer;

        for( pageOffset = 0 ; pageOffset <pageCount ; pageOffset++ )
        {
            opLength = pageSize;
            position = pageOffset*pageSize;
            opBuffer = readbackBuffer;
            originalCode = devPrv->m_codeBuffer + position;;
            byteOffset = 0;

            ret = Isp_ReadPage( pageOffset, byteOffset, opBuffer, opLength );
            if ( ret )
            {
                int compareResult;
                int currOffset;

                compareResult = memcmp( opBuffer, originalCode, opLength );
                currOffset = pageOffset * pageSize;

                //if ( ( 0 != compareResult ) || GenRandomFailed(retry) )
                if ( ( 0 != compareResult ) )
                {
                    int ii;

                    LOGD("compare fail at page %d", pageOffset);
                    for( ii=0 ; ii<opLength ; ii++ )
                    {
                        if (opBuffer[ii] != originalCode[ii])
                        {
                            LOGD("Wdata[%d]=%02X != Rdata[%d]=%02X", ii, originalCode[ii], ii, opBuffer[ii]);
                        }
                        else
                        {
                            LOGD("data[%d]=%02X", ii, opBuffer[ii]);
                        }

                    }
                    LOGD( "Verify failed" );

                    ret = false;
                    break;
                }
                else
                {
                    // compare ok
                    retry = retryMax;
                }
            }
            else
            {
                LOGD( "Isp_ReadPage failed" );
                break;
            }
        }
    }

    // write fwtag
    if ( ret )
    {
        unsigned char FwTag[8] = { 0x55, 0x44, 0xC3, 0x65, 0x16, 0xA0, 0x1A, 0x59 };
        pageOffset = 0x2060/256;
        byteOffset = 0x2060%256;
        opLength = 8;

        ret = Isp_WritePage( pageOffset, byteOffset, FwTag, opLength );
    }

    return ret;
}

bool Isp_ReadPage( int PageOffset, int ByteOffset, void *Buffer, int Length )
{
    //unsigned char cmd[128];
    //unsigned char data[32];
    unsigned char *outBuffer;
    unsigned char i2cAddr;
    unsigned char i2cAddrRw;
    unsigned long flag;
    //unsigned long result;
    unsigned long offset;
    unsigned long pageSize;
    unsigned long dev;
    //unsigned long t1, t2;
    int remainLength;
    int maxReadLength;
    int readLength;
    bool ret = false;

    unsigned char pageOffset = PageOffset;
    unsigned char byteOffset = ByteOffset & 0xFF;
   
    //_TRACE(( "%s() ++ \r\n", __FUNC__ ));

    dev = DevIndex;
    flag = 0;
    i2cAddr = gIAP->IAP_I2cAddr;
    i2cAddrRw = gIAP->ISP_I2cAddr;
    pageSize = 256;//gIAP->PageSize;

    outBuffer = (unsigned char*)Buffer;
    maxReadLength = 128;
    remainLength = Length;
    readLength = 8;//maxReadLength;
	LOGD("Isp_ReadPage pageOffset %X pageSize %X \n",pageOffset,pageSize);

    while( remainLength )
    {
        if ( remainLength < maxReadLength )
        {
            //readLength = remainLength;
        }

        ClearReadFifo();
        offset = pageOffset * pageSize + byteOffset;
		LOGD("offset %X  readLength  %X \n",offset,readLength);
        if ( SetStartOffset( offset ) )
        {
            if ( SetByteCount( readLength ) )//
            {
                if ( EnableReadProcess() )
                {
                    //if ( this->WaitForStatus( 0x08, 0x20, 10000 ) )
                    {
                        int reRead = 0;
                        //__REREAD:
                        if ( GetReadFifo( outBuffer, readLength ) )
                        {
                            int c=0;
                            int j;

                            for( j=0 ; j<readLength ; j++ )
                            {
                                c += outBuffer[j];
                            }
                            if ( c==0 && reRead < 3 )
                            {
                                reRead++;
                                //goto __REREAD;
                            }

                            #if 0
                            if ( this->WaitForStatus( 0x08, 0x20, 3000 ) )
                            {
                                remainLength -= readLength;
                                outBuffer += readLength;
                                byteOffset += readLength;
                                ret = true;
                                continue;
                            }
                            else
                            {
                                _ERROR(("Isp_ReadPage Error WaitForStatus");
                                ret = false;
                            }
                            #else
                                remainLength -= readLength;
                                outBuffer += readLength;
                                byteOffset += readLength;
                                ret = true;
                                continue;
                            #endif
                        }
                        else
                        {
                           LOGD("Isp_ReadPage Error GetReadFifo %p %d", outBuffer, readLength );
                            ret = false;
                        }
                    }
                    //else
                    //{
                    //_TRACE(("Isp_WritePage Error WaitForStatus");
                    //}
                }
                else
                {
                    LOGD("Isp_ReadPage Error EnableReadProcess");
                    ret = false;
                }
            }
            else
            {
                LOGD("Isp_ReadPage Error SetByteCount %d", readLength );
                ret = false;
            }
        }
        else
        {
            LOGD("Isp_ReadPage Error SetStartOffset %d %d %d %d", pageOffset, pageSize, byteOffset, offset );
            ret = false;
        }
        break;
    }

    return ret;
}
#endif

#if 0
static bool Read( void *Buffer, int BufferLength, int StartOffset, int *ReadLength )
{
    //unsigned char cmd[8];
    //unsigned char data[32];
    unsigned char *outBuffer = (unsigned char*)Buffer;
    unsigned char pageOffset;
    unsigned char byteOffset;
    bool ret = false;
    int offset=StartOffset; // write from offset 0
    int pageCount;
    int pageSize = gIAP->PageSize;
    int opLength = pageSize;
    int readLength;

    readLength=0;
    pageCount = (BufferLength + (pageSize-1)) / pageSize;

    while( BufferLength )
    {
        pageOffset = (unsigned char)(offset>>8);
        byteOffset = offset & 0xFF;

        if ( pageSize > BufferLength )
        {
            opLength = BufferLength;
        }

        ret = Isp_ReadPage( pageOffset, byteOffset, outBuffer, opLength );
        if ( ret )
        {
            BufferLength -= opLength;
            offset += opLength;
            outBuffer += opLength;
            readLength += opLength;
        }
        else
        {
            break;
        }
    }

    if ( ReadLength )
    {
        *ReadLength = readLength;
    }

    return ret;
}


#endif

int LeaveISP(void)
{
    unsigned char cmd[8];
    unsigned char i2cAddr;
    unsigned char regTmp =0;
    unsigned long flag;
    unsigned long result;
    unsigned long dev;
    bool ret = false;

    //_TRACE(("%s++\r\n", __FUNC__));
    dev = DevIndex;
    flag = 0;
    i2cAddr = gIAP->IAP_I2cAddr;

    cmd[0] = 0x16;

    result = S_OK;//ReadI2CBuf2( dev, i2cAddr, cmd[0], &regTmp, 1, flag );
    if ( S_OK == result )
    {
        regTmp &= ~0x80;
        cmd[1] = regTmp;

        result = S_OK;//WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
        if ( S_OK == result )
        {
            //pccmd w f2 ff 88
            if ( devPrv->m_chipRev == 0xA0 )
            {
                cmd[0] = 0x06;
                cmd[1] = 0x03;
                result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
                LOGD( "set %02x => %02x, result = %d", cmd[0], cmd[1], result );

                cmd[0] = 0x06;
                cmd[1] = 0x07;
                result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
                LOGD( "set %02x => %02x, result = %d", cmd[0], cmd[1], result );
            }


            if ( devPrv->m_chipRev == 0xA0 )
            {
                cmd[0]=0x2e;
                cmd[1]=0x00;
                result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );

                cmd[0]=0x2e;
                cmd[1]=0x00;
                result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );

                cmd[0]=0x2e;
                cmd[1]=0x01;
                result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );

                cmd[0]=0x2e;
                cmd[1]=0x01;
                result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
            }

            cmd[0]=0xf2;
            cmd[1]=0xff;
            result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );

            cmd[0]=0xf2;
            cmd[1]=0xff;
            result = WriteI2CBuf( dev, i2cAddr, &cmd[0], 2, flag );
            if ( S_OK == result )
            {
                ret = true;
            }
            else
            {
                LOGD( "set %02x => %02x, result = %d", cmd[0], cmd[1], result );
                ret = false;
            }
        }
    }



    return ret;
}

//const unsigned char s_UpdateCode[] = 
//{
//  //#include "it6516fw.h"
//};

//extern const unsigned char s_UpdateCode[];
int s_UpdateCodeLength;
char s_fwVersion[32]={0};

int FwUpdateInit(void)
{
    int i;
    LOGD("%s++\r\n", __FUNCTION__);
    //devPrv->m_eraseBootCode = TAG_ERASE_BOOT_CODE;
    devPrv->m_fullyCompare = 1;
   // s_UpdateCodeLength = sizeof(bin);

    for (i=0 ; i<32 ; i++)
    {
        s_fwVersion[i] = bin[0x2080+i];
    }

    LOGD("version : [%s]\r\n", s_fwVersion);

    return 0;
}



int IT6622_MCU_UPG    (void)
{

	BYTE upg_path[] = {"USB:IT6622.BIN"};
	
	int fd = open(upg_path, O_RDONLY, 0);
	int file_size = get_file_size(upg_path);
	if(fd < 0)
	{
		LOGD(" HDMI upg open file error\n ");
		//HdmiEvt *e = Q_NEW(HdmiEvt, HDMI_UPG_SIG);
		//e->vol = HDMI_UPG_NG;
		// QACTIVE_POST(UserApp_get(), (QEvt *)e, (void*)0);
		ITE6622_UPG_DISPLAY(HDMI_UPG_NG);
		return -1;
	}

	bin = malloc(file_size);
	if(bin == NULL)
	{
		LOGD(" HDMI malloc err\n ");
		return -1;
	}

	s_UpdateCodeLength = file_size;
	read(fd, bin, file_size);
	
    FwUpdateInit();
 #if 1     
    LOGD("Updating firmware...\r\n");
            
    if ( false == TryEnterISP(0) )
    {
        LOGD("Err TryEnterISP\r\n");
        return -1;
    }

#if 1
    if ( false == Erase() )
    {
        LOGD("Err Erase\r\n");
        return -1;
    }
#endif
    LOGD("Begin Write...%08x, len=%d\r\n", (int)bin, s_UpdateCodeLength);
    if ( false == Write( bin, s_UpdateCodeLength) )
    {
        LOGD("Err Write\r\n");
        return -1;
    }

    if ( false == LeaveISP() )
    {
        LOGD("Err LeaveISP\r\n");
        return -1;
    }
#endif
    LOGD("Updating firmware...done...\r\n");

	return 0;
}

