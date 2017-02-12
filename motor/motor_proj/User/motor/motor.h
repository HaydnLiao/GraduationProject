
#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x.h"

#define TIM_PERIOD		((uint16_t)(10000-1))	//10ms
#define TIM_PRESCALER	((uint16_t)(72-1))		//72MHz/72=1MHz 1us
#define TIM_DEADTIME	((uint8_t)(0x08))

void TIM1_GPIO_Config(void);
void TIM1_PWM_Config(void);
void Motor_Initiate(void);
void Motor_ShutDown(void);
void Motor_Turn(uint8_t speed, uint8_t direction);


#endif

