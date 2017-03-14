
#include "stm32f10x.h"
#include "led.h"
#include "systick.h"

int main(void)
{
	Led_Init();
	Systick_Init();

	while(1)
	{
		LED0_ON;
		Delay_ms(500);
		LED1_ON;
		Delay_ms(500);
		LED0_OFF;
		Delay_ms(500);
		LED1_OFF;
		Delay_ms(500);
	}
}

