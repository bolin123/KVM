#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "Sys.h"

typedef enum
{
    PROTO_CMD_PRINTER_SET = 1,
    PROTO_CMD_PRINTER_GET, //2
    PROTO_CMD_DISPLAY_SET, //3
    PROTO_CMD_DISPLAY_GET, //4
    PROTO_CMD_SOUND_SET,   //5
    PROTO_CMD_SOUND_GET,   //6
    PROTO_CMD_KVM_SWITCH,  //7
    PROTO_CMD_KEYBOARD_CODE, //8
}ProtocolCmd_t;

typedef enum
{
    PROTO_DIR_FROM_MASTER = 0,
    PROTO_DIR_FROM_SLAVE,
}ProtocolDir_t;

typedef enum
{
    PROTOCOL_DEVID_DISPLAY_CTRL,
    PROTOCOL_DEVID_SWITCH,
}ProtocolDevID_t;

typedef void (*ProtocolAckHandle)(ProtocolCmd_t cmd);

typedef struct
{
    ProtocolDevID_t devid;
    uint8_t frameBuff[128];
    uint8_t frameCount;
    uint8_t datalength;
    SysTime_t lastTime;
    ProtocolAckHandle ackHandle;
}ProtocolDev_t;

typedef void (*ProtocolEventHandle)(ProtocolDevID_t id, ProtocolCmd_t cmd, ProtocolDir_t dir, uint8_t *data, uint8_t dlen);


int ProtocolDataToFrame(ProtocolCmd_t cmd, ProtocolDir_t dir, uint8_t *data, uint8_t dlen, uint8_t *buff);
void ProtocolRecvByte(ProtocolDev_t *dev, uint8_t data);
void ProtocolPoll(void);
void ProtocolInit(ProtocolEventHandle eventHandle);

#endif

