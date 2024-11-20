#ifndef __CONTROL_H__
#define __CONTROL_H__

#include <stdint.h>
#include <stdio.h>
#include "global.h"
#include "usart.h"
#include "tim.h"
#include "motors.h"
#include "transmission.h"
#include "debug.h"
#include "security.h"
#include "onboard_led.h"

void start_control_loop();
void control();

#endif