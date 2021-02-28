/*
 * control.c
 *
 *  Created on: Feb. 20, 2021
 *      Author: damie
 */

//to do:
//implement FIFO for previous results storing
//Implement moving average to filter accelerometer data
#include "mpu.h"
int16_t control_targets[3]; //holds target angles of [x,y,z] orientations
int16_t control_errors[6];  //holds errors of [x,y,z,xgyro,ygyro,zgyro]
int16_t control_output[3];  //holds most recent output values of [x,y,z] orientations
int16_t control_k=1;
int16_t control_dk=0;
uint32_t control_time_old = HAL_GetTick();
uint32_t control_dt = 0;

void Control_Set_Target(x_tilt, y_tilt, z_tilt)
{
	control_targets[MPU_AXIS_X] = x_tilt;
	control_targets[MPU_AXIS_Y] = y_tilt;
	control_targets[MPU_AXIS_Z] = z_tilt;
}


int16_t Control_Calculate_Error(target, current)
{
	int16_t error = x_target - x_current;
	return error;
}

void Control_Update_Errors()
{
	Mpu_Update_Values();
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

int16_t Control_Calculate_Output(const axis)
{	//fix dk control by adding history of error
	int16_t output = control_k*(control_errors[axis]) + -1*control_dk*(control_errors[axis + 3]);
	return output;
}

void Control_Update_Outputs(const axis)
{
	Control_Update_Dt();
	control_outputs[MPU_AXIS_X] = Control_Calulate_Output(MPU_AXIS_X);
	control_outputs[MPU_AXIS_Y] = Control_Calulate_Output(MPU_AXIS_Y);
	control_outputs[MPU_AXIS_Z] = Control_Calulate_Output(MPU_AXIS_Z);
}

void Control_Update_Dt()
{
	control_dt =  HAL_GetTick() - control_time_old;
	control_time_old = Hal_GetTick();
}



