#include "motors.h"

/**
 * @brief initialisation des 4 ESC
 * ils sont commandé par des signaux PWM en sortie du timer 1 de 400Hz
 * plage de 1ms->2ms => 500->1000
 */
void ESC_init()
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
    htim1.Instance->CNT = 0;
    TIM1->CCR1 = 500;
    TIM1->CCR2 = 500;
    TIM1->CCR3 = 500;
    TIM1->CCR4 = 500;
    if (debug)
    {
        print_to_console("\nESC 1                 : initialized", 36);
        print_to_console("\nESC 2                 : initialized", 36);
        print_to_console("\nESC 3                 : initialized", 36);
        print_to_console("\nESC 4                 : initialized", 36);
    }
}

/**
 * @brief Set the motors rotation speed (0->500)
 *
 * @param top_left_motor
 * @param top_right_motor
 * @param bottom_left_motor
 * @param bottom_right_motor
 */
void ESC_setvalues(uint16_t top_left_motor, uint16_t top_right_motor, uint16_t bottom_left_motor, uint16_t bottom_right_motor)
{
    TIM1->CCR1 = 500 + top_left_motor;
    TIM1->CCR2 = 500 + top_right_motor;
    TIM1->CCR3 = 500 + bottom_left_motor;
    TIM1->CCR4 = 500 + bottom_right_motor;
}