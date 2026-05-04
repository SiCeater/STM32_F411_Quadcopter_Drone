#ifndef __CONTROL_LOOP_H__
#define __CONTROL_LOOP_H__

#include <stdint.h>
#include <stdio.h>
#include "global.h"
#include "usart.h"
#include "tim.h"
#include "esc_driver.h"
#include "remote_driver.h"
#include "debug.h"
#include "security.h"
#include "led_driver.h"

void start_control_loop();
void control();

#endif