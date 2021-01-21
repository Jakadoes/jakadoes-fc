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
#define CAM_ALERT_FIRE_DETECT 0x01
#define CAM_COM_POLL_ALERT 0x11
#define CAM_I2C_ADDRESS 0x26 //CHANGE THIS TO MATCH

#endif /* INC_CAM_H_ */

uint8_t cam_alert_rx_buffer[1];

void    Cam_Poll_Alert();
void    Cam_Transmit_Alert();
uint8_t Cam_Is_Ready();
