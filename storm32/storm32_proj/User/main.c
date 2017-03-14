
#include "stm32f10x.h"
#include "led.h"
#include "systick.h"
#include "usart1.h"
#include "mpu6050.h"

#define SYSTEM_PERIOD		(20)	//unit: ms
#define LED_BLINK_PERIOD	(500)	//unit: ms
#define MPU_SAMPLE_RATE		(50)	//unit: Hz
#define MPU_DLPF_SWITCH		(1)
#define MPU_ACCEL_WEIGHT	(0.1)
#define MPU_CAL_PERIOD		(SYSTEM_PERIOD/1000)	//unit: s

int main(void)
{
	uint8_t rtnValue = 0;
	uint32_t cntValue = 0;

	Led_Init();
	Systick_Init();
	Usart1_Init(115200);	//conflict with RC0 and RC1

	LED0_ON;
	LED1_ON;

	rtnValue = Mpu6050_Init(MPU_SAMPLE_RATE, MPU_DLPF_SWITCH);//sample rate 50Hz enable DLPF
	while(rtnValue)
	{
		printf("Error-mpu6050-%d\r\n", rtnValue);
		Delay_ms(200);
		rtnValue = Mpu6050_Init(MPU_SAMPLE_RATE, MPU_DLPF_SWITCH);//sample rate 50Hz enable DLPF
	}

	while(1)
	{
		if(cntValue < LED_BLINK_PERIOD/SYSTEM_PERIOD)
		{
			cntValue += 1;
		}
		else
		{
			cntValue = 0;
			LED0_TOGGLE;
		}
		Mpu6050_CalPitchRoll(MPU_ACCEL_WEIGHT, MPU_CAL_PERIOD);
		printf("pitch: %f roll: %f\r\n", Mpu6050_Pitch, Mpu6050_Roll);
		Delay_ms(SYSTEM_PERIOD);
	}
}

