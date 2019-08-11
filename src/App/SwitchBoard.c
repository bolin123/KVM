#include "SwitchBoard.h"
#include "Protocol.h"
#include "VTStaticQueue.h"

#define SWITCH_BOARD_UART_BOARDRATE 9600

static ProtocolDev_t g_sprotoDevice;
static VTSQueueDef(uint8_t, g_switchBuff, 256);

static void lowSend(uint8_t *data, uint16_t len)
{
    HalUartWrite(HAL_UART_PORT_SWITCH, data, len);
}

static void switchBoardUartRecv(uint8_t *data, uint16_t len)
{
    uint16_t i;

    for(i = 0; i < len; i++)
    {
        if(VTSQueueHasSpace(g_switchBuff))
        {
            VTSQueuePush(g_switchBuff, data[i]);
        }
    }
}

static void swtichProtoHandle(void)
{
    uint8_t dbyte;
    while(VTSQueueCount(g_switchBuff))
    {
        HalIRQEnable(false);
        dbyte = VTSQueueFront(g_switchBuff);
        HalIRQEnable(true);
        VTSQueuePop(g_switchBuff);
        ProtocolRecvByte(&g_sprotoDevice, dbyte);
    }
}

void SwitchBoardDataSend(uint8_t cmd, uint8_t *data, uint16_t len)
{
    uint8_t buff[64];
    uint16_t length;
    
    length = ProtocolDataToFrame((ProtocolCmd_t)cmd, PROTO_DIR_FROM_MASTER, data, len, buff);

    lowSend(buff, length);
}

void SwitchBoardSendKeyValue(uint8_t id, uint8_t value)
{
    uint8_t buff[16];
    uint16_t len;
    uint8_t data[2];

    data[0] = id;
    data[1] = value;
    len = ProtocolDataToFrame(PROTO_CMD_KEYBOARD_CODE, PROTO_DIR_FROM_MASTER, data, sizeof(data), buff);

    lowSend(buff, len);
}

void SwitchBoardInit(void)
{
	HalUartConfig_t config;
    config.baudrate = SWITCH_BOARD_UART_BOARDRATE;
    config.flowControl = 0;
    config.parity = 1;
    config.wordLength = USART_WordLength_8b;
    config.recvCb = switchBoardUartRecv;
    HalUartConfig(HAL_UART_PORT_SWITCH, &config);

    g_sprotoDevice.devid = PROTOCOL_DEVID_SWITCH;
    g_sprotoDevice.frameCount = 0;
    g_sprotoDevice.datalength = 0;
}

void SwitchBoardPoll(void)
{
    swtichProtoHandle();
}

