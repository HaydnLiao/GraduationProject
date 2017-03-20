
#include "motor.h"

void Motor_Init(void)
{
	sineArraySize = sizeof(pwmSin)/sizeof(uint8_t);
	phaseShift = sineArraySize / 3;
	currentStepA = 0;
	currentStepB = currentStepA + phaseShift;
	currentStepC = currentStepB + phaseShift;
	
	Motor0_Init();
}

void Motor0_Init(void)
{
	GPIO_InitTypeDef gpioInit;
	TIM_TimeBaseInitTypeDef timeBaseInit;
	TIM_OCInitTypeDef ocInit;
	/**
		motor0 pitch	pb1 -> tim3_ch4
						pb0 -> tim3_ch3
						pa7 -> tim3_ch2
	*/

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	//pitch-gpio
	gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	gpioInit.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOA, &gpioInit);
	gpioInit.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOB, &gpioInit);

	//pitch-timer-timebase
	timeBaseInit.TIM_Prescaler = TIM_PRESCALER;
	timeBaseInit.TIM_Period = TIM_PERIOD;
	timeBaseInit.TIM_CounterMode = TIM_CounterMode_Up;
	timeBaseInit.TIM_ClockDivision = TIM_CKD_DIV1;
	timeBaseInit.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &timeBaseInit);

	//pitch-timer-ouputchannel
	ocInit.TIM_OCMode = TIM_OCMode_PWM1;
	ocInit.TIM_OutputState = TIM_OutputState_Enable;
	//ocInit.TIM_OutputNState = TIM_OutputNState_Disable;
	ocInit.TIM_OCPolarity = TIM_OCPolarity_High;
	//ocInit.TIM_OCNPolarity = TIM_OCNPolarity_High;
	ocInit.TIM_OCIdleState = TIM_OCIdleState_Reset;
	//ocInit.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	ocInit.TIM_Pulse = 0;
	TIM_OC2Init(TIM3, &ocInit);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC3Init(TIM3, &ocInit);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC4Init(TIM3, &ocInit);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
}

void Motor0_Run(mdir_t mdir, uint8_t speed)
{
	uint8_t increment = 1;
	//TIM_Cmd(TIM3, DISABLE);
	TIM_SetCompare2(TIM3, (uint16_t)(pwmSin[currentStepA]));
	currentStepA = (currentStepA + increment) % sineArraySize;
	TIM_SetCompare3(TIM3, (uint16_t)(pwmSin[currentStepB]));
	currentStepB = (currentStepB + increment) % sineArraySize;
	TIM_SetCompare4(TIM3, (uint16_t)(pwmSin[currentStepC]));
	currentStepC = (currentStepC + increment) % sineArraySize;
	//TIM_Cmd(TIM3, ENABLE);
}

