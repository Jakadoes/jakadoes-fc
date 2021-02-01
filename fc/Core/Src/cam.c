/*
 * cam.c
 *
 *  Created on: Jan 15, 2021
 *      Author: damien sabljak
 */

#include "cam.h"
I2C_HandleTypeDef hi2c2;
#include"stm32f1xx_hal.h"
#define HANDLE_MODE_POLLING     0
#define HANDLE_MODE_ALERT       1
#define HANDLE_MODE_PHOTOSEND   2

uint8_t  cam_alert_rx_buffer[1];
uint8_t  cam_photo_rx_buffer[4000];
uint8_t  handle_mode = 0;
uint32_t handle_photo_index = 0;
uint8_t  handle_photo_length = 10;
uint8_t  handle_photo_stop = 3000;

uint8_t Cam_Is_Ready()
{
	//returns HAL address
	uint8_t cam_ready_status = I2c_IsDeviceReady(CAM_I2C_ADDRESS);
	return cam_ready_status;
}

void Cam_Poll_Alert()
{
	//uint8_t test = 0x22;
	//send single read command
	//receive 8 bits of data
	uint8_t command = CAM_COM_POLL_ALERT;
	//test = 0x22;
	//Radio_Transmit_Raw(&test, 1);
	I2c_Master_Transmit(CAM_I2C_ADDRESS, &command, 1);
	//HAL_Delay(1000);
	//uint8_t cam_status = Cam_Is_Ready();

	//Radio_Transmit_Raw(&cam_status, 1);
	//if(Cam_Is_Ready() == HAL_OK)//line needs to be not busy for error not to occur
	//{
		 //test = 0x33;
		 //Radio_Transmit_Raw(&test, 1);
		 //HAL_Delay(600);
		 I2c_Master_Receive(CAM_I2C_ADDRESS, cam_alert_rx_buffer, 1);
		 //Radio_Transmit_Raw(&test, 1);
		 //Radio_Transmit_Raw(&cam_alert_rx_buffer, 1);
		 //transmission is getting stuck on, causing the cam to stay in receiving mode
		 //very important to send and expect to recieve the same amount of bytes, otherwise EIO and busy lock down errors
	//}
}

void Cam_Poll_Image(uint32_t startIndex, uint8_t numBytes)
{//receives image buffer data starting at startIndex of length numBytes
	uint8_t command = CAM_COM_POLL_IMAGE;
	uint8_t details[5];//format [startIndex [4:1] , numBytes[0]
		details[4] = (uint8_t) (startIndex >> 8*3) & 0x000F;
		details[3] = (uint8_t) (startIndex >> 8*2) & 0x000F;
		details[2] = (uint8_t) (startIndex >> 8*1) & 0x000F;
		details[1] = (uint8_t) startIndex & 0x000F;
		details[0] = numBytes;
	I2c_Master_Transmit(CAM_I2C_ADDRESS, &command, 1);//transmit image command
	I2c_Master_Transmit(CAM_I2C_ADDRESS, &details, 5);//transmit details of requested transmission
	I2c_Master_Receive(CAM_I2C_ADDRESS, cam_photo_rx_buffer, numBytes);//receive requested information
}

void Cam_Transmit_Alert()
{//send alert status to ground station
	MAV_Send_Msg_Named_Value_Int("FireAlert", (uint32_t) cam_alert_rx_buffer[0]);
}

void Cam_Transmit_Photo(uint32_t startIndex, uint8_t numBytes)
{
	uint8_t photo_data[numBytes];
	strncpy(&photo_data, &cam_photo_rx_buffer[startIndex], numBytes);
	//Radio_Transmit_Raw(&photo_data, numBytes);
	MAV_send_File_Transfer_Protocol(&photo_data, numBytes);
}

void Cam_Handle()
{
	if(handle_mode == HANDLE_MODE_POLLING)//wait for fire detect
	{
		Cam_Poll_alert();
		if(cam_alert_rx_buffer[0] == CAM_ALERT_FIRE_DETECT)
		{
			Cam_Transmit_Alert();//send alert
			handle_mode = HANDLE_MODE_ALERT;
		}
	}
	else if(handle_mode == HANDLE_MODE_ALERT)//wait for photo request
	{
		if(1)//later wait for mavlink request to send photo
		{
			handle_mode == HANDLE_MODE_PHOTOSEND;
		}
	}
	else if(handle_mode == HANDLE_MODE_PHOTOSEND)
	{
		if(handle_photo_index < handle_photo_stop)
		{
			Cam_Transmit_Photo(handle_photo_index,handle_photo_length);//send photo data
			handle_photo_index += handle_photo_length;
		}
		else
		{
			handle_mode = HANDLE_MODE_POLLING;
		}
	}
}
