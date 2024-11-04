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
	(void)HAL_UART_Transmit(huart_inst, (uint8_t *)data, size, HAL_MAX_DELAY);
}

void print_image(uint16_t *image) {
    char buffer[200];  // Allocate a larger buffer to hold all data
    int buffer_length = 0;  // Current length of the buffer
    int pixels_per_line = 3; // Number of pixels to print per line
    int pixel_count = 0;      // Count the number of pixels printed in the current line

    for (int j = 0; j < 30; j++) {
        // Extract RGB components from RGB565 format
        uint8_t red = (uint8_t)((image[j] & 0xF800) >> 11);
        uint8_t green = (uint8_t)((image[j] & 0x07E0) >> 5);
        uint8_t blue = (uint8_t)(image[j] & 0x001F);

        // Format each pixel's color values in hexadecimal
        // Use buffer_length to append to the end of the buffer
        buffer_length += sprintf(buffer + buffer_length, "(R=0x%02X G=0x%02X B=0x%02X)  ", red, green, blue);

        pixel_count++;
        // After printing a certain number of pixels, send a newline
        if (pixel_count >= pixels_per_line) {
            buffer[buffer_length] = '\0';	//Ensure print can add appropriate spacing
            print(buffer); // Send the current buffer via UART
            buffer_length = 0;  // Reset buffer length for the next line
            pixel_count = 0;     // Reset pixel count
        }
    }
}


