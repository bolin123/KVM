#ifndef HAL_H
#define HAL_H

#include "HalCtype.h"
#include "HalGPIO.h"
#include "HalTimer.h"
#include "HalUart.h"
#include "HalExti.h"

typedef enum
{
    HAL_UART_PORT_DBG = 0, //uart1
	HAL_UART_PORT_KVM,     //uart2
	HAL_UART_PORT_KVM2KEY, //uart3
	HAL_UART_PORT_PRINT,   //uart4
	HAL_UART_PORT_DISPLAY, //uart5
	HAL_UART_PORT_KVM1KEY, //uart6
}HalUartPort_t;

uint32_t HalTimerCount(void);
void HalReboot(void);
void HalInit(void);
void HalPoll(void);
#endif

