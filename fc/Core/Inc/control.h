/*
 * control.h
 *
 *  Created on: Feb. 20, 2021
 *      Author: damie
 */

#ifndef INC_CONTROL_H_
#define INC_CONTROL_H_
#define CONTROL_PITCH  0x00
#define CONTROL_ROLL   0x01
#define CONTROL_THRUST 0x02
#define CONTROL_YAW    0x03
#include "stdint.h"


#endif /* INC_CONTROL_H_ */

int16_t control_targets[4]; //holds target angles of [x,y,z] orientations
int16_t control_errors[6];  //holds errors of [x,y,z,xgyro,ygyro,zgyro]
int16_t control_output[3];  //holds most recent output values of [x,y,z] orientations
int16_t control_k;
int16_t control_dk;
uint32_t control_time_old;
uint32_t control_dt;

void Control_Set_Target(int16_t pitch,int16_t roll,int16_t thrust, int16_t yaw);
void Control_Tick();
void Control_Update_Errors();
void Control_Update_Outputs();
