
#include "systick.h"
#include <stdio.h>

void Systick_Init(void)
{
	if(SysTick_Config(SystemCoreClock / 1000000))//1us
	{		
		printf("Error:Systick_Init()\r\n");
		while(1);
	}
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;//关闭滴答定时器
	NVIC_SetPriority(SysTick_IRQn, 0);	//设为最高优先级
}

void Delay_us(uint32_t delayedTime)
{ 
	SystickDelayedTime = delayedTime;
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
	while(SystickDelayedTime != 0);
}

void CalledBySystickHandler(void)
{
	if(SystickDelayedTime > 0)
	{
		SystickDelayedTime -= 1;
	}
}

