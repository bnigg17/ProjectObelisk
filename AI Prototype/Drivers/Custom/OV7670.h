/*
 * OV7670.h
 *
 *  Created on: Sep 30, 2024
 *      Author: bnigg2
 */
#ifndef CUSTOM_OV7670_H_
#define CUSTOM_OV7670_H_

void camera_init(I2C_HandleTypeDef *hi2c);
uint8_t write_byte_to_camera(uint8_t reg, const uint8_t * data);
uint8_t read_test();
uint8_t read_from_camera(const uint8_t * SubAddress);
void read_image();



#endif /* CUSTOM_OV7670_H_ */
