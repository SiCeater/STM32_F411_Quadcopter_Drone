#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "usart.h"
#include "global.h"
#include "transmission.h"

void UART_Transmit_LL(USART_TypeDef *USARTx, uint8_t *data, uint16_t size);
void print_received_data();
void print_imu_datas();
void print_imu_register(uint8_t register_value);
void test_uart1();
void test_uart2();
void print_to_console(char *buffer, uint16_t buffer_size);

#endif