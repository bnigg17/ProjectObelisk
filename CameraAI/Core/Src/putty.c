/*
 * putty.c
 *
 *  Created on: Oct 5, 2024
 *      Author: bnigg2
 */

#include "main.h"
#include "stm32f7xx_hal.h"
#include "ov7670.h"

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


void terminal_recieve(uint8_t * rxBuff, uint32_t bytes){
	HAL_StatusTypeDef ret = HAL_UART_Receive(huart_inst, rxBuff, bytes, HAL_MAX_DELAY);
    if(ret == HAL_ERROR){
    	print("Receive Error");
    }
}


void print_mod(const uint8_t * data, uint32_t size){
	HAL_StatusTypeDef ret = HAL_UART_Transmit(huart_inst, (uint8_t *)data, size, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		print("print error");
	}
}


#define NUM_PIXELS OV7670_WIDTH * OV7670_HEIGHT

//void print_image(uint32_t *image) {
//    uint8_t buffer[3 * NUM_PIXELS + 1];  // 320 * 6 rows * 3 pixels
//    uint32_t length = 0;
//
//
//    buffer[length++] = (uint8_t)'S';  // Start character
//
//    for (int i = 0; i < NUM_PIXELS/2; i++) {
//        // Extract RGB components from RGB565 format
//    	uint16_t pixel1 = (uint16_t)((image[i] & 0xFFFF0000) >> 16);
//    	uint8_t red1 = (uint8_t)(((pixel1 & 0xF800) >> 11) * 255 / 31);    // 5 bits to 8 bits
//		uint8_t green1 = (uint8_t)(((pixel1 & 0x07E0) >> 5) * 255 / 63);   // 6 bits to 8 bits
//		uint8_t blue1 = (uint8_t)((pixel1 & 0x001F) * 255 / 31);           // 5 bits to 8 bits
//
//		uint16_t pixel2 = (uint16_t)(image[i] & 0x0000FFFF);
//    	uint8_t red2 = (uint8_t)(((pixel2 & 0xF800) >> 11) * 255 / 31);    // 5 bits to 8 bits
//		uint8_t green2 = (uint8_t)(((pixel2 & 0x07E0) >> 5) * 255 / 63);   // 6 bits to 8 bits
//		uint8_t blue2 = (uint8_t)((pixel2 & 0x001F) * 255 / 31);           // 5 bits to 8 bits
//
//
//        // Store each color component in the buffer
//        buffer[length++] = red1;
//        buffer[length++] = green1;
//        buffer[length++] = blue1;
//        buffer[length++] = red2;
//        buffer[length++] = green2;
//        buffer[length++] = blue2;
//    }
//    // Transmit the buffer with the specified length
//    print_mod(buffer, length);
//}

void print_image(uint32_t *image) {
    uint8_t buffer[2 * NUM_PIXELS + 1];  // 320 * 6 rows * 3 pixels
    uint32_t length = 0;


    buffer[length++] = (uint8_t)'S';  // Start character

    for (int i = 0; i < NUM_PIXELS/2; i++) {
        // Extract RGB components from RGB565 format
    	uint8_t byte0 = (uint8_t)((0xFF000000 & image[i]) >> 24);
    	uint8_t byte1 = (uint8_t)((0x00FF0000 & image[i]) >> 16);
    	uint8_t byte2 = (uint8_t)((0x0000FF00 & image[i]) >> 8);
    	uint8_t byte3 = (uint8_t)(0x000000FF & image[i]);


        // Store each color component in the buffer
        buffer[length++] = byte0;
        buffer[length++] = byte1;
        buffer[length++] = byte2;
        buffer[length++] = byte3;
    }
    // Transmit the buffer with the specified length
    print_mod(buffer, length);
}



