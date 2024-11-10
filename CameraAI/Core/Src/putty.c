/*
 * putty.c
 *
 *  Created on: Oct 5, 2024
 *      Author: bnigg2
 */

#include "main.h"
#include "stm32f7xx_hal.h"

#define CHAR_ROWS 30
#define CHAR_COLS 80

UART_HandleTypeDef * huart_inst;

void init_putty(UART_HandleTypeDef * huart){
    huart_inst = huart;
}

/* CANNOT EXCEED 80 CHARACTERS, only fits default size PuTTY window*/
void print(const char * data){
	uint8_t output[80] = {' ', ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', ' ',' ',' ',' ',' ',' ',' ',' ',' '};
	int i = 0;
	while(data[i] != '\0'){
		output[i] = data[i];
		i++;
	}
	(void)HAL_UART_Transmit(huart_inst, output, 80, 0xFFFF);
}

void printHex(uint8_t data){
    // Each byte can be represented in 2 hex digits
    const char hex_digits[] = "0123456789ABCDEF";
    char hex_str[5];
    // Fill in the two characters for the hex representation
    hex_str[0] = '0';
    hex_str[1] = 'x';
    hex_str[2] = hex_digits[(data >> 4) & 0x0F]; // High nibble
    hex_str[3] = hex_digits[data & 0x0F];        // Low nibble
    hex_str[4] = '\0';                           // Null-terminate the string
    print(hex_str);

}


/*
Inteneded for recieve input for PuTTy terminal
*/
void putty_recieve(uint8_t * rxBuff){
    return;
}

//Coherently 'print' the image data of size 160x120
//void print_image(uint16_t * image){
//
//	for (int j = 0; j < 10; j++){
//		uint8_t red = (uint8_t)((image[j] & 0xF800) >> 11);
//		uint8_t green = (uint8_t)((image[j] & 0x07E0) >> 5);
//		uint8_t blue = (uint8_t)(image[j] & 0x001F);
//		print("New Pixel");
//		printHex(red);
//		printHex(green);
//		printHex(blue);
//	}
//}

void print_mod(const char * data, uint32_t size){
	HAL_StatusTypeDef ret = HAL_UART_Transmit(huart_inst, (uint8_t *)data, size, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		print("print error");
	}
}

#define NUM_PIXELS 30
#define PIXEL_FORMAT_SIZE 17

void print_image(uint16_t *image) {
    char buffer[PIXEL_FORMAT_SIZE*NUM_PIXELS+2];  // Allocate a larger buffer to hold all data, 23 is format size, 2 is for start and stop char
    int buffer_length = 0;  // Current length of the buffer
    buffer[buffer_length++] = 'S';	// Start char
    for (int j = 0; j < NUM_PIXELS; j++) {
        // Extract RGB components from RGB565 format
        uint8_t red = (uint8_t)((image[j] & 0xF800) >> 11);
        uint8_t green = (uint8_t)((image[j] & 0x07E0) >> 5);
        uint8_t blue = (uint8_t)(image[j] & 0x001F);

        // Use buffer_length to append to the end of the buffer
        buffer_length += snprintf(buffer + buffer_length, PIXEL_FORMAT_SIZE, "(0x%02X,0x%02X,0x%02X)", red, green, blue);

    }
    buffer[buffer_length++] = 'E';	// End char
    print_mod(buffer, buffer_length);
}


