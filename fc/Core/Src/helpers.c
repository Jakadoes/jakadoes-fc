/*
 * helpers.c
 *
 *  Created on: Jan. 12, 2021
 *      Author: damie
 */

#include "stdint.h"

void Helper_Int8arr_To_Int32(uint8_t* arr, uint32_t *newNum)
{//converts a uint8_t[3] into a uint32_t, where uint8_t[0] is LSB
	*newNum = arr[2] + (arr[1]<<8) + (arr[0]<<(2*8));
	//*newNum = 0x11 + (0x22<<8) + (0x33<<(2*8)); //this works
}

void Helper_Int16arr_To_Int8arr(uint16_t* arr, uint8_t size, uint8_t* newArr)
{	//verified
	//converts an unsigned 16 bit array into an unsigned 8 bit array for easier reading over uart / radio debug
	//note, size corresponds to the size of the INPUT 16 bit array, # of elements
	//WITHOUT using the function, the MSB will print before the LSB, so for example [0x1234, 0x9876] will appear as [34 12 76 98]
	for (uint8_t index_8=0;index_8<2*size;index_8++)
	{
		if(index_8%2 == 1)//lower byte
		{
			newArr[index_8] = arr[index_8/2] & 0x00FF;//mask for only lower byte
		}
		else
		{
			newArr[index_8] = (arr[index_8/2] >>8 ) & 0x00FF;//shift and mask upper byte
		}
	}
	//previously used test code:
	/*
	uint8_t poop[12];
    Helper_Int16arr_To_Int8arr(&baro_coeffs, 6, &poop);
	Radio_Transmit_Raw(&poop, 2*6);
	*/
}


void Helper_Int32_To_Int8arr(uint32_t num, uint8_t* newArr)
{	//verified
	//converts 32bit number to 4 long byte array for easier reading through uart
	//without helper, the MSB would write first, followed by LSB so 0x19293949 would be 49 39 29 19
	newArr[3] = num & 0xFF;
	newArr[2] = (num>>8) & 0xFF;
	newArr[1] = (num>>16) & 0xFF;
	newArr[0] = (num>>24) & 0xFF;
	//previously used test code:
	/*
	 * int32_t test = 0x19293949;
	   int8_t test2[4];
	   Helper_Int32_To_Int8arr(test, &test2);
	   Radio_Transmit_Raw(&test2, 4);
	 */
}
void Helper_Int64_To_Int8arr(uint64_t num, uint8_t* newArr)
{	//converts 64bit number to 8 long byte array for easier reading through uart
	//without helper, the MSB would write first, followed by LSB so 0x19293949 would be 49 39 29 19
	newArr[7] = num & 0xFF;
	newArr[6] = num>>8 & 0xFF;
	newArr[5] = (num>>(8*2)) & 0xFF;
	newArr[4] = (num>>(8*3)) & 0xFF;
	newArr[3] = (num>>(8*4)) & 0xFF;
	newArr[2] = (num>>(8*5)) & 0xFF;
	newArr[1] = (num>>(8*6)) & 0xFF;
	newArr[0] = (num>>(8*7)) & 0xFF;
	//previously used test code:
	/*
	 * int32_t test = 0x19293949;
	   int8_t test2[4];
	   Helper_Int32_To_Int8arr(test, &test2);
	   Radio_Transmit_Raw(&test2, 4);
	 */
}
