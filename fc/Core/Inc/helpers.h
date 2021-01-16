/*
 * helpers.h
 *
 *  Created on: Jan. 12, 2021
 *      Author: damie
 */

#ifndef INC_HELPERS_H_
#define INC_HELPERS_H_
#include "stdint.h"


#endif /* INC_HELPERS_H_ */

void Helper_Int8arr_To_Int32(uint8_t* arr, uint32_t *newNum);
void Helper_Int16arr_To_Int8arr(uint16_t* arr, uint8_t size, uint8_t* newArr);
void Helper_Int32_To_Int8arr(uint32_t num, uint8_t* newArr);
