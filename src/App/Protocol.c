#include "Protocol.h"
#include "Sys.h"

#define PROTOCOL_PREAMBLE_HEAD 0x7E

typedef struct
{
    uint8_t head[2];
    uint8_t length;
    uint8_t cmd;
    uint8_t dir;
    uint8_t data[];
}Protocol_t;

typedef enum
{
    PROTO_CMD_PRINTER_SET = 1,
    PROTO_CMD_PRINTER_GET,
    PROTO_CMD_DISPLAY_SET,
    PROTO_CMD_DISPLAY_GET,
    PROTO_CMD_SOUND_SET,
    PROTO_CMD_SOUND_GET,
    PROTO_CMD_KVM_SWITCH,
    PROTO_CMD_KEYBOARD_CODE,
}ProtocolCmd_t;

typedef enum
{
    PROTO_DIR_MASTER_REQUEST = 0,
    PROTO_DIR_SLAVE_REPLY,
}ProtocolDir_t;

static uint8_t g_pbuff[512];
static uint16_t g_pbuffCount = 0; 

static uint8_t checkSum(uint8_t *data, uint16_t len)
{
    return 0;
}

static void protocolDataSend(ProtocolCmd_t cmd, ProtocolDir_t dir, uint8_t *data, uint8_t dlen)
{
    uint8_t buff[512];
    Protocol_t *proto = (Protocol_t *)buff;

    proto->head[0] = PROTOCOL_PREAMBLE_HEAD;
    proto->head[1] = PROTOCOL_PREAMBLE_HEAD;
    proto->length = dlen + 3;
    proto->cmd = cmd;
    proto->dir = dir;
    memcpy(proto->data, data, dlen);
    buff[dlen + sizeof(Protocol_t)] = checkSum(&buff[2], dlen + 3);
    HalUartWrite(HAL_UART_PORT_PRINT, buff, dlen + sizeof(Protocol_t) + 1);
}

static void protocolParse(ProtocolCmd_t cmd, ProtocolDir_t dir, uint8_t *data, uint8_t dlen)
{
    SysLog("cmd = %d", cmd);
    if(dir == PROTO_DIR_MASTER_REQUEST)
    {
        switch (cmd)
        {
        case PROTO_CMD_PRINTER_SET:
        break;
        case PROTO_CMD_PRINTER_GET:
        break;
        case PROTO_CMD_DISPLAY_SET:
        break;
        case PROTO_CMD_DISPLAY_GET:
        break;
        case PROTO_CMD_SOUND_SET:
        break;
        case PROTO_CMD_SOUND_GET:
        break;
        case PROTO_CMD_KVM_SWITCH:
        break;
        case PROTO_CMD_KEYBOARD_CODE:
        break;
        default:
        break;
        }
        uint8_t reply = 0x01;
        protocolDataSend(cmd, PROTO_DIR_SLAVE_REPLY, &reply, 1);
    }
    
}

void ProtocolDataRecv(uint8_t *data, uint16_t length)
{
    uint16_t i;
    static uint16_t packetlen = 0;

    for(i = 0; i < length; i++)
    {
        g_pbuff[g_pbuffCount++] = data[i];

        if(g_pbuffCount == 1)
        {
            if(data[i] != PROTOCOL_PREAMBLE_HEAD)
            {
                g_pbuffCount = 0;
            }
        }
        else if(g_pbuffCount == 2)
        {
            if(data[i] != PROTOCOL_PREAMBLE_HEAD)
            {
                g_pbuffCount = 0;
            }
        }
        else if(g_pbuffCount == 3)
        {
            packetlen  = data[i];
        }
        else if(g_pbuffCount == packetlen + 3) //got a packet
        {
            if(checkSum(&g_pbuff[2], g_pbuffCount - 2) == data[i])
            {
                Protocol_t *proto = (Protocol_t *)g_pbuff;
                protocolParse((ProtocolCmd_t)proto->cmd, (ProtocolDir_t)proto->dir, proto->data, proto->length - 3);
            }
            packetlen = 0;
            g_pbuffCount = 0;
        }
    }
}

void ProtocolInit(void)
{
    HalUartConfig_t config;
    config.baudrate = 9600;
    config.flowControl = 0;
    config.parity = 1;
    config.wordLength = USART_WordLength_8b;
    config.recvCb = ProtocolDataRecv;
    HalUartConfig(HAL_UART_PORT_PRINT, &config);
}

void ProtocolPoll(void)
{
}

