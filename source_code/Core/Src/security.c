#include "security.h"

void connection_lost_routine()
{
    // onboard_led_off();

    // if (debug)
    //     print_to_console("\nremote connection lost !\n", 26);

    // ICI DESACTIVER LA LOOP CONTROL

    while (missed_transfers!=0)
    {
        ESC_setvalues(0,0,0,0);
    }

    // ICI REACTIVER LA LOOP CONTROL

    // onboard_led_on();
    // print_to_console("\nremote connection re-etablished !\n", 35);
}