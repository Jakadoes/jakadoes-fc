/*
 * radio.h
 *
 *  Created on: Dec. 21, 2020
 *      Author: Damien Sabljak
 */

#ifndef INC_RADIO_H_
#define INC_RADIO_H_

#include "mavlink.h"
#include "stm32f1xx_hal.h"
#include "stdint.h"


void Radio_Transmit_Raw(uint8_t* message, uint16_t messageSize);
void Radio_Recieve_Raw(uint8_t* messageDestination, int numBytesToRecieve);

#endif /* INC_RADIO_H_ */
