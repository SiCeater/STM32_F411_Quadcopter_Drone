#include "interrupt.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  // if (htim == &htimx)// Remplacez htimX par votre handle Timer
  // {
  //
  // }
  if (htim == &htim10) // securité en cas de deconnection de la telecommande
  {
    missed_transfers++;
    if (missed_transfers > max_missed_transfers) {
      connection_lost_routine();
    }
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &huart1) { // si on recoit on remet le compteur a 0
        __HAL_TIM_SET_COUNTER(&htim10, 0); // Remet le compteur du Timer à 0
        missed_transfers = 0;            // Remet le compteur de manquements à 0
    }
}

