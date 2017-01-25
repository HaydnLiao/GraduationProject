
#include "stm32f10x.h"
#include "led.h"
#include "systick.h"
#include "uart1.h"
#include "mpu6050.h"


extern float Mpu6050_Temp;		//Temperature calculated result

int main(void)
{
	uint8_t data;
	LEDR_ON;
	Led_Init();
	Systick_Init();
	Uart1_Init((uint32_t)115200);
	Mpu6050_Init();
	LEDR_OFF;
	
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

	data = 0x01;
	if(I2C1_WriteData(DEVICE_ADDRESS, PWR_MGMT_1, &data, 1))
	{
		printf("Error-pwr1!\r\n");
	}
	data = 0x00;
	if(I2C1_WriteData(DEVICE_ADDRESS, PWR_MGMT_2, &data, 1))
	{
		printf("Error-pwr2!\r\n");
	}
	
	data = 0x03;
	if(I2C1_WriteData(DEVICE_ADDRESS, 0x1A, &data, 1))
	{
		printf("Error-1a!\r\n");
	}
	
	while(1)
	{
		data = 0xff;
		if(I2C1_ReadData(DEVICE_ADDRESS, 0x75, &data, 1))
		{
			printf("Error-75!\r\n");
		}
		else
		{
			//printf("ID: 0x%x\r\n", data);
		}
		if(Mpu6050_GetAccelData())
		{
			printf("Error-GetAccelData");
		}
		else
		{
			//printf("ax: %d ay: %d az: %d\r\n", AccelData[0], AccelData[1], AccelData[2]);
		}
		if(Mpu6050_GetTempData())
		{
			printf("Error-GetTempData");
		}
		else
		{
			printf("%.2f¡æ\r\n", Mpu6050_Temp);
		}
		if(Mpu6050_GetGyroData())
		{
			printf("Error-GetGyroData");
		}
		else
		{
			//printf("gx: %d gy: %d gz: %d\r\n", GyroData[0], GyroData[1], GyroData[2]);
		}

		//LEDG_ON;
		//Delay_ms(1000);
		//LEDG_OFF;
		Delay_ms(50);
	}
	//return 0;
}

