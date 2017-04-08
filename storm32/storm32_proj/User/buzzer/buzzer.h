
#ifndef __BUZZER_H
#define __BUZZER_H

#include "stm32f10x.h"

#define BUZZER_ON		GPIO_SetBits(GPIOC, GPIO_Pin_3)
#define BUZZER_OFF		GPIO_ResetBits(GPIOC, GPIO_Pin_3)
#define BUZZER_TOGGLE	GPIOC->ODR ^= GPIO_Pin_3

void Buzzer_Init(void);

#endif

