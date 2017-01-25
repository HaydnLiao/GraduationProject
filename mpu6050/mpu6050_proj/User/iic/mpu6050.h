
#ifndef __MPU6050_H
#define __MPU6050_H

#include "stm32f10x.h"
#include "i2c1.h"

#define DEVICE_ADDRESS	0x68	//mpu6050 I2C address AD0=0

#define ACCEL_XOUT_HIGH	0x3B	//Accelerometer x-axes MSB
#define ACCEL_XOUT_LOW	0x3C	//Accelerometer x-axes LSB
#define ACCEL_YOUT_HIGH	0x3D	//Accelerometer y-axes MSB
#define ACCEL_YOUT_LOW	0x3E	//Accelerometer y-axes LSB
#define ACCEL_ZOUT_HIGH	0x3F	//Accelerometer z-axes MSB
#define ACCEL_ZOUT_LOW	0x40	//Accelerometer z-axes LSB
#define TEMP_OUT_HIGH	0x41	//Temperature MSB
#define TEMP_OUT_LOW	0x42	//Temperature LSB
#define GYRO_XOUT_HIGH	0x43	//Angular rate x-axes MSB
#define GYRO_XOUT_LOW	0x44	//Angular rate x-axes LSB
#define GYRO_YOUT_HIGH	0x45	//Angular rate y-axes MSB
#define GYRO_YOUT_LOW	0x46	//Angular rate y-axes LSB
#define GYRO_ZOUT_HIGH	0x47	//Angular rate z-axes MSB
#define GYRO_ZOUT_LOW	0x48	//Angular rate z-axes LSB

#define USER_CONTROL	0x6A	//FIFO/I2C_MST EN/RESET
#define PWR_MGMT_1		0x6B	//power mode and clock source
#define PWR_MGMT_2		0x6C	//the frequency of wake-ups in Accelerometer
#define WHO_AM_I		0x75	//the identity of device b-110 100-

static int16_t AccelData[3];	//Accelerometer data
static int16_t TempData;		//Temperature data
static int16_t GyroData[3];		//Gyroscope data

extern float Mpu6050_Temp;		//Temperature calculated result

void Mpu6050_Init(void);
uint8_t Mpu6050_GetAccelData(void);
uint8_t Mpu6050_GetTempData(void);
uint8_t Mpu6050_GetGyroData(void);

#endif

