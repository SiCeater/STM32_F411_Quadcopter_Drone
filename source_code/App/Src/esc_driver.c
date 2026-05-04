#include "esc_driver.h"
#include "global.h"
#include <stdint.h>

/**
 * @brief initialisation des 4 ESC
 * ils sont commandé par des signaux PWM en sortie du timer 1 de 1000Hz
 * plage de 125 µs-> 250 µs => 2500->5000
 */
void ESC_init()
{
    // Configurer les rapports cycliques initiaux des canaux (1ms = 500)
    LL_TIM_OC_SetCompareCH1(TIM1, 2500); //top left motor
    LL_TIM_OC_SetCompareCH2(TIM1, 2500); //top right motor
    LL_TIM_OC_SetCompareCH3(TIM1, 2500); //bottom right motor
    LL_TIM_OC_SetCompareCH4(TIM1, 2500); //bottom left motor

    LL_TIM_EnableAllOutputs(TIM1);

    // Démarrer le compteur du Timer 1
    LL_TIM_EnableCounter(TIM1);
    
    // Remettre le compteur du Timer 1 à 0
    LL_TIM_SetCounter(TIM1, 0);

    LL_TIM_EnableAllOutputs(TIM1);
    
    // Activer les sorties PWM des canaux du Timer 1
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1);
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH2);
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3);
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH4);

    // Debug optionnel
    if (debug)
    {
        print_to_console("\nESC 1                 : initialized", 36);
        print_to_console("\nESC 2                 : initialized", 36);
        print_to_console("\nESC 3                 : initialized", 36);
        print_to_console("\nESC 4                 : initialized", 36);
    }
}

/**
 * @brief Set the motors rotation speed (0->2500)
 *
 * @param top_left_motor
 * @param top_right_motor
 * @param bottom_left_motor
 * @param bottom_right_motor
 */
void ESC_set_values(uint16_t top_left_motor, uint16_t top_right_motor, uint16_t bottom_right_motor, uint16_t bottom_left_motor)
{
    if (top_left_motor>2500) {
        top_left_motor=2500;
    }
    else if (top_left_motor<0) {
        top_left_motor=0;
    }
    LL_TIM_OC_SetCompareCH1(TIM1, 2500 + top_left_motor);
    if (top_right_motor>2500) {
        top_right_motor=2500;
    }
    else if (top_right_motor<0) {
        top_right_motor=0;
    }
    LL_TIM_OC_SetCompareCH2(TIM1, 2500 + top_right_motor);
    if (bottom_right_motor>2500) {
        bottom_right_motor=2500;
    }
    else if (bottom_right_motor<0) {
        bottom_right_motor=0;
    }
    LL_TIM_OC_SetCompareCH3(TIM1, 2500 + bottom_right_motor);
    if (bottom_left_motor>2500) {
        bottom_left_motor=2500;
    }
    else if (bottom_left_motor<0) {
        bottom_left_motor=0;
    }
    LL_TIM_OC_SetCompareCH4(TIM1, 2500 + bottom_left_motor);
}

void ESC_set_global_values()
{
    ESC_set_values(m1, m2, m3, m4);
}

void ESC_TLMS(uint16_t x)
{
    if (x>2500) {
        x=2500;
    }
    else if (x<0) {
        x=0;
    }
    LL_TIM_OC_SetCompareCH1(TIM1, 2500 + x);
}

void ESC_TRMS(uint16_t x)
{
    if (x>2500) {
        x=2500;
    }
    else if (x<0) {
        x=0;
    }
    LL_TIM_OC_SetCompareCH2(TIM1, 2500 + x);
}

void ESC_BRMS(uint16_t x)
{
    if (x>2500) {
        x=2500;
    }
    else if (x<0) {
        x=0;
    }
    LL_TIM_OC_SetCompareCH3(TIM1, 2500 + x);
}

void ESC_BLMS(uint16_t x)
{
    if (x>2500) {
        x=2500;
    }
    else if (x<0) {
        x=0;
    }
    LL_TIM_OC_SetCompareCH4(TIM1, 2500 + x);
}