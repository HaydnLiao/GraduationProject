
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
#include "buzzer.h"

#define GREEN_BLINK_PERIOD	((uint16_t)(300))	//unit: ms
#define RED_BLINK_PERIOD	((uint16_t)(200))	//unit: ms
#define MPU_SAMPLE_RATE		((uint16_t)(50))	//unit: Hz
#define MPU_DLPF_SWITCH		((uint8_t)(1))		//1:on 0:off
#define MPU_ACCEL_WEIGHT	((float)(0.05))
#define MPU_CAL_PERIOD		((float)(SYSTEM_PERIOD/1000.0))		//unit: s
#define POS_INIT_DIFF		((float)(3.0))		//unit: degree
#define POS_INTI_SPEED		((uint16_t)(3))	//unit: degree per period
#define LIPO_CAL_WEIGHT		((float)(0.5))//old data weight
#define LIPO_LOW_VOLTAGE	((float)(7.0))//unit: v 3.5v*S 2S->7v 3S->10.5v 4S->14v
#define MPU_CALI_DELAY		((uint16_t)(1000))	//unit: ms
#define MPU_CALI_TIMES		((uint16_t)(1000))//1000 times about 5s
#define MPU_GYPO_Z_BOUND	((float)(1.0))

//#define MPU_YAW_BIAS_MAX	((float)(1.0))

#define STEP_ONE_POS_INIT	((uint8_t)(0))
#define STEP_TWO_MPU_CALI	((uint8_t)(1))
#define STEP_THREE_MAIN		((uint8_t)(2))

int main(void)
{
	uint8_t stepRun = 0, rtnValue = 0, flagVoltageLow = 0;
	uint32_t cntGreen = 0, cntRed = 0, cntCalibrate = 0, cntCaliDelay = 0;
	float pidPitch = 0.0, pidRoll = 0.0, pidYaw = 0.0;
	float yawBoard = 0.0, yawMpu = 0.0;
	float gypoZBiasBoard = 0.0, gypoZBiasMpu = 0.0;
	float gypoMedianBoard = 0.0;

	stepRun = STEP_TWO_MPU_CALI;	//Debug

	Led_Init();
	Systick_Init();
	Usart1_Init(115200);	//conflict with RC0 and RC1

	LED0_ON;
	LED1_ON;

	Lipo_Init();
	Buzzer_Init();
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
		cntGreen = (cntGreen+1) % (GREEN_BLINK_PERIOD/SYSTEM_PERIOD/(STEP_THREE_MAIN - stepRun + 1));//position initialization
		if(cntGreen == 0)
		{
			LED0_TOGGLE;
		}
		Lipo_CalVoltage(LIPO_CAL_WEIGHT);//when the voltage is low voltage sampling stops
		//printf("lipo:%fv\r\n", Lipo_Voltage);
		if(flagVoltageLow == 0 && Lipo_Voltage > LIPO_LOW_VOLTAGE)
		{
			LED1_ON;
			BUZZER_OFF;
			cntRed = 0;//avoid voltage jump
		}
		else
		{
			cntRed = (cntRed+1) % (RED_BLINK_PERIOD/SYSTEM_PERIOD);
			if(cntRed == 0)
			{
				LED1_TOGGLE;
				//BUZZER_TOGGLE;
				//flagVoltageLow = 1;//must power off and restart
			}
		}

		if(flagVoltageLow == 0)
		{

			//position initialization
			if(stepRun == STEP_ONE_POS_INIT)
			{
				Mpu6050_CalPitchRoll(MPU_ACCEL_WEIGHT, MPU_CAL_PERIOD);//get pitch and roll angle
				printf("[#1]pitch: %f roll: %f\r\n", Mpu6050_Pitch, Mpu6050_Roll);
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
					Motor0_Run((mdir_t)(0), 0);
					Motor1_Run((mdir_t)(0), 0);
					stepRun = STEP_TWO_MPU_CALI;
				}
			}
			else if(stepRun == STEP_TWO_MPU_CALI)
			{
				//cal the parameter to calibrate two mpu6050
				cntCaliDelay += 1;
				if(cntCaliDelay > MPU_CALI_DELAY/SYSTEM_PERIOD)
				{
					if(cntCalibrate < MPU_CALI_TIMES)
					{
						Mpu6050_GetGyroData();
						BoardMpu_GetGyroData();
						gypoZBiasMpu +=  Mpu6050_Gyro_Z;
						gypoZBiasBoard += BoardMpu_Gyro_Z;
						cntCalibrate += 1;
					}
					else
					{
						gypoZBiasMpu /= MPU_CALI_TIMES;
						gypoZBiasBoard /= MPU_CALI_TIMES;
						/**
						if(fabs(gypoZBiasMpu) > MPU_YAW_BIAS_MAX)
						{
							gypoZBiasMpu = 0.0;
						}
						if(fabs(gypoZBiasBoard) > MPU_YAW_BIAS_MAX)
						{
							gypoZBiasBoard = 0.0;
						}
						*/
						stepRun = STEP_THREE_MAIN;
					}
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
				//if(fabs(Mpu6050_Pitch) > POS_INIT_DIFF)
				{
					pidPitch = PID_Motor0(Mpu6050_Pitch, 0.0);
					Motor0_Run((mdir_t)(pidPitch > 0), (uint16_t)(fabs(pidPitch)));//pitch angle greather than zero, motor run clockwise
				}
				/**else
				{
					Motor0_Run((mdir_t)(0), 0);
				}*/
				pidRoll = PID_Motor1(Mpu6050_Roll, 0.0);
				Motor1_Run((mdir_t)(pidRoll > 0), (uint16_t)(fabs(pidRoll)));//roll angle greather than zero, motor run clockwise

				yawMpu +=  Mpu6050_Gyro_Z-gypoZBiasMpu;
				yawBoard += BoardMpu_Gyro_Z-gypoZBiasBoard;
				gypoMedianBoard = MedianFilter(BoardMpu_Gyro_Z-gypoZBiasBoard);
				//printf("%f,%f\r\n", BoardMpu_Gyro_Z, gypoMedianBoard);
				if(fabs(gypoMedianBoard) > MPU_GYPO_Z_BOUND)
				{
					//negative direction
					Motor2_Run((mdir_t)(gypoMedianBoard > 0), (uint16_t)(fabs(gypoMedianBoard)));//yaw  greather than zero, motor run clockwise
				}
				else
				{
					pidYaw = PID_Motor2(yawMpu - yawBoard, 0.0);
					Motor2_Run((mdir_t)(pidYaw > 0), (uint16_t)(fabs(pidYaw)));//yaw  greather than zero, motor run clockwise
				}
				//printf("%f,%f,%f,%f,%f\r\n", gypoZBiasMpu, gypoZBiasBoard, yawMpu, yawBoard, pidYaw);
				//yawBoard -= pidYaw;
			}
		}
		else
		{
			Motor0_Shutdown();
			Motor1_Shutdown();
			Motor2_Shutdown();
		}
		Delay_ms(SYSTEM_PERIOD);
	}
}

