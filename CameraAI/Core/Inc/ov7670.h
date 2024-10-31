/*
 * ov7670.h
 *
 *      Author: bnigg2 and bmalo4
 *      helper code from take-iwiw
 */

#ifndef OV7670_OV7670_H_
#define OV7670_OV7670_H_

#define OV7670_MODE_QVGA_RGB565 0
#define OV7670_MODE_QVGA_YUV    1

#define OV7670_CAP_CONTINUOUS   0
#define OV7670_CAP_SINGLE_FRAME 1

/*** Internal Const Values, Macros ***/
#define OV7670_QVGA_WIDTH  320
#define OV7670_QVGA_HEIGHT 240

HAL_StatusTypeDef ov7670_init(DCMI_HandleTypeDef *p_hdcmi, DMA_HandleTypeDef *p_hdma_dcmi, I2C_HandleTypeDef *p_hi2c, uint16_t * dstAddr);
HAL_StatusTypeDef ov7670_config(uint32_t mode);
HAL_StatusTypeDef ov7670_startCap(uint32_t capMode, uint32_t destAddress);
HAL_StatusTypeDef ov7670_stopCap();
void ov7670_registerCallback(void (*cbHsync)(uint32_t h), void (*cbVsync)(uint32_t v));

#endif /* OV7670_OV7670_H_ */
