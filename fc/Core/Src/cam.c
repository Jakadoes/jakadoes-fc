/*
 * cam.c
 *
 *  Created on: Jan 15, 2021
 *      Author: damien sabljak
 */

#include "cam.h"
#include "stdint.h"
#include "i2c.h"

uint8_t Cam_Is_Ready()
{
	//returns HAL address
	uint8_t cam_ready_status = I2c_IsDeviceReady(CAM_I2C_ADDRESS);
	return cam_ready_status;
}

void Cam_Poll_Alert()
{
	//send single read command
	//receive 8 bits of data
	uint8_t command = CAM_COM_POLL_ALERT; //coefficient is shifted one to left (check data sheet)
	I2c_Master_Transmit(CAM_I2C_ADDRESS, &command, 1);
	//I2c_Master_Receive(BARO_I2C_ADDRESS, baro_prom_rx_buffer, 2);
}
