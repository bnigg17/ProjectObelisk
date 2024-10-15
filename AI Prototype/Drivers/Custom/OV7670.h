/*
 * OV7670.h
 *
 *  Created on: Sep 30, 2024
 *      Author: bnigg2
 */
#ifndef CUSTOM_OV7670_H_
#define CUSTOM_OV7670_H_


void camera_init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef write_byte_to_camera(uint8_t reg, uint8_t * data);
void read_test();
HAL_StatusTypeDef read_byte_from_camera(uint8_t reg, uint8_t * data);
uint16_t * read_image();

void ram_test();


#endif /* CUSTOM_OV7670_H_ */
