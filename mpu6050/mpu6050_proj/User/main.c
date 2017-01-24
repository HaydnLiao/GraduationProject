
#include "stm32f10x.h"
#include "led.h"
#include "systick.h"
#include "uart1.h"
#include "i2c1.h"

int main(void)
{
	uint8_t data;
	LEDR_ON;
	Led_Init();
	Systick_Init();
	Uart1_Init((uint32_t)115200);
	I2C1_Init(400000);	//400kHz
	printf("abc\r\n");
	LEDR_OFF;
	/**
	Delay_ms(100);
	data = 0x80;
	if(I2C1_WriteData(DEVICE_ADDRESS, 0x6b, &data, 1))
	{
		printf("Error-6b-1!\r\n");
	}
	Delay_ms(100);
	data = 0x00;
	if(I2C1_WriteData(DEVICE_ADDRESS, 0x6b, &data, 1))
	{
		printf("Error-6b-2!\r\n");
	}
	if(I2C1_WriteData(DEVICE_ADDRESS, 0x38, &data, 1))
	{
		printf("Error-38!\r\n");
	}
	if(I2C1_WriteData(DEVICE_ADDRESS, 0x6a, &data, 1))
	{
		printf("Error-6a!\r\n");
	}
	if(I2C1_WriteData(DEVICE_ADDRESS, 0x23, &data, 1))
	{
		printf("Error-23!\r\n");
	}
	data = 0x80;
	if(I2C1_WriteData(DEVICE_ADDRESS, 0x37, &data, 1))
	{
		printf("Error-37!\r\n");
	}
	*/
	while(1)
	{
		data = 0xff;
		if(I2C1_ReadData(DEVICE_ADDRESS, 0x75, &data, 1))
		{
			printf("Error-75!\r\n");
		}
		else
		{
			printf("0x%x\r\n", data);
		}
		//LEDG_ON;
		//Delay_ms(1000);
		//LEDG_OFF;
		Delay_ms(1000);
	}
	//return 0;
}

