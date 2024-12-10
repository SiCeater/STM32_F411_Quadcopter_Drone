#include "onboard_led.h"

void onboard_led_on()
{
    // Écrire un RESET (0) pour allumer la LED
    LL_GPIO_ResetOutputPin(onboard_led_GPIO_Port, onboard_led_Pin);
}

void onboard_led_off()
{
    // Écrire un SET (1) pour éteindre la LED
    LL_GPIO_SetOutputPin(onboard_led_GPIO_Port, onboard_led_Pin);
}

void onboard_led_toggle()
{
    // Basculer l'état actuel de la broche (SET <-> RESET)
    LL_GPIO_TogglePin(onboard_led_GPIO_Port, onboard_led_Pin);
}
