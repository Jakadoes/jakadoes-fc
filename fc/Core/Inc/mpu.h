/*
 * mpu.h
 *
 *  Created on: Jan. 13, 2021
 *      Author: damie
 */

#ifndef INC_MPU_H_
#define INC_MPU_H_
#include "stdint.h"
#define MPU_I2C_ADDRESS    0x68
#define MPU_REG_CONFIG     0x1A
#define MPU_REG_PWR_MGMT_1 0x6B

#endif /* INC_MPU_H_ */

uint8_t Mpu_Is_Ready();
void    Mpu_Wake();
