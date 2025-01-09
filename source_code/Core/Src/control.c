#include "control.h"

/**
 * @brief boucle de controle PID
 * 
 */
void start_control_loop()
{
    LL_TIM_EnableIT_UPDATE(TIM1);   // 400Hz control loop
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
    uint16_t value = 0;
    if (trame_decodee[0]>=100)
    {
        value = trame_decodee[0];
        value = value - 100;
        value = value * 5;
        if(value>=500)
            value=500;
    }
    ESC_setvalues(value,value,value,value);
}
