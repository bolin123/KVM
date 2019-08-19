#include "Protocol.h"

#define PROTOCOL_PREAMBLE_HEAD 0x7E

typedef struct
{
    uint8_t head[2];
    uint8_t length;
    uint8_t cmd;
    uint8_t dir;
    uint8_t data[];
}Protocol_t;

static ProtocolEventHandle g_protoEventHandle = NULL;

static uint8_t checkSum(uint8_t *data, uint16_t len)
{
    uint16_t i;
    uint8_t sum = 0;

    for(i = 0; i < len; i++)
    {
        sum += data[i];
    }
    return sum;
}

int ProtocolDataToFrame(ProtocolCmd_t cmd, ProtocolDir_t dir, uint8_t *data, uint8_t dlen, uint8_t *buff)
{
    Protocol_t *proto = (Protocol_t *)buff;

    proto->head[0] = PROTOCOL_PREAMBLE_HEAD;
    proto->head[1] = PROTOCOL_PREAMBLE_HEAD;
    proto->length = dlen + 3;
    proto->cmd = (uint8_t)cmd;
    proto->dir = dir;
    memcpy(proto->data, data, dlen);
    buff[dlen + sizeof(Protocol_t)] = checkSum(&buff[2], dlen + 3);
    return (dlen + sizeof(Protocol_t) + 1);
}

static void protocolParse(ProtocolDevID_t id, ProtocolCmd_t cmd, ProtocolDir_t dir, uint8_t *data, uint8_t dlen)
{
    SysLog("cmd = %d", cmd);

    if(g_protoEventHandle)
    {
        g_protoEventHandle(id, cmd, dir, data, dlen);
    }
    
#if 0
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
#endif    
}

void ProtocolRecvByte(ProtocolDev_t *dev, uint8_t data)
{
//    uint16_t i;

    if(dev != NULL)
    {
        if(SysTimeHasPast(dev->lastTime, 100))
        {
            dev->frameCount = 0;
        }
        dev->lastTime = SysTime();
        dev->frameBuff[dev->frameCount++] = data;

        if(dev->frameCount == 1)
        {
            if(data != PROTOCOL_PREAMBLE_HEAD)
            {
                dev->frameCount = 0;
            }
        }
        else if(dev->frameCount == 2)
        {
            if(data != PROTOCOL_PREAMBLE_HEAD)
            {
                dev->frameCount = 0;
            }
        }
        else if(dev->frameCount == 3)
        {
            dev->datalength  = data;
        }
        else if(dev->frameCount == dev->datalength + 3) //got a packet
        {
            if(checkSum(&dev->frameBuff[2], dev->frameCount - 2) == data)
            {
                Protocol_t *proto = (Protocol_t *)dev->frameBuff;
                protocolParse(dev->devid, (ProtocolCmd_t)proto->cmd, (ProtocolDir_t)proto->dir, proto->data, proto->length - 3);
            }
            dev->datalength = 0;
            dev->frameCount = 0;
        }
    }
    
}

void ProtocolInit(ProtocolEventHandle eventHandle)
{
    g_protoEventHandle = eventHandle;
/*
    HalUartConfig_t config;
    config.baudrate = 9600;
    config.flowControl = 0;
    config.parity = 1;
    config.wordLength = USART_WordLength_8b;
    config.recvCb = ProtocolDataRecv;
    HalUartConfig(HAL_UART_PORT_PRINT, &config);
*/
}

void ProtocolPoll(void)
{
}

