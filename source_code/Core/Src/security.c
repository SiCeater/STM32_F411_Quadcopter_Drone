#include "security.h"

void connection_lost_routine()
{
    // onboard_led_off();

    // if (debug)
    //     print_to_console("\nremote connection lost !\n", 26);
    while (missed_transfers>=max_missed_transfers)
    {
        ESC_setvalues(0,0,0,0);
    }
    // onboard_led_on();
    // print_to_console("\nremote connection re-etablished !\n", 35);
}