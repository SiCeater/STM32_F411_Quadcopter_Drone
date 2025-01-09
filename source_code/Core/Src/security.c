#include "security.h"

void connection_lost_routine()
{

    if (debug)
        print_to_console("\nremote connection lost !\n", 26);

    // ICI DESACTIVER LA LOOP CONTROL ET SECURITY
    LL_TIM_DisableIT_UPDATE(TIM1);
    LL_TIM_DisableIT_UPDATE(TIM10);
    ESC_setvalues(0,0,0,0);
    onboard_led_on();

    do
    {
        if (LL_USART_IsActiveFlag_IDLE(USART1))// on teste si ya eu une nouvelle reconnexion
            missed_transfers=0;
        LL_USART_ClearFlag_IDLE(USART1); // Efface le drapeau IDLE
    }
    while(missed_transfers==10);

    onboard_led_off();

    // ICI REACTIVER LA LOOP CONTROL ET SECURITY
    LL_TIM_EnableIT_UPDATE(TIM1);
    LL_TIM_EnableIT_UPDATE(TIM10);

    if(debug)
        print_to_console("\nremote connection re-etablished !\n", 35);
}