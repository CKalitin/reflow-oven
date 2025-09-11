#include "uart.h"
#include <string.h>

/**
  * @brief Send a string over UART using its own buffer
  * @param huart: Pointer to UART_HandleTypeDef object
  * @param str: Pointer to the null-terminated string to send
  * @retval None
  */
void UART_Send_String(UART_HandleTypeDef *huart, const char *str) {
    char uart_tx_buff[100] = {0};
    strncpy(uart_tx_buff, str, sizeof(uart_tx_buff) - 1);
    HAL_UART_Transmit(huart, (uint8_t*)uart_tx_buff, strlen(uart_tx_buff), 1000);
}