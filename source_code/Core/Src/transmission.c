#include "transmission.h"

void start_remote_routine()
{
  LL_USART_EnableIT_RXNE(USART1); // activation de l'interruption sur l'uart de la telecommande (securité)
  UART_Receive_DMA(USART1, transmit, 7);
}

void UART_Receive_DMA(USART_TypeDef *USARTx, uint8_t *buffer, uint16_t size)
{
    // Configure l'adresse source (registre DR de l'UART)
    LL_DMA_SetPeriphAddress(DMA2, LL_DMA_STREAM_2, LL_USART_DMA_GetRegAddr(USARTx));

    // Configure l'adresse de destination (le buffer où les données seront stockées)
    LL_DMA_SetMemoryAddress(DMA2, LL_DMA_STREAM_2, (uint32_t)buffer);

    // Configure la taille des données à transférer
    LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_2, size);

    // Activer le mode circulaire si besoin (facultatif)
     LL_DMA_SetMode(DMA2, LL_DMA_STREAM_2, LL_DMA_MODE_CIRCULAR);

    // Activer le DMA Stream pour démarrer la réception
    LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_2);

    // Activer le DMA sur l'UART pour la réception
    LL_USART_EnableDMAReq_RX(USARTx);
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