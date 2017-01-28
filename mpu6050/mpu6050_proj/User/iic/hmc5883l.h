
#ifndef __HMC5883L_H
#define __HMC5883L_H

#include <math.h>
#include "stm32f10x.h"
#include "i2c1.h"

#define HMC5883L_ADDRESS	0x1E
#define HMC5883L_ID_A		0x48
#define HMC5883L_ID_B		0x34
#define HMC5883L_ID_C		0x33

#define CONFIG_REG_A		0x00//samples averaged and data output rate
#define CONFIG_REG_B		0x01//gain configuration
#define MODE_REG			0x02//select the operating mode
#define DATA_OUTPUT_X_MSB	0x03//channel X MSB
#define DATA_OUTPUT_X_LSB	0x04//channel X LSB
#define DATA_OUTPUT_Z_MSB	0x05//channel Z MSB
#define DATA_OUTPUT_Z_LSB	0x06//channel Z LSB
#define DATA_OUTPUT_Y_MSB	0x07//channel Y MSB
#define DATA_OUTPUT_Y_LSB	0x08//channel Y LSB
#define STATUS_REG			0x09//indicate device status 
#define ID_REG_A			0x0A//used to indentify the device 0x48
#define ID_REG_B			0x0B//used to indentify the device 0x34
#define ID_REG_C			0x0C//used to indentify the device 0x33

#define FACTOR_FIELD_RANGE_0_88	(1370.0)
#define FACTOR_FIELD_RANGE_1_3	(1090.0)
#define FACTOR_FIELD_RANGE_1_9	(820.0)
#define FACTOR_FIELD_RANGE_2_5	(660.0)
#define FACTOR_FIELD_RANGE_4	(440.0)
#define FACTOR_FIELD_RANGE_4_7	(390.0)
#define FACTOR_FIELD_RANGE_5_6	(330.0)
#define FACTOR_FIELD_RANGE_8_1	(230.0)

#define MATH_PI	3.1416

static int16_t XData;
static int16_t YData;
static int16_t ZData;

extern uint8_t Hmc5883l_ID_A;
extern uint8_t Hmc5883l_ID_B;
extern uint8_t Hmc5883l_ID_C;
extern float Hmc5883l_Gauss_X;
extern float Hmc5883l_Gauss_Y;
extern float Hmc5883l_Gauss_Z;
extern float Hmc5883l_Yaw;

uint8_t Hmc5883l_Init(void);
uint8_t Hmc5883l_GetDevID(void);
uint8_t Hmc5883l_GetData(void);
void Hmc5883l_CalYaw(float pitch, float roll);

#endif

