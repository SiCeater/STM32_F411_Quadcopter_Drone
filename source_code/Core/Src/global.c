#include "global.h"

uint8_t transmit[7] = {0};
uint8_t trame_decodee[6] = {0};
bool remote_already_seen = false;
bool connected_flag = false;
bool debug = false;
uint16_t top_left_motor = 0;
uint16_t top_right_motor = 0;
uint16_t bottom_left_motor = 0;
uint16_t bottom_right_motor = 0;
uint8_t mag_adjust[3] = {0};
uint8_t imu_spi_miso_buffer[32] = {0};
bool DMA_transfer_flag = 0;
int16_t AccData[3] = {0};
int16_t MagData[3] = {0};
int16_t GyroData[3] = {0};
volatile uint8_t missed_transfers;
uint8_t max_missed_transfers=10;