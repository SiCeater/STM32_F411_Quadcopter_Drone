#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdint.h>
#include <stdio.h>
#include "usart.h"
#include "global.h"
#include "transmission.h"

void print_received_data();
void print_imu_datas();
void print_imu_register(uint8_t register_value);
void test_uart1();
void test_uart2();
void print_to_console(char *buffer, uint16_t buffer_size);

#endif