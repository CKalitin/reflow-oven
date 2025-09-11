#include "adc.h"
#include "uart.h"

/**
  * @brief Take numSamples ADC values, average them, then return both raw and error adjusted values
  * @param hadc: ADC_HandleTypeDef object
  * @param numSamples: Number of ADC values to average
  * @param msPerObv: Milliseconds between each ADC observation
  * @param adcValuesAveraged: Pointer to the averaged ADC value
  * @param adcValuesAdjusted: Pointer to the averaged ADC value, adjusted for error
  * @retval adcValuesAveraged, int
  */
uint16_t Get_Averaged_ADC_Values(ADC_HandleTypeDef hadc, int numSamples, int msPerObv){
  int adcValuesSum = 0;

  HAL_ADC_Start(&hadc);
  HAL_ADC_PollForConversion(&hadc, 1);

  // Get numSamples ADC values, each 1ms apart
  for (int i = 0; i < numSamples; i++){
    adcValuesSum += HAL_ADC_GetValue(&hadc) + ADC_VALUE_ERROR;
    HAL_Delay(msPerObv);
  }

  return adcValuesSum / numSamples; // Set pointer output
}

uint32_t ADC_Val_To_Temperature(uint16_t adc_value, UART_HandleTypeDef *huart) {
  // Voltage divider formula: Vout = Vsource * (R_thermistor / (R_thermistor + R_div)
  // Solve for R_thermistor: R_thermistor = (R_div * Vout) / (Vsource - Vout)
  
  double v_out = V_SOURCE * adc_value / 4095;
  double r_thermistor = R_DIV * v_out / (V_SOURCE - v_out);

  // Temperature Formula: T = 1 / (1/T0 + (1/B) * ln(R/R0))

  double temperature = 1 / (1 / T0 + (1 / B) * log(r_thermistor / R0));

  // Convert from Kelvin to Celsius
  temperature -= 273.15;

  // Print values over UART using integers
  int v_out_mv = (int)(v_out * 1000);
  int r_therm_int = (int)r_thermistor;
  int temp_centideg = (int)(temperature * 100);
  char buf[100];
  snprintf(buf, sizeof(buf), "Adc: %d, Vout: %d mV, R_thermistor: %d ohm, Temperature: %d.%02d C\r\n", adc_value, v_out_mv, r_therm_int, temp_centideg / 100, temp_centideg % 100);
  UART_Send_String(huart, buf);

  return (uint32_t)(temperature * 1000); // Return temperature in millidegrees Celsius
}