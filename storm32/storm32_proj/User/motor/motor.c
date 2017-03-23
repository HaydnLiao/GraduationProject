
#include "motor.h"
#include <stdio.h>

void Motor_Init(void)
{
	SineArray_Init();
	Motor0_Init();
}

void SineArray_Init(void)
{
	uint16_t cntL = 0;
	sineArraySize = 1000 / SYSTEM_PERIOD / MOTOR_MAX_SPEED;
	phaseShift = sineArraySize / 3;
	currentStepA = 0;
	currentStepB = currentStepA + phaseShift;
	currentStepC = currentStepB + phaseShift;
	//printf("%d %d %d %d %d\r\n", sineArraySize, phaseShift, currentStepA, currentStepB, currentStepC);
	for(cntL = 0; cntL < sineArraySize; cntL++)
	{
		pwmSin[cntL] = (uint16_t)( ( sin( (cntL+1)*1.0/sineArraySize*2*MATH_PI )+1 )*TIM_PERIOD/2 );
		//printf("[%d]%d\r\n", cntL, pwmSin[cntL]);
	}
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

void Motor0_Run(mdir_t mdir, uint16_t speed)		//speed unit: °/s
{
	static uint16_t preSpeed = 0;
	static uint16_t timeout = 0;
	static uint16_t cntTime = 0;

	if(speed != preSpeed)
	{
		timeout = MOTOR_MAX_SPEED * 360 / speed / sineArraySize;	//faster speed shorter timeout
		preSpeed = speed;
	}
	if(speed == 0)
	{
		
	}
	else
	{
		if(cntTime < timeout)
		{
			printf("[%d]%d %d\r\n", cntTime, timeout, speed);
			cntTime += 1;
		}
		else
		{
			cntTime = 0;	
			//TIM_Cmd(TIM3, DISABLE);
			//Motor0_Disable();
			TIM_SetCompare2(TIM3, pwmSin[currentStepA]);
			TIM_SetCompare3(TIM3, pwmSin[currentStepB]);
			TIM_SetCompare4(TIM3, pwmSin[currentStepC]);
			//Motor0_Enable();
			//TIM_Cmd(TIM3, ENABLE);
		
			//uint16_t has not negative so add sineArraySize
			currentStepA = (currentStepA + sineArraySize + mdir_cal_factor[mdir]) % sineArraySize;
			currentStepB = (currentStepB + sineArraySize + mdir_cal_factor[mdir]) % sineArraySize;
			currentStepC = (currentStepC + sineArraySize + mdir_cal_factor[mdir]) % sineArraySize;
			//printf("%d %d %d\r\n", pwmSin[currentStepA], pwmSin[currentStepB], pwmSin[currentStepC]);
		}
	}
}

void Motor0_Disable(void)
{
	TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Disable);
	TIM_CCxCmd(TIM3, TIM_Channel_3, TIM_CCx_Disable);
	TIM_CCxCmd(TIM3, TIM_Channel_4, TIM_CCx_Disable);
}

void Motor0_Enable(void)
{
	TIM_CCxCmd(TIM3, TIM_Channel_2, TIM_CCx_Enable);
	TIM_CCxCmd(TIM3, TIM_Channel_3, TIM_CCx_Enable);
	TIM_CCxCmd(TIM3, TIM_Channel_4, TIM_CCx_Enable);
}


