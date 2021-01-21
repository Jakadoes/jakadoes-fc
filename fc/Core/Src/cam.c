/*
 * cam.c
 *
 *  Created on: Jan 15, 2021
 *      Author: damien sabljak
 */

#include "cam.h"
I2C_HandleTypeDef hi2c2;
#include"stm32f1xx_hal.h"

uint8_t cam_alert_rx_buffer[1];
uint8_t Cam_Is_Ready()
{
	//returns HAL address
	uint8_t cam_ready_status = I2c_IsDeviceReady(CAM_I2C_ADDRESS);
	return cam_ready_status;
}

void Cam_Poll_Alert()
{
	uint8_t test = 0x22;
	//send single read command
	//receive 8 bits of data
	uint8_t command = CAM_COM_POLL_ALERT; //coefficient is shifted one to left (check data sheet)
	test = 0x22;
	//Radio_Transmit_Raw(&test, 1);
	I2c_Master_Transmit(CAM_I2C_ADDRESS, &command, 1);
	//HAL_Delay(1000);
	uint8_t cam_status = Cam_Is_Ready();

	//Radio_Transmit_Raw(&cam_status, 1);
	//if(Cam_Is_Ready() == HAL_OK)//line needs to be not busy for error not to occur
	//{
		 test = 0x33;
		 //Radio_Transmit_Raw(&test, 1);
		 //HAL_Delay(600);
		 I2c_Master_Receive(CAM_I2C_ADDRESS, cam_alert_rx_buffer, 1);
		 //Radio_Transmit_Raw(&test, 1);
		 //HAL_I2C_Master_Receive_DMA (&hi2c2, CAM_I2C_ADDRESS,cam_alert_rx_buffer, 1)
		 //Radio_Transmit_Raw(&cam_alert_rx_buffer, 1);
		 //transmission is getting stuck on, causing the cam to stay in receiving mode
		 //very important to send and expect to recieve the same amount of bytes, otherwise EIO and busy lock down errors
	//}
}

void Cam_Transmit_Alert()
{//send alert status to ground station
	MAV_Send_Msg_Named_Value_Int("FireAlert", (uint32_t) cam_alert_rx_buffer[0]);
}
