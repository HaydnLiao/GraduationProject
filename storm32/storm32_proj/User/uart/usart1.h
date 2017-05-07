
#ifndef __USART_1_H
#define __USART_1_H

#include "stm32f10x.h"
#include "global_math.h"

#define USART1_RX_LEN	(256)
extern uint8_t usart1RxBuff[USART1_RX_LEN];
extern volatile uint16_t cntUsart1RxBuff;

void Usart1_Init(uint32_t bps);

#endif

