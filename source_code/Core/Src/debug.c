#include "debug.h"

void UART_Transmit_LL(USART_TypeDef *USARTx, uint8_t *data, uint16_t size)
{
  for (uint16_t i = 0; i < size; i++)
  {
    // Attendre que le registre de transmission soit vide (TXE)
    while (!LL_USART_IsActiveFlag_TXE(USARTx));
    // Écrire le caractère dans le registre de transmission
    LL_USART_TransmitData8(USARTx, data[i]);
  }
  // Attendre que la transmission soit terminée (TC)
  while (!LL_USART_IsActiveFlag_TC(USARTx));
}

/**
 * @brief affichage des données reçues de la télécommande sur l'UART de debug (2)
 *
 */
void print_received_data()
{
  char tab_uart_debug[50] = {0};
  sprintf(tab_uart_debug, "\nY_D:%3d X_D:%3d Y_G:%3d X_G:%3d B_D:%d B_G:%d", trame_decodee[0], trame_decodee[1], trame_decodee[2], trame_decodee[3], trame_decodee[4], trame_decodee[5]);
  UART_Transmit_LL(USART2, (uint8_t *)tab_uart_debug, 44);
}

/**
 * @brief affichage des données de la centrale inertielle sur l'UART de debug (2)
 * 
 */
void print_imu_datas()
{
  char tab_imu_debug[50] = {0};
  sprintf(tab_imu_debug, "\nG_X:%6d G_Y:%6d G_Z:%6d ", GyroData[0], GyroData[1], GyroData[2]);
  UART_Transmit_LL(USART2, (uint8_t *)tab_imu_debug, 34);
  sprintf(tab_imu_debug, "\nA_X:%6d A_Y:%6d A_Z:%6d ", AccData[0], AccData[1], AccData[2]);
  UART_Transmit_LL(USART2, (uint8_t *)tab_imu_debug, 34);
  sprintf(tab_imu_debug,"\nM_X:%5d M_Y:%5d M_Z:%5d ",MagData[0],MagData[1],MagData[2]);
  UART_Transmit_LL(USART2,(uint8_t*)tab_imu_debug,31);
}

void print_imu_register(uint8_t register_value)
{
  char tab_imu_debug[32] = {0};
  sprintf(tab_imu_debug, "\nregister value received : %3d", register_value);
  UART_Transmit_LL(USART2, (uint8_t *)tab_imu_debug, 30);
}

void test_uart1() // 38400 baud
{
  uint8_t test = 'U';
  uint8_t retour_ligne = 10;

  UART_Transmit_LL(USART1, &test, 1);
  UART_Transmit_LL(USART1, &retour_ligne, 1);
}

void test_uart2() // 115200 baud
{
  uint8_t test = 'U';
  uint8_t retour_ligne = 10;

  UART_Transmit_LL(USART2, &test, 1);
  UART_Transmit_LL(USART2, &retour_ligne, 1);
}

void print_to_console(char *buffer, uint16_t buffer_size)
{
  UART_Transmit_LL(USART2, (uint8_t *)buffer, buffer_size);
}