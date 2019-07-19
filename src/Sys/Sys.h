#ifndef SYS_H
#define SYS_H

#include "Hal.h"

#define SysTime_t uint32_t
#define SysTime HalTimerCount
#define SysTimeHasPast(old, past) ((SysTime() - (old)) > (past))

void SysInit(void);
void SysPoll(void);

#endif
