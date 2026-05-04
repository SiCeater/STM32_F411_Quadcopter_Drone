#ifndef __ESC_DRIVER_H__
#define __ESC_DRIVER_H__

#include "tim.h"
#include "global.h"
#include "debug.h"

void ESC_init();
void ESC_set_values(uint16_t top_left_motor, uint16_t top_right_motor, uint16_t bottom_left_motor, uint16_t bottom_right_motor);
void ESC_set_global_values();
void ESC_TLMS(uint16_t x);
void ESC_TRMS(uint16_t x);
void ESC_BRMS(uint16_t x);
void ESC_BLMS(uint16_t x); 

#endif