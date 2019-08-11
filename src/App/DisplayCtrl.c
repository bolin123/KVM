#include "DisplayCtrl.h"
#include "Protocol.h"
#include "VTStaticQueue.h"

#define DISPLAY_CTRL_UART_BOARDRATE 9600

static ProtocolDev_t g_dprotoDevice;
static VTSQueueDef(uint8_t, g_displayBuff, 256);

static void lowSend(uint8_t *data, uint16_t len)
{
    HalUartWrite(HAL_UART_PORT_DISPLAY, data, len);
}

static void diplayUartRecv(uint8_t *data, uint16_t len)
{
    uint16_t i;

    for(i = 0; i < len; i++)
    {
        if(VTSQueueHasSpace(g_displayBuff))
        {
            VTSQueuePush(g_displayBuff, data[i]);
        }
    }
}

static void displayProtoHandle(void)
{
    uint8_t dbyte;
    while(VTSQueueCount(g_displayBuff))
    {
        HalIRQEnable(false);
        dbyte = VTSQueueFront(g_displayBuff);
        HalIRQEnable(true);
        VTSQueuePop(g_displayBuff);
        ProtocolRecvByte(&g_dprotoDevice, dbyte);
    }
}

void DisplayCtrlDataSend(uint8_t cmd, uint8_t *data, uint16_t len)
{
    uint8_t buff[64];
    uint16_t length;
    
    length = ProtocolDataToFrame((ProtocolCmd_t)cmd, PROTO_DIR_FROM_SLAVE, data, len, buff);

    lowSend(buff, length);
}

void DisplayCtrlInit(void)
{
    HalUartConfig_t config;
    config.baudrate = DISPLAY_CTRL_UART_BOARDRATE;
    config.flowControl = 0;
    config.parity = 1;
    config.wordLength = USART_WordLength_8b;
    config.recvCb = diplayUartRecv;
    HalUartConfig(HAL_UART_PORT_DISPLAY, &config);

    g_dprotoDevice.devid = PROTOCOL_DEVID_DISPLAY_CTRL;
    g_dprotoDevice.frameCount = 0;
    g_dprotoDevice.datalength = 0;
}

void DisplayCtrlPoll(void)
{
    displayProtoHandle();
}

