
#include "stm32f10x.h"
#include "led.h"
#include "systick.h"
#include "uart1.h"
#include "mpu6050.h"
#include "hmc5883l.h"

#define SENSOR_DATA_TEST 0
#define DEVIATION_TEST 0
#define GYRO_ANGLE_TEST 0
#define OFFSET_TEST 0

int main(void)
{
	uint8_t rtnValue, data;
#if DEVIATION_TEST
	double ax, ay, az, t, gx, gy, gz;
	double axx, ayy, azz, tt, gxx, gyy, gzz;
	uint32_t cntCal;
#endif
#if GYRO_ANGLE_TEST
	uint16_t cntLoop;
	float gyroAngleX,  gyroAngleY, gyroAngleZ;
#endif
#if OFFSET_TEST
	const uint16_t OffsetCalTimes = 1000;
	uint16_t cntCal;
	float axOffset, ayOffset, azOffset;
#endif

	Led_Init();
	Systick_Init();
	Uart1_Init((uint32_t)115200);
	I2C1_Init(400000);//400kHz frequency is unused
	LEDR_ON;
	
	rtnValue = Mpu6050_Init(50, 1);//sample rate 50Hz enable DLPF
	while(rtnValue)
	{
		printf("Error-mpu6050-%d\r\n", rtnValue);
		Delay_ms(200);
		rtnValue = Mpu6050_Init(50, 1);//sample rate 50Hz enable DLPF
	}
		
	rtnValue = Hmc5883l_Init();
	while(rtnValue)
	{
		printf("Error-hmc5883l-%d\r\n", rtnValue);
		Delay_ms(200);
		rtnValue = Hmc5883l_Init();
	}
	Hmc5883l_Calibrate(20, 0.8);//calibrate 20s and new values' reliability are 80%
	LEDR_OFF;

#if DEVIATION_TEST	
	cntCal = 0;
	ax = ay = az = t = gx = gy = gz = 0.0;
	axx = ayy = azz = tt = gxx = gyy = gzz = 0.0;
#endif
#if GYRO_ANGLE_TEST
	cntLoop = 0;
	gyroAngleX = gyroAngleY = gyroAngleZ = 0.0;
#endif
#if OFFSET_TEST
	cntCal = 0;
	axOffset = ayOffset = azOffset = 0.0;
#endif

	while(1)
	{
#if SENSOR_DATA_TEST
		if(Mpu6050_GetAccelData())
		{
			printf("Error-GetAccelData\r\n");
		}
		else
		{
			printf("ax: %-5.4f ay: %-5.4f az: %-5.4f\t", Mpu6050_Accel_X, Mpu6050_Accel_Y, Mpu6050_Accel_Z);
		}
		if(Mpu6050_GetTempData())
		{
			printf("Error-GetTempData\r\n");
		}
		else
		{
			printf("%-5.4f¡æ\t", Mpu6050_Temp);
		}
		if(Mpu6050_GetGyroData())
		{
			printf("Error-GetGyroData\r\n");
		}
		else
		{
			printf("gx: %-5.4f gy: %-5.4f gz: %-5.4f\r\n", Mpu6050_Gyro_X, Mpu6050_Gyro_Y, Mpu6050_Gyro_Z);
		}
#endif
#if DEVIATION_TEST			
		ax = (ax*cntCal+Mpu6050_Accel_X)/(cntCal+1);
		ay = (ay*cntCal+Mpu6050_Accel_Y)/(cntCal+1);
		az = (az*cntCal+Mpu6050_Accel_Z)/(cntCal+1);
		t = (t*cntCal+Mpu6050_Temp)/(cntCal+1);
		gx = (gx*cntCal+Mpu6050_Gyro_X)/(cntCal+1);
		gy = (gy*cntCal+Mpu6050_Gyro_Y)/(cntCal+1);
		gz = (gz*cntCal+Mpu6050_Gyro_Z)/(cntCal+1);
		
		axx = (axx + (Mpu6050_Accel_X - ax) * (Mpu6050_Accel_X - ax)) / cntCal;
		ayy = (ayy + (Mpu6050_Accel_Y - ay) * (Mpu6050_Accel_Y - ay)) / cntCal;
		azz = (azz + (Mpu6050_Accel_Z - az) * (Mpu6050_Accel_Z - az)) / cntCal;
		tt = (tt + (Mpu6050_Temp - t) * (Mpu6050_Temp - t)) / cntCal;
		gxx = (gxx + (Mpu6050_Gyro_X - gx) * (Mpu6050_Gyro_X - gx)) / cntCal;
		gyy = (gyy + (Mpu6050_Gyro_Y - gy) * (Mpu6050_Gyro_Y - gy)) / cntCal;
		gzz = (gzz + (Mpu6050_Gyro_Z - gz) * (Mpu6050_Gyro_Z - gz)) / cntCal;
		
		cntCal += 1;
		printf("%d: %f %f %f %f %f %f %f \t", cntCal, ax, ay, az, t, gx, gy, gz);
		printf("%d: %f %f %f %f %f %f %f \r\n", cntCal, axx, ayy, azz, tt, gxx, gyy, gzz);
#endif
#if GYRO_ANGLE_TEST
		cntLoop += 1;
		Mpu6050_GetGyroData();
		Mpu6050_Gyro_X = Mpu6050_Gyro_X + 0.24155;
		Mpu6050_Gyro_Y = Mpu6050_Gyro_Y + 1.46968;
		Mpu6050_Gyro_Z = Mpu6050_Gyro_Z + 1.94873;
		printf("gx: %-5.4f gy: %-5.4f gz: %-5.4f \r\n", Mpu6050_Gyro_X, Mpu6050_Gyro_Y, Mpu6050_Gyro_Z);
		gyroAngleX += Mpu6050_Gyro_X;
		gyroAngleY += Mpu6050_Gyro_Y;
		gyroAngleZ += Mpu6050_Gyro_Z;
		if(cntLoop > 10)
		{
			Mpu6050_GetAccelData();
			Mpu6050_CalPitchRoll();
			printf("pitch: %-10f roll: %-10f \t", Mpu6050_Pitch, Mpu6050_Roll);
			printf("gPitch: %-10f gRoll: %-10f gYaw: %-10f \r\n", gyroAngleX, gyroAngleY, gyroAngleZ);
			cntLoop = 0;
		}
#endif
		//Mpu6050_CalPitchRoll();
		//printf("pitch: %-10f roll: %-10f \r\n", Mpu6050_Pitch, Mpu6050_Roll);
		//LEDG_ON;
		//Delay_ms(1000);
		//LEDG_OFF;
#if OFFSET_TEST
		if(cntCal < OffsetCalTimes)
		{
			cntCal += 1;
			if(Mpu6050_GetAccelData())
			{
				printf("Error-GetAccelData\r\n");
			}
			else
			{
				printf("ax: %-5.4f ay: %-5.4f az: %-5.4f\r\n", Mpu6050_Accel_X, Mpu6050_Accel_Y, Mpu6050_Accel_Z);
			}
			axOffset += Mpu6050_Accel_X;
			ayOffset += Mpu6050_Accel_Y;
			azOffset += Mpu6050_Accel_Z - 1.0;
		}
		else if(cntCal == OffsetCalTimes)
		{
			cntCal += 1;
			axOffset = -axOffset/OffsetCalTimes;
			ayOffset = -ayOffset/OffsetCalTimes;
			azOffset = -azOffset/OffsetCalTimes;
		}
		else
		{
			if(Mpu6050_GetAccelData())
			{
				printf("Error-GetAccelData\r\n");
			}
			else
			{
				Mpu6050_CalPitchRoll();
				printf("ax: %-8.4f ay: %-8.4f az: %-8.4f \t", Mpu6050_Accel_X, Mpu6050_Accel_Y, Mpu6050_Accel_Z);
				printf("%-8.4f  %-8.4f  %-8.4f \t", axOffset, ayOffset, azOffset);
				printf("ax: %-8.4f ay: %-8.4f az: %-8.4f \t", Mpu6050_Accel_X+axOffset, Mpu6050_Accel_Y+ayOffset, Mpu6050_Accel_Z+azOffset);
				printf("pitch: %-10f roll: %-10f \r\n", Mpu6050_Pitch, Mpu6050_Roll);
			}
		}
#endif
		Mpu6050_CalPitchRoll(0.1, 5/1000);
		Hmc5883l_GetData();
		//Hmc5883l_CalYaw(Mpu6050_Pitch, Mpu6050_Roll);
		Hmc5883l_CalYaw(0, 0);
		//Hmc5883l_Yaw = 0.1*Hmc5883l_Yaw + (1-0.1)*(Hmc5883l_Yaw+Mpu6050_Gyro_Z*5/1000);
		//printf("%f %f\t ", Mpu6050_Accel_Y, Mpu6050_Accel_Z);
		printf("pitch: %f roll: %f yaw: %f\r\n", Mpu6050_Pitch, Mpu6050_Roll, Hmc5883l_Yaw);
		Delay_ms(5);
	}
}

