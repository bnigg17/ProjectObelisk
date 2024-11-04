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
volatile uint16_t image[WIDTH*HEIGHT];

// File variable needed to map camera and i2c
I2C_HandleTypeDef * i2c_inst;
DCMI_HandleTypeDef * dcmi_inst;

const uint8_t OV7670_reg[OV7670_REG_NUM][2] = { { 0x12, 0x80 },

// Image format
		{ 0x12, 0x8 },		// 0x14 = QVGA size, RGB mode; 0x8 = QCIF, YUV, 0xc = QCIF (RGB)
		{ 0xc, 0x8 }, //
		{ 0x11, 0b1000000 }, //

		{ 0xb0, 0x84 },		//Color mode (Not documented??)

		// Hardware window
		{ 0x11, 0x01 },		//PCLK settings, 15fps
		{ 0x32, 0x80 },		//HREF
		{ 0x17, 0x17 },		//HSTART
		{ 0x18, 0x05 },		//HSTOP
		{ 0x03, 0x0a },		//VREF
		{ 0x19, 0x02 },		//VSTART
		{ 0x1a, 0x7a },		//VSTOP

		// Scalling numbers
		{ 0x70, 0x3a },		//X_SCALING
		{ 0x71, 0x35 },		//Y_SCALING
		{ 0x72, 0x11 },		//DCW_SCALING
		{ 0x73, 0xf0 },		//PCLK_DIV_SCALING
		{ 0xa2, 0x02 },		//PCLK_DELAY_SCALING

		// Matrix coefficients
		{ 0x4f, 0x80 }, //
		{ 0x50, 0x80 }, //
		{ 0x51, 0x00 }, //
		{ 0x52, 0x22 }, //
		{ 0x53, 0x5e }, //
		{ 0x54, 0x80 }, //
		{ 0x58, 0x9e },

		// Gamma curve values
		{ 0x7a, 0x20 }, //
		{ 0x7b, 0x10 }, //
		{ 0x7c, 0x1e }, //
		{ 0x7d, 0x35 }, //
		{ 0x7e, 0x5a }, //
		{ 0x7f, 0x69 }, //
		{ 0x80, 0x76 }, //
		{ 0x81, 0x80 }, //
		{ 0x82, 0x88 }, //
		{ 0x83, 0x8f }, //
		{ 0x84, 0x96 }, //
		{ 0x85, 0xa3 }, //
		{ 0x86, 0xaf }, //
		{ 0x87, 0xc4 }, //
		{ 0x88, 0xd7 }, //
		{ 0x89, 0xe8 },

		// AGC and AEC parameters
		{ 0xa5, 0x05 }, //
		{ 0xab, 0x07 }, //
		{ 0x24, 0x95 }, //
		{ 0x25, 0x33 }, //
		{ 0x26, 0xe3 }, //
		{ 0x9f, 0x78 }, //
		{ 0xa0, 0x68 }, //
		{ 0xa1, 0x03 }, //
		{ 0xa6, 0xd8 }, //
		{ 0xa7, 0xd8 }, //
		{ 0xa8, 0xf0 }, //
		{ 0xa9, 0x90 }, //
		{ 0xaa, 0x94 }, //
		{ 0x10, 0x00 },

		// AWB parameters
		{ 0x43, 0x0a }, //
		{ 0x44, 0xf0 }, //
		{ 0x45, 0x34 }, //
		{ 0x46, 0x58 }, //
		{ 0x47, 0x28 }, //
		{ 0x48, 0x3a }, //
		{ 0x59, 0x88 }, //
		{ 0x5a, 0x88 }, //
		{ 0x5b, 0x44 }, //
		{ 0x5c, 0x67 }, //
		{ 0x5d, 0x49 }, //
		{ 0x5e, 0x0e }, //
		{ 0x6c, 0x0a }, //
		{ 0x6d, 0x55 }, //
		{ 0x6e, 0x11 }, //
		{ 0x6f, 0x9f }, //
		{ 0x6a, 0x40 }, //
		{ 0x01, 0x40 }, //
		{ 0x02, 0x60 }, //
		{ 0x13, 0xe7 },

		// Additional parameters
		{ 0x34, 0x11 }, //
		{ 0x3f, 0x00 }, //
		{ 0x75, 0x05 }, //
		{ 0x76, 0xe1 }, //
		{ 0x4c, 0x00 }, //
		{ 0x77, 0x01 }, //
		{ 0xb8, 0x0a }, //
		{ 0x41, 0x18 }, //
		{ 0x3b, 0x12 }, //
		{ 0xa4, 0x88 }, //
		{ 0x96, 0x00 }, //
		{ 0x97, 0x30 }, //
		{ 0x98, 0x20 }, //
		{ 0x99, 0x30 }, //
		{ 0x9a, 0x84 }, //
		{ 0x9b, 0x29 }, //
		{ 0x9c, 0x03 }, //
		{ 0x9d, 0x4c }, //
		{ 0x9e, 0x3f }, //
		{ 0x78, 0x04 }, //
		{ 0x0e, 0x61 }, //
		{ 0x0f, 0x4b }, //
		{ 0x16, 0x02 }, //
		{ 0x1e, 0x00 }, //
		{ 0x21, 0x02 }, //
		{ 0x22, 0x91 }, //
		{ 0x29, 0x07 }, //
		{ 0x33, 0x0b }, //
		{ 0x35, 0x0b }, //
		{ 0x37, 0x1d }, //
		{ 0x38, 0x71 }, //
		{ 0x39, 0x2a }, //
		{ 0x3c, 0x78 }, //
		{ 0x4d, 0x40 }, //
		{ 0x4e, 0x20 }, //
		{ 0x69, 0x00 }, //
		{ 0x6b, 0x3a }, //
		{ 0x74, 0x10 }, //
		{ 0x8d, 0x4f }, //
		{ 0x8e, 0x00 }, //
		{ 0x8f, 0x00 }, //
		{ 0x90, 0x00 }, //
		{ 0x91, 0x00 }, //
		{ 0x96, 0x00 }, //
		{ 0x9a, 0x00 }, //
		{ 0xb1, 0x0c }, //
		{ 0xb2, 0x0e }, //
		{ 0xb3, 0x82 }, //
		{ 0x4b, 0x01 } };

HAL_StatusTypeDef OV7670_init() {
	uint8_t data, i = 0;
	HAL_StatusTypeDef err;

	// Configure camera registers
	for (i = 0; i < OV7670_REG_NUM; i++) {
		data = OV7670_reg[i][1];
		err = write_byte_to_camera(OV7670_reg[i][0], &data);
		print("Writing register: ");
		printHex(i);

		if (err == HAL_ERROR) {
			print("Failed to update register\r\n");
			break;
		}

		HAL_Delay(2);
	}

	return err;
}

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

HAL_StatusTypeDef startCapture(){
	HAL_StatusTypeDef ret = HAL_DCMI_Stop(dcmi_inst);
	if (ret == HAL_ERROR){
		print("HAL_ERROR on DCMI stop");
		return HAL_ERROR;
	}
	ret = HAL_DCMI_Start_DMA(dcmi_inst, DCMI_MODE_SNAPSHOT, (uint32_t)image, WIDTH * HEIGHT);
	if (ret == HAL_ERROR){
		print("HAL_ERROR on DCMI start DMA");
		return HAL_ERROR;
	}
	print("capture complete");
	return HAL_OK;
}

void print_image_data(){
	for (int j = 40; j < WIDTH-40; j++){
		printHex((uint8_t)(image[WIDTH*80 + j] & 0x0F));
	}
}


void camera_init(I2C_HandleTypeDef *hi2c, DCMI_HandleTypeDef * hdcmi){
	i2c_inst = hi2c;
	dcmi_inst = hdcmi;
	//Send transmission to initialize RGB 565 mode
//	uint8_t reset_signal = 0x80;
//	uint8_t RGB_mode_COM7 = 0x04;
//	uint8_t RGB_mode_COM15 = 0x10;
//	HAL_StatusTypeDef ret = write_byte_to_camera(COM7, &reset_signal);
//	if(ret == HAL_ERROR){
//		print("Failed to software reset on COM7");
//	}
//	HAL_Delay(1);	//Requires 1ms delay before modifying other registers
//	uint8_t data;
//	ret = read_byte_from_camera(DBLV, &data);		// Get register content
//	print("DBLV Before:");
//	printHex(data);
//	data &= 0x3F;
////	data |= 0x80;
////	data |= 0x40;
////	data |= 0xC0;									// Modify the PLL scalar bits only, force [7:6] to 1
//	ret = write_byte_to_camera(DBLV, &data);		// Set the bits in register
//	print("DBLV After:");
//	printHex(data);
//	ret = read_byte_from_camera(CLKRC, &data);
//	print("CLKRC Before:");
//	printHex(data);
//	data |= 0x40;									// Force [5:0] to 0
//	ret = write_byte_to_camera(CLKRC, &data);
//	print("CLKRC After:");
//	printHex(data);
//	ret = read_byte_from_camera(COM10, &data);      // Specify HREF/HSYNC to work
//	print("COM10 Before:");
//	printHex(data);
//	data |= 0x60;
//	ret = write_byte_to_camera(COM10, &data);
//	print("COM10 After:");
//	printHex(data);
//	if(ret == HAL_ERROR){
//		print("Failed to set internal clk properly");
//	}
//	ret = write_byte_to_camera(COM7, &RGB_mode_COM7);
//	if(ret == HAL_ERROR){
//		print("Failed to set COM7 for RGB");
//	}
//	ret = write_byte_to_camera(COM15, &RGB_mode_COM15);
//	if(ret == HAL_ERROR){
//		print("Failed to set COM15 for RGB");
//	}
//	print("Camera Initialized");
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

//void OV7670_GPIO_Init() {
//	GPIO_InitTypeDef GPIO_InitStructure;
//
//	// PG9 - VSYNC, PG11 - D3
//	GPIO_InitStructure.Pin = GPIO_PIN_9 | GPIO_PIN_11;
//	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
//	GPIO_InitStructure.Pull = GPIO_NOPULL;
//	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//	GPIO_InitStructure.Alternate = GPIO_AF13_DCMI;
//	HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
//
//	// PD3 - D5
//	GPIO_InitStructure.Pin = GPIO_PIN_5;
//	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
//	GPIO_InitStructure.Pull = GPIO_NOPULL;
//	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//	GPIO_InitStructure.Alternate = GPIO_AF13_DCMI;
//	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
//
//	// PE4 - D4, PE5 - D6, PE6 - D7
//	GPIO_InitStructure.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
//	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
//	GPIO_InitStructure.Pull = GPIO_NOPULL;
//	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//	GPIO_InitStructure.Alternate = GPIO_AF13_DCMI;
//	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
//
//	// PC6 - D0, PC7 - D1, PC8 - D2
//	GPIO_InitStructure.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8;
//	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
//	GPIO_InitStructure.Pull = GPIO_NOPULL;
//	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//	GPIO_InitStructure.Alternate = GPIO_AF13_DCMI;
//	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
//
//	// PA4 - HSYNC, PA6 - PIXCLK
//	GPIO_InitStructure.Pin = GPIO_PIN_4 | GPIO_PIN_6;
//	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
//	GPIO_InitStructure.Pull = GPIO_NOPULL;
//	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//	GPIO_InitStructure.Alternate = GPIO_AF13_DCMI;
//	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
//
//
//
//}




