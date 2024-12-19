#include "control.h"

/**
 * @brief boucle de controle PID
 * 
 */
void start_control_loop()
{
    LL_TIM_EnableIT_UPDATE(TIM1);   // 400Hz loop
    LL_TIM_EnableIT_UPDATE(TIM10);  // security loop
    LL_TIM_EnableCounter(TIM1);
    LL_TIM_EnableCounter(TIM10);
    if (debug)
        print_to_console("\ncontrol loop          : started", 32);
}

/**
 * @brief routine de controle du drone dans l'espace
 *
 */
void control()
{
    // debug_count++;
    // if (debug_count==200)
    // {
    //     onboard_led_on();
    // }
    // if (debug_count==400)
    // {
    //     onboard_led_off();
    //     debug_count=0;
    // }

}
