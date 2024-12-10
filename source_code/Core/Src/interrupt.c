#include "interrupt.h"

void TIM1_UP_TIM10_IRQHandler(void)
{
    // Vérifiez la source de l'interruption
    if (LL_TIM_IsActiveFlag_UPDATE(TIM1))
    {
      LL_TIM_ClearFlag_UPDATE(TIM1); // Nettoyer le flag
      control();
    }
    if (LL_TIM_IsActiveFlag_UPDATE(TIM10))
    {
      LL_TIM_ClearFlag_UPDATE(TIM10); // Nettoyez le flag d'interruption de TIM10
      missed_transfers++;
      if (missed_transfers > max_missed_transfers) {
        connection_lost_routine();
      }
      // Code pour gérer la fin de la période de TIM10
    }
}

void USART1_IRQHandler(void)
{
    // Vérifiez si l'interruption provient de la réception d'un caractère (RXNE flag)
    if (LL_USART_IsActiveFlag_RXNE(USART1))
    {
        // Lire le caractère reçu pour vider le flag RXNE
        //uint8_t received_data = LL_USART_ReceiveData8(USART1); peut etre que cette ligne vide le buffer avant que le DMA le fasse

        // Remettre le compteur du Timer à 0
        LL_TIM_SetCounter(TIM10, 0);

        // Remettre à zéro le compteur de manquements
        missed_transfers = 0;

        // (Optionnel) Ajouter ici un traitement pour `received_data` si nécessaire
    }
}

void DMA2_Stream2_IRQHandler(void)
{
    if (LL_DMA_IsActiveFlag_TC2(DMA2)) // Vérifie la fin de transfert
    {
        LL_DMA_ClearFlag_TC2(DMA2); // Efface le drapeau
        // Code pour traiter les données reçues
    }

    if (LL_DMA_IsActiveFlag_TE2(DMA2)) // Vérifie les erreurs DMA
    {
        LL_DMA_ClearFlag_TE2(DMA2); // Efface le drapeau
        // Gestion des erreurs
    }
}

