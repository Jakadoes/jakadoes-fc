/*
 * motor.c
 *
 *  Created on: Dec. 28, 2020
 *      Author: damien sabljak
 */
#define MOTOR_ARMED 1
#define MOTOR_DISARMED 2
#define MOTOR_ARMING_PULSE_WIDTH 7
#include"stm32f1xx_hal.h"

TIM_HandleTypeDef htim4;
uint8_t timChannels[] = {TIM_CHANNEL_1,TIM_CHANNEL_2,TIM_CHANNEL_3,TIM_CHANNEL_4};

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
	__HAL_TIM_SET_COMPARE(&htim4, timChannels[MotorNum], speed_converted);
}

void Motor_Set_Speed_All(int16_t speed_scaled1, int16_t speed_scaled2, int16_t speed_scaled3, int16_t speed_scaled4)
{
	//CONVENTION from front left clockwise
	Motor_Set_Speed_Single(0, speed_scaled1);
	Motor_Set_Speed_Single(3, speed_scaled2);
	Motor_Set_Speed_Single(1, speed_scaled3);
	Motor_Set_Speed_Single(2, speed_scaled4);
}



