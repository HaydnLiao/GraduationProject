
#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

/**
#define LED0_ON		GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define LED0_OFF	GPIO_SetBits(GPIOB, GPIO_Pin_12)
#define LED1_ON		GPIO_ResetBits(GPIOB, GPIO_Pin_13)
#define LED1_OFF	GPIO_SetBits(GPIOB, GPIO_Pin_13)
*/

#define BIT_SET(port, pin)		(port->BSRR = pin)
#define BIT_RESET(port, pin)	(port->BRR = pin)
#define BIT_REVERSE(port, pin)	(port->ODR ^= pin)

#define LED0_ON			BIT_SET(GPIOB, GPIO_Pin_12)
#define LED0_OFF		BIT_RESET(GPIOB, GPIO_Pin_12)
#define LED0_TOGGLE		BIT_REVERSE(GPIOB, GPIO_Pin_12)
#define LED1_ON			BIT_SET(GPIOB, GPIO_Pin_13)
#define LED1_OFF		BIT_RESET(GPIOB, GPIO_Pin_13)
#define LED1_TOGGLE		BIT_REVERSE(GPIOB, GPIO_Pin_13)

void Led_Init(void);

#endif

