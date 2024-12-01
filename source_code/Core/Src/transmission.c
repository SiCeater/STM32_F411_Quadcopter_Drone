#include "transmission.h"

void start_remote_routine()
{
  HAL_UART_Receive_DMA(&huart1, transmit, 7);
}

/**
 * @brief fontion qui remet le tableau "transmit" en ordre apres la réception en cas de décallage d'indice
 * puis stocke le tableau remis en ordre dans "trame_decodee"
 */
void decode_trame()
{
  uint8_t trame_ordonnee[7] = {0, 0, 0, 0, 0, 0, 0};
  uint8_t debut_trame = 0;

  for (uint8_t i = 0; i < 7; i++)
  {
    if (transmit[i] == 250)
      debut_trame = i;
  }

  if (debut_trame != 0)
    for (uint8_t i = 0; i < 7; i++)
    {
      if ((debut_trame + i) >= 7)
        trame_ordonnee[i] = transmit[i - (7 - debut_trame)];
      else
        trame_ordonnee[i] = transmit[i + debut_trame];
    }
  else
    for (uint8_t i = 0; i < 7; i++)
    {
      trame_ordonnee[i] = transmit[i];
    }

  for (uint8_t i = 0; i < 6; i++)
  {
    trame_decodee[i] = trame_ordonnee[i + 1];
  }
}