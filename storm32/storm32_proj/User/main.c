
#include "stm32f10x.h"
#include "global_math.h"
#include "led.h"
#include "systick.h"
#include "usart1.h"
#include "mpu6050.h"
#include "motor.h"
#include "pid.h"
#include "boardmpu.h"
#include "lipo.h"

#define GREEN_BLINK_PERIOD	((uint16_t)(250))	//unit: ms
#define RED_BLINK_PERIOD	((uint16_t)(100))	//unit: ms
#define MPU_SAMPLE_RATE		((uint16_t)(50))	//unit: Hz
#define MPU_DLPF_SWITCH		((uint8_t)(1))		//1:on 0:off
#define MPU_ACCEL_WEIGHT	((float)(0.1))
#define MPU_CAL_PERIOD		((float)(SYSTEM_PERIOD/1000.0))		//unit: s
#define POS_INIT_DIFF		((float)(1.0))		//unit: degree
#define POS_INTI_SPEED		((uint16_t)(10))	//unit: degree per second
#define LIPO_CAL_WEIGHT		((float)(0.8))//old data weight
#define LIPO_LOW_VOLTAGE	((float)(6.0))//unit: v

int main(void)
{
	uint8_t rtnValue = 0, flagInitFinish = 0;
	uint32_t cntGreen = 0, cntRed = 0;
	float pidPitch = 0.0, pidRoll = 0.0;

	Led_Init();
	Systick_Init();
	Usart1_Init(115200);	//conflict with RC0 and RC1

	LED0_ON;
	LED1_ON;

	Lipo_Init();
	rtnValue = Mpu6050_Init(MPU_SAMPLE_RATE, MPU_DLPF_SWITCH);//sample rate 50Hz enable DLPF
	while(rtnValue)
	{
		printf("Error-mpu6050-%d\r\n", rtnValue);
		Delay_ms(200);
		rtnValue = Mpu6050_Init(MPU_SAMPLE_RATE, MPU_DLPF_SWITCH);//sample rate 50Hz enable DLPF
	}
	rtnValue = BoardMpu_Init(MPU_SAMPLE_RATE, MPU_DLPF_SWITCH);//sample rate 50Hz enable DLPF
	while(rtnValue)
	{
		printf("Error-boardmpu-%d\r\n", rtnValue);
		Delay_ms(200);
		rtnValue = BoardMpu_Init(MPU_SAMPLE_RATE, MPU_DLPF_SWITCH);//sample rate 50Hz enable DLPF
	}
	Motor_Init();

	while(1)
	{
		cntGreen = (cntGreen+1) % (GREEN_BLINK_PERIOD/SYSTEM_PERIOD/(2-flagInitFinish%2));//position initialization
		cntRed = (cntRed+1) % (RED_BLINK_PERIOD/SYSTEM_PERIOD);
		if(cntGreen == 0)
		{
			LED0_TOGGLE;
		}
		if(Lipo_Voltage > LIPO_LOW_VOLTAGE)
		{
			LED1_ON;
		}
		else if(cntRed == 0)
		{
			LED1_TOGGLE;
		}

		Lipo_CalVoltage(LIPO_CAL_WEIGHT);
		//printf("lipo:%fv\r\n", Lipo_Voltage);

		//position initialization
		if(flagInitFinish == 0)
		{
			Mpu6050_CalPitchRoll(MPU_ACCEL_WEIGHT, MPU_CAL_PERIOD);//get pitch and roll angle
			//printf("[#1]pitch: %f roll: %f\r\n", Mpu6050_Pitch, Mpu6050_Roll);
			if(fabs(Mpu6050_Roll) > POS_INIT_DIFF)
			{
				Motor1_Run((mdir_t)(Mpu6050_Roll > 0), POS_INTI_SPEED);//roll angle greather than zero, motor run clockwise
			}
			else if(fabs(Mpu6050_Pitch) > POS_INIT_DIFF)
			{
				Motor0_Run((mdir_t)(Mpu6050_Pitch > 0), POS_INTI_SPEED);//pitch angle greather than zero, motor run clockwise
			}
			else
			{
				flagInitFinish = 1;
			}
		}
		else
		{
			Mpu6050_CalPitchRoll(MPU_ACCEL_WEIGHT, MPU_CAL_PERIOD);
			//printf("[#1]pitch: %f roll: %f\r\n", Mpu6050_Pitch, Mpu6050_Roll);
			BoardMpu_CalPitchRoll(MPU_ACCEL_WEIGHT, MPU_CAL_PERIOD);
			//printf("[#2]pitch: %f roll: %f\r\n", BoardMpu_Pitch, BoardMpu_Roll);

		#if DEBUG_USART1_PID
			Usart1StringToFloat();	//usart1-debug-pid parameters
		#endif
			pidPitch = PID_Motor0(Mpu6050_Pitch, 0.0);
			Motor0_Run((mdir_t)(pidPitch > 0), (uint16_t)(fabs(pidPitch)));//pitch angle greather than zero, motor run clockwise
			pidRoll = PID_Motor1(Mpu6050_Roll, 0.0);
			Motor1_Run((mdir_t)(pidRoll > 0), (uint16_t)(fabs(pidRoll)));//roll angle greather than zero, motor run clockwise
			//Motor2_Run((mdir_t)1, (uint16_t)(360));
		}
		Delay_ms(SYSTEM_PERIOD);
	}
}

