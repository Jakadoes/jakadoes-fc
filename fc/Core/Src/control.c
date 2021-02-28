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
uint16_t control_targets[3]; //holds target angles of [x,y,z] orientations
uint16_t control_errors_last[3];  //holds errors of [x,y,z]
uint16_t control_errors[3];  //holds errors of [x,y,z]
uint16_t control_output[3];  //holds most recent output values of [x,y,z] orientations
uint16_t control_k=1;
uint16_t control_dk=1;
uint32_t control_time_old = HAL_GetTick();
uint32_t control_dt = 0;

void Control_Set_Target(x_tilt, y_tilt, z_tilt)
{
	control_targets[MPU_AXIS_X] = x_tilt;
	control_targets[MPU_AXIS_Y] = y_tilt;
	control_targets[MPU_AXIS_Z] = z_tilt;
}


uint16_t Control_Calculate_Error(target, current)
{
	uint8_t error = x_target - x_current;
	return error;
}

void Control_Update_Errors()
{
	Mpu_Update_Values();
	control_errors_last[MPU_AXIS_X] = control_errors[MPU_AXIS_X];//store last results
	control_errors_last[MPU_AXIS_Y] = control_errors[MPU_AXIS_Y];
	control_errors_last[MPU_AXIS_Z] = control_errors[MPU_AXIS_Z];
	control_errors[MPU_AXIS_X] = Control_Calculate_Error(control_targets[MPU_AXIS_X], mpu_acc[MPU_AXIS_X]);//update current results
	control_errors[MPU_AXIS_Y] = Control_Calculate_Error(control_targets[MPU_AXIS_Y], mpu_acc[MPU_AXIS_Y]);
	control_errors[MPU_AXIS_Z] = Control_Calculate_Error(control_targets[MPU_AXIS_Z], mpu_acc[MPU_AXIS_Z]);
}

uint16_t Control_Calculate_Output(const axis)
{	//fix dk control by adding history of error
	output = control_k*(control_erros[axis]) + control_dk*((control_errors[axis] - control_errors_last[axis])/control_dt);
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



