#include "Sys.h"
#include "Hal.h"
#include "KVM.h"

#define SYS_FIRMWARE_VERSION "1.0.1"

static uint8_t g_errcode = 0;

uint8_t SysErrcodeGet(void)
{
    return g_errcode;
}

void SysErrcodeSet(bool set, uint8_t err)
{
    if(set)
    {
        g_errcode |= err;
    }
    else
    {
        g_errcode &= ~err;
    }
}

void SysFlashWrite(uint32_t addr, uint8_t *buff, uint16_t size)
{
    HalFlashWrite(addr, buff, size);
}

void SysFlashRead(uint32_t addr, uint8_t *buff, uint16_t size)
{
    HalFlashRead(addr, buff, size);
}

static void printChars(void)
{
    SysPrint("\n");
    SysPrint("\n");
    SysPrint("\n");    
    SysPrint("\n");
    SysPrint("***********************************************\n");
    SysPrint("*** Firmware Version %s\n", SYS_FIRMWARE_VERSION);
    SysPrint("*** Build Date:%s %s\n", __DATE__, __TIME__);
    SysPrint("*** FPGA ID = %02x\n", HalI2CReadByte(HAL_FPGA_IIC_ADDR, 0x01));
    SysPrint("***********************************************\n");
}

void SysInit(void)
{
    HalInit();
    KVMInit();
    printChars();
}

void SysPoll(void)
{
    HalPoll();
    KVMPoll();
}



