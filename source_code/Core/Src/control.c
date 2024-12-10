#include "control.h"

/**
 * @brief boucle de controle PID
 * 
 */
void start_control_loop()
{
    LL_TIM_EnableIT_UPDATE(TIM1); // 400Hz loop
    LL_TIM_EnableCounter(TIM1);
    if (debug)
        print_to_console("\ncontrol loop          : started", 32);
}

/**
 * @brief routine de controle du drone dans l'espace
 *
 */
void control()
{
}
