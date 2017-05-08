
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
#include "joystick.h"

#define GREEN_BLINK_PERIOD	((uint16_t)(300))	//unit: ms
#define RED_BLINK_PERIOD	((uint16_t)(200))	//unit: ms
#define MPU_SAMPLE_RATE		((uint16_t)(50))	//unit: Hz
#define MPU_DLPF_SWITCH		((uint8_t)(1))		//1:on 0:off
#define MPU_ACCEL_WEIGHT	((float)(0.05))
#define MPU_CAL_PERIOD		((float)(SYSTEM_PERIOD/1000.0))		//unit: s
#define POS_INIT_DIFF		((float)(1.0))		//unit: degree
#define POS_INTI_SPEED		((uint16_t)(120))	//unit: degree per second
#define POS_INIT_JUDGE		((uint16_t)(200))
#define LIPO_CAL_WEIGHT		((float)(0.99))//old data weight
#define LIPO_LOW_VOLTAGE	((float)(7.0))//unit: v 3.5v*S 2S->7v 3S->10.5v 4S->14v
#define JOY_CAL_WEIGHT		((float)(0.9))//old data weight
#define MPU_CALI_DELAY		((uint16_t)(1000))	//unit: ms
#define MPU_CALI_TIMES		((uint16_t)(800))//800 times about 8s
#define MPU_GYPO_Z_BOUND	((float)(0.2))
#define YAW_AUTO_OL_FACTOR	((float)(3000))
#define ANGLE_MAX_SPEED		((uint16_t)(1000))	//unit: degree per second
#define YAW_MAX_SPEED		((uint16_t)(800))	//unit: degree per second

#define HANDLE_PITCH_UPPER	((float)(55.0))
#define HANDLE_PITCH_LOWER	((float)(-55.0))
#define HANDLE_ROLL_UPPER	((float)(50.0))
#define HANDLE_ROLL_LOWER	((float)(-50.0))
#define CAMERA_PITCH_UPPER	((float)(50.0))
#define CAMERA_PITCH_LOWER	((float)(-50.0))
#define CAMERA_YAW_UPPER	((float)(30.0))
#define CAMERA_YAW_LOWER	((float)(-30.0))

#define MPU_YAW_BIAS_MAX	((float)(-1.000))
#define MPU_YAW_BIAS_MIN	((float)(-1.250))
#define BOARD_YAW_BIAS_MAX	((float)(-0.450))
#define BOARD_YAW_BIAS_MIN	((float)(-5.500))

#define STEP_ONE_POS_INIT	((uint8_t)(0))
#define STEP_TWO_MPU_CALI	((uint8_t)(1))
#define STEP_THREE_MAIN		((uint8_t)(2))

int main(void)
{
	uint8_t stepRun = 0, rtnValue = 0, flagVoltageLow = 0;
	uint32_t cntGreen = 0, cntRed = 0, cntPosInit = 0, cntCalibrate = 0, cntCaliDelay = 0;
	float pitchSpeed = 0.0, rollSpeed = 0.0, yawSpeed = 0.0;
	float yawBoard = 0.0, yawMpu = 0.0, yawDiff = 0.0;
	float gypoZBiasBoard = 0.0, gypoZBiasMpu = 0.0, gypoMedianBoard = 0.0;
	float joyExpPitch = 0.0, joyExpYaw = 0.0;

	//stepRun = STEP_TWO_MPU_CALI;	//Debug

	Led_Init();
	Systick_Init();
#ifdef DEBUG_P0
	Usart1_Init(115200);	//conflict with RC0 and RC1
#endif

	LED0_ON;
	LED1_ON;

	Lipo_Init();
	Buzzer_Init();
	Joystick_Init();
	rtnValue = Mpu6050_Init(MPU_SAMPLE_RATE, MPU_DLPF_SWITCH);//sample rate 50Hz enable DLPF
	while(rtnValue)
	{
		DEBUG_PRINT("Error-mpu6050-%d\r\n", rtnValue);
		Delay_ms(200);
		rtnValue = Mpu6050_Init(MPU_SAMPLE_RATE, MPU_DLPF_SWITCH);//sample rate 50Hz enable DLPF
	}
	rtnValue = BoardMpu_Init(MPU_SAMPLE_RATE, MPU_DLPF_SWITCH);//sample rate 50Hz enable DLPF
	while(rtnValue)
	{
		DEBUG_PRINT("Error-boardmpu-%d\r\n", rtnValue);
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
		//DEBUG_PRINT("lipo:%fv\r\n", Lipo_Voltage);
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
				BUZZER_TOGGLE;
				flagVoltageLow = 1;//must power off and restart
				DEBUG_PRINT("WARNING:%fv\r\n", Lipo_Voltage);
			}
		}

		if(flagVoltageLow == 0)
		{

			//position initialization
			if(stepRun == STEP_ONE_POS_INIT)
			{
				Mpu6050_CalPitchRoll(MPU_ACCEL_WEIGHT, MPU_CAL_PERIOD);//get pitch and roll angle
				DEBUG_PRINT("[#1]pitch: %f roll: %f\r\n", Mpu6050_Pitch, Mpu6050_Roll);
				/**if(fabs(Mpu6050_Roll) > POS_INIT_DIFF)
				{
					Motor1_Run((mdir_t)(Mpu6050_Roll < 0), POS_INTI_SPEED);//roll angle greather than zero, motor run anticlockwise
				}
				else if(fabs(Mpu6050_Pitch) > POS_INIT_DIFF)
				{
					Motor0_Run((mdir_t)(Mpu6050_Pitch > 0), POS_INTI_SPEED);//pitch angle greather than zero, motor run clockwise
				}*/
				if(fabs(Mpu6050_Roll) > POS_INIT_DIFF || fabs(Mpu6050_Pitch) > POS_INIT_DIFF)
				{
					Motor1_Run((mdir_t)(Mpu6050_Roll < 0), POS_INTI_SPEED);//roll angle greather than zero, motor run anticlockwise
					Motor0_Run((mdir_t)(Mpu6050_Pitch > 0), POS_INTI_SPEED);//pitch angle greather than zero, motor run clockwise
				}
				else
				{
					cntPosInit += 1;
					if(cntPosInit > POS_INIT_JUDGE)
					{
						Motor0_Run((mdir_t)(0), 0);
						Motor1_Run((mdir_t)(0), 0);
						stepRun = STEP_TWO_MPU_CALI;
					}
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
						//Bias clipping 
						DEBUG_PRINT("%f,%f\r\n", gypoZBiasMpu, gypoZBiasBoard);
						if(gypoZBiasMpu > MPU_YAW_BIAS_MAX || gypoZBiasMpu < MPU_YAW_BIAS_MIN
							|| gypoZBiasBoard > BOARD_YAW_BIAS_MAX || gypoZBiasBoard < BOARD_YAW_BIAS_MIN)
						{
							gypoZBiasMpu = gypoZBiasBoard = 0.0;
							cntCalibrate = 0;
						}
						else
						{
							stepRun = STEP_THREE_MAIN;
						}
					}
				}
			}
			else
			{
			#if DEBUG_USART1_PID
				Usart1StringToFloat();	//usart1-debug-pid parameters
			#endif

				Mpu6050_CalPitchRoll(MPU_ACCEL_WEIGHT, MPU_CAL_PERIOD);//#1 pitch roll
				DEBUG_PRINT("[#1]%f,%f ", Mpu6050_Pitch, Mpu6050_Roll);
				BoardMpu_CalPitchRoll(MPU_ACCEL_WEIGHT, MPU_CAL_PERIOD);//#2 pitch roll
				DEBUG_PRINT("[#2]%f,%f ", BoardMpu_Pitch, BoardMpu_Roll);
				yawMpu +=  (Mpu6050_Gyro_Z-gypoZBiasMpu) / (1000 / SYSTEM_PERIOD);//#1 yaw
				yawBoard += (BoardMpu_Gyro_Z-gypoZBiasBoard) / (1000 / SYSTEM_PERIOD);//#2 yaw
				yawDiff = yawMpu - yawBoard;//#1 yaw - #2 yaw
				DEBUG_PRINT("%f ", yawDiff);
				gypoMedianBoard = MedianFilter(BoardMpu_Gyro_Z-gypoZBiasBoard);//#2 angular rate
				//DEBUG_PRINT("%f,%f,%f,%f,%f\r\n", gypoZBiasMpu, gypoZBiasBoard, yawMpu, yawBoard, gypoMedianBoard);
				//DEBUG_PRINT("%f,%f,%f ", yawMpu, yawBoard, gypoMedianBoard);

				//#2 pitch&roll angle clipping 
				if(BoardMpu_Pitch > HANDLE_PITCH_UPPER || BoardMpu_Pitch < HANDLE_PITCH_LOWER
					|| BoardMpu_Roll > HANDLE_ROLL_UPPER || BoardMpu_Roll < HANDLE_ROLL_LOWER)
				{
					pitchSpeed = rollSpeed = yawSpeed = 0.0;
				}
				else
				{
					if(Joystick_ObtainMode())//Manual mode
					{
						Joystick_CalXY(JOY_CAL_WEIGHT);
						Joystick_ConvertAngle(&joyExpYaw, &joyExpPitch);
						//#1 expect pitch, #1-#2 yaw angle clipping 
						joyExpPitch = INTERVAL_CONSTRAINT(joyExpPitch, CAMERA_PITCH_UPPER, CAMERA_PITCH_LOWER);
						joyExpYaw = INTERVAL_CONSTRAINT(joyExpYaw, CAMERA_YAW_UPPER, CAMERA_YAW_LOWER);
						//DEBUG_PRINT("%f,%f\r\n", joyExpPitch, joyExpYaw);

						pitchSpeed = PID_Motor0(Mpu6050_Pitch, joyExpPitch);//#1 pitch
						yawSpeed = PID_Motor2(yawDiff, joyExpYaw);//#1 yaw - #2 yaw
					}
					else//Auto mode
					{
						pitchSpeed = PID_Motor0(Mpu6050_Pitch, 0.0);//#1 pitch
						if(fabs(gypoMedianBoard) > MPU_GYPO_Z_BOUND)
						{
							//negative direction
							//yawSpeed = gypoMedianBoard;//#2 angular rate
							yawSpeed = gypoMedianBoard/(1000/SYSTEM_PERIOD)*YAW_AUTO_OL_FACTOR;
						}
						else
						{
							yawSpeed = PID_Motor2(yawDiff, 0.0);//#1 yaw - #2 yaw
						}
					}
					rollSpeed = PID_Motor1(Mpu6050_Roll, 0.0);//#1 roll
				}
				DEBUG_PRINT("%f,%f,%f\r\n", pitchSpeed, rollSpeed, yawSpeed);

				pitchSpeed = INTERVAL_CONSTRAINT(pitchSpeed, ANGLE_MAX_SPEED, ANGLE_MAX_SPEED*(-1));
				rollSpeed = INTERVAL_CONSTRAINT(rollSpeed, ANGLE_MAX_SPEED, ANGLE_MAX_SPEED*(-1));
				yawSpeed = INTERVAL_CONSTRAINT(yawSpeed, YAW_MAX_SPEED, YAW_MAX_SPEED*(-1));

				Motor0_Run((mdir_t)(pitchSpeed > 0), (uint16_t)(fabs(pitchSpeed)));//pitch angle greather than zero, motor run clockwise
				Motor1_Run((mdir_t)(rollSpeed < 0), (uint16_t)(fabs(rollSpeed)));//roll angle greather than zero, motor run anticlockwise
				Motor2_Run((mdir_t)(yawSpeed > 0), (uint16_t)(fabs(yawSpeed)));//yaw  angle difference greather than zero, motor run clockwise
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

