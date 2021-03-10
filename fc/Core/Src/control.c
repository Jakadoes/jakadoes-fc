/*
 * control.c
 *
 *  Created on: Feb. 20, 2021
 *      Author: damie
 */

//to do:
//implement FIFO for previous results storing
//Implement moving average to filter accelerometer data
#include "control.h"
#include "mpu.h"
#include "stm32f1xx_hal.h"
int16_t control_targets[4]; //holds target angles of [pitch(y), roll(x), thrust, yaw(z) targets]
int16_t control_errors[6];  //holds errors of [x,y,z,xgyro,ygyro,zgyro]
int16_t control_output[3];  //holds most recent output values of [x,y,z] orientations
int16_t control_k=0;//uni gain
int16_t control_dk=1;//derivative gain
uint32_t control_time_old = 0;
uint32_t control_dt = 0;

void Control_Set_Target(int16_t pitch,int16_t roll,int16_t thrust, int16_t yaw)
{
	control_targets[CONTROL_PITCH] = pitch;
	control_targets[CONTROL_ROLL] = roll;
	control_targets[CONTROL_THRUST] = thrust;
	control_targets[CONTROL_YAW] = yaw;
}

void Control_Tick()
{
	Control_Update_Errors();
	Control_Update_Outputs();
	//MAV_Send_Msg_Named_Value_Int("target", control_targets[0]);
}

int16_t Control_Calculate_Error(int16_t target,int16_t current)
{
	int16_t error = target*(-10) - current;//factor on target needed to compensate some conversion factor
	return error;
}

void Control_Update_Errors()
{
	Mpu_Update_Values_MA();
	//accelerometer
	control_errors[MPU_AXIS_X] = Control_Calculate_Error(control_targets[MPU_AXIS_X], mpu_acc[MPU_AXIS_X]);
	control_errors[MPU_AXIS_Y] = Control_Calculate_Error(control_targets[MPU_AXIS_Y], mpu_acc[MPU_AXIS_Y]);
	control_errors[MPU_AXIS_Z] = Control_Calculate_Error(control_targets[MPU_AXIS_Z], mpu_acc[MPU_AXIS_Z]);
	//gyroscope NOTE: switch rotation axis with accelerometer axis
	//+'ve gyro error is in proper direction
	control_errors[MPU_AXIS_Y+3] = ((control_errors[MPU_AXIS_X] > 0) - (control_errors[MPU_AXIS_X] < 0))* mpu_gyro[MPU_AXIS_X];
	control_errors[MPU_AXIS_X+3] = ((control_errors[MPU_AXIS_Y] > 0) - (control_errors[MPU_AXIS_Y] < 0))* mpu_gyro[MPU_AXIS_Y];
	control_errors[MPU_AXIS_Z+3] = ((control_errors[MPU_AXIS_Z] > 0) - (control_errors[MPU_AXIS_Z] < 0))* mpu_gyro[MPU_AXIS_Z];
}

int16_t Control_Calculate_Output(int axis)
{	//fix dk control by adding history of error
	int16_t output = control_k*(control_errors[axis]) + -1*control_dk*(control_errors[axis + 3]);
	return output;
}

void Control_Update_Outputs()
{
	//Control_Update_Dt();
	control_output[CONTROL_PITCH] = Control_Calculate_Output(CONTROL_PITCH)/-42;//arbitary scaling
	control_output[CONTROL_ROLL] = Control_Calculate_Output(CONTROL_ROLL)/-42;
	//control_output[MPU_AXIS_Z] = Control_Calculate_Output(MPU_AXIS_Z);
}

void Control_Update_Dt()
{
	control_dt =  HAL_GetTick() - control_time_old;
	control_time_old = HAL_GetTick();
}



