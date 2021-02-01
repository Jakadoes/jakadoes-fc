/*
 * mavlink.h
 *
 *  Created on: Dec. 21, 2020
 *      Author: damie
 */

#ifndef INC_MAVLINK_H_
#define INC_MAVLINK_H_

#include "..\..\Drivers\c_library_v2\standard\mavlink.h"
#include "radio.h"

#endif /* INC_MAVLINK_H_ */

void MAV_Parse_Data();
void MAV_Send_Debug_Statement();
void MAV_send_File_Transfer_Protocol(uint8_t payload[], uint8_t payload_len);
