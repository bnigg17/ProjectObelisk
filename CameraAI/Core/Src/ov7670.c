/*
 * ov7670.c
 *
 *      Author: bnigg2 and bmalo4
 */
#include "main.h"
#include "stm32f7xx_hal.h"
#include "common.h"
#include "ov7670.h"
#include "ov7670Config.h"
#include "ov7670Reg.h"
#include "putty.h"


/*** Internal Static Variables ***/
static DCMI_HandleTypeDef *sp_hdcmi;
static DMA_HandleTypeDef  *sp_hdma_dcmi;
static I2C_HandleTypeDef  *sp_hi2c;
static uint32_t    s_destAddressForContiuousMode;
static void (* s_cbHsync)(uint32_t h);
static void (* s_cbVsync)(uint32_t v);
static uint32_t s_currentH;
static uint32_t s_currentV;

/*** Internal Function Declarations ***/
static HAL_StatusTypeDef ov7670_write(uint8_t regAddr, uint8_t data);
static HAL_StatusTypeDef ov7670_read(uint8_t regAddr, uint8_t *data);

/*** External Function Defines ***/
HAL_StatusTypeDef ov7670_init(DCMI_HandleTypeDef *p_hdcmi, DMA_HandleTypeDef *p_hdma_dcmi, I2C_HandleTypeDef *p_hi2c, uint16_t * dstAddr)
{
  sp_hdcmi     = p_hdcmi;
  sp_hdma_dcmi = p_hdma_dcmi;
  sp_hi2c      = p_hi2c;
  //s_destAddressForContiuousMode = 0;
  s_destAddressForContiuousMode = (uint32_t)dstAddr;


  HAL_GPIO_WritePin(CAMERA_RESET_GPIO_Port, CAMERA_RESET_Pin, GPIO_PIN_RESET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(CAMERA_RESET_GPIO_Port, CAMERA_RESET_Pin, GPIO_PIN_SET);
  HAL_Delay(100);

  uint8_t buffer[1];
  HAL_StatusTypeDef ret = HAL_OK;
  ret |= ov7670_write(0x12, 0x80);  // RESET
  HAL_Delay(30);

  ret |= ov7670_read(0x0b, buffer);

//  print("OV7670 Inialized");
  return ret;
}

HAL_StatusTypeDef ov7670_config(uint32_t mode)
{
  ov7670_stopCap();
  ov7670_write(0x12, 0x80);  // RESET
  HAL_Delay(30);
  HAL_StatusTypeDef ret = HAL_OK;
  for(int i = 0; OV7670_reg[i][0] != REG_END; i++) {
    ret |= ov7670_write(OV7670_reg[i][0], OV7670_reg[i][1]);
    HAL_Delay(1);
  }
  if(ret == HAL_ERROR){
	  print("OV7670 Config Fail");
  }
  else{
//	  print("OV7670 Config Success");
  }
  return ret;
}

HAL_StatusTypeDef ov7670_startCap(uint32_t capMode, uint32_t destAddress)
{
	HAL_StatusTypeDef ret;
	ret = ov7670_stopCap();
	if(ret == HAL_ERROR){
		print("OVO7607 stop capture error");
	}
	if (capMode == OV7670_CAP_CONTINUOUS) {
		/* note: continuous mode automatically invokes DCMI, but DMA needs to be invoked manually */
		s_destAddressForContiuousMode = destAddress;
		ret = HAL_DCMI_Start_DMA(sp_hdcmi, DCMI_MODE_CONTINUOUS, destAddress, OV7670_QVGA_WIDTH * OV7670_QVGA_HEIGHT/2);
		if(ret != HAL_OK){
			print("OVO7607 start capture continous error");
		}
	}
	else if (capMode == OV7670_CAP_SINGLE_FRAME) {
		//s_destAddressForContiuousMode = 0;
		ret = HAL_DCMI_Start_DMA(sp_hdcmi, DCMI_MODE_SNAPSHOT, destAddress, OV7670_QVGA_WIDTH*OV7670_QVGA_HEIGHT/2);
		if(ret == HAL_ERROR){
			print("OVO7607 start capture snapshot error");
		}
	}

  return ret;
}

HAL_StatusTypeDef ov7670_stopCap()
{
	HAL_StatusTypeDef ret = HAL_DCMI_Stop(sp_hdcmi);
	if(ret != HAL_OK){
		print("OV7670 Stop Capture Error");
	}
	return ret;
}

void ov7670_registerCallback(void (*cbHsync)(uint32_t h), void (*cbVsync)(uint32_t v))
{
  s_cbHsync = cbHsync;
  s_cbVsync = cbVsync;
}

void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
//	print("HERE Frame event");
////  printf("FRAME %d\n", HAL_GetTick());
//  if(s_cbVsync)s_cbVsync(s_currentV);
//  if(s_destAddressForContiuousMode != 0) {
//    HAL_DMA_Start_IT(hdcmi->DMA_Handle, (uint32_t)&hdcmi->Instance->DR, s_destAddressForContiuousMode, OV7670_QVGA_WIDTH * OV7670_QVGA_HEIGHT/2);
//  }
  s_currentV++;
  s_currentH = 0;
}

void HAL_DCMI_VsyncEventCallback(DCMI_HandleTypeDef *hdcmi)
{
//	char buffer[33];
//	sprintf(buffer, "VSYNC - New frame is available/n");
//	print_mod(buffer, 33);
	printHex(s_currentV++);
	ov7670_stopCap();
	print_image((uint16_t *)s_destAddressForContiuousMode);
    //HAL_DMA_Start_IT(hdcmi->DMA_Handle, (uint32_t)&hdcmi->Instance->DR, s_destAddressForContiuousMode, OV7670_QVGA_WIDTH * OV7670_QVGA_HEIGHT/2);
}

//void HAL_DCMI_LineEventCallback(DCMI_HandleTypeDef *hdcmi)
//{
////  printf("HSYNC %d\n", HAL_GetTick());
//  if(s_cbHsync)s_cbHsync(s_currentH);
//  s_currentH++;
//}

/*** Internal Function Defines ***/

HAL_StatusTypeDef ov7670_write(uint8_t reg, uint8_t data){
	//TODO: complete the write to camera transmission function, need to refer to I2C exercise on how writing worked
	// Populate buffer
	uint8_t txBuff[2];
	txBuff[0] = reg;	// Byte 0 is always register addr
	txBuff[1] = data;	// Byte n carry data
	HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(sp_hi2c, SLAVE_ADDR, txBuff, 2, HAL_MAX_DELAY);
	HAL_Delay(5);
	if(ret == HAL_ERROR){
		print("HAL_ERROR for 3 phase write for camera write");
	}
	return ret;	// Success
}

HAL_StatusTypeDef ov7670_read(uint8_t reg, uint8_t * data) {
	uint8_t regRead = reg;
	uint8_t * rxBuff = data;
	HAL_StatusTypeDef ret = HAL_I2C_Master_Transmit(sp_hi2c, SLAVE_ADDR | 0x01, &regRead, 1, HAL_MAX_DELAY);
	if(ret == HAL_ERROR){
		print("HAL_ERROR for 2 phase write required for camera read");
	}
	ret = HAL_I2C_Master_Receive(sp_hi2c, SLAVE_ADDR | 0x01, rxBuff, 1, HAL_MAX_DELAY);
	if(ret == HAL_ERROR){
		print("HAL_ERROR for 2 phase read required for camera read");
	}
	return ret;
}

/* Functions for testing */
#if TEST_MODE

void ov7670_snapshot(uint8_t mode){
	/* TODO need to make register writes to test setting configurations */
	switch(mode){
	case 0:
		//ov7670_write(OV7670_reg[i][0], OV7670_reg[i][1]);
	case 1:
		//ov7670_write(OV7670_reg[i][0], OV7670_reg[i][1]);
	case 2:
		//ov7670_write(OV7670_reg[i][0], OV7670_reg[i][1]);
	default:
		return;
	}
	ov7670_startCap(OV7670_CAP_SINGLE_FRAME, s_destAddressForContiuousMode);
}

#endif
