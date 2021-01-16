/*
 * barometer.c
 *
 *  Created on: Jan 5, 2021
 *      Author: damien sabljak
 *
 *      TO DO:
 *      write code to calculate second order approximation for temperatures below 20 degrees C
 *      implement simple averaging LPF filter for consistent readings
 */
#include "stdint.h"
#include"stm32f1xx_hal.h"
#include "i2c.h"
#include "barometer.h"
#include "helpers.h"

uint8_t baro_prom_rx_buffer[2];
uint8_t baro_temp_rx_buffer[3]; //buffer temperature messages
uint8_t baro_press_rx_buffer[3];//buffer for pressure messages
uint8_t baro_ready_status;      //check hal manual page 25 for enum readouts
uint16_t baro_coeffs[6];
uint16_t baro_test2[6] = {0x1234, 0x4321, 0x1010, 0x8765, 0x9983};
int32_t pressure;

uint8_t Baro_Is_Ready()
{
	//returns HAL address
	uint8_t baro_ready_status = I2c_IsDeviceReady(BARO_I2C_ADDRESS);
	return baro_ready_status;
}
//coefficient setup methods
void Baro_Read_Prom_Coefficient(int8_t coeff)
{
	//send single read command
	//receive 16 bits of data, 8 bit chunks with ack in between
	uint8_t command = BARO_PROM_READ_COMMAND_START + (coeff << 1); //coefficient is shifted one to left (check data sheet)
	I2c_Master_Transmit(BARO_I2C_ADDRESS, &command, 1);
	I2c_Master_Receive(BARO_I2C_ADDRESS, baro_prom_rx_buffer, 2);
}
void Baro_Get_Coefficients()
{//retreive calibration coefficients from ROM (adresses [1-6])
	for(int8_t i=1;i<7;i++)
	{
		Baro_Read_Prom_Coefficient(i);
		//convert from array to int 16
		int16_t coeff = ( baro_prom_rx_buffer[0]<<8 ) + baro_prom_rx_buffer[1];
		baro_coeffs[i-1] =coeff;
	}
}

//measurement request methods
//note, high osr values appear to make the readings more sensitive
void Baro_Request_Temp(int8_t baro_osr)
{	//sends request to barometer, barometer will display as not busy once ready to transmit
	uint8_t command = BARO_TEMP_CONV_COMMAND_START + baro_osr; //coefficient is shifted one to left (check data sheet)
	I2c_Master_Transmit(BARO_I2C_ADDRESS, &command, 1);
}
void Baro_Request_Press(int8_t baro_osr)
{	//sends request to barometer, barometer will display as not busy once ready to transmit
	uint8_t command = BARO_PRESS_CONV_COMMAND_START + baro_osr; //coefficient is shifted one to left (check data sheet)
	I2c_Master_Transmit(BARO_I2C_ADDRESS, &command, 1);
}
void Baro_Get_Temp()
{//receives what was requested, make sure to check business before requesting
	uint8_t command = BARO_ADC_READ_COMMAND;
	I2c_Master_Transmit(BARO_I2C_ADDRESS, &command, 1);
	I2c_Master_Receive(BARO_I2C_ADDRESS, baro_temp_rx_buffer, 3);
	//baro_adc_rx_buffer[0] = 1;
	//baro_adc_rx_buffer[1] = 2;
	//baro_adc_rx_buffer[2] = 3;
}
void Baro_Get_Press()
{//receives what was requested, make sure to check business before requesting
	uint8_t command = BARO_ADC_READ_COMMAND;
	I2c_Master_Transmit(BARO_I2C_ADDRESS, &command, 1);
	I2c_Master_Receive(BARO_I2C_ADDRESS, baro_press_rx_buffer, 3);
	//baro_adc_rx_buffer[0] = 1;
	//baro_adc_rx_buffer[1] = 2;
	//baro_adc_rx_buffer[2] = 3;
}

//measurement calculation methods
void Baro_Calculate_Altitude()
{//calculates altitude from given temperature and pressure values in buffers
	//NOTE::::: CHANGE RX_BUFFER TO UINT VALUES
	int8_t test2[4];
	//convert array to 32bit number
	uint32_t temp_value = 0;
	uint32_t press_value = 0;
	uint8_t tempArr[4];
	uint8_t dtArr[4];
	uint8_t finalArr[8];
	uint8_t offArr[8];
	Helper_Int8arr_To_Int32(&baro_temp_rx_buffer, &temp_value);
	Helper_Int8arr_To_Int32(&baro_press_rx_buffer, &press_value);

	//caulate temperature
	int32_t dt = temp_value - baro_coeffs[4]; //verified
	//Helper_Int32_To_Int8arr(temp_value, &tempArr);
	//Helper_Int32_To_Int8arr(press_value, &dtArr);
	//Radio_Transmit_Raw(&baro_press_rx_buffer, 4);
	//HAL_Delay(500);
	//Radio_Transmit_Raw(&tempArr, 4);
	//HAL_Delay(500);
	//Radio_Transmit_Raw(&dtArr, 4);
	int64_t temp = 2000 + ( (dt*baro_coeffs[5])>>23 ); //documentation is wrong, recommends 32 which wont fit
	//Helper_Int64_To_Int8arr(temp, &finalArr);
	//HAL_Delay(500);
	//Radio_Transmit_Raw(&finalArr, 8);

	//calculate pressure
	int64_t off = ( (baro_coeffs[1]<<16) & 0xFFFFFFFF ) + ( ( ((int64_t) baro_coeffs[3]) * dt) >>7 );//verified. IMPORTANT: cast to signed int before multiplication
	//Helper_Int64_To_Int8arr(off, &offArr);
	//HAL_Delay(500);
	//Radio_Transmit_Raw(&offArr, 8);

	int64_t sens = ( ((int64_t) baro_coeffs[0]) <<15) + ( ( ((int64_t) baro_coeffs[2]) *dt)>>8 );//verified
	//Helper_Int64_To_Int8arr(sens, &offArr);
	//HAL_Delay(500);
	//Radio_Transmit_Raw(&offArr, 8);

	int64_t press_temp = ( ((int64_t) press_value) * (sens>>21) - off )>>15; //requires larger sized variable for calcs
	//Helper_Int64_To_Int8arr(press_temp, &offArr);
	//HAL_Delay(500);
	//Radio_Transmit_Raw(&offArr, 8);
	//HAL_Delay(500);
	pressure = press_temp;//cast to 32int
//baro raw buffer, pressure value,

}

/* code used to send barometer data over the air:
 *
 *
 *before while loop:
 *uint8_t baro_flag = 5;
 *during while loop:
 *uint8_t mode[4] = {0x44, 0x00, 0x44};
		if(baro_flag == 0)
		{
			mode[1] = 0x00;
			Baro_Is_Ready();
			//Radio_Transmit_Raw(&mode, 4);
			//Radio_Transmit_Raw(&baro_ready_status, 1);

			Baro_Request_Press(BARO_OSR_4096);
			baro_flag = 1;
			HAL_Delay(10);//currently, this is needed to get proper values back
		}
		else if (baro_flag == 1 && ( Baro_Is_Ready()== HAL_OK ))
		{
			mode[1] = 0x11;
			Baro_Get_Press();
			//Radio_Transmit_Raw(&baro_temp_rx_buffer, 3);
			//Radio_Transmit_Raw(&mode, 4);
			Baro_Request_Temp(BARO_OSR_4096);
			baro_flag = 2;
			HAL_Delay(10);
		}
		else if (baro_flag == 2 && ( Baro_Is_Ready()== HAL_OK ))
		{
			//char newline[2] = {'\n', '\n'};
			mode[1] = 0x22;
			Baro_Get_Temp();
			Baro_Calculate_Altitude();
			//Radio_Transmit_Raw(&mode, 4);
			//Radio_Transmit_Raw(&newline, 2);
			uint8_t pressArr[4];
			Helper_Int32_To_Int8arr(pressure, &pressArr);
			Radio_Transmit_Raw(&pressArr, 4);
			HAL_Delay(100); //DO NOT Delay when real time flight controls are required
			baro_flag = 0;
		}
		else if(baro_flag == 5 && ( Baro_Is_Ready() == HAL_OK))
		{//startup - get coefficients and send them out
			mode[1] = 0x55;
			Baro_Get_Coefficients();
			//Radio_Transmit_Raw(&mode, 4);
			//Radio_Transmit_Raw(&rx_buffer, 4);
			//Radio_Transmit_Raw(&baro_coeffs, 2*6);
			baro_flag = 0;
			HAL_Delay(1000);
		}

 */
