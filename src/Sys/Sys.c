#include "Sys.h"
#include "Hal.h"
#include "KVM.h"

#define SYS_FIRMWARE_VERSION "1.0.1"

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



