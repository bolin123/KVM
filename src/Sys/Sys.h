#ifndef SYS_H
#define SYS_H

#include "Hal.h"

#define SysTime_t uint32_t
#define SysTime HalTimerCount
#define SysTimeHasPast(old, past) ((SysTime() - (old)) > (past))

#define SysPrint printf
#define SysLog(...) do{\
                    SysPrint("%s :", __FUNCTION__);\
                    SysPrint(__VA_ARGS__);\
                    SysPrint("\r\n");\
                    }while(0)


uint8_t SysErrcodeGet(void);
void SysErrcodeSet(bool set, uint8_t err);

void SysFlashWrite(uint32_t addr, uint8_t *buff, uint16_t size);
void SysFlashRead(uint32_t addr, uint8_t *buff, uint16_t size);
void SysInit(void);
void SysPoll(void);

#endif

