#include "control_loop.h"

/**
 * @brief boucle de controle PID
 * 
 */
void start_control_loop()
{
    LL_TIM_EnableIT_UPDATE(TIM1);   // 1000Hz control loop qui est calee sur le timer 1 (syncro ESC)
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
    decode_trame();
    //faire un truc avec les valeurs decodées recues ici
    ESC_set_global_values();
}
