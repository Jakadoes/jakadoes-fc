/*
 * barometer.h
 *
 *  Created on: Jan 5, 2021
 *      Author: damie
 */

#ifndef INC_BAROMETER_H_
#define INC_BAROMETER_H_

#define BARO_I2C_ADDRESS 119 //7 bit address, 1110111
#define BARO_PROM_READ_COMMAND_START  0xA0  //read prom values, address to be added to this
#define BARO_PRESS_CONV_COMMAND_START 0x40  //convert pressure values, address to be added to this
#define BARO_TEMP_CONV_COMMAND_START  0x50  //convert temperature values, address to be added to this to complete command
#define BARO_ADC_READ_COMMAND         0X00  //read adc value
#define BARO_OSR_256 0
#define BARO_OSR_512 2
#define BARO_OSR_1024 4
#define BARO_OSR_2048 6
#define BARO_OSR_4096 8


#endif /* INC_BAROMETER_H_ */

//variables
uint8_t baro_prom_rx_buffer[2];
uint8_t baro_temp_rx_buffer[3]; //buffer for pressure and temperature messages
uint8_t baro_ready_status; //check hal manual page 25 for enum readouts
uint8_t baro_test;
uint16_t baro_coeffs[6];
int32_t pressure;

//methods
uint8_t Baro_Is_Ready();
void Baro_Read_Prom_Coefficient(int8_t coeff);
void Baro_Get_Coefficients();
void Baro_Request_Temp(int8_t baro_osr);
void Baro_Request_Press(int8_t baro_osr);
void Baro_Get_Temp();
void Baro_Get_Press();
void Baro_Calculate_Altitude();
