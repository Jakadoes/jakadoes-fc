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
uint32_t RADIO_RX_TIMEOUT = 500;

struct Radio {
	int baudRate;

};

void Transmit(){

}

//send a raw message through uart without mavlink
void Radio_Transmit_Raw(uint8_t* message, uint16_t messageSize){
	HAL_UART_Transmit(&huart1, message, messageSize,HAL_MAX_DELAY);
}
void Radio_Recieve_Raw(uint8_t* messageDestination, int numBytesToRecieve){
	HAL_UART_Receive (&huart1,messageDestination, numBytesToRecieve , RADIO_RX_TIMEOUT);
}
void Radio_Transmit_Square(){
	//transmits a square wave over uart for diagnostics
	uint8_t sqr =0x55;
	uint8_t sqr_arr[10] = {sqr,sqr,sqr,sqr,sqr,sqr,sqr,sqr,sqr,sqr};
	Radio_Transmit_Raw(&sqr_arr, sizeof(sqr_arr));
}
