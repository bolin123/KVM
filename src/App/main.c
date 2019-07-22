#include "Sys.h"

int main(void)
{
    SysInit();
    while(1)
    {
        SysPoll();
    }
}


