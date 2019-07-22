#ifndef SYS_H
#define SYS_H

#include "Hal.h"

#define SysTime_t uint32_t
#define SysTime HalTimerCount
#define SysTimeHasPast(old, past) ((SysTime() - (old)) > (past)

#define SysPrint printf
#define SysLog(...) do{\
                    SysPrint("%s :", __FUNCTION__);\
                    SysPrint(__VA_ARGS__);\
                    SysPrint("\r\n");\
                    }while(0)

void SysInit(void);
void SysPoll(void);

#endif

