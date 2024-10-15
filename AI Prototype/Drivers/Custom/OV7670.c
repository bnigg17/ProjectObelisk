/*
 * OV7670.c
 *
 *  Created on: Sep 30, 2024
 *      Author: bnigg2
 */
#include "../../Core/Inc/main.h"
#include "putty.h"
#include "OV7670.h"

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
#define COM6 0x0F //PID register for reading test

//Instantiate a structure to store the pixels of the image
uint16_t image[640*480];

// File variable needed to map camera and i2c
I2C_HandleTypeDef * i2c_inst;

/*
 * Wrapper for I2C communication to camera
 * Reg - The slave register to manipulate
 * Data - Pointer to data to be transmitted
 * */
HAL_StatusTypeDef write_byte_to_camera(uint8_t reg, uint8_t * data){
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
	HAL_Delay(5);
	if(ret == HAL_ERROR){
		print("HAL_ERROR for 3 phase write for camera write");
	}
	return ret;	// Success
}

HAL_StatusTypeDef read_byte_from_camera(uint8_t reg, uint8_t * data) {
	uint8_t regRead = reg;
	uint8_t * rxBuff = data;
	HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(i2c_inst, READ_ADDR, &regRead, 1, HAL_MAX_DELAY);
	if(ret == HAL_ERROR){
		print("HAL_ERROR for 2 phase write required for camera read");
	}
	ret = HAL_I2C_Master_Receive(i2c_inst, READ_ADDR, rxBuff, 1, HAL_MAX_DELAY);
	if(ret == HAL_ERROR){
		print("HAL_ERROR for 2 phase read required for camera read");
	}
	return ret;
}


uint16_t * read_image(){
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
			image[row_index*640 + column_index] = pixel;
			column_index++;
			HSYNC =  HAL_GPIO_ReadPin(HSYNC_GPIO_Port, HSYNC_Pin);
		}
		row_index++;
		VSYNC =  HAL_GPIO_ReadPin(GPIOC, VSYNC_Pin);
	}
	return &image;
}

void camera_init(I2C_HandleTypeDef *hi2c){
	i2c_inst = hi2c;
	/* Reset currently driven high by wire GPIO has no effect */
//	HAL_GPIO_WritePin(GPIOA, CAM_RST_Pin, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(GPIOA, CAM_RST_Pin, GPIO_PIN_SET);
	/* May need to be messed with */
//	HAL_GPIO_WritePin(PWDN_GPIO_Port, PWDN_Pin, GPIO_PIN_SET);
	//Send transmission to initialize RGB 565 mode
	uint8_t reset_signal = 0x80;
	uint8_t RGB_mode_COM7 = 0x04;
	uint8_t RGB_mode_COM15 = 0x10;
	HAL_StatusTypeDef ret = write_byte_to_camera(COM7, &reset_signal);
	if(ret == HAL_ERROR){
		print("Failed to software reset on COM7");
	}
	HAL_Delay(1);	//Requires 1ms delay before modifying other registers
	ret = write_byte_to_camera(COM7, &RGB_mode_COM7);
	if(ret == HAL_ERROR){
		print("Failed to set COM7 for RGB");
	}
	ret = write_byte_to_camera(COM15, &RGB_mode_COM15);
	if(ret == HAL_ERROR){
		print("Failed to set COM15 for RGB");
	}
	print("Camera Initialized");
	return;
}

void read_test(){
	print("Read Test:");
	uint8_t data;
	uint8_t PID_reg = 0x0A; // PID reg has constant value of 0x76
	(void)read_byte_from_camera(PID_reg, &data);
	if(data == 0x76){
		print("Read Works");
	}
	else{
		print("Read Fail");
	}
	printHex(data);
}

