
/*
 * radio.c
 *
 *  Created on: Dec. 21, 2020
 *      Author: Damien Sabljak
 */
//mavlink protocol driver
//#ifndef MAVLINK_USE_CONVENIENCE_FUNCTIONS
//#define MAVLINK_USE_CONVENIENCE_FUNCTIONS //allows use of easier functions in MAVLINK
//#endif

#include "mavlink.h"


//****variables*****
//user defined
int MAV_MAX_PARSE_AMT = 279; //determines maximum bytes in buffer to parse before passing control (timeout)
//mavlink common variables
mavlink_status_t status;
mavlink_message_t msg;
mavlink_rc_channels_scaled_t rc_channels_scaled;
int chan = MAVLINK_COMM_0;
uint8_t SYSTEM_ID = 2;
uint8_t COMPONENT_ID = 3;//this can be used later to assign names of components on drone
uint32_t TIME_BOOT_MS = 0;
//uint8_t PROPER_STX = 0xFF;
uint8_t test[5] = {0xFE, 0xFE, 0xFE, 0xFE, 0xFE};
char rx_buffer_mav[279];
uint8_t byte_arr[279];

void MAV_Parse_Data(){
	char test[4] = "yayy";
	Radio_Recieve_Raw(&byte_arr, sizeof(byte_arr));
	//Radio_Transmit_Raw(&byte_arr, sizeof(byte_arr));
	//Radio_Transmit_Raw(&"msgid", 4);

	for(int i=0;i<279;i++)
	{
		 if (mavlink_parse_char(chan, byte_arr[i], &msg, &status))//if any mavlink message is sent
	     {
			 //Radio_Transmit_Raw(&test, 4);
			 //MAV_Send_Debug_Statement_Default();
			 //MAV_Send_Debug_Statement("msgid", msg.msgid); //WARNING: UNCOMMENTING THIS MAY CAUSE SOME OF THE OTHER MESSAGE TO NOT SEND
			 //printf("Received message with ID %d, sequence: %d from component %d of system %d\n", msg.msgid, msg.seq, msg.compid, msg.sysid);
			 // ... DECODE THE MESSAGE PAYLOAD HERE ...
			 switch (msg.msgid)
			 {
			 	 case MAVLINK_MSG_ID_RC_CHANNELS_SCALED:
			 	 {
			 		 MAV_Send_Debug_Statement("poop", msg.msgid);
			 		 //decode
			 		 mavlink_msg_rc_channels_scaled_decode(&msg, &rc_channels_scaled);
			 		 MAV_Send_Debug_Statement("rc1", rc_channels_scaled.chan1_scaled);
			 		 break;
			 	 }
			 }
			 break;
	     }
	}
}


void MAV_Send_Debug_Statement(char message[], uint32_t value)
{
	//create struct and fill in data
	///char test[5] = "abcde";
	mavlink_named_value_int_t msgStruct;
	strcpy(msgStruct.name,message);
	msgStruct.time_boot_ms = 4;
	msgStruct.value = value;
	mavlink_msg_named_value_int_encode(SYSTEM_ID, COMPONENT_ID, &msg, &msgStruct);
	mavlink_msg_to_send_buffer(&rx_buffer_mav, &msg);
	//MAV_Correct_STX(&rx_buffer_mav);//used to correct error with wrong STX for some reason
	Radio_Transmit_Raw(&rx_buffer_mav, sizeof(rx_buffer_mav));
	//Radio_Transmit_Raw(&test, sizeof(test));
}

void MAV_Send_Debug_Statement_Default()
{
	char name[10] = "abcdefghij;";
	//create struct and fill in data
	mavlink_named_value_int_t msgStruct;
	strcpy(&msgStruct.name,&name);
	msgStruct.time_boot_ms = 4;
	msgStruct.value = 7;
	mavlink_msg_named_value_int_encode(SYSTEM_ID, COMPONENT_ID, &msg, &msgStruct);
	mavlink_msg_to_send_buffer(&rx_buffer_mav, &msg);
	//MAV_Correct_STX(&rx_buffer_mav);//used to correct error with wrong STX for some reason
	Radio_Transmit_Raw(&rx_buffer_mav, sizeof(rx_buffer_mav));
	//Radio_Transmit_Raw(&test, sizeof(test));
}

/* OLD stuff
 void MAV_Correct_STX(uint8_t* rx_buffer){
	rx_buffer[0] = PROPER_STX;
}
 */
