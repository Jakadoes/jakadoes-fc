/*
 * cam.h
 *
 *  Created on: Jan 15, 2021
 *      Author: damien sabljak
 */

#ifndef INC_CAM_H_
#define INC_CAM_H_

#include "stdint.h"
#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "i2c.h"
#include "radio.h"
#define CAM_COM_POLL_MODE     0x11//command to request current camera mode
#define CAM_COM_POLL_IMG_DATA 0x13//command to request photo data
#define CAM_COM_POLL_IMG_INFO 0x15//command to request photo meta data (size of array, dimensions, etc)
#define CAM_I2C_ADDRESS 0x26 //CHANGE THIS TO MATCH

#endif /* INC_CAM_H_ */

uint8_t cam_alert_rx_buffer[1];
uint8_t cam_photo_rx_buffer[4000];

void    Cam_Poll_Mode();
void    Cam_Transmit_Alert();
void    Cam_Transmit_Photo(uint32_t startIndex, uint8_t numBytes);
uint8_t Cam_Is_Ready();
void Cam_Handle();
