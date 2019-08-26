#include "Hal.h"

static uint32_t g_timerCount = 0;

//redirect "printf()"
int fputc(int ch, FILE *f)
{
	HalUartWrite(HAL_UART_PORT_DBG, (const uint8_t *)&ch, 1);
	return ch;
}

void HalTimerPast1ms(void)
{
	g_timerCount++;
}

uint32_t HalTimerCount(void)
{
	return g_timerCount;
}

static void halClkInit(void)
{

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
		/**/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

}

static void logPrintInit(void)
{
    HalUartConfig_t config;
    config.baudrate = 115200;
    config.flowControl = 0;
    config.parity = 1;
    config.wordLength = USART_WordLength_8b;
    config.recvCb = NULL;
    HalUartConfig(HAL_UART_PORT_DBG, &config);
}

void HalIRQEnable(bool enable)
{
    if(enable)
    {
        __enable_irq();
    }
    else
    {
        __disable_irq(); 
    }
}

void HalReboot(void)
{
	printf("Sys reboot!!!\n");
	__set_FAULTMASK(1); 
    NVIC_SystemReset();
}

#if !defined(HAL_OLD_KVM_BOARD)
static void inputSwitchIOInit(void)
{
    HalGPIOConfig(0x4c, HAL_IO_OUTPUT); //pe12
    HalGPIOConfig(0x4d, HAL_IO_OUTPUT); //pe13
    HalGPIOConfig(0x4e, HAL_IO_OUTPUT); //pe14
    HalGPIOConfig(0x4f, HAL_IO_OUTPUT); //pe15

    HalGPIOConfig(0x04, HAL_IO_OUTPUT); //pa4
    HalGPIOConfig(0x05, HAL_IO_OUTPUT); //pa5
    HalGPIOConfig(0x06, HAL_IO_OUTPUT); //pa6
    HalGPIOConfig(0x07, HAL_IO_OUTPUT); //pa6
    
    HalGPIOConfig(0x11, HAL_IO_OUTPUT); //pb1
    HalGPIOConfig(0x10, HAL_IO_OUTPUT); //pb0
    HalGPIOConfig(0x24, HAL_IO_OUTPUT); //pc4
    HalGPIOConfig(0x25, HAL_IO_OUTPUT); //pc5

    
    HalGPIOSetLevel(0x4f, 0);
    HalGPIOSetLevel(0x11, 0);
    HalGPIOSetLevel(0x07, 0);
}
#endif

void HalInit(void)
{
	SystemInit();
	halClkInit();
	HalGPIOInitialize();
	HalUartInitialize();
	HalTimerInitialize();
#if defined(HAL_OLD_KVM_BOARD)
    HalI2CInit();
#else
    inputSwitchIOInit();
#endif
	//HalExtiInit();
	logPrintInit();
}

void HalPoll(void)
{
}

