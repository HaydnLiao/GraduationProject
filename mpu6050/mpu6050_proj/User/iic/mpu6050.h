
#ifndef __MPU6050_H
#define __MPU6050_H

#include "stm32f10x.h"
#include "i2c1.h"

#define MPU6050_ADDRESS	0x68	//mpu6050 I2C address AD0=0
#define MPU6050_ID		0x68	//mpu6050 WHO_AM_I=0x68

#define SMPRT_DIV		0x19	//specify the divider to generate the sample rate
#define CONFIG			0x1A	//FSYNC pin sampling and DLPF setting
#define GYRO_CONFIG		0x1B	//gyroscope self-test and full scale range
#define ACCEL_CONFIG	0x1C	//accelerometer self-test, full scale range and DHPF
#define FIFO_EN			0x23	//select the sensor data loaded into the FIFO buffer
#define INT_PIN_CFG		0x37	//the behavior of interrupt signals at the INT pins
#define INT_ENABLE		0x38	//interrupt generation

#define ACCEL_XOUT_HIGH	0x3B	//Accelerometer x-axis MSB
#define ACCEL_XOUT_LOW	0x3C	//Accelerometer x-axis LSB
#define ACCEL_YOUT_HIGH	0x3D	//Accelerometer y-axis MSB
#define ACCEL_YOUT_LOW	0x3E	//Accelerometer y-axis LSB
#define ACCEL_ZOUT_HIGH	0x3F	//Accelerometer z-axis MSB
#define ACCEL_ZOUT_LOW	0x40	//Accelerometer z-axis LSB
#define TEMP_OUT_HIGH	0x41	//Temperature MSB
#define TEMP_OUT_LOW	0x42	//Temperature LSB
#define GYRO_XOUT_HIGH	0x43	//Angular rate x-axis MSB
#define GYRO_XOUT_LOW	0x44	//Angular rate x-axis LSB
#define GYRO_YOUT_HIGH	0x45	//Angular rate y-axis MSB
#define GYRO_YOUT_LOW	0x46	//Angular rate y-axis LSB
#define GYRO_ZOUT_HIGH	0x47	//Angular rate z-axis MSB
#define GYRO_ZOUT_LOW	0x48	//Angular rate z-axis LSB

#define USER_CONTROL	0x6A	//FIFO/I2C_MST EN/RESET
#define PWR_MGMT_1		0x6B	//power mode and clock source
#define PWR_MGMT_2		0x6C	//wake-up frequency in Accelerometer only low power mode
#define WHO_AM_I		0x75	//the identity of device b-110 100-

#define INTERVAL_CONSTRAINT(x, max, min)	((x)>(max)?(max):((x)<(min)?(min):(x)))

static int16_t AccelData[3];	//Accelerometer data
static int16_t TempData;		//Temperature data
static int16_t GyroData[3];		//Gyroscope data

extern uint8_t Mpu6050_ID;		//Identity of device
extern int16_t Mpu6050_Accel_X;	//Aceeleration x-axis
extern int16_t Mpu6050_Accel_Y;	//Aceeleration y-axis
extern int16_t Mpu6050_Accel_Z;	//Aceeleration z-axis
extern float Mpu6050_Temp;		//Temperature calculated result
extern int16_t Mpu6050_Gyro_X;	//Angular rate x-axis
extern int16_t Mpu6050_Gyro_Y;	//Angular rate y-axis
extern int16_t Mpu6050_Gyro_Z;	//Angular rate z-axis

uint8_t Mpu6050_Init(uint16_t sampleRate, uint8_t flagDLPF);
uint8_t Mpu6050_GetDevID(void);
uint8_t Mpu6050_GetAccelData(void);
uint8_t Mpu6050_GetTempData(void);
uint8_t Mpu6050_GetGyroData(void);
uint8_t Mpu6050_SetSampleRate(uint16_t sampleRate, uint8_t flagDLPF);
uint8_t Mpu6050_SetDLPF(uint16_t bandWidth, uint8_t flagDLPF);

#endif

