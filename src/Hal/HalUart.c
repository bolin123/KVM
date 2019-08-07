#include "HalUart.h"

#define HAL_UART_PORT_NUM 6

static HalUartConfig_t g_uartConfig[HAL_UART_PORT_NUM];
static USART_TypeDef *g_uart[HAL_UART_PORT_NUM] = {USART1, USART2, USART3, UART4, UART5, USART6};

void HalUartConfig(uint8_t uart, HalUartConfig_t *config)
{   
    USART_InitTypeDef uartcfg;
    USART_TypeDef *uartNo;
    
    USART_StructInit(&uartcfg);
    if(uart >= HAL_UART_PORT_NUM)
    {
        return;
    }
    uartNo = g_uart[uart];
    g_uartConfig[uart] = *config;

    USART_Cmd(uartNo, DISABLE);
    USART_ITConfig(uartNo, USART_IT_RXNE, DISABLE);
    
    uartcfg.USART_BaudRate = config->baudrate;
    uartcfg.USART_WordLength = config->wordLength;
    uartcfg.USART_Parity = config->parity;
    uartcfg.USART_StopBits = USART_StopBits_1;
    uartcfg.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    uartcfg.USART_HardwareFlowControl = config->flowControl;

    USART_Init(uartNo, &uartcfg);

    USART_ClearITPendingBit(uartNo, USART_IT_RXNE);
    USART_ITConfig(uartNo, USART_IT_RXNE, ENABLE);
    
    USART_Cmd(uartNo, ENABLE);
}

static void nvicInit(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


static void ioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
#if 1
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1); 
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

    //uart1 io
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;  //TX
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //??????
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //??
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
     
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;  //RX
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //??????
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //??
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); //
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); //
    //uart2 io
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;  //TX
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //??????
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //??
    GPIO_Init(GPIOA, &GPIO_InitStructure);
     
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;  //RX
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //??????
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //??
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //uart3 io
    //GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3); //
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3); //
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;  //TX
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //??????
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //??
    GPIO_Init(GPIOC, &GPIO_InitStructure);
     
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;  //RX
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //??????
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //??
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    //uart4 io
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4); //Tx
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4); //
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;  //TX
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);
     
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;  //RX
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //uart5 io
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5); //Tx
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5); //
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;  //TX
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_Init(GPIOC, &GPIO_InitStructure);
     
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;  //RX
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    //uart6 io
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6); //Tx
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6); //
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;  //TX
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_Init(GPIOC, &GPIO_InitStructure);
     
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;  //RX
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif
}

void HalUartInitialize(void)
{
    nvicInit();
    ioInit();
}

void HalUartPoll(void)
{
}

void HalUartWrite(uint8_t uart, const uint8_t *data, uint16_t len)
{
    uint16_t i;
    USART_TypeDef *uartNo;
    if(uart >= HAL_UART_PORT_NUM)
    {
        return;
    }
    uartNo = g_uart[uart];

    for(i = 0; i < len; i++)
    {
        USART_SendData(uartNo, (uint16_t)data[i]);
        while(USART_GetFlagStatus(uartNo, USART_FLAG_TC) == RESET);
    }
    
}

void USART1_IRQHandler(void)
{
    uint8_t data;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
        data = USART_ReceiveData(USART1);
        if(g_uartConfig[0].recvCb != NULL)
        {
            g_uartConfig[0].recvCb(0, &data, 1);
        }
    }
}

void USART2_IRQHandler(void)
{
    uint8_t data;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
        data = USART_ReceiveData(USART2);
        if(g_uartConfig[1].recvCb != NULL)
        {
            g_uartConfig[1].recvCb(1, &data, 1);
        }
    }
}

void USART3_IRQHandler(void)
{
    uint8_t data;
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
        data = USART_ReceiveData(USART3);
        if(g_uartConfig[2].recvCb != NULL)
        {
            g_uartConfig[2].recvCb(2, &data, 1);
        }
    }
}

void UART4_IRQHandler(void)
{
    uint8_t data;
    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(UART4, USART_IT_RXNE);
        data = USART_ReceiveData(UART4);
        if(g_uartConfig[3].recvCb != NULL)
        {
            g_uartConfig[3].recvCb(3, &data, 1);
        }
    }
}

void UART5_IRQHandler(void)
{
    uint8_t data;
    if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(UART5, USART_IT_RXNE);
        data = USART_ReceiveData(UART5);
        if(g_uartConfig[4].recvCb != NULL)
        {
            g_uartConfig[4].recvCb(4, &data, 1);
        }
    }
}


void USART6_IRQHandler(void)
{
    uint8_t data;
    if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)
    {
        USART_ClearITPendingBit(USART6, USART_IT_RXNE);
        data = USART_ReceiveData(USART6);
        if(g_uartConfig[5].recvCb != NULL)
        {
            g_uartConfig[5].recvCb(5, &data, 1);
        }
    }
}



