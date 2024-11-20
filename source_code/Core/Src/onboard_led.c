#include "onboard_led.h"

void onboard_led_on()
{
    HAL_GPIO_WritePin(onboard_led_GPIO_Port, onboard_led_Pin, GPIO_PIN_RESET); // reset = allumage de la led sur PA1
}

void onboard_led_off()
{
    HAL_GPIO_WritePin(onboard_led_GPIO_Port, onboard_led_Pin, GPIO_PIN_SET); // set = on etteint la led sur PA1
}

void onboard_led_toggle()
{
    HAL_GPIO_TogglePin(onboard_led_GPIO_Port, onboard_led_Pin);
}
