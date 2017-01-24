
#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

#define LEDG_ON		GPIO_ResetBits(GPIOB, GPIO_Pin_0)
#define LEDG_OFF	GPIO_SetBits(GPIOB, GPIO_Pin_0)
#define LEDB_ON		GPIO_ResetBits(GPIOB, GPIO_Pin_1)
#define LEDB_OFF	GPIO_SetBits(GPIOB, GPIO_Pin_1)
#define LEDR_ON		GPIO_ResetBits(GPIOB, GPIO_Pin_5)
#define LEDR_OFF	GPIO_SetBits(GPIOB, GPIO_Pin_5)

void Led_Init(void);

#endif

