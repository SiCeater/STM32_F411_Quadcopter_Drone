#ifndef __MOTORS_H__
#define __MOTORS_H__

#include <stdint.h>
#include <stdio.h>
#include "tim.h"
#include "global.h"
#include "debug.h"

void ESC_init();
void ESC_setvalues(uint16_t top_left_motor, uint16_t top_right_motor, uint16_t bottom_left_motor, uint16_t bottom_right_motor);

#endif