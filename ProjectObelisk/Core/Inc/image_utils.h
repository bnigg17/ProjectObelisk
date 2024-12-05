/*
 * image_utils.h
 *
 *  Created on: Dec 2, 2024
 *      Author: BRADE
 */

#include <stdint.h>

#ifndef IMAGE_UTILS_H_
#define IMAGE_UTILS_H_

uint32_t YUVtoRGB888(int16_t Y, int16_t U, int16_t V);
uint16_t RGB888toRGB565(uint32_t RGB);
uint16_t YUVtoRGB565(int16_t Y, int16_t U, int16_t V);
uint32_t RGB565toRGB888(uint16_t RGB);

#endif /* IMAGE_UTILS_H_ */
