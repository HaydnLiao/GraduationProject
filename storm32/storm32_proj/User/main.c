
#include "stm32f10x.h"
#include "global_math.h"
#include "led.h"
#include "systick.h"
#include "usart1.h"
#include "mpu6050.h"
#include "motor.h"
#include "pid.h"

#define LED_BLINK_PERIOD	((uint16_t)(500))	//unit: ms
#define MPU_SAMPLE_RATE		((uint16_t)(50))	//unit: Hz
#define MPU_DLPF_SWITCH		((uint8_t)(1))		//1:on 0:off
#define MPU_ACCEL_WEIGHT	((float)(0.1))
#define MPU_CAL_PERIOD		((float)(SYSTEM_PERIOD/1000.0))		//unit: s

int main(void)
{
	uint8_t rtnValue = 0;
	uint32_t cntValue = 0;
	float pidPitch = 0.0, pidRoll = 0.0;

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
	Motor_Init();

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
		//printf("pitch: %f roll: %f\r\n", Mpu6050_Pitch, Mpu6050_Roll);

	#if DEBUG_USART1_PID
		Usart1StringToFloat();	//usart1-debug-pid parameters
	#endif
		pidPitch = PID_Motor0(Mpu6050_Pitch, 0.0);
		Motor0_Run((mdir_t)(pidPitch > 0), (uint16_t)(fabs(pidPitch)));
		pidRoll = PID_Motor1(Mpu6050_Roll, 0.0);
		Motor1_Run((mdir_t)(pidRoll>0), (uint16_t)(fabs(pidRoll)));
		//Motor2_Run((mdir_t)1, (uint16_t)(360));

		Delay_ms(SYSTEM_PERIOD);
	}
}

