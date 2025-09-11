#include "1602alib.h"

// Define global variables
uint8_t current_position = 0;

void LCD_Init(){
	// Just in case something was there before
	// Eg. If reset is pressed and the display is still powered, we need to clear the display
	// See https://x.com/CKalitin/status/1906462865516851547
	LCD_Clear_Display();

	HAL_GPIO_WritePin(RW_GPIO_Port, RW_Pin, 0); // Write R/W pin low

	HAL_GPIO_WritePin(POWER_GPIO_Port, POWER_Pin, 1); // Power up display

 	// Appendix C Page 11 of the datasheet for post power up delay
	// https://www.crystalfontz.com/products/document/964/CFAH1602XYYHJP_v2.0.pdf
	HAL_Delay(40);

	// Page 46 of this datasheet: https://cdn.sparkfun.com/assets/9/5/f/7/b/HD44780.pdf
	// It shows a startup sequence sending 0b00000011 three times, then 0b00000010 once
	// 0b00000011 just sets the interface to 8-bit mode, for some reason this makes it work
	for (int i = 0; i < 3; i++) {
		LCD_Send_4(0, 0b00000011); // Function set (Set interface to be 8 bits long)
		HAL_Delay(1); // Wait a bit between each command
	}
	LCD_Send_4(0, 0b00000010); // Function set (Set interface to be 4 bits long)
	
	LCD_Send_8(0, 0b00101000); // 001D NF00: D=Date Length (4 bits is low, 8 bits high), N=Number of lines, F=Font Size
	LCD_Send_8(0, 0b00001100); // 0000 1DCB: D=Display On/Off, C=Cursor, B=Blink
	LCD_Send_8(0, 0b00000110); // 0000 01IS: I=Increment (1) / Decrement (0), S=Shift
	
	LCD_Clear_Display();
}

// Send an 8-bit command to the display
void LCD_Send_8(uint8_t RS_Pin_value, uint8_t data){
	// We send the higher order bits (left most) first, then the lower order bits. So we get 4+4=8 bits total
	// Look at the top of page 11 in the 1602A documentation for more info https://www.crystalfontz.com/products/document/964/CFAH1602XYYHJP_v2.0.pdf

	LCD_Send_4(RS_Pin_value, data >> 4); // Send higher order bits first
	LCD_Send_4(RS_Pin_value, data); // Send lower order bits second
}

// Send a 4-bit command to the display
// Sends 4 lower order bits in data, eg. 0b0000XXXX
void LCD_Send_4(uint8_t RS_Pin_value, uint8_t data) {
	// Register select pin. RS=0: Command, RS=1: Data
	// Read the documentation: https://www.crystalfontz.com/products/document/964/CFAH1602XYYHJP_v2.0.pdf
	HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, RS_Pin_value);

	// Send lower order bits (right most) first
	HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, data & 0b00000001);
	HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, data & 0b00000010);
	HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, data & 0b00000100);
	HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, data & 0b00001000);

	LCD_Pulse_E_Pin();
}

// We need to pulse the "Enable" pin to read the data pins
// Operation enable signal. Falling edge triggers reading data.
void LCD_Pulse_E_Pin() {
	HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, 1);
	HAL_Delay(E_Pin_Pulse_Delay); // This could be a single microsecond, consult the datasheet if bothered to do so
	HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, 0);
	HAL_Delay(E_Pin_Pulse_Delay);
}

void LCD_Clear_Display() {
    LCD_Send_8(0, 0b00000001);
}

// str: The string to print, char_delay_ms: The delay between each character
void LCD_Send_Word(char *str, uint8_t size, uint32_t char_delay_ms){
	uint8_t u_arr[size];
	LCD_Char_Array_To_Uint8(str, u_arr, size);
	
	// If the size is larger than a single line, clear the display and print from position zero
	// You get an ugly seam in the middle of the word, but this is better than the alternative
	if (size > 16) {
		current_position = 0;
		LCD_Set_Cursor_Pos(0);
		LCD_Clear_Display();
		LCD_Send_Char_Array_At_Cursor(u_arr, 16, char_delay_ms);
		LCD_Set_Cursor_Pos(16);
		LCD_Send_Char_Array_At_Cursor(u_arr + 16, size - 16, char_delay_ms);
		return;
	}

	// If size is greater than the remaining space on line 2, clear the display and print from position zero
	if (current_position + size > 32) {
		current_position = 0;
		LCD_Set_Cursor_Pos(0);
		LCD_Clear_Display();
		LCD_Send_Char_Array_At_Cursor(u_arr, size, char_delay_ms);
		return;
	}

	// If size is greater than the remaining space on line 1, go to line 2
	if (current_position + size >= 16 && current_position < 16) {
		current_position = 16;
		LCD_Set_Cursor_Pos(16);
		LCD_Send_Char_Array_At_Cursor(u_arr, size, char_delay_ms);
		return;
	}

	// Otherwise, print the string
	LCD_Send_Char_Array_At_Cursor(u_arr, size, char_delay_ms);
}

// This prints a char array to the display starting at the current cursor position
// str: The string to print, char_delay_ms: The delay between each character
void LCD_Send_Char_Array_At_Cursor(uint8_t *arr, uint8_t size, uint32_t char_delay_ms) {
	for (int i = 0; i < size; i++) {
		LCD_Send_8(1, arr[i]);
		HAL_Delay(char_delay_ms);
	}
	current_position += size;
}

void LCD_Set_Cursor_Pos(uint8_t pos) {
	// I'm using this command from Appendix C page 6 of the datasheet (5.2.8)
    // Set Display Data RAM Address (1xxxxxxx) to the Address Counter
    // 0b10000000 = first line, 0b11000000 = second line

	if (pos < 16) {
		LCD_Send_8(0, 0b10000000 + pos);
	} else {
		LCD_Send_8(0, 0b11000000 + pos - 16);
	}
}

// Convert char array to uint8_t array
void LCD_Char_Array_To_Uint8(char *c, uint8_t *u, uint8_t size) {
    while (*c && size > 0) {
        char_to_uint8(*c, u);
        c++;
        u++;
		size--;
    }
}

// For Debugging
void Enable_All_Pins() {
	HAL_GPIO_WritePin(D0_GPIO_Port, D0_Pin, 1);
    HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, 1);
    HAL_GPIO_WritePin(D2_GPIO_Port, D2_Pin, 1);
    HAL_GPIO_WritePin(D3_GPIO_Port, D3_Pin, 1);
    HAL_GPIO_WritePin(D4_GPIO_Port, D4_Pin, 1);
    HAL_GPIO_WritePin(D5_GPIO_Port, D5_Pin, 1);
    HAL_GPIO_WritePin(D6_GPIO_Port, D6_Pin, 1);
    HAL_GPIO_WritePin(D7_GPIO_Port, D7_Pin, 1);
    HAL_GPIO_WritePin(E_GPIO_Port, E_Pin, 1);
    HAL_GPIO_WritePin(RS_GPIO_Port, RS_Pin, 1);
    HAL_GPIO_WritePin(RW_GPIO_Port, RW_Pin, 1);
    HAL_GPIO_WritePin(POWER_GPIO_Port, POWER_Pin, 1);
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 1);
}
