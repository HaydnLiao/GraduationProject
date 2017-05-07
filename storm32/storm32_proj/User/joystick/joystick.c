
#include "joystick.h"

uint16_t Joystick_X = 0, Joystick_Y = 0;
__IO uint16_t Joystick_Z = 0;

void Joystick_Init(void)
{
	JoystickGPIO_Init();
	JoystickADC_Init();
	//JoystickEXTI_Init();
	JoystickPara_Init();
}

void JoystickGPIO_Init(void)
{
	GPIO_InitTypeDef gpioInitStruct;
	/**
		X -> PC0 ADC12_IN10
		Y -> PC1 ADC12_IN11
		Z -> PC2 EXTI2
	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	gpioInitStruct.GPIO_Mode = GPIO_Mode_AIN;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOC, &gpioInitStruct);
	gpioInitStruct.GPIO_Mode = GPIO_Mode_IPD;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOC, &gpioInitStruct);
}

void JoystickADC_Init(void)
{
	DMA_InitTypeDef dmaInitStruct;
	ADC_InitTypeDef adcInitStruct;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	DMA_DeInit(DMA1_Channel1);
	dmaInitStruct.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC1->DR));
	dmaInitStruct.DMA_MemoryBaseAddr = (uint32_t)ADC1_Value;
	dmaInitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
	dmaInitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dmaInitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dmaInitStruct.DMA_BufferSize = JOY_ADC_CHN_NUM;
	dmaInitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dmaInitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable; 
	dmaInitStruct.DMA_Mode = DMA_Mode_Circular;
	dmaInitStruct.DMA_Priority = DMA_Priority_High;
	dmaInitStruct.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &dmaInitStruct);

	DMA_Cmd(DMA1_Channel1 , ENABLE);
	
	adcInitStruct.ADC_Mode = ADC_Mode_Independent;
	adcInitStruct.ADC_ScanConvMode = ENABLE;//multichannel
	adcInitStruct.ADC_ContinuousConvMode = ENABLE;
	adcInitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//use software trigger
	adcInitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	adcInitStruct.ADC_NbrOfChannel = JOY_ADC_CHN_NUM;
	ADC_Init(ADC1, &adcInitStruct);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);//PCLK2/8 = 9MHz
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5);//sample time 55.5 periods
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_55Cycles5);//sample time 55.5 periods

	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
/**
void JoystickEXTI_Init(void)
{
	NVIC_InitTypeDef nvicInitStruct;
	EXTI_InitTypeDef extiInitStruct;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	nvicInitStruct.NVIC_IRQChannel = EXTI2_IRQn;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 6;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicInitStruct);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);
	extiInitStruct.EXTI_Line = EXTI_Line2;
	extiInitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	extiInitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	extiInitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&extiInitStruct);
}

void EXTI2_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line2) == SET)
	{
		Joystick_Z ^= 0x01;
		EXTI_ClearITPendingBit(EXTI_Line2);
	}  
}
*/

void JoystickPara_Init(void)
{
	/**
		X 	-> yaw		4096~0	-> +55~-55 degree
		Y 	-> pitch	4096~0	-> +55~-55 degree
	*/
	K_X = (JOY_ANGLE_X_LOWER - JOY_ANGLE_X_UPPER) * 1.0 / (JOY_ANALOG_LOWER - JOY_ANALOG_UPPER);
	B_X = JOY_ANGLE_X_LOWER - K_X * JOY_ANALOG_LOWER;
	K_Y = (JOY_ANGLE_Y_LOWER - JOY_ANGLE_Y_UPPER) * 1.0 / (JOY_ANALOG_LOWER - JOY_ANALOG_UPPER);
	B_Y = JOY_ANGLE_Y_LOWER - K_Y * JOY_ANALOG_LOWER;
	//DEBUG_PRINT("%f,%f,%f,%f ",K_X, K_Y, B_X, B_Y);
}

void Joystick_CalXY(float calWeight)
{
	Joystick_X = (uint16_t)(calWeight*Joystick_X + (1-calWeight)*ADC1_Value[0]);
	Joystick_Y = (uint16_t)(calWeight*Joystick_Y + (1-calWeight)*ADC1_Value[1]);
	//DEBUG_PRINT("%d,%d ", Joystick_X, Joystick_Y);
}

/**
void Joystick_CalZ(void)
{
	static uint8_t cntKeyDown = 0;
	if(cntKeyDown < JOY_KEY_DOWN_PERIOD)
	{
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2) == 0x00)
		{
			cntKeyDown += 1;
		}
		else
		{
			cntKeyDown = 0;
		}
	}
	else if(cntKeyDown < JOY_KEY_DOWN_PERIOD*2)
	{
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2) == 0x01)
		{
			cntKeyDown += 1;
		}
		else
		{
			cntKeyDown = JOY_KEY_DOWN_PERIOD;
		}
	}
	else
	{
		cntKeyDown = 0;
		Joystick_Z ^= 0x01;
	}
	DEBUG_PRINT("%d\r\n", Joystick_Z);
}
*/

//0: AutoMode, 1: ManualMode;
uint8_t Joystick_ObtainMode(void)
{
	static uint8_t cntKeyDown = 0;
	if(cntKeyDown < JOY_KEY_DOWN_PERIOD)
	{
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2) == 0x00)
		{
			cntKeyDown += 1;
		}
		else
		{
			cntKeyDown = 0;
		}
	}
	else if(cntKeyDown < JOY_KEY_DOWN_PERIOD*2)
	{
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2) == 0x01)
		{
			cntKeyDown += 1;
		}
		else
		{
			cntKeyDown = JOY_KEY_DOWN_PERIOD;
		}
	}
	else
	{
		cntKeyDown = 0;
		Joystick_Z ^= 0x01;
	}
	//DEBUG_PRINT("%d\r\n", Joystick_Z);

	return (Joystick_Z&0x01);
}

//convert analog values into expect angles
void Joystick_ConvertAngle(float* xAngle, float* yAngle)
{
	uint16_t xCal, yCal;
	xCal = INTERVAL_CONSTRAINT(Joystick_X - JOY_ANALOG_BIAS_X, JOY_ANALOG_UPPER, JOY_ANALOG_LOWER);
	yCal = INTERVAL_CONSTRAINT(Joystick_Y - JOY_ANALOG_BIAS_Y, JOY_ANALOG_UPPER, JOY_ANALOG_LOWER);
	//DEBUG_PRINT("%d,%d ", xCal, yCal);
	*xAngle = K_X * xCal + B_X;
	*yAngle = K_Y * yCal + B_Y;
}

