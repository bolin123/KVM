#include "KVM.h"
#include "Keyboard.h"
#include "Protocol.h"
#include "SwitchBoard.h"
#include "DisplayCtrl.h"

typedef enum
{
    KVM_SRC_DEVICE_NONE = 0, //与所有主机断开连接 
    KVM_SRC_DEVICE_PC1,      //连接到 PC1 上
    KVM_SRC_DEVICE_PC2,  
    KVM_SRC_DEVICE_PC3,
    KVM_SRC_DEVICE_PC4,
    KVM_SRC_DEVICE_CLIENT,  //连接到客户端主控板上
    KVM_SRC_DEVICE_BM,      //连接到 BM 主板上
    KVM_SRC_DEVICE_UAV,     //无人机
    KVM_SRC_DEVICE_DECODER, //解码板
}KVMSrcDevice_t;

typedef enum
{
    KVM_DST_DEVICE_KVM1 = 1,
    KVM_DST_DEVICE_KVM2,
    KVM_DST_DEVICE_LCD,
}KVMDstDeivce_t;

static void setPrinterConnect(KVMSrcDevice_t connect)
{
    SysLog("connect %d", connect);
    switch (connect)
    {
    case KVM_SRC_DEVICE_NONE:
        HalGPIOSetLevel(HAL_PRINTER_IO_EN, 0);
        HalGPIOSetLevel(HAL_PRINTER_IO_C0, 0);
        HalGPIOSetLevel(HAL_PRINTER_IO_C1, 0);
        HalGPIOSetLevel(HAL_PRINTER_IO_C2, 0);
        break;
    case KVM_SRC_DEVICE_PC1:
        HalGPIOSetLevel(HAL_PRINTER_IO_EN, 1);
        HalGPIOSetLevel(HAL_PRINTER_IO_C0, 0);
        HalGPIOSetLevel(HAL_PRINTER_IO_C1, 0);
        HalGPIOSetLevel(HAL_PRINTER_IO_C2, 0);
        break;
    case KVM_SRC_DEVICE_PC2:
        HalGPIOSetLevel(HAL_PRINTER_IO_EN, 1);
        HalGPIOSetLevel(HAL_PRINTER_IO_C0, 1);
        HalGPIOSetLevel(HAL_PRINTER_IO_C1, 0);
        HalGPIOSetLevel(HAL_PRINTER_IO_C2, 0);
        break;
    case KVM_SRC_DEVICE_PC3:
        HalGPIOSetLevel(HAL_PRINTER_IO_EN, 1);
        HalGPIOSetLevel(HAL_PRINTER_IO_C0, 0);
        HalGPIOSetLevel(HAL_PRINTER_IO_C1, 1);
        HalGPIOSetLevel(HAL_PRINTER_IO_C2, 0);
        break;
    case KVM_SRC_DEVICE_PC4:
        HalGPIOSetLevel(HAL_PRINTER_IO_EN, 1);
        HalGPIOSetLevel(HAL_PRINTER_IO_C0, 1);
        HalGPIOSetLevel(HAL_PRINTER_IO_C1, 1);
        HalGPIOSetLevel(HAL_PRINTER_IO_C2, 0);
        break;
    case KVM_SRC_DEVICE_CLIENT:
        HalGPIOSetLevel(HAL_PRINTER_IO_EN, 1);
        HalGPIOSetLevel(HAL_PRINTER_IO_C0, 0);
        HalGPIOSetLevel(HAL_PRINTER_IO_C1, 0);
        HalGPIOSetLevel(HAL_PRINTER_IO_C2, 1);
        break;
    case KVM_SRC_DEVICE_BM:
        HalGPIOSetLevel(HAL_PRINTER_IO_EN, 1);
        HalGPIOSetLevel(HAL_PRINTER_IO_C0, 1);
        HalGPIOSetLevel(HAL_PRINTER_IO_C1, 0);
        HalGPIOSetLevel(HAL_PRINTER_IO_C2, 1);
        break;
    default:
        break;
    }
    
}

static void fpgaCtrl(KVMDstDeivce_t dst, KVMSrcDevice_t src)
{
    uint8_t reg;
    

    if(dst == KVM_DST_DEVICE_KVM1)
    {
        reg = 0x03;
    }
    else if(KVM_DST_DEVICE_KVM2 == dst)
    {
        reg = 0x04;
    }
    else
    {
        reg = 0x05;
    }
    SysLog("reg %02x, value %02x", reg, src);

    if(src <= KVM_SRC_DEVICE_BM)
    {
        HalI2CWriteByte(HAL_FPGA_IIC_ADDR, reg, src);
        SysPrint("reg %02x = %02x", reg, HalI2CReadByte(HAL_FPGA_IIC_ADDR, reg));
    }
    else
    {
        HalI2CWriteByte(HAL_FPGA_IIC_ADDR, reg, 0); //不与任何主机连接
        SysPrint("reg %02x = %02x", reg, HalI2CReadByte(HAL_FPGA_IIC_ADDR, reg));
    }
}

static void kvmKeyHandle(KeyboardID_t id, KeyboardKey_t key)
{
    SysLog("key = %d, id = %d", key, id);
    SwitchBoardSendKeyValue((uint8_t)id, (uint8_t)key);
}

static void kvmConfig(uint8_t cmd)
{
    if(cmd == 0x01) //reboot
    {
        SysPrint("reboot...\n");
        HalReboot();
    }
    else //恢复出厂设置
    {
        //
        SysPrint("factory...\n");
    }
}

static void kvmProtoHandle(ProtocolDevID_t id, ProtocolCmd_t cmd, ProtocolDir_t dir, uint8_t *data, uint8_t dlen)
{
    bool ack = false;
    uint8_t ackData;
    SysLog("cmd = %d", cmd);
    if(id == PROTOCOL_DEVID_DISPLAY_CTRL) //显示屏控制器
    {
        switch(cmd)
        {
        case PROTO_CMD_PRINTER_SET:
            setPrinterConnect((KVMSrcDevice_t)data[0]);
            ack = true;
            break;
        case PROTO_CMD_PRINTER_GET:
            break;
        case PROTO_CMD_DISPLAY_SET:
            fpgaCtrl((KVMDstDeivce_t)data[0], (KVMSrcDevice_t)data[1]);
            SwitchBoardDataSend(cmd, data, dlen);
            // TODO : status update
            
            ack = true;
            break;
        case PROTO_CMD_DISPLAY_GET:
            break;
        case PROTO_CMD_SOUND_SET:
            SwitchBoardDataSend(cmd, data, dlen);
            // TODO : status update
            
            ack = true;
            break;
        case PROTO_CMD_SOUND_GET:
            break;
        case PROTO_CMD_KVM_SWITCH:
            SwitchBoardDataSend(cmd, data, dlen);
            kvmConfig(data[0]);
        
            ack = true;
            break;
        default:
            break;
        }
        if(ack)
        {
            ackData = 0;
            DisplayCtrlDataSend(cmd, &ackData, 1);
        }
    }
    else //音视频切换板
    {
    }

    
}

static void printCtrlIOInit(void)
{
    HalGPIOConfig(HAL_PRINTER_IO_EN, HAL_IO_OUTPUT);
    HalGPIOConfig(HAL_PRINTER_IO_C0, HAL_IO_OUTPUT);
    HalGPIOConfig(HAL_PRINTER_IO_C1, HAL_IO_OUTPUT);
    HalGPIOConfig(HAL_PRINTER_IO_C2, HAL_IO_OUTPUT);
}

void KVMInit(void)
{
    printCtrlIOInit();
    KeyboardInit(kvmKeyHandle);
    ProtocolInit(kvmProtoHandle);
    SwitchBoardInit();
    DisplayCtrlInit();
}

void KVMPoll(void)
{
	KeyboardPoll();
    SwitchBoardPoll();
    ProtocolPoll();
    DisplayCtrlPoll();
}

