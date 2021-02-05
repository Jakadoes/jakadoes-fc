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
#define MPU_REG_GYRO_X     0x43//data is given in two bytes [high and then low]
#define MPU_REG_GYRO_Y     0x45
#define MPU_REG_GYRO_Z     0x47
#define MPU_REG_ACC_X      0x3B//data is given in two bytes [high and then low]
#define MPU_REG_ACC_Y      0x3D
#define MPU_REG_ACC_Z      0x3F
#define MPU_AXIS_X 0x01
#define MPU_AXIS_Y 0x02
#define MPU_AXIS_Z 0x03

#endif /* INC_MPU_H_ */

uint8_t Mpu_Is_Ready();
void    Mpu_Wake();
