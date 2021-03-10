/*
 * motor.h
 *
 *  Created on: Dec. 28, 2020
 *      Author: damie
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_
#define MOTOR_FRONTLEFT  0x00
#define MOTOR_FRONTRIGHT 0x03
#define MOTOR_BACKLEFT   0x02
#define MOTOR_BACKRIGHT 0x01
#include"stm32f1xx_hal.h"

#endif /* INC_MOTOR_H_ */

int16_t motor_speeds[4];//holds information of most recently configured motor speeds

void Motor_Set_Speed_Guided();
