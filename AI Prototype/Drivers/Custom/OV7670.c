/*
 * OV7670.c
 *
 *  Created on: Sep 30, 2024
 *      Author: bnigg2
 */
#include "../../Core/Inc/main.h"

/*
 * Can communicate with either I2C or SCCB. Difference is I2C needs pull up resistors for 1's and 0's only. SCCB allow for impeaded state
 * which is why pull ups are not needed.00
 *
 *
 */

#define WRITE_ADDR 0x42
#define READ_ADDR 0x43
#define COM7 0x12	// 0 - Normal operation, Resets all regs and resets bit to 0 (i.e normal operation mode)
#define COM2 0x09	// Standby Mode Enable - Active High
#define COM15 0x40 //RGB setting

// File variable needed to map camera and i2c
I2C_HandleTypeDef * i2c_inst;

//Instantiate a structure to store the pixels of the image
uint16_t image[480][640];

/*
 * Wrapper for I2C communication to camera
 * Reg - The slave register to manipulate
 * Data - Pointer to data to be transmitted
 * */
uint8_t write_byte_to_camera(uint8_t reg, const uint8_t * data){
	//TODO: complete the write to camera transmission function, need to refer to I2C exercise on how writing worked
	// Check for valid pointer
	if(data == NULL){
		return -1;
	}
	// Populate buffer
	uint8_t txBuff[2];
	txBuff[0] = reg;	// Byte 0 is always register addr
	txBuff[1] = *data;	// Byte n carry data
	HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(i2c_inst, WRITE_ADDR, txBuff, 2, HAL_MAX_DELAY);
	if(ret == HAL_ERROR){
		return -1;
	}
	return 0;	// Success
}

void read_from_camera(const uint8_t * SubAddress) {
	uint8_t buffer[2] = {READ_ADDR, SubAddress};
	//TODO: again should refer to I2C exercise about how buffer scanning works
	return;
}


void read_image(){
	// Instantiate GPIO objects
	GPIO_PinState HSYNC =  HAL_GPIO_ReadPin(HSYNC_GPIO_Port, HSYNC_Pin);
	GPIO_PinState PCLK = HAL_GPIO_ReadPin(GPIOA, PCLK_Pin);
	GPIO_PinState Data[8];
	GPIO_PinState VSYNC =  HAL_GPIO_ReadPin(GPIOC, VSYNC_Pin);
	while(VSYNC == GPIO_PIN_RESET){ //wait for rising edge
		VSYNC =  HAL_GPIO_ReadPin(GPIOC, VSYNC_Pin);
	}
	while(VSYNC == GPIO_PIN_SET){ //wait for falling edge
		VSYNC =  HAL_GPIO_ReadPin(GPIOC, VSYNC_Pin);
	}
	int row_index = 0;
	while(VSYNC == GPIO_PIN_RESET){ //While in this image frame
		int column_index = 0;
		HSYNC = HAL_GPIO_ReadPin(HSYNC_GPIO_Port, HSYNC_Pin);
		while(HSYNC == GPIO_PIN_RESET){ //wait for rising edge, indicating the start of a row
				HSYNC =  HAL_GPIO_ReadPin(HSYNC_GPIO_Port, HSYNC_Pin);
		}
		while(HSYNC == GPIO_PIN_SET){ //data in this row
			PCLK = HAL_GPIO_ReadPin(GPIOA, PCLK_Pin);
			while(PCLK == GPIO_PIN_RESET){ //wait for rising edge, indicating the transmission of a byte
				PCLK = HAL_GPIO_ReadPin(GPIOA, PCLK_Pin);
			}
			Data[7] =  HAL_GPIO_ReadPin(GPIOC, D0_Pin);
			Data[6] =  HAL_GPIO_ReadPin(GPIOB, D1_Pin);
			Data[5] =  HAL_GPIO_ReadPin(GPIOC, D2_Pin);
			Data[4] =  HAL_GPIO_ReadPin(GPIOB, D3_Pin);
			Data[3] =  HAL_GPIO_ReadPin(GPIOC, D4_Pin);
			Data[2] =  HAL_GPIO_ReadPin(GPIOB, D5_Pin);
			Data[1] =  HAL_GPIO_ReadPin(GPIOC, D6_Pin);
			Data[0] =  HAL_GPIO_ReadPin(GPIOA, D7_Pin);
			uint16_t pixel = 0;
			for(int i = 0; i < 8; i++){
				pixel <<= 1;
				if(Data[i] == GPIO_PIN_RESET){
					pixel |= 0x0000;
				}
				else{
					pixel |= 0x0001;
				}
			}
			PCLK = HAL_GPIO_ReadPin(GPIOA, PCLK_Pin);
			while(PCLK == GPIO_PIN_RESET){ //wait for rising edge, indicating the transmission of a byte
				PCLK = HAL_GPIO_ReadPin(GPIOA, PCLK_Pin);
			}
			Data[7] =  HAL_GPIO_ReadPin(GPIOC, D0_Pin);
			Data[6] =  HAL_GPIO_ReadPin(GPIOB, D1_Pin);
			Data[5] =  HAL_GPIO_ReadPin(GPIOC, D2_Pin);
			Data[4] =  HAL_GPIO_ReadPin(GPIOB, D3_Pin);
			Data[3] =  HAL_GPIO_ReadPin(GPIOC, D4_Pin);
			Data[2] =  HAL_GPIO_ReadPin(GPIOB, D5_Pin);
			Data[1] =  HAL_GPIO_ReadPin(GPIOC, D6_Pin);
			Data[0] =  HAL_GPIO_ReadPin(GPIOA, D7_Pin);
			for(int i = 0; i < 8; i++){
				pixel <<= 1;
				if(Data[i] == GPIO_PIN_RESET){
					pixel |= 0x0000;
				}
				else{
					pixel |= 0x0001;
				}
			}
			image[row_index][column_index] = pixel;
			column_index++;
			HSYNC =  HAL_GPIO_ReadPin(HSYNC_GPIO_Port, HSYNC_Pin);
		}
		row_index++;
		VSYNC =  HAL_GPIO_ReadPin(GPIOC, VSYNC_Pin);
	}
}

void camera_init(I2C_HandleTypeDef *hi2c){
	i2c_inst = hi2c;
	//First reset and turn PWDN high so we aren't in SUSPEND mode
	HAL_GPIO_WritePin(GPIOA, CAM_RST_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, CAM_RST_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PWDN_GPIO_Port, PWDN_Pin, GPIO_PIN_RESET);
	//Send transmission to initialize RGB 565 mode
	uint8_t RGB_mode_COM7 = 0x04;
	uint8_t RGB_mode_COM15 = 0x10;
	write_byte_to_camera(COM7, &RGB_mode_COM7);
	write_byte_to_camera(COM15, &RGB_mode_COM15);
	print("thing");
	return;
}

