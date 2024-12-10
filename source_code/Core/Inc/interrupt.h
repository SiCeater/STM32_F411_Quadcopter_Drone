#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include "global.h"
#include "tim.h"
#include "usart.h"
#include "control.h"
#include "imu.h"
#include "debug.h"
#include "security.h"

void TIM1_UP_TIM10_IRQHandler(void);
void USART1_IRQHandler(void);
void DMA2_Stream2_IRQHandler(void);

#endif