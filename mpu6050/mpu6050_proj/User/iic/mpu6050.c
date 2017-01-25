
#include "mpu6050.h"

float Mpu6050_Temp;		//Temperature calculated result

void Mpu6050_Init(void)
{
	I2C1_Init(400000);	//400kHz unused
}

uint8_t Mpu6050_GetAccelData(void)
{
	uint8_t buff[6];
	if(I2C1_ReadData(DEVICE_ADDRESS, ACCEL_XOUT_HIGH, buff, 6))
	{
		return 1;
	}
	AccelData[0] = (buff[0] & 0x00ff)<<8 | buff[1];
	AccelData[1] = (buff[2] & 0x00ff)<<8 | buff[3];
	AccelData[2] = (buff[4] & 0x00ff)<<8 | buff[5];
	printf("%-6d %-6d %-6d\t", AccelData[0], AccelData[1], AccelData[2]);
	return 0;
}

uint8_t Mpu6050_GetTempData(void)
{
	uint8_t buff[2];
	if(I2C1_ReadData(DEVICE_ADDRESS, TEMP_OUT_HIGH, buff, 2))
	{
		return 1;
	}
	TempData = (buff[0] & 0x00ff)<<8 | buff[1];
	Mpu6050_Temp = TempData/340.0 + 36.53;
	//printf("%.6f\t", Mpu6050_Temp);
	return 0;
}

uint8_t Mpu6050_GetGyroData(void)
{
	uint8_t buff[6];
	if(I2C1_ReadData(DEVICE_ADDRESS, GYRO_XOUT_HIGH, buff, 6))
	{
		return 1;
	}
	GyroData[0] = (buff[0] & 0x00ff)<<8 | buff[1];
	GyroData[1] = (buff[2] & 0x00ff)<<8 | buff[3];
	GyroData[2] = (buff[4] & 0x00ff)<<8 | buff[5];
	printf("%-6d %-6d %-6d\r\n", GyroData[0], GyroData[1], GyroData[2]);
	return 0;
}


