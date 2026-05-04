
#ifndef __REMOTE_DRIVER_H__
#define __REMOTE_DRIVER_H__

#include "global.h"
#include "usart.h"

void start_remote_routine();
void decode_trame();
void UART_Receive_DMA(uint8_t *buffer, uint16_t size);

#endif

