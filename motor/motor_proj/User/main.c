
#include "stm32f10x.h"
#include "led.h"
#include "systick.h"
#include "uart1.h"
#include "motor.h"

int main(void)
{
	Led_Init();
	Systick_Init();
	Uart1_Init(115200);
	Motor_Initiate();
	while(1)
	{
		Motor_Turn(90, 1);
		/**
		LEDG_ON;
		Delay_ms(500);
		LEDG_OFF;
		Delay_ms(200);
		*/
	}
}

