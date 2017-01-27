
#include "stm32f10x.h"
#include "led.h"
#include "systick.h"
#include "uart1.h"
#include "mpu6050.h"

#define DEVIATION_TEST 0

int main(void)
{
	uint8_t data;
#if DEVIATION_TEST
	double ax, ay, az, t, gx, gy, gz;
	double axx, ayy, azz, tt, gxx, gyy, gzz;
	uint32_t cntCal;
#endif
	
	Led_Init();
	Systick_Init();
	Uart1_Init((uint32_t)115200);
	LEDR_ON;
	data = Mpu6050_Init(50, 1);//sample rate 50Hz enable DLPF
	while(data)
	{
		printf("Error-%d\r\n", data);
		Delay_ms(200);
		data = Mpu6050_Init(50, 1);//sample rate 50Hz enable DLPF
	}
	LEDR_OFF;

#if DEVIATION_TEST	
	cntCal = 0;
	ax=ay=az=t=gx=gy=gz=0.0;
	axx=ayy=azz=tt=gxx=gyy=gzz=0.0;
#endif

	while(1)
	{
		if(Mpu6050_GetAccelData())
		{
			printf("Error-GetAccelData\r\n");
		}
		else
		{
			//printf("ax: %-5.4f ay: %-5.4f az: %-5.4f\t", Mpu6050_Accel_X, Mpu6050_Accel_Y, Mpu6050_Accel_Z);
		}
		if(Mpu6050_GetTempData())
		{
			printf("Error-GetTempData\r\n");
		}
		else
		{
			//printf("%-5.4f¡æ\t", Mpu6050_Temp);
		}
		if(Mpu6050_GetGyroData())
		{
			printf("Error-GetGyroData\r\n");
		}
		else
		{
			//printf("gx: %-5.4f gy: %-5.4f gz: %-5.4f\r\n", Mpu6050_Gyro_X, Mpu6050_Gyro_Y, Mpu6050_Gyro_Z);
		}

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
		Mpu6050_CalPitchRoll();
		printf("pitch: %-10f roll: %-10f \r\n", Mpu6050_Pitch, Mpu6050_Roll);
		//LEDG_ON;
		//Delay_ms(1000);
		//LEDG_OFF;
		Delay_ms(50);
	}
}

