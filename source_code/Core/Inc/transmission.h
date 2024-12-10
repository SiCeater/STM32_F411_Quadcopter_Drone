
#ifndef __TRANSMISSION_H__
#define __TRANSMISSION_H__

#include "global.h"
#include "usart.h"
#include "debug.h"

void start_remote_routine();
void decode_trame();
void UART_Receive_DMA(USART_TypeDef *USARTx, uint8_t *buffer, uint16_t size);

#endif

