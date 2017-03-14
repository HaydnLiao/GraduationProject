
#include "systick.h"

void Systick_Init(void)
{
	if(SysTick_Config(SystemCoreClock / 1000000))//1us
	{
		while(1);
	}
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;//turn off systick
	NVIC_SetPriority(SysTick_IRQn, 0);	//set the highest priority
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

void SysTick_Handler(void)
{
	CalledBySystickHandler();
}

