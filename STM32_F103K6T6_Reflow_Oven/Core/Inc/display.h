#include "main.h"
#include "1602alib.h"

void Display_Temperature(uint32_t temperature, UART_HandleTypeDef *huart);
void Display_Voltage(uint16_t voltage, UART_HandleTypeDef *huart);