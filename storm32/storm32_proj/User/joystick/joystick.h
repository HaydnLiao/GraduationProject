
#ifndef __JOYSTICK_H
#define __JOYSTICK_H

#include "stm32f10x.h"
#include "global_math.h"

#define JOY_ANALOG_UPPER	(4096)
#define JOY_ANALOG_LOWER	(0)
#define JOY_ANGLE_X_UPPER	(55)
#define JOY_ANGLE_X_LOWER	(-55)
#define JOY_ANGLE_Y_UPPER	(55)
#define JOY_ANGLE_Y_LOWER	(-55)
#define JOY_ANALOG_BIAS_X	(19)//static X - 2048 
#define JOY_ANALOG_BIAS_Y	(-15)//static Y - 2048
#define JOY_KEY_DOWN_PERIOD	(10)

#define JOY_ADC_CHN_NUM		2
__IO static uint16_t ADC1_Value[JOY_ADC_CHN_NUM];
extern uint16_t Joystick_X, Joystick_Y;
extern __IO uint16_t Joystick_Z;

static float K_X = 0.0, K_Y = 0.0;
static float B_X = 0.0, B_Y = 0.0;

void Joystick_Init(void);
void JoystickGPIO_Init(void);
void JoystickADC_Init(void);
//void JoystickEXTI_Init(void);
void JoystickPara_Init(void);
void Joystick_CalXY(float calWeight);
//void Joystick_CalZ(void);
uint8_t Joystick_ObtainMode(void);
void Joystick_ConvertAngle(float* xAngle, float* yAngle);

#endif

