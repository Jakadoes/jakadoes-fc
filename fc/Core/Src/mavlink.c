
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
mavlink_rc_channels_scaled_t rc_channels_scaled_msg;
int chan = MAVLINK_COMM_0;
uint8_t SYSTEM_ID = 2;
uint8_t COMPONENT_ID = 3;//this can be used later to assign names of components on drone
uint32_t TIME_BOOT_MS = 0;
//uint8_t PROPER_STX = 0xFF;
uint8_t test[5] = {0xFE, 0xFE, 0xFE, 0xFE, 0xFE};
char rx_buffer_mav[279];
uint8_t byte_arr[279];
uint8_t motor_armed = 0;

void MAV_Parse_Data(){
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
			 		//decode
			 		mavlink_msg_rc_channels_scaled_decode(&msg, &rc_channels_scaled_msg);
			 		//MAV_Send_Debug_Statement("rc1", rc_channels_scaled_msg.chan1_scaled);
			 		//MAV_Send_Debug_Statement("rc2", rc_channels_scaled_msg.chan2_scaled);
			 		//MAV_Send_Debug_Statement("rc3", rc_channels_scaled_msg.chan3_scaled);

			 		 //for now, set this to arm motors
			 		if(motor_armed == 0){
			 			Motor_Arm();
			 			motor_armed = 1;
			 		}
			 		//set motor speed
			 		Motor_Set_Speed_All(rc_channels_scaled_msg.chan1_scaled, rc_channels_scaled_msg.chan2_scaled, rc_channels_scaled_msg.chan3_scaled, rc_channels_scaled_msg.chan4_scaled);
			 		break;
			 	 }
			 }//end switch
			 break;
	     }//end if
	}//end for
}

void MAV_Send_Raw_Imu()
{//sends IMU data through radio to ground station
	//uint8_t test = 255;
	//int8_t test2 = -1;
	//Radio_Transmit_Raw(&test, 1);
	//Radio_Transmit_Raw(&test2, 1);
	//test2 = (int8_t) test;
	//Radio_Transmit_Raw(&test2, 1);
	//HAL_Delay(500);
	//Radio_Transmit_Raw(&test, 1);
	//create buffer of proper length
	//int PACKET_STATIC_SIZE = 10 + 3 + payload_len + 2; //mavlink[FTP header + payload]mavlink
	int PACKET_STATIC_SIZE = 10 + 29 + 2 ; //mavlink[imu data ]mavlink
	uint8_t buffer[PACKET_STATIC_SIZE];
	//test = 0x22;
	//Radio_Transmit_Raw(&test, 1);
	//create struct and fill in data
	mavlink_raw_imu_t msgStruct;
	msgStruct.time_usec = (uint64_t) 0x3333333333333333;
	//msgStruct.xacc  = mpu_acc[MPU_AXIS_X];
	//test = 0x33;
	//Radio_Transmit_Raw(&test, 1);
	//msgStruct.yacc  = mpu_acc[MPU_AXIS_Y];
	//msgStruct.zacc  = mpu_acc[MPU_AXIS_Z];
	//HAL_Delay(500);
	msgStruct.xacc  = (int16_t)mpu_acc[MPU_AXIS_X];
	msgStruct.yacc  = (int16_t)mpu_acc[MPU_AXIS_Y];
	msgStruct.zacc  = (int16_t)mpu_acc[MPU_AXIS_Z];
	msgStruct.xgyro = (int16_t)mpu_gyro[MPU_AXIS_X];
	msgStruct.ygyro = (int16_t)mpu_gyro[MPU_AXIS_Y];
	msgStruct.zgyro = (int16_t)mpu_gyro[MPU_AXIS_Z];
	msgStruct.xmag  = (int16_t)0x1111;
	msgStruct.ymag  = (int16_t)0x1111;
	msgStruct.zmag  = (int16_t)0x1111;
	msgStruct.id    = (uint8_t)0x11;
	msgStruct.temperature = (int16_t)0x1111;
	//test = 0x44;
	//Radio_Transmit_Raw(&test, 1);
	//memcpy(msgStruct.payload,payload_new, sizeof(payload_new));

	//encode and serialize
	mavlink_msg_raw_imu_encode(SYSTEM_ID, COMPONENT_ID, &msg, &msgStruct);
	mavlink_msg_to_send_buffer(&buffer, &msg);
	//test = 0x55;
	//Radio_Transmit_Raw(&test, 1);
	//transmit
	Radio_Transmit_Raw(&buffer, sizeof(buffer));
}

void MAV_Send_Msg_Named_Value_Int(char message[], uint32_t value)
{
	//create buffer of static proper length (static for this mavlink message)
	int PACKET_STATIC_SIZE = 30; //used to determine buffer size, and reduce bad 00's being sent
	char buffer[PACKET_STATIC_SIZE];

	//create struct and fill in data
	mavlink_named_value_int_t msgStruct;
	strcpy(msgStruct.name,message);
	msgStruct.time_boot_ms = 4;
	msgStruct.value = value;
	//encode and serialize
	mavlink_msg_named_value_int_encode(SYSTEM_ID, COMPONENT_ID, &msg, &msgStruct);
	mavlink_msg_to_send_buffer(&buffer, &msg);
	//transmit
	Radio_Transmit_Raw(&buffer, sizeof(buffer));
}

void MAV_Send_File_Transfer_Protocol(uint8_t payload[], uint8_t payload_len)
{   //sends network id, target system, target component, and payload
    //**NOTE: uses payload within payload, see FTP protocol

	//define variables not used (add as arguments for increased functionality)
	uint8_t network_id = 0;//for broadcast
	uint8_t target_system = 0;//for broadcast
	uint8_t target_component = 0;//for broadcast
	uint8_t payload_new[251] = {0};//copy payload to new array for fixed size of 251
	payload_new[0] = payload_len; //encode first byte as amount of space used in payload

	for (uint8_t i=1;i<251;i++)
	{	if(i <payload_len+1)
		{
			//Radio_Transmit_Raw(&payload[i-1], 1);
			//HAL_Delay(500);
			payload_new[i] = payload[i-1];
		}
		else
		{
			payload_new[i] = 0xFF;//fill dead space
		}
	}
	//Radio_Transmit_Raw(&payload_new,payload_len+1 );
	//HAL_Delay(500);
	//strcpy(payload_new,payload);

	//create buffer of proper length
	//int PACKET_STATIC_SIZE = 10 + 3 + payload_len + 2; //mavlink[FTP header + payload]mavlink
	int PACKET_STATIC_SIZE = 10 + 3 + 251 + 2; //mavlink[FTP header + payload]mavlink
	uint8_t buffer[PACKET_STATIC_SIZE];

	//create struct and fill in data
	mavlink_file_transfer_protocol_t msgStruct;
	msgStruct.target_network = network_id;
	msgStruct.target_system = target_system;
	msgStruct.target_component = target_component;
	memcpy(msgStruct.payload,payload_new, sizeof(payload_new));

	//encode and serialize
	mavlink_msg_file_transfer_protocol_encode(SYSTEM_ID, COMPONENT_ID, &msg, &msgStruct);
	mavlink_msg_to_send_buffer(&buffer, &msg);

	//transmit
	Radio_Transmit_Raw(&buffer, sizeof(buffer));

}

void MAV_Send_Debug_Statement(char message[], uint32_t value)
{
	MAV_Send_Msg_Named_Value_Int(&message, value);
}

void MAV_Send_Debug_Statement_Default()
{
	char name[10] = "abcdefghij;";
	MAV_Send_Msg_Named_Value_Int(&name, 7);
}

/* OLD stuff
 void MAV_Correct_STX(uint8_t* rx_buffer){
	rx_buffer[0] = PROPER_STX;
}
 */
