/*
 * i2c.h
 *
 *  Created on: Jan 5, 2021
 *      Author: damie
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_



#endif /* INC_I2C_H_ */

uint8_t I2c_IsDeviceReady(uint8_t Address);
void I2c_Master_Transmit(uint8_t Address, uint8_t* message, uint16_t messageSize);
void I2c_Master_Receive(uint8_t Address, uint8_t* messageDestination, uint16_t messageSize);
