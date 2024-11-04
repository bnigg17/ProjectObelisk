/*
 * OV7670.h
 *
 *  Created on: Sep 30, 2024
 *      Author: bnigg2
 */
#ifndef CUSTOM_OV7670_H_
#define CUSTOM_OV7670_H_

#define OV7670_REG_NUM 			123


void camera_init(I2C_HandleTypeDef *hi2c, DCMI_HandleTypeDef * hdcmi);
HAL_StatusTypeDef write_byte_to_camera(uint8_t reg, uint8_t * data);
void read_test();
HAL_StatusTypeDef read_byte_from_camera(uint8_t reg, uint8_t * data);
HAL_StatusTypeDef startCapture();
void print_image_data();
HAL_StatusTypeDef OV7670_init();
//void OV7670_GPIO_Init();


#endif /* CUSTOM_OV7670_H_ */
