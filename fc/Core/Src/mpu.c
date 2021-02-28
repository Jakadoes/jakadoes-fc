/*
 * mpu.c
 *
 *  Created on: Jan. 13, 2021
 *      Author: damien sabljak
 */
#include "mpu.h"
#include "stdint.h"
#include "i2c.h"

uint8_t mpu_rx_buffer[1];
uint8_t mpu_gyro_buffer[2];
uint8_t mpu_acc_buffer[2];
uint8_t mpu_setting_buffer[1];
uint16_t mpu_gyro[3] = {0};//stored data as [x,y,z]
uint16_t mpu_acc[3] = {0};//stored data as [x,y,z]

uint8_t Mpu_Is_Ready()
{
	//returns HAL address
	uint8_t mpu_ready_status = I2c_IsDeviceReady(MPU_I2C_ADDRESS);
	return mpu_ready_status;
}

void Mpu_Wake()
{//set initial configs on power management 1 register
	//power settings
	uint8_t pwr_config = 0b00000001;
	Mpu_Write(MPU_REG_PWR_MGMT_1, pwr_config);
	//accelerometer settings
	uint8_t acc_config = 0b00000000;//set full scale range
	Mpu_Write(MPU_REG_ACC_CONFIG, acc_config);
	//gyrometer settings
	uint8_t gyro_config = 0b00000000;//set full scale range
	Mpu_Write(MPU_REG_GYRO_CONFIG, gyro_config);
}

void Mpu_Update_Values()
{//need to address HAL delay with proper blocking protocol
	Mpu_Get_Acc_Data(MPU_AXIS_X);
	HAL_Delay(1);
	Mpu_Get_Acc_Data(MPU_AXIS_Y);
	HAL_Delay(1);
	Mpu_Get_Acc_Data(MPU_AXIS_Z);
	HAL_Delay(1);
	Mpu_Get_Gyro_Data(MPU_AXIS_X);
	HAL_Delay(1);
	Mpu_Get_Gyro_Data(MPU_AXIS_Y);
	HAL_Delay(1);
}

void Mpu_Get_Gyro_Data(uint8_t gyro_axis)
{
	uint8_t command = MPU_REG_GYRO_X + 2*gyro_axis;
	Mpu_Read(command, &mpu_gyro_buffer);
	mpu_gyro[gyro_axis] = (((uint16_t) mpu_gyro_buffer[0])<<8) + ((uint16_t) mpu_gyro_buffer[1]);
}

void Mpu_Get_Acc_Data(uint8_t acc_axis)
{
	uint8_t command = MPU_REG_ACC_X + 2*acc_axis;
	Mpu_Read(command, &mpu_acc_buffer);
	//data is high byte first
	//Radio_Transmit_Raw(&mpu_acc_buffer, 2);
	//HAL_Delay(1000);
	mpu_acc[acc_axis] = (((uint16_t) mpu_acc_buffer[0])<<8) + ((uint16_t) mpu_acc_buffer[1]);

}

void Mpu_Write(uint8_t regNum, uint8_t writeValue)
{//input: register number to access, value to write to that register
	uint8_t command[2];//see write sequence in data sheet
	command[0] = regNum;
	command[1] = writeValue;
	I2c_Master_Transmit(MPU_I2C_ADDRESS, &command, 2);
}

void Mpu_Read(uint8_t regNum, uint8_t* buffer )
{
	uint8_t command = regNum;
	I2c_Master_Transmit(MPU_I2C_ADDRESS, &command, 1);//request register
	I2c_Master_Receive(MPU_I2C_ADDRESS, buffer, 2);

}

