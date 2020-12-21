/*
 * radio.c
 *
 *  Created on: Dec. 21, 2020
 *      Author: Damien Sabljak
 *      high level api to control radio on drone
 */
#include "stdint.h"
#include "stm32f1xx_hal.h"
UART_HandleTypeDef huart1;

struct Radio {
	int baudRate;
};

void Transmit(){

}

//send a raw message through uart without mavlink
void radio_transmit_raw(uint8_t* message){
	HAL_UART_Transmit(&huart1,message, sizeof(message),HAL_MAX_DELAY);
}
void radio_recieve_raw(uint8_t* message){
	HAL_UART_Receive (&huart1,message, sizeof(message),10);
}
