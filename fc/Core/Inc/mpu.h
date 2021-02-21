/*
 * mpu.h
 *
 *  Created on: Jan. 13, 2021
 *      Author: damie
 */

#ifndef INC_MPU_H_
#define INC_MPU_H_
#include "stdint.h"
#define MPU_I2C_ADDRESS     0x68
#define MPU_REG_CONFIG      0x1A
#define MPU_REG_PWR_MGMT_1  0x6B
#define MPU_REG_ACC_CONFIG  0x1C
#define MPU_REG_GYRO_CONFIG 0x1B
#define MPU_REG_GYRO_X      0x43//data is given in two bytes [high and then low]
#define MPU_REG_GYRO_Y      0x45
#define MPU_REG_GYRO_Z      0x47
#define MPU_REG_ACC_X       0x3B//data is given in two bytes [high and then low]
#define MPU_REG_ACC_Y       0x3D
#define MPU_REG_ACC_Z       0x3F
#define MPU_AXIS_X 0x00
#define MPU_AXIS_Y 0x01
#define MPU_AXIS_Z 0x02

uint8_t  mpu_rx_buffer[1];
uint8_t  mpu_gyro_buffer[2];
uint8_t  mpu_acc_buffer[2];
uint16_t mpu_acc[3];//stored data as [x,y,z]
uint16_t  mpu_gyro[3];//stored data as [x,y,z]

#endif /* INC_MPU_H_ */

uint8_t Mpu_Is_Ready();
void    Mpu_Wake();
void Mpu_Get_Gyro_Data(uint8_t gyro_axis);
void Mpu_Get_Acc_Data(uint8_t acc_axis);
void Mpu_Read(uint8_t regNum, uint8_t* buffer );
