#include "HalExti.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_gpio.h"

extern void PS2ClkTrigger(void);
void EXTI15_10_IRQHandler(void)
{
    //PS2BitRecv(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11));
    PS2ClkTrigger();
    EXTI_ClearITPendingBit(EXTI_Line12);
}

void HalExtiInit(void)
{
	/*
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_11; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);
*/
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, GPIO_PinSource12);

    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line12; 
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt; 
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE; 
    EXTI_Init(&EXTI_InitStructure);
    EXTI_ClearITPendingBit(EXTI_Line12);

    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the USARTx Interrupt */ 
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure); 
    
}

