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

uint8_t Mpu_Is_Ready()
{
	//returns HAL address
	uint8_t mpu_ready_status = I2c_IsDeviceReady(MPU_I2C_ADDRESS);
	return mpu_ready_status;
}

void Mpu_Wake()
{//set initial configs on power management 1 register
	uint8_t config = 0b00000001;
	Mpu_Write(MPU_REG_PWR_MGMT_1, config);
}

void Mpu_Get_Gyro_Data()
{

}

void Mpu_Write(uint8_t regNum, uint8_t writeValue)
{//input: register number to access, value to write to that register
	uint8_t command[2];//see write sequence in data sheet
	command[0] = regNum;
	command[1] = writeValue;
	I2c_Master_Transmit(MPU_I2C_ADDRESS, &command, 2);
}

void Mpu_Read(uint8_t regNum)
{
	uint8_t command = regNum;
	I2c_Master_Transmit(MPU_I2C_ADDRESS, &command, 1);//request register

}

