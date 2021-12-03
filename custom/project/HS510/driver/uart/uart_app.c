#include <string.h>
#include "qpc.h"
#define LOG_MODULE_ID  ID_APUSR
#define LOG_EXTRA_STR  "[UART]"
#include "log_utils.h"
#include "custom_sig.h"
#include "task_attribute_user.h"
#include "fsi_api.h"
#include "uart_app.h"
#include "initcall.h"
#include "exstd_interfaces.h"
#include "pinmux.h"
#include "touch_app.h"
#include "Usersetting.h"
#include "uart_interface.h"

QActive *UserApp_get(void) ;



#define UART_PACKET_MAX	32

//#define SUPPORT_UART1
#define SUPPORT_UART2


#define DEFAULT_BAUDRETE	115200


//Q_DEFINE_THIS_FILE

/*..........................................................................*/


typedef struct {     
    QActive super;  
    QTimeEvt timeEvtUart; 

	int uart0_avtive;
	int uart1_avtive;
	int uart2_avtive;

	int fd;
	
} UartApp;


static UartApp l_UartApp; /* the UartApp active object */
static QActive * const AO_UartApp = &l_UartApp.super;



static UartApp_Func *UartApp_Device[UARTAPP_MAX] = 
{
	// uart 0
	NULL,

	// uart 1
	NULL, //&auto_test,

	// uart 2
	&BT_ATS2819,

};

//*******************************************************************

QActive *UartApp_get(void) {
    	return AO_UartApp;
}



int UartApp_SetKey_Sig(void *  sender,int sig, int para)
{

	LOGD("[%s]   --> sig  %d\n",__FUNCTION__, sig);

	UartKeyEvt *evt = Q_NEW(UartKeyEvt, UARTAPP_KEY_SIG);
	evt->key = sig;
	evt->src= para;
	QACTIVE_POST(AO_UartApp,  (QEvt *)evt,  sender);
	
	
	return 0;
}


int UartApp_Device_Init(QActive *me, enum UARTAPP_DEVICE id)
{
	int ret = -1;
	UartApp_Func *device = UartApp_Device[id];

	if(device != NULL && device->UartApp_Init != 	NULL)
		ret = device->UartApp_Init(me);

	return ret;
}

int UartApp_Device_Read(QActive *me, enum UARTAPP_DEVICE id, unsigned char *buf, unsigned int size)
{
	int ret = -1;
	UartApp_Func *device = UartApp_Device[id];
	
	if(device != NULL && device->UartApp_read != NULL)
		ret = device->UartApp_read(me,buf, size);

	return ret;
}


int UartApp_Device_Polling(QActive *me, enum UARTAPP_DEVICE id)
{
	int ret = -1;
	UartApp_Func *device = UartApp_Device[id];
	
	if(device != NULL && device->UartApp_polling != NULL)
		ret = device->UartApp_polling(me);

	return ret;
}


int UartApp_Device_Key(QActive *me, enum UARTAPP_DEVICE id, int sig, int para)
{
	int ret = -1;
	UartApp_Func *device = UartApp_Device[id];
	
	if(device != NULL && device->UartApp_Key != NULL)
		ret = device->UartApp_Key(me, sig, para);

	return ret;
}

int UartApp_Device_SetUartHandle(QActive *me, enum UARTAPP_DEVICE id, int fd)
{
	int ret = -1;
	UartApp_Func *device = UartApp_Device[id];

	if(device != NULL && device->UartApp_SetHandle != NULL)
		ret = device->UartApp_SetHandle(me, fd);

	return ret;
}



/******************************************************************************************/
/**
 * \fn          void UserApp_MCUIF_Reader_Listener(QActive * const me, void *pParam)
 *
 * \brief       function for listener regist, system will run this function when get UART1_UAER_READ_SIG signal
 *
 * \param   	QActive * const me:(Input) AO handler ,\n
 *              
 *
 * \return	    none.	
 *			
 *
 ******************************************************************************************/ 

QState UartApp_active(UartApp * const me, QEvt const * const e) {
    QState status;
	int baudrate = -1;
	
	
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            LOGD("UartApp_active ENTRY; \n");
            status = Q_HANDLED();
            break;
        }
	case Q_EXIT_SIG: {
            LOGD("UartApp_active EXIT; \n");
            status = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            LOGD("UartApp_active INIT; \n");        

		baudrate = -1;

		
#ifdef SUPPORT_UART2
		baudrate = UartApp_Device_Init(&me->super, UARTAPP_UART2);
		if(baudrate>= 0)
		{
			me->uart2_avtive = 1;
		}
#endif

		LOGD(" uart init %d-%d\n ", me->uart1_avtive, me->uart2_avtive);

		if(me->uart1_avtive || me->uart2_avtive )
			QTimeEvt_armX(&me->timeEvtUart, TICKS_PER_100MS, 0);   

            status = Q_HANDLED();
            break;
        }

	case UARTAPP_TICK_SIG:{

		//LOGD("UARTAPP_TICK_SIG; \n");    
		
		/*
		if( me->uart1_avtive )
		{
			len = read(me->fd, packet_buf, 256);
			if(len > 0)
				UartApp_Device_Read(&me->super, UARTAPP_UART1, packet_buf, len);
			UartApp_Device_Polling(&me->super, UARTAPP_UART1);
		}
		*/
		
		if( me->uart2_avtive )
		{
			UartApp_Device_Polling(&me->super, UARTAPP_UART2);
		}
		
		QTimeEvt_armX(&me->timeEvtUart, 1, 0);

		status = Q_HANDLED();
		break;
	}

	case UARTAPP_KEY_SIG:{

		LOGD(" UARTAPP_KEY_SIG %d-%d\n ", me->uart1_avtive, me->uart2_avtive);
		UartKeyEvt *uartEvt = ((UartKeyEvt *)e);
		//if( me->uart1_avtive )
		//	UartApp_Device_Key(&me->super, UARTAPP_UART1, uartEvt->key, uartEvt->src);

		if( me->uart2_avtive )
		{
			UartApp_Device_Key(&me->super, UARTAPP_UART2, uartEvt->key, uartEvt->src);	
		}
		status = Q_HANDLED();
		
		break;
	}	
        
        default: {
            status = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status;
}



QState UartApp_initial(UartApp * const me, QEvt const * const e) {
    	(void)e; /* avoid compiler warning about unused parameter */

	LOGD("UartApp_initial; \n");

    	return Q_TRAN(&UartApp_active);
}




void UartApp_ctor(void) {

	LOGD("UartApp_ctor in; \n");
	
    	UartApp * const me = &l_UartApp;

		me->uart0_avtive = 0;
		me->uart1_avtive = 0;
		me->uart2_avtive = 0;
    
    	QActive_ctor(&me->super, Q_STATE_CAST(&UartApp_initial));
	
	QTimeEvt_ctorX(&me->timeEvtUart, &me->super, UARTAPP_TICK_SIG, 0U);

}



void UartApp_Start(void)
{
    static QEvt const *pstUartAppScannerQueueSto[50];
    
   UartApp_ctor();

    QACTIVE_START(AO_UartApp,           /*!< AO to start */
                                TSK_UARTAPP_PRIO, 			/*!< QP priority of the AO */
                                pstUartAppScannerQueueSto,      /*!< event queue storage */
                                Q_DIM(pstUartAppScannerQueueSto), /*!< queue length [events] */
                                TSK_UARTAPP_NAME,             /*!< AO name */
                                TSK_UARTAPP_STACK_DEPTH,       /*!< size of the stack [bytes] */
                                (QEvt *)0);             /*!< initialization event */    
}

AO_INIT(UartApp_Start);


#if 0
static UINT32 _pfUartTestCommand(const char *pcCommandString __UNUSED, long cParameterNumber __UNUSED)
{
	INT32 fd = 0;
	char wbuf[TEST_LEN] = {0};
	char rbuf[TEST_LEN] = {0};
	int ret = 0,ret1= 0;;
	int i;
	int uart_id;

	LOGD("uart test \n");
	LOGD("SYSCLK %d \n", sp_clock_get(SP_CLOCK_ID_SYSTEM));

	/*********** init write data **********************/
	for(i = 0; i < TEST_LEN; i++){
		wbuf[i] = 0x30 + i%10;
	}

	/*********** uart 2 fsi interface test **********************/
	for(uart_id = 2; uart_id <= 2; uart_id++){
		//open uart device
		if(uart_id == 1)
		{
			LOGD("open uart1 driver\n");
			fd = open("C:/uart1", O_RDWR, 0);
			if(fd < 0){
				LOGD("open uart1 fail\n");
				continue;
			}
		}else if(uart_id == 2){
			LOGD("open uart2 driver\n");
			fd = open("C:/uart2", O_RDWR, 0);
			if(fd < 0){
				LOGD("open uart2 fail fd=%d\n",fd);
				continue;
			}
		}


		for(i = 3; i < 6; i++){
			//set uart parameter
			LOGD("-------------------------------------------------------------\n");
			if(i < 3){
				LOGD("--nomal-- uart %d baud %d test\n", uart_id, 115200*(i%3+1));
				ret =ioctl(fd, NORMAL_UART, 115200*(i%3+1));
			}else{
				LOGD("--uadma-- uart %d baud %d test\n", uart_id, 115200*(i%3+1));
				ret =ioctl(fd, UADMA_UART_NO_FLOWCTR, 115200*(i%3+1));
			}


			//send and receive
			if(ret == UART_DRV_SUCCESS)	{
				ret = write(fd, wbuf, TEST_LEN);
				LOGD("write ret %d\n", ret);

				ret = 0;
				while(ret < TEST_LEN)
				{
					memset(rbuf, 0x0, TEST_LEN);
					ret += read(fd, rbuf, TEST_LEN);
					if(ret1 != ret)
					{
						ret1 = ret;
						LOGD("read total ret=%d, rbuf:%s\n", ret, rbuf);
					}

				}
			}
		}

		//test finish, close the uart
		close(fd);
	}

	return 0;
}
#endif

