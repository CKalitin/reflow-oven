#include "main.h"

#define B 4036.0
#define R0 100000.0
#define R_DIV 985.0
#define T0 298.15
#define V_SOURCE 3.28

#define ADC_VALUE_ERROR 65 // Add this to ADC reading to account for error in measurement (error is least significant bits), see project notes https://docs.google.com/document/d/1razJQ7vhXl0n5-KTTxf6exUxA-YA-EfNmRcggGIF1KQ/edit?usp=sharing

// Note that UART_HandleTypeDef and ADC_HandleTypeDef are parameters so that we can change if we use UART1, UART2, etc. or ADC1, ADCx, etc.
uint16_t Get_Averaged_ADC_Values(ADC_HandleTypeDef, int, int);

uint32_t ADC_Val_To_Temperature(uint16_t, UART_HandleTypeDef*);