/*
 * cam.c
 *
 *  Created on: Jan 15, 2021
 *      Author: damien sabljak
 */

#include "cam.h"
I2C_HandleTypeDef hi2c2;
#include"stm32f1xx_hal.h"
#define CAM_MODE_POLLING       0x11
#define CAM_MODE_ALERT         0x22
#define CAM_MODE_STORED        0x33
#define CAM_MODE_COMPLETE      0x44
#define CAM_GS_IMG_DELETE      0x11
#define CAM_GS_IMG_DOWNLOAD    0x11

uint8_t  cam_alert_rx_buffer[1];
uint8_t  cam_photoInfo_buffer[3];
uint8_t  cam_photo_rx_buffer[4000] = {0};
uint8_t  handle_mode = CAM_MODE_POLLING;
uint8_t  handle_gs_request  = CAM_GS_IMG_DOWNLOAD;
uint32_t handle_photo_index = 0;//defines current position in photo transfer
uint32_t handle_photo_size  = 50;//defines length of each FTP packet data
uint32_t handle_photo_stop  = 100;//defines length of image array to be sent

uint8_t Cam_Is_Ready()
{
	//returns HAL address
	uint8_t cam_ready_status = I2c_IsDeviceReady(CAM_I2C_ADDRESS);
	return cam_ready_status;
}

void Cam_Set_I2C(uint8_t state)
{//uses GPIO pin to indicate to camera either to listen to I2C or resume its duties
	if(state == 1)
	{
		HAL_GPIO_WritePin (GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
		HAL_GPIO_WritePin (GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);//green LED on
	}
	else if (state == 0)
	{
		HAL_GPIO_WritePin (GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
		HAL_GPIO_WritePin (GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
	}
}
void Cam_Poll_Mode()
{
	//uint8_t test = 0x22;
	//send single read command
	//receive 8 bits of data
	uint8_t command = CAM_COM_POLL_MODE;
	//test = 0x22;
	//Radio_Transmit_Raw(&test, 1);
	I2c_Master_Transmit(CAM_I2C_ADDRESS, &command, 1);
	//HAL_Delay(1000);
	//uint8_t cam_status = Cam_Is_Ready();

	//Radio_Transmit_Raw(&cam_status, 1);
	//if(Cam_Is_Ready() == HAL_OK)//line needs to be not busy for error not to occur
	//{
		 //test = 0x33;
		 //HAL_Delay(600);
		 //Radio_Transmit_Raw(&test, 1);
		 I2c_Master_Receive(CAM_I2C_ADDRESS, cam_alert_rx_buffer, 1);
		 //Radio_Transmit_Raw(&test, 1);
		 //Radio_Transmit_Raw(&cam_alert_rx_buffer, 1);
		 //transmission is getting stuck on, causing the cam to stay in receiving mode
		 //very important to send and expect to recieve the same amount of bytes, otherwise EIO and busy lock down errors
	//}
}

void Cam_Poll_Img_Info()
{   //recieve metadata about camera
	//[0:2]: handle_photo_stop
	uint8_t command = CAM_COM_POLL_IMG_INFO;
	I2c_Master_Transmit(CAM_I2C_ADDRESS, &command, 1);
	I2c_Master_Receive(CAM_I2C_ADDRESS, cam_photoInfo_buffer, 3);
	handle_photo_stop  = ((uint32_t)cam_photoInfo_buffer[0]) + (((uint32_t) cam_photoInfo_buffer[1])<<8) + (((uint32_t) cam_photoInfo_buffer[2])<<16);
}

void Cam_Poll_Img_Data(uint32_t startIndex, uint8_t numBytes)
{//receives image buffer data starting at startIndex of length numBytes
	uint8_t command = CAM_COM_POLL_IMG_DATA;
	uint8_t details[5];//format [startIndex [4:1] , numBytes[0]
		details[4] = (uint8_t) (startIndex >> 8*3) & 0x00FF;
		details[3] = (uint8_t) (startIndex >> 8*2) & 0x00FF;
		details[2] = (uint8_t) (startIndex >> 8*1) & 0x00FF;
		details[1] = (uint8_t) startIndex & 0x00FF;
		details[0] = numBytes;
	//uint8_t test = 0x22;
	//Radio_Transmit_Raw(&test, 1);
	I2c_Master_Transmit(CAM_I2C_ADDRESS, &command, 1);//transmit image command
	//HAL_Delay(500);
	//test = 0x33;
	//Radio_Transmit_Raw(&test, 1);
	I2c_Master_Transmit(CAM_I2C_ADDRESS, &details, 5);//transmit details of requested transmission
	HAL_Delay(100);//this delay may be needed
	//test = 0x44;
	//Radio_Transmit_Raw(&test, 1);
	uint8_t temp_buffer[numBytes];//cannot index rx_buffer directly here!
	I2c_Master_Receive(CAM_I2C_ADDRESS, &temp_buffer, numBytes);//receive requested information
	//Radio_Transmit_Raw(&test, 1);
	///transfer temp to photo buffer
	for (uint32_t i=0; i<numBytes; i++)
	{
		cam_photo_rx_buffer[startIndex + i] = temp_buffer[i];
	}
	HAL_Delay(500);
}

void Cam_Transmit_Alert()
{//send alert status to ground station
	MAV_Send_Msg_Named_Value_Int("FireAlert", (uint32_t) cam_alert_rx_buffer[0]);
}

void Cam_Transmit_Photo(uint32_t startIndex, uint8_t numBytes)
{
	uint8_t photo_data[numBytes+3];
	photo_data[2] = (uint8_t) (startIndex >> 8*2) & 0x00FF;
	photo_data[1] = (uint8_t) (startIndex >> 8*1) & 0x00FF;
	//photo_data[2] = 1;
	//photo_data[1] = 1;
	photo_data[0] = (uint8_t)  startIndex         & 0x00FF;//encode in first 3 bytes the start index

	for (uint32_t i=0;i<numBytes;i++)
	{
		photo_data[i+3] = cam_photo_rx_buffer[startIndex + i];
	}
	//strncpy(&photo_data[3], &cam_photo_rx_buffer[startIndex], numBytes);
	Radio_Transmit_Raw(&photo_data, numBytes+3);
	HAL_Delay(500);
	MAV_send_File_Transfer_Protocol(&photo_data, numBytes+3);
}

void Cam_Transmit_Photo_Debug(uint32_t startIndex, uint8_t numBytes)
{//transmits specified photodata as raw serial data (no mavlink)
	uint8_t photo_data[numBytes];
	strncpy(&photo_data, &cam_photo_rx_buffer[startIndex], numBytes);
	//Radio_Transmit_Raw(&photo_data, numBytes);
	Radio_Transmit_Raw(&photo_data, numBytes);
}

void Cam_Handle()
{
	//MAV_Send_Debug_Statement_Default();
	if(handle_mode == CAM_MODE_POLLING)//wait for fire detect
	{
		Cam_Set_I2C(1);
		HAL_Delay(500);
		if (Cam_Is_Ready() == HAL_OK)
		{
			Cam_Poll_Mode();
			Cam_Transmit_Alert();
			if(cam_alert_rx_buffer[0] ==  CAM_MODE_ALERT || cam_alert_rx_buffer[0] ==  CAM_MODE_STORED )
			{
				Cam_Transmit_Alert();//send alert
				handle_mode = CAM_MODE_ALERT;
			}
			else
			{
				Cam_Set_I2C(0);
				HAL_Delay(5000);
			}
		}
	}
	else if(handle_mode == CAM_MODE_ALERT)//wait for photo request and image to be stored
	{
		Cam_Set_I2C(1);
		HAL_Delay(500);
		if (Cam_Is_Ready() == HAL_OK)
		{
			Cam_Poll_Mode();
			Cam_Transmit_Alert();
		}

		if(handle_gs_request == CAM_GS_IMG_DOWNLOAD && cam_alert_rx_buffer[0] ==  CAM_MODE_STORED)//mavlink request + stored image needed
		{
			Cam_Poll_Img_Info();
			handle_mode = CAM_MODE_STORED;
		}
		else
		{
			Cam_Set_I2C(0);
			HAL_Delay(5000);
		}
	}
	else if(handle_mode == CAM_MODE_STORED)//start downlink of photo data
	{
		//MAV_Send_Debug_Statement_Default();
		Cam_Set_I2C(1);
		HAL_Delay(50);
		MAV_Send_Msg_Named_Value_Int("imgStop0", cam_photoInfo_buffer[0]);
		MAV_Send_Msg_Named_Value_Int("imgStop1", cam_photoInfo_buffer[1]);
		MAV_Send_Msg_Named_Value_Int("imgStop2", cam_photoInfo_buffer[2]);
		if(handle_photo_index < handle_photo_stop)
		{
			//MAV_Send_Debug_Statement_Default();
			if (Cam_Is_Ready() == HAL_OK)
			{
				//MAV_Send_Debug_Statement_Default();
				Cam_Poll_Img_Data(handle_photo_index, handle_photo_size);
				Cam_Transmit_Photo(handle_photo_index,handle_photo_size);//send photo data
				handle_photo_index += handle_photo_size;
			}
		}
		else
		{
			handle_mode = CAM_MODE_COMPLETE;
		}
	}
	else if(handle_mode == CAM_MODE_COMPLETE)
	{
		MAV_Send_Msg_Named_Value_Int("CAM_DONE", (uint32_t) cam_alert_rx_buffer[0]);
		HAL_Delay(5000);
	}
	Cam_Set_I2C(0);
}
