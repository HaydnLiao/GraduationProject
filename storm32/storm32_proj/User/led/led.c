
#include "led.h"   

void Led_Init(void)
{
	GPIO_InitTypeDef ledInitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
	ledInitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
	ledInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	ledInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &ledInitStruct);
	GPIO_SetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13);
}

