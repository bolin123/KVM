#include "KVM.h"
#include "Keyboard.h"
#include "Protocol.h"
#include "SwitchBoard.h"
#include "DisplayCtrl.h"

#define KVM_CONFIG_VALID_FLAG 0xa5

typedef enum
{
    KVM_SET_RESULT_VALID = 0,
    KVM_SET_RESULT_INVALID,
}KVMSetResult_t;

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

typedef struct
{
    uint8_t flag; //有效标志位，0xa5
    uint8_t printerConnect; //打印机连接
    uint8_t dstConnect[3];  //主机连接 0=KVM1,1=KVM2,2=LCD
    uint8_t soundConnect;   //音响连接
}KVMConfig_t;

static uint8_t g_avmErrcode = 1;
static bool g_kvm1DuCtrlFlag = false;
static bool g_kvm2DuCtrlFlag = false;

/*设置打印机连接*/
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

/*键盘等输入设备切换设置*/
static void inputSwitchControl(KVMDstDeivce_t dst, KVMSrcDevice_t src)
{
#if defined(HAL_OLD_KVM_BOARD)
    
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
#else
    uint8_t kvm1IO[4] = {0x4f, 0x4c, 0x4d, 0x4e};
    uint8_t kvm2IO[4] = {0x07, 0x04, 0x05, 0x06};
    uint8_t lcdIO[4]  = {0x11, 0x24, 0x25, 0x10};
    uint8_t *io;
    if(dst == KVM_DST_DEVICE_KVM1)
    {
        io = kvm1IO;
    }
    else if(KVM_DST_DEVICE_KVM2 == dst)
    {
        io = kvm2IO;
    }
    else
    {
        io = lcdIO;
    }

    switch(src)
    {

    case KVM_SRC_DEVICE_NONE:
    case KVM_SRC_DEVICE_UAV:
    case KVM_SRC_DEVICE_DECODER:
        HalGPIOSetLevel(io[0], 0);
        break;
    case KVM_SRC_DEVICE_PC1:
        HalGPIOSetLevel(io[0], 1);
        HalGPIOSetLevel(io[1], 1);
        HalGPIOSetLevel(io[2], 0);
        HalGPIOSetLevel(io[3], 1);
        break;
    case KVM_SRC_DEVICE_PC2:
        HalGPIOSetLevel(io[0], 1);
        HalGPIOSetLevel(io[1], 1);
        HalGPIOSetLevel(io[2], 0);
        HalGPIOSetLevel(io[3], 0);
        break;
    case KVM_SRC_DEVICE_PC3:
        HalGPIOSetLevel(io[0], 1);
        HalGPIOSetLevel(io[1], 0);
        HalGPIOSetLevel(io[2], 1);
        HalGPIOSetLevel(io[3], 0);
        break;
    case KVM_SRC_DEVICE_PC4:
        HalGPIOSetLevel(io[0], 1);
        HalGPIOSetLevel(io[1], 0);
        HalGPIOSetLevel(io[2], 1);
        HalGPIOSetLevel(io[3], 1);
        break;
    case KVM_SRC_DEVICE_CLIENT:
        HalGPIOSetLevel(io[0], 1);
        HalGPIOSetLevel(io[1], 0);
        HalGPIOSetLevel(io[2], 0);
        HalGPIOSetLevel(io[3], 1);
        break;
    case KVM_SRC_DEVICE_BM:
        HalGPIOSetLevel(io[0], 1);
        HalGPIOSetLevel(io[1], 0);
        HalGPIOSetLevel(io[2], 0);
        HalGPIOSetLevel(io[3], 0);
        break;
    default:
        break;
    }
#endif
}

static void setKVMFactoryConfig(void)
{
    KVMConfig_t config;
    SysLog("");
    memset(&config, 0, sizeof(KVMConfig_t)); //全部断开
    config.flag = KVM_CONFIG_VALID_FLAG;
    SysFlashWrite(HAL_FLASH_ADDR_CONFIGS, (uint8_t *)&config, sizeof(KVMConfig_t));
    
    //hardware config
    inputSwitchControl(KVM_DST_DEVICE_KVM1, (KVMSrcDevice_t)config.dstConnect[0]);
    inputSwitchControl(KVM_DST_DEVICE_KVM2, (KVMSrcDevice_t)config.dstConnect[1]);
    inputSwitchControl(KVM_DST_DEVICE_LCD, (KVMSrcDevice_t)config.dstConnect[2]);
    setPrinterConnect((KVMSrcDevice_t)config.printerConnect);
}

static void kvmKeyHandle(KeyboardID_t id, KeyboardKey_t key)
{
    SysLog("key = %d, id = %d", key, id);
    SwitchBoardSendKeyValue((uint8_t)id, (uint8_t)key);
    if(key == KEYBOARD_KEY_DOUBLE_CTRL)
    {
        if(id == KEYBOARD_ID_KVM1)
        {
            g_kvm1DuCtrlFlag = true;
        }
        else
        {
            g_kvm2DuCtrlFlag = true;
        }
    }
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
        SysPrint("factory...\n");
        setKVMFactoryConfig();
    }
}

static void kvmProtoHandle(ProtocolDevID_t id, ProtocolCmd_t cmd, ProtocolDir_t dir, uint8_t *data, uint8_t dlen)
{
    uint8_t cid;
    bool ack = false;
    uint8_t buff[3];
    KVMSetResult_t result = KVM_SET_RESULT_VALID;
    KVMConfig_t config;
    
    SysLog("cmd = %d", cmd);
    if(id == PROTOCOL_DEVID_DISPLAY_CTRL) //显示屏控制器
    {
        switch(cmd)
        {
        case PROTO_CMD_PRINTER_SET:
            if(data[0] <= 7) // 0 ~ 7
            {
                setPrinterConnect((KVMSrcDevice_t)data[0]);
                SysFlashRead(HAL_FLASH_ADDR_CONFIGS, (uint8_t *)&config, sizeof(KVMConfig_t));
                config.printerConnect = data[0];
                config.flag = KVM_CONFIG_VALID_FLAG;
                SysFlashWrite(HAL_FLASH_ADDR_CONFIGS, (uint8_t *)&config, sizeof(KVMConfig_t));
            }
            else
            {
                result = KVM_SET_RESULT_INVALID;
            }
            ack = true;
            break;
        case PROTO_CMD_PRINTER_GET:
            SysFlashRead(HAL_FLASH_ADDR_CONFIGS, (uint8_t *)&config, sizeof(KVMConfig_t));
            DisplayCtrlDataSend(PROTO_CMD_PRINTER_GET, &config.printerConnect, 1);
            break;
        case PROTO_CMD_DISPLAY_SET:
            cid = data[0] - 1; //dst 1~3
            if(cid < 3)
            {
                inputSwitchControl((KVMDstDeivce_t)data[0], (KVMSrcDevice_t)data[1]);
                SwitchBoardDataSend(cmd, data, dlen);
                SysFlashRead(HAL_FLASH_ADDR_CONFIGS, (uint8_t *)&config, sizeof(KVMConfig_t));
                config.dstConnect[cid] = data[1];
                config.flag = KVM_CONFIG_VALID_FLAG;
                SysFlashWrite(HAL_FLASH_ADDR_CONFIGS, (uint8_t *)&config, sizeof(KVMConfig_t));
            }
            else
            {
                result = KVM_SET_RESULT_INVALID;
            }
            
            ack = true;
            break;
        case PROTO_CMD_DISPLAY_GET:
            cid = data[0] - 1; //data 1~3
            if(cid < 3)
            {
                SysFlashRead(HAL_FLASH_ADDR_CONFIGS, (uint8_t *)&config, sizeof(KVMConfig_t));
                DisplayCtrlDataSend(PROTO_CMD_DISPLAY_GET, &config.dstConnect[cid], 1);
            }
            break;
        case PROTO_CMD_SOUND_SET:
            if(data[0] <= 7) // 0 ~ 7
            {
                SwitchBoardDataSend(cmd, data, dlen);
                SysFlashRead(HAL_FLASH_ADDR_CONFIGS, (uint8_t *)&config, sizeof(KVMConfig_t));
                config.soundConnect = data[0];
                config.flag = KVM_CONFIG_VALID_FLAG;
                SysFlashWrite(HAL_FLASH_ADDR_CONFIGS, (uint8_t *)&config, sizeof(KVMConfig_t));
            }
            else
            {
                result = KVM_SET_RESULT_INVALID;
            }
            
            ack = true;
            break;
        case PROTO_CMD_SOUND_GET:
            SysFlashRead(HAL_FLASH_ADDR_CONFIGS, (uint8_t *)&config, sizeof(KVMConfig_t));
            DisplayCtrlDataSend(PROTO_CMD_SOUND_GET, &config.soundConnect, 1);
            break;
        case PROTO_CMD_KVM_SWITCH: // KVM 切换板重启或恢复出厂设置
            SwitchBoardDataSend(cmd, data, dlen);
            kvmConfig(data[0]);
        
            ack = true;
            break;
        case PROTO_CMD_KVMERROR_GET:
            buff[0] = (SysErrcodeGet() == 0); //kvm errcode
            buff[1] = g_avmErrcode;
            DisplayCtrlDataSend(PROTO_CMD_KVMERROR_GET, buff, 2);
            break;
        default:
            break;
        }
        if(ack)
        {
            DisplayCtrlDataSend(cmd, &result, 1);
        }
    }
    else //音视频切换板
    {
        if(cmd == PROTO_CMD_AVMERROR_GET)
        {
            g_avmErrcode = data[0];
            SysPrint("avm errcode = %d\n", g_avmErrcode);
        }
        else if(cmd == PROTO_CMD_KEYBOARD_CODE)
        {
            if(data[1] == 0x00) //菜单处于关闭状态
            {
                SysFlashRead(HAL_FLASH_ADDR_CONFIGS, (uint8_t *)&config, sizeof(KVMConfig_t));
                if(data[0] == 0x00)//配置有效
                {
                    config.dstConnect[0] = data[2]; //kvm1
                    inputSwitchControl(KVM_DST_DEVICE_KVM1, (KVMSrcDevice_t)data[2]);
                    config.dstConnect[1] = data[3]; //kvm2
                    inputSwitchControl(KVM_DST_DEVICE_KVM2, (KVMSrcDevice_t)data[3]);
                    config.dstConnect[2] = data[4]; //lcd
                    inputSwitchControl(KVM_DST_DEVICE_LCD, (KVMSrcDevice_t)data[3]);

                    config.soundConnect  = data[5]; //box
                    config.flag = KVM_CONFIG_VALID_FLAG;
                    SysFlashWrite(HAL_FLASH_ADDR_CONFIGS, (uint8_t *)&config, sizeof(KVMConfig_t));
                }
                else
                {
                    inputSwitchControl(KVM_DST_DEVICE_KVM1, (KVMSrcDevice_t)config.dstConnect[0]);
                    inputSwitchControl(KVM_DST_DEVICE_KVM2, (KVMSrcDevice_t)config.dstConnect[1]);
                    inputSwitchControl(KVM_DST_DEVICE_LCD, (KVMSrcDevice_t)config.dstConnect[2]);
                }
                
            }
            else //菜单打开
            {
                if(g_kvm1DuCtrlFlag)
                {
                    inputSwitchControl(KVM_DST_DEVICE_KVM1, KVM_SRC_DEVICE_NONE);
                    g_kvm1DuCtrlFlag = false;
                }

                if(g_kvm2DuCtrlFlag)
                {
                    inputSwitchControl(KVM_DST_DEVICE_KVM2, KVM_SRC_DEVICE_NONE);
                    g_kvm2DuCtrlFlag = false;
                }
                
            }
        }
        else
        {
        }
    }

    
}

static void getAVMErrcode(void)
{
    static SysTime_t lastTime = 0;

    if(SysTimeHasPast(lastTime, 5000)) //5s查询一次
    {
        SwitchBoardGetError();
        lastTime = SysTime();
    }
}

static void printCtrlIOInit(void)
{
    HalGPIOConfig(HAL_PRINTER_IO_EN, HAL_IO_OUTPUT);
    HalGPIOConfig(HAL_PRINTER_IO_C0, HAL_IO_OUTPUT);
    HalGPIOConfig(HAL_PRINTER_IO_C1, HAL_IO_OUTPUT);
    HalGPIOConfig(HAL_PRINTER_IO_C2, HAL_IO_OUTPUT);
}

static void switchBoardInit(void)
{
    KVMConfig_t config;
    static bool inited = false;
    uint8_t data[2];

    if(!inited && SysTime() > 1000)
    {
        SysFlashRead(HAL_FLASH_ADDR_CONFIGS, (uint8_t *)&config, sizeof(KVMConfig_t));
        data[0] = KVM_DST_DEVICE_KVM1;
        data[1] = config.dstConnect[0];
        SwitchBoardDataSend(PROTO_CMD_DISPLAY_SET, data, 2);
        HalWaitMs(100);
        data[0] = KVM_DST_DEVICE_KVM2;
        data[1] = config.dstConnect[1];
        SwitchBoardDataSend(PROTO_CMD_DISPLAY_SET, data, 2);
        HalWaitMs(100);
        data[0] = config.soundConnect;
        SwitchBoardDataSend(PROTO_CMD_SOUND_SET, data, 1);
        HalWaitMs(100);
        inited = true;
    }
}

static void checkKVMConfigs(void)
{
    KVMConfig_t config;
    SysFlashRead(HAL_FLASH_ADDR_CONFIGS, (uint8_t *)&config, sizeof(KVMConfig_t));
    if(config.flag != KVM_CONFIG_VALID_FLAG)
    {
        setKVMFactoryConfig();
    }
    else
    {
    
        //hardware config
        inputSwitchControl(KVM_DST_DEVICE_KVM1, (KVMSrcDevice_t)config.dstConnect[0]);
        inputSwitchControl(KVM_DST_DEVICE_KVM2, (KVMSrcDevice_t)config.dstConnect[1]);
        inputSwitchControl(KVM_DST_DEVICE_LCD, (KVMSrcDevice_t)config.dstConnect[2]);
        setPrinterConnect((KVMSrcDevice_t)config.printerConnect);
    }

}

void KVMInit(void)
{
    printCtrlIOInit();
    checkKVMConfigs();	
    KeyboardInit(kvmKeyHandle);
    ProtocolInit(kvmProtoHandle);
    SwitchBoardInit();
    DisplayCtrlInit();
}

void KVMPoll(void)
{
    switchBoardInit();
    KeyboardPoll();
    SwitchBoardPoll();
    ProtocolPoll();
    DisplayCtrlPoll();
    getAVMErrcode();
}

