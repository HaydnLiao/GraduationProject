
#include "stm32f10x.h"
#include "led.h"
#include "systick.h"
#include "uart1.h"
#include "mpu6050.h"

int main(void)
{
	uint8_t data;
	Led_Init();
	Systick_Init();
	Uart1_Init((uint32_t)115200);
	LEDR_ON;
	data = Mpu6050_Init(100, 1);//sample rate 50Hz enable DLPF
	while(data)
	{
		printf("Error-%d\r\n", data);
		Delay_ms(200);
		data = Mpu6050_Init(50, 1);//sample rate 50Hz enable DLPF
	}
	LEDR_OFF;

	while(1)
	{
		if(Mpu6050_GetAccelData())
		{
			printf("Error-GetAccelData\r\n");
		}
		else
		{
			printf("ax: %-5d ay: %-5d az: %-5d\t", Mpu6050_Accel_X, Mpu6050_Accel_Y, Mpu6050_Accel_Z);
		}
		if(Mpu6050_GetTempData())
		{
			printf("Error-GetTempData\r\n");
		}
		else
		{
			printf("%-5.2f¡æ\t", Mpu6050_Temp);
		}
		if(Mpu6050_GetGyroData())
		{
			printf("Error-GetGyroData\r\n");
		}
		else
		{
			printf("gx: %-5d gy: %-5d gz: %-5d\r\n", Mpu6050_Gyro_X, Mpu6050_Gyro_Y, Mpu6050_Gyro_Z);
		}

		//LEDG_ON;
		//Delay_ms(1000);
		//LEDG_OFF;
		Delay_ms(50);
	}
}

