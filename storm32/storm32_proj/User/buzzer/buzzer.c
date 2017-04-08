
#include "buzzer.h"

void Buzzer_Init(void)
{
	GPIO_InitTypeDef gpioInitSturct;
	/**
		BUT -> pc3
	*/

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	gpioInitSturct.GPIO_Mode = GPIO_Mode_Out_PP;
	gpioInitSturct.GPIO_Speed = GPIO_Speed_50MHz;
	gpioInitSturct.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOC, &gpioInitSturct);

	GPIO_ResetBits(GPIOC, GPIO_Pin_3);
}
