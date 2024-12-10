
#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

extern uint8_t transmit[7];
extern uint8_t trame_decodee[6];
extern bool connected_flag;
extern bool debug;
extern bool remote_already_seen;
extern uint16_t top_left_motor;
extern uint16_t top_right_motor;
extern uint16_t bottom_left_motor;
extern uint16_t bottom_right_motor;
extern uint8_t mag_adjust[3];
extern uint8_t imu_spi_miso_buffer[32];
extern bool DMA_transfer_flag;
extern int16_t AccData[3];
extern int16_t MagData[3];
extern int16_t GyroData[3];
extern volatile uint8_t missed_transfers;
extern uint8_t max_missed_transfers;

#endif
    
