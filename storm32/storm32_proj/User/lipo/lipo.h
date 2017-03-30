
#ifndef __LIPO_H
#define __LIPO_H

#include "stm32f10x.h"

static __IO uint16_t ADC1_Value;
extern float Lipo_Voltage;

void Lipo_Init(void);
void LipoGPIO_Init(void);
void LipoADC_Init(void);
void Lipo_CalVoltage(void);

#endif
