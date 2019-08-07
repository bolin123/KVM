#include "KVM.h"
#include "Keyboard.h"
#include "Protocol.h"

typedef void (*KVMUartRecv_cb)(struct KVMUart_st *uart, uint8_t *data, uint16_t len);
typedef void (*KVMFrameParse_cb)(struct KVMUart_st *uart, uint8_t *data, uint16_t len);

typedef struct KVMUart_st
{
	HalUartPort_t port;
	bool gotframe;
	SysTime_t lastTime;
	uint8_t buff[256];
	uint16_t buffcount;
	KVMUartRecv_cb recvCb;
	KVMFrameParse_cb frameParse;
}KVMUart_t;

static KVMUart_t g_uartHandle[HAL_UART_PORT_NUM];

void KVMUartRecv(uint8_t port, uint8_t *data, uint16_t len)
{
	if(port < HAL_UART_PORT_NUM && g_uartHandle[port].recvCb)
	{
		g_uartHandle[port].recvCb(&g_uartHandle[port], data, len);
	}
}

void KVMUartSend(HalUartPort_t port, uint8_t *data, uint8_t len)
{
	if(port < HAL_UART_PORT_NUM)
	{
		HalUartWrite(port, data, len);
	}
}

static void kvmUartConfig(HalUartPort_t port, uint32_t boardrate, KVMUartRecv_cb recvCb)
{
	HalUartConfig_t config;
	if(port < HAL_UART_PORT_NUM)
	{

	    config.baudrate = boardrate;
	    config.flowControl = 0;
	    config.parity = 1;
	    config.wordLength = USART_WordLength_8b;
	    config.recvCb = KVMUartRecv;
	    HalUartConfig(port, &config);

		g_uartHandle[port].gotframe = false;
		g_uartHandle[port].port = port;
		g_uartHandle[port].buffcount = 0;
		g_uartHandle[port].recvCb = recvCb;
	}
}

static void kvmUartFrameHandle(void)
{
	uint8_t i;

	for(i = 0; i < HAL_UART_PORT_NUM; i++)
	{
		if(g_uartHandle[i].gotframe)
		{
			g_uartHandle[i].frameParse(&g_uartHandle[i], g_uartHandle[i].buff, g_uartHandle[i].buffcount);
			g_uartHandle[i].gotframe = false;
			g_uartHandle[i].buffcount = 0;
		}
	}
}

static void uartConfigs(void)
{
	kvmUartConfig(HAL_UART_PORT_PRINT, 9600, ProtocolDataRecv);
	kvmUartConfig(HAL_UART_PORT_DISPLAY, 9600, ProtocolDataRecv);
	kvmUartConfig(HAL_UART_PORT_KVM1KEY, 115200, KeyboardDataRecv);
	kvmUartConfig(HAL_UART_PORT_KVM2KEY, 115200, KeyboardDataRecv);
}

void KVMInit(void)
{
	//uartConfigs();
    KeyboardInit();
    ProtocolInit();
}

void KVMPoll(void)
{
	//kvmUartFrameHandle();
	KeyboardPoll();
}

