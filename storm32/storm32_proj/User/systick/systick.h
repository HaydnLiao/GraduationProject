
#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"

#define Delay_ms(x) Delay_us(1000*x)

static __IO uint32_t SystickDelayedTime;

void Systick_Init(void);
void Delay_us(uint32_t delayedTime);
void CalledBySystickHandler(void);

#endif

