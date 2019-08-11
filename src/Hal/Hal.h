#ifndef HAL_H
#define HAL_H

#include "HalCtype.h"
#include "HalGPIO.h"
#include "HalTimer.h"
#include "HalUart.h"
#include "HalExti.h"
#include "HalI2C.h"

typedef enum
{
    HAL_UART_PORT_DBG = 0, //uart1 调试串口
	HAL_UART_PORT_SWITCH,  //uart2 音视频矩阵切换板串口
	HAL_UART_PORT_KVM2KEY, //uart3 检测 KVM2 按键串口
	HAL_UART_PORT_PRINT,   //uart4 打印机控制器串口
	HAL_UART_PORT_DISPLAY, //uart5 显示屏控制器串口
	HAL_UART_PORT_KVM1KEY, //uart6 检测 KVM1 按键串口
}HalUartPort_t;

#define HAL_PRINTER_IO_EN 0x40 //pe0
#define HAL_PRINTER_IO_C0 0x42 //pe2
#define HAL_PRINTER_IO_C1 0x43 //pe3
#define HAL_PRINTER_IO_C2 0x44 //pe4

#define HAL_FPGA_IIC_ADDR 0x90

void HalIRQEnable(bool enable);
uint32_t HalTimerCount(void);
void HalReboot(void);
void HalInit(void);
void HalPoll(void);
#endif

