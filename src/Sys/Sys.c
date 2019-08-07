#include "Sys.h"
#include "Hal.h"
#include "KVM.h"


static void printChars(void)
{
    SysPrint("\n");
    SysPrint("\n");
    SysPrint("\n");    
    SysPrint("\n");
    SysPrint("***********************************************\n");
    SysPrint("---------------------start---------------------\n");
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



