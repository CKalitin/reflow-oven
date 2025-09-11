#include "display.h"

void Display_Temperature(uint32_t temperature, UART_HandleTypeDef *huart){
    int temp_int = temperature / 1000;
    int temp_frac = temperature % 1000;

    uint8_t display_str[16];
    snprintf((char *)display_str, sizeof(display_str), "T: %d.%03d C\r\n", temp_int, temp_frac);

    //UART_Send_String(huart, (char *)display_str);

    display_str[strlen((char *)display_str) - 2] = '\0'; // remove final two characters (carriage return and newline) for LCD display
    
    LCD_Set_Cursor_Pos(0); // Set cursor to first row, first column
    LCD_Send_Char_Array_At_Cursor(display_str, strlen((char *)display_str), 0);
}

void Display_Voltage(uint16_t voltage, UART_HandleTypeDef *huart){
    int volt_int = voltage / 1000;
    int volt_frac = voltage % 1000;

    uint8_t display_str[16];
    snprintf((char *)display_str, sizeof(display_str), "V: %d.%03d V\r\n", volt_int, volt_frac);
    
    //UART_Send_String(huart, (char *)display_str);

    display_str[strlen((char *)display_str) - 2] = '\0'; // remove final two characters (carriage return and newline) for LCD display
    
    LCD_Set_Cursor_Pos(16); // Set cursor to second row, first column
    LCD_Send_Char_Array_At_Cursor(display_str, strlen((char *)display_str), 0);
}