#include "security.h"

void connection_lost_routine()
{

    if (debug)
        print_to_console("\nremote connection lost !\n", 26);

    // ICI DESACTIVER LA LOOP CONTROL
    NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn);

    while (missed_transfers!=0)
    {
        ESC_setvalues(0,0,0,0);
        onboard_led_on();
    }
    onboard_led_off();

    // ICI REACTIVER LA LOOP CONTROL
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

    if(debug)
        print_to_console("\nremote connection re-etablished !\n", 35);
}