
#include "lipo.h"
#include <stdio.h>

float Lipo_Voltage = 0;

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

	gpioInitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	gpioInitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOA, &gpioInitStruct);
}

void LipoADC_Init(void)
{
	ADC_InitTypeDef adcInitStruct;
	NVIC_InitTypeDef nvicInitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	adcInitStruct.ADC_Mode = ADC_Mode_Independent;
	adcInitStruct.ADC_ScanConvMode = DISABLE;//single channel
	adcInitStruct.ADC_ContinuousConvMode = ENABLE;
	adcInitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//use software trigger
	adcInitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	adcInitStruct.ADC_NbrOfChannel = 1;//one convert channel
	ADC_Init(ADC1, &adcInitStruct);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);//PCLK2/8 = 9MHz
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_55Cycles5);//sample time 55.5 periods
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);//adc end of convertion interrupt
	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

	nvicInitStruct.NVIC_IRQChannel = ADC1_2_IRQn;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 4;
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicInitStruct);
}

void ADC1_2_IRQHandler(void)
{
	//if(ADC_GetITStatus(ADC1, ADC_IT_EOC) == SET)
	{
		ADC1_Value = ADC_GetConversionValue(ADC1);
		//ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
	}
}

void Lipo_CalVoltage(float calWeight)
{
	/**
		1936 -> 12.00v
		1612 -> 10.00v
		1291 -> 8.00v
		969  -> 6.00v
		647  -> 4.00v
	*/
	//Lipo_Voltage = (float)ADC1_Value/4096*3.3;
	Lipo_Voltage = calWeight*Lipo_Voltage + (1-calWeight)*ADC1_Value * 0.0062 - 0.0161;//fitted equation
	if(Lipo_Voltage < 0.01)
	{
		Lipo_Voltage = 0.0;
	}
	//printf("lipo:%d %f\r\n", ADC1_Value, Lipo_Voltage);
}

