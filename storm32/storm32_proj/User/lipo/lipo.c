
#include "lipo.h"

float Lipo_Voltage = 0.0;

void Lipo_Init(void)
{
	LipoGPIO_Init();
	LipoADC_Init();
}

void LipoGPIO_Init(void)
{
	GPIO_InitTypeDef gpioInitStruct;
	/**
		PA5 -> ADC12_IN5
	*/

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	gpioInitStruct.GPIO_Mode = GPIO_Mode_AIN;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOA, &gpioInitStruct);
}

void LipoADC_Init(void)
{
	ADC_InitTypeDef adcInitStruct;
	NVIC_InitTypeDef nvicInitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);

	adcInitStruct.ADC_Mode = ADC_Mode_Independent;
	adcInitStruct.ADC_ScanConvMode = DISABLE;//single channel
	adcInitStruct.ADC_ContinuousConvMode = ENABLE;
	adcInitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//use software trigger
	adcInitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	adcInitStruct.ADC_NbrOfChannel = 1;//one convert channel
	ADC_Init(ADC2, &adcInitStruct);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);//PCLK2/8 = 9MHz
	ADC_RegularChannelConfig(ADC2, ADC_Channel_5, 1, ADC_SampleTime_55Cycles5);//sample time 55.5 periods
	ADC_ITConfig(ADC2, ADC_IT_EOC, ENABLE);//adc end of convertion interrupt
	ADC_Cmd(ADC2, ENABLE);

	ADC_ResetCalibration(ADC2);
	while(ADC_GetResetCalibrationStatus(ADC2));
	ADC_StartCalibration(ADC2);
	while(ADC_GetCalibrationStatus(ADC2));

	ADC_SoftwareStartConvCmd(ADC2, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	nvicInitStruct.NVIC_IRQChannel = ADC1_2_IRQn;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 4;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicInitStruct);
}

void ADC1_2_IRQHandler(void)
{
	//if(ADC_GetITStatus(ADC2, ADC_IT_EOC) == SET)
	{
		ADC2_Value = ADC_GetConversionValue(ADC2);
		//ADC_ClearITPendingBit(ADC2, ADC_IT_EOC);
	}
}

void Lipo_CalVoltage(float calWeight)
{
	//static float temp = 0.0;
	/**
		1936 -> 12.00v
		1612 -> 10.00v
		1291 -> 8.00v
		969  -> 6.00v
		647  -> 4.00v
	*/
	//Lipo_Voltage = (float)ADC2_Value/4096*3.3;
	//Lipo_Voltage = calWeight*Lipo_Voltage + (1-calWeight)*ADC2_Value * 0.0062 - 0.0161;//fitted equation
	
	//Lipo_Voltage = calWeight*Lipo_Voltage + (1-calWeight)*(ADC2_Value/4096*3.3/1.5*11.5);
	if(Lipo_Voltage < LIPO_VOL_LOWEST)
	{
		Lipo_Voltage = ADC2_Value*0.0061767578125;
	}
	else
	{
		Lipo_Voltage = calWeight*Lipo_Voltage + (1-calWeight)*(ADC2_Value*0.0061767578125);
	}
	/**if(Lipo_Voltage < 0.01)
	{
		Lipo_Voltage = 0.0;
	}*/
	//temp = ADC2_Value*0.0061767578125;
	//DEBUG_PRINT("lipo:%d %f\r\n", ADC2_Value, Lipo_Voltage);
	//DEBUG_PRINT("%f,%f\r\n", temp, Lipo_Voltage);
}

