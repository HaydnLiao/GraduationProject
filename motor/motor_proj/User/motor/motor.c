
#include "motor.h"

void TIM1_GPIO_Config(void)
{
	GPIO_InitTypeDef gpioInitStruct;

	/** TIM1 GPIO Full Remap
	TIM1_CH1N	--> PE8
	TIM1_CH1	--> PE9
	TIM1_CH2N	--> PE10
	TIM1_CH2	--> PE11
	TIM1_CH3N	--> PE12
	TIM1_CH3	--> PE13
	TIM1_BKIN	--> PE15
	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE);	

	gpioInitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &gpioInitStruct);
}

void TIM1_PWM_Config(void)

{
	TIM_TimeBaseInitTypeDef timeBaseInitStruct;
	TIM_OCInitTypeDef		ocInitStruct;
	TIM_BDTRInitTypeDef		bdtrInitStruct;

	TIM_DeInit(TIM1);

	timeBaseInitStruct.TIM_Prescaler = TIM_PRESCALER;
	timeBaseInitStruct.TIM_Period = TIM_PERIOD;
	timeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	timeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	timeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &timeBaseInitStruct);

	ocInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	ocInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	ocInitStruct.TIM_OutputNState = TIM_OutputNState_Enable;
	ocInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	ocInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;
	ocInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
	ocInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	ocInitStruct.TIM_Pulse = 0;
	TIM_OC1Init(TIM1, &ocInitStruct);
	TIM_OC2Init(TIM1, &ocInitStruct);
	TIM_OC3Init(TIM1, &ocInitStruct);

	TIM_ARRPreloadConfig(TIM1, ENABLE);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

	bdtrInitStruct.TIM_OSSRState = TIM_OSSRState_Disable;
	bdtrInitStruct.TIM_OSSIState = TIM_OSSIState_Disable;
	bdtrInitStruct.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
	bdtrInitStruct.TIM_DeadTime = TIM_DEADTIME;
	bdtrInitStruct.TIM_Break = TIM_Break_Disable;
	bdtrInitStruct.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
	TIM_BDTRConfig(TIM1, &bdtrInitStruct);

	TIM_Cmd(TIM1, ENABLE);
	//TIM_CCPreloadControl(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

void Motor_Initiate(void)
{	
	TIM1_GPIO_Config();
	TIM1_PWM_Config();
	Motor_ShutDown();
}

void Motor_ShutDown(void)
{
	TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable);
    TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);
    TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
    TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Disable);
    TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
    TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Disable);
}

void Motor_Turn(uint8_t speed, uint8_t direction)
{
	const uint8_t MaxSteps = 6;
	static uint8_t step = 0;
	uint16_t pulseWidth;
	pulseWidth = speed*TIM_PERIOD/100;

	Motor_ShutDown();
	switch(step)
	{
		case 0:
			TIM_SetCompare1(TIM1, pulseWidth);
            TIM_SetCompare3(TIM1, TIM_PERIOD);
			TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable);   //u+
    		TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable); //w-
		break;
		case 1:
			TIM_SetCompare2(TIM1, pulseWidth);
			TIM_SetCompare3(TIM1, TIM_PERIOD);
			TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable);   //v+
    		TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable); //w-
		break;
		case 2:
			TIM_SetCompare2(TIM1, pulseWidth);
            TIM_SetCompare1(TIM1, TIM_PERIOD);
			TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable);   //v+
    		TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable); //u-

		break;
		case 3:
			TIM_SetCompare3(TIM1, pulseWidth);
            TIM_SetCompare1(TIM1, TIM_PERIOD);
			TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable);   //w+
    		TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable); //u-

		break;
		case 4:
			TIM_SetCompare3(TIM1, pulseWidth);
            TIM_SetCompare2(TIM1, TIM_PERIOD);
			TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable);   //w+
    		TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable); //v-
		break;
		case 5:
			TIM_SetCompare1(TIM1, pulseWidth);
            TIM_SetCompare2(TIM1, TIM_PERIOD);
			TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable);   //u+
    		TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable); //v-

		break;
		default:
		break;
	}
	if(direction)
	{
		step = (step+1) % MaxSteps;
	}
	else
	{
		if(0 == step)
		{
			step = MaxSteps;
		}
		step = (step-1) % MaxSteps;
	}
}

