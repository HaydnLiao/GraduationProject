
#ifndef __LIPO_H
#define __LIPO_H

#include "stm32f10x.h"

#define LIPO_VOL_LOWEST ((float)(0.001))

__IO static uint16_t ADC2_Value;
extern float Lipo_Voltage;

void Lipo_Init(void);
void LipoGPIO_Init(void);
void LipoADC_Init(void);
void Lipo_CalVoltage(float calWeight);

#endif
