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

// File variable needed to map camera and i2c
I2C_HandleTypeDef * i2c_inst;

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

void camera_init(I2C_HandleTypeDef *hi2c){
	i2c_inst = hi2c;
	//First reset and turn PWDN high so we aren't in SUSPEND mode
	HAL_GPIO_WritePin(GPIOA, CAM_RST_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, CAM_RST_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PWDN_GPIO_Port, PWDN_Pin, GPIO_PIN_SET);
	//Send transmission to initialize RGB mode
	uint8_t RGB_mode = 0x02;
	write_byte_to_camera(COM7, &RGB_mode);
	return;
}

