/*
 * motor.c
 *
 *  Created on: Dec. 28, 2020
 *      Author: damien sabljak
 */
#define MOTOR_ARMED 1
#define MOTOR_DISARMED 2
#define MOTOR_ARMING_PULSE_WIDTH 7
#define MOTOR_FRONTLEFT  0x00
#define MOTOR_FRONTRIGHT 0x03
#define MOTOR_BACKLEFT   0x02
#define MOTOR_BACKTRIGHT 0x01
#include"stm32f1xx_hal.h"

TIM_HandleTypeDef htim4;
uint8_t timChannels[] = {TIM_CHANNEL_1,TIM_CHANNEL_2,TIM_CHANNEL_3,TIM_CHANNEL_4};
uint16_t motor_maxPower = 1200;//limits max throttle of drone

void Motor_Arm()
{
	//ARM PWM Signals
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);

	uint16_t pulse_width = 7;
}

void Motor_Set_Speed_Single(uint8_t MotorNum, int16_t speed_scaled)
{
	//input: (0 to 1000), this gets converted to between 800 and 1600 (see clock speed calcs)
	int16_t speed_converted = 8*(speed_scaled/10) + 800;
	if(speed_converted > motor_maxPower){speed_converted = motor_maxPower;}//govenor (safety feature)
	__HAL_TIM_SET_COMPARE(&htim4, timChannels[MotorNum], speed_converted);
}

void Motor_Set_Speed_All(int16_t speed_scaled1, int16_t speed_scaled2, int16_t speed_scaled3, int16_t speed_scaled4)
{//input: (0 to 1000)
	//CONVENTION from front left clockwise
	Motor_Set_Speed_Single(MOTOR_FRONTLEFT,  speed_scaled1);
	Motor_Set_Speed_Single(MOTOR_FRONTRIGHT, speed_scaled2);
	Motor_Set_Speed_Single(MOTOR_BACKTRIGHT, speed_scaled3);
	Motor_Set_Speed_Single(MOTOR_BACKLEFT,   speed_scaled4);
}

void Motor_Set_Speed_Controlled(uint16_t thrust, uint16_t pitch, uint16_t roll, uint16_t yaw)
{	//input values: -1000 to 1000
	//max power: thrust + 3*thrust*pitch/100
	//+thrust is forward, +roll is right, +yaw is CW
	int16_t frontLeft   = thrust + -1*thrust*(pitch)/100 +    thrust*(roll)/100 + -1*thrust*(yaw)/100;
	int16_t frontRight  = thrust + -1*thrust*(pitch)/100 + -1*thrust*(roll)/100 +    thrust*(yaw)/100;
	int16_t backRight   = thrust +    thrust*(pitch)/100 + -1*thrust*(roll)/100 + -1*thrust*(yaw)/100;
	int16_t backLeft    = thrust +    thrust*(pitch)/100 +    thrust*(roll)/100 + 	  thrust*(yaw)/100;
	//clamp outputs:
	frontLeft  = (((frontLeft)  < (0)) ? (0) : (frontLeft));   frontLeft  = (((frontLeft)  > (1000)) ? (1000) : (frontLeft));
	frontRight = (((frontRight) < (0)) ? (0) : (frontRight));  frontRight =  (((frontRight) > (1000)) ? (1000) : (frontRight));
	backRight  = (((backRight)  < (0)) ? (0) : (backRight));   backRight  = (((backRight)  > (1000)) ? (1000) : (backRight));
	backLeft  = (((backLeft)   < (0)) ? (0) : (backLeft));     backLeft  = (((backLeft)   > (1000)) ? (1000) : (backLeft));
	//output: 0 to 1000
	Motor_Set_Speed_All(frontLeft, frontRight, backLeft, backRight);
}



