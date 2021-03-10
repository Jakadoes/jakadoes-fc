/*
 * mpu.c
 *
 *  Created on: Jan. 13, 2021
 *      Author: damien sabljak
 */
#include "mpu.h"
#include "stdint.h"
#include "i2c.h"
#include "stm32f1xx_hal.h"
#define MPU_MA_SIZE 5
uint8_t  mpu_rx_buffer[1];
uint8_t  mpu_gyro_buffer[2];
uint8_t  mpu_acc_buffer[2];
uint8_t  mpu_setting_buffer[1];
uint16_t mpu_gyro[3] = {0};//stored data as [x,y,z]
int16_t  mpu_ma_x[MPU_MA_SIZE];//x most recent points, used for MA filter
int16_t  mpu_ma_y[MPU_MA_SIZE];//x most recent points, used for MA filter
int16_t  mpu_ma_z[MPU_MA_SIZE];//x most recent points, used for MA filter
uint8_t  mpu_ma_index = 0;//keeps track of most recent insert in MA
int16_t  mpu_acc[3] = {0};//stored data as [x,y,z]
int16_t  mpu_cal[3];     //holds calibration data of accelerometer (board is tilted up slightly)

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

void Mpu_Calibrate()
{//sets calibration values as current accelerometer reading to account for board tilt
	Mpu_Update_Values();
	mpu_cal[MPU_AXIS_X] = mpu_acc[MPU_AXIS_X];
	mpu_cal[MPU_AXIS_Y] = mpu_acc[MPU_AXIS_Y];
	mpu_cal[MPU_AXIS_Z] = mpu_acc[MPU_AXIS_Z];
}

void Mpu_Update_Values_MA()
{//need to address HAL delay with proper blocking protocol
	if(Mpu_Is_Ready() == HAL_OK)
	{
		//update moving average arrays
		Mpu_Get_Acc_Data_MA(MPU_AXIS_X);
		HAL_Delay(1);
		Mpu_Get_Acc_Data_MA(MPU_AXIS_Y);
		HAL_Delay(1);
		Mpu_Get_Acc_Data_MA(MPU_AXIS_Z);
		//calculate new MA
		mpu_acc[MPU_AXIS_X] = Mpu_Calc_Acc_From_MA(&mpu_ma_x);
		mpu_acc[MPU_AXIS_Y] = Mpu_Calc_Acc_From_MA(&mpu_ma_y);
		mpu_acc[MPU_AXIS_Z] = Mpu_Calc_Acc_From_MA(&mpu_ma_z);
		mpu_ma_index +=1;
		if(mpu_ma_index >= MPU_MA_SIZE)
		{
			mpu_ma_index = 0;
		}
		HAL_Delay(1);
		//gyro stuff
		Mpu_Get_Gyro_Data(MPU_AXIS_X);
		HAL_Delay(1);
		Mpu_Get_Gyro_Data(MPU_AXIS_Y);
		HAL_Delay(1);

	}
	else
	{
		MAV_Send_Msg_Named_Value_Int("mpu_err", 0);
	}
}
void Mpu_Update_Values()
{//need to address HAL delay with proper blocking protocol
	if(Mpu_Is_Ready() == HAL_OK)
	{
		//update without MA
		Mpu_Get_Acc_Data(MPU_AXIS_X);
		HAL_Delay(1);
		Mpu_Get_Acc_Data(MPU_AXIS_Y);
		HAL_Delay(1);
		Mpu_Get_Acc_Data(MPU_AXIS_Z);
		HAL_Delay(1);
		//gyro stuff
		Mpu_Get_Gyro_Data(MPU_AXIS_X);
		HAL_Delay(1);
		Mpu_Get_Gyro_Data(MPU_AXIS_Y);
		HAL_Delay(1);
	}
	else
	{
		MAV_Send_Msg_Named_Value_Int("mpu_err", 0);
	}

}

void Mpu_Get_Gyro_Data(uint8_t gyro_axis)
{
	uint8_t command = MPU_REG_GYRO_X + 2*gyro_axis;
	Mpu_Read(command, &mpu_gyro_buffer);
	mpu_gyro[gyro_axis] = (((uint16_t) mpu_gyro_buffer[0])<<8) + ((uint16_t) mpu_gyro_buffer[1]);
}

int16_t Mpu_Calc_Acc_From_MA(uint16_t *ma_arr)
{
	int32_t value = 0;
	for(uint8_t i=0;i<MPU_MA_SIZE;i++)
	{
		value += ma_arr[i];
	}
	//value = value/((int16_t) MPU_MA_SIZE+1);
	return ((int16_t) value)/MPU_MA_SIZE;
}
void Mpu_Get_Acc_Data_MA(uint8_t acc_axis)
{
	uint8_t command = MPU_REG_ACC_X + 2*acc_axis;
	Mpu_Read(command, &mpu_acc_buffer);
	//data is high byte first
	//Radio_Transmit_Raw(&mpu_acc_buffer, 2);
	//HAL_Delay(1000);
	int16_t value = (((uint16_t) mpu_acc_buffer[0])<<8) + ((uint16_t) mpu_acc_buffer[1]);
	if     (acc_axis == MPU_AXIS_X){mpu_ma_x[mpu_ma_index] = value - mpu_cal[acc_axis];}
	else if(acc_axis == MPU_AXIS_Y){mpu_ma_y[mpu_ma_index] = value - mpu_cal[acc_axis];}
	else if(acc_axis == MPU_AXIS_Z){mpu_ma_z[mpu_ma_index] = value - mpu_cal[acc_axis];}
}

void Mpu_Get_Acc_Data(uint8_t acc_axis)
{
	uint8_t command = MPU_REG_ACC_X + 2*acc_axis;
	Mpu_Read(command, &mpu_acc_buffer);
	//data is high byte first
	//Radio_Transmit_Raw(&mpu_acc_buffer, 2);
	//HAL_Delay(1000);
	mpu_acc[acc_axis] = (((uint16_t) mpu_acc_buffer[0])<<8) + ((uint16_t) mpu_acc_buffer[1]);
	mpu_acc[acc_axis] = mpu_acc[acc_axis] - mpu_cal[acc_axis];
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

