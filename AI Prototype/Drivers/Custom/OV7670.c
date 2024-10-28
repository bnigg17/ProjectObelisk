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
#define COM10 0x15 //for horizontal sync setting
#define COM6 0x0F //PID register for reading test
#define DBLV 0x6B // PLL scalar for input clock. Bits [7:6]
#define CLKRC 0x11 // PLL Divisor for input clock. Bits [5:0]
#define HREF 0x32
#define WIDTH 160
#define HEIGHT 120

//Instantiate a structure to store the pixels of the image
uint16_t image[WIDTH*HEIGHT];

// File variable needed to map camera and i2c
I2C_HandleTypeDef * i2c_inst;
DCMI_HandleTypeDef * dcmi_inst;

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
//HAL_StatusTypeDef HAL_DCMI_Start_DMA(DCMI_HandleTypeDef *hdcmi, uint32_t DCMI_Mode, uint32_t pData, uint32_t Length)

//HAL_StatusTypeDef startCapture(){
//	HAL_DCMI_Start_DMA(dcmi_inst, DCMI_MODE_CONTINUOUS, &image, WIDTH * LENGTH);
//
//}


void camera_init(I2C_HandleTypeDef *hi2c, DCMI_HandleTypeDef * hdcmi){
	i2c_inst = hi2c;
	dcmi_inst = hdcmi;
	//Send transmission to initialize RGB 565 mode
	uint8_t reset_signal = 0x80;
	uint8_t RGB_mode_COM7 = 0x04;
	uint8_t RGB_mode_COM15 = 0x10;
	HAL_StatusTypeDef ret = write_byte_to_camera(COM7, &reset_signal);
	if(ret == HAL_ERROR){
		print("Failed to software reset on COM7");
	}
	HAL_Delay(1);	//Requires 1ms delay before modifying other registers
	uint8_t data;
	ret = read_byte_from_camera(DBLV, &data);		// Get register content
	print("DBLV Before:");
	printHex(data);
	data &= 0x3F;
//	data |= 0x80;
//	data |= 0x40;
//	data |= 0xC0;									// Modify the PLL scalar bits only, force [7:6] to 1
	ret = write_byte_to_camera(DBLV, &data);		// Set the bits in register
	print("DBLV After:");
	printHex(data);
	ret = read_byte_from_camera(CLKRC, &data);
	print("CLKRC Before:");
	printHex(data);
	data |= 0x40;									// Force [5:0] to 0
	ret = write_byte_to_camera(CLKRC, &data);
	print("CLKRC After:");
	printHex(data);
	ret = read_byte_from_camera(COM10, &data);      // Specify HREF/HSYNC to work
	print("COM10 Before:");
	printHex(data);
	data |= 0x60;
	ret = write_byte_to_camera(COM10, &data);
	print("COM10 After:");
	printHex(data);
	if(ret == HAL_ERROR){
		print("Failed to set internal clk properly");
	}
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

void ram_test(){
	int counter = 0;
	for(int i = 0; i < 640 * 480; i++){
		image[i] = (uint16_t)(i % 640);
	}
	uint16_t buffer[80];
	for(int i = 0; i < 640 * 480; i++){
		if(counter < 40){
			buffer[i%40] = image[i];
		}
		else{
			counter = 0;
			print(buffer);
		}
		counter++;
	}
}

