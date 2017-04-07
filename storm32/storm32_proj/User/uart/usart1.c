
#include "usart1.h"
//#include "stm32f10x.h"
//#include <stdio.h>

uint8_t usart1RxBuff[USART1_RX_LEN] = {0};
volatile uint16_t cntUsart1RxBuff = 0;

void Usart1_Init(uint32_t bps)
{
	GPIO_InitTypeDef gpioInitStruct;
	USART_InitTypeDef usartInitStruct;
	NVIC_InitTypeDef nvicInitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1, ENABLE);
	
	gpioInitStruct.GPIO_Pin = GPIO_Pin_9;
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInitStruct);
	gpioInitStruct.GPIO_Pin = GPIO_Pin_10;
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &gpioInitStruct);

	USART_DeInit(USART1);
	usartInitStruct.USART_BaudRate = bps;
	usartInitStruct.USART_WordLength = USART_WordLength_8b;
	usartInitStruct.USART_Parity = USART_Parity_No;
	usartInitStruct.USART_StopBits = USART_StopBits_1;
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &usartInitStruct);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	nvicInitStruct.NVIC_IRQChannel = USART1_IRQn;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 2;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicInitStruct);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
}

void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		usart1RxBuff[cntUsart1RxBuff] = USART_ReceiveData(USART1) & 0xff;
		cntUsart1RxBuff = (cntUsart1RxBuff+1) % USART1_RX_LEN;
		//USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t)ch);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	return (int)ch;
}

int fgetc(FILE *f)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	return (int)USART_ReceiveData(USART1);
}

