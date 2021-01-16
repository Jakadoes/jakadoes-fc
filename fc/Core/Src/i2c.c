/*
 * i2c.c
 *
 *  Created on: Jan 5, 2021
 *      Author: damien sabljak
 */
#include "stdint.h"
#include"stm32f1xx_hal.h"

I2C_HandleTypeDef hi2c2;
#define I2C_TRIALS_DEFAULT 40
#define I2C_TIMEOUT_DEFAULT 400

uint8_t I2c_IsDeviceReady(uint8_t Address)
{
	uint16_t devAddress = Address << 1;//address must be shifted to the left before use (check HAL documentation)
	//uint16_t devAddress = 0b1110111000000000;
	//uint16_t devAddress = 0b0000000001110111;
	uint8_t test[2];
	//test[1] = devAddress & 0xff;
	//test[0] = devAddress >>8;
	//Radio_Transmit_Raw(&test, 2);
	return HAL_I2C_IsDeviceReady(&hi2c2, devAddress, I2C_TRIALS_DEFAULT, I2C_TIMEOUT_DEFAULT);
}

void I2c_Master_Transmit(uint8_t Address, uint8_t* message, uint16_t messageSize)
{
	uint16_t devAddress = Address << 1;//address must be shifted to the left once before use (check HAL documentation)
	HAL_I2C_Master_Transmit(&hi2c2, devAddress, message, messageSize, I2C_TIMEOUT_DEFAULT);
}

void I2c_Master_Receive(uint8_t Address, uint8_t* messageDestination, uint16_t messageSize)
{
	uint16_t devAddress = Address << 1;//address must be shifted to the left before use (check HAL documentation)
	HAL_I2C_Master_Receive(&hi2c2, devAddress, messageDestination, messageSize, I2C_TIMEOUT_DEFAULT);
}
