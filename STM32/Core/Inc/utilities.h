/*
 * utilities.h
 *
 *  Created on: Mar 24, 2021
 *      Author: Jonathan
 */

#ifndef INC_UTILITIES_H_
#define INC_UTILITIES_H_

#include "main.h"
#include "tim.h"

void delay_us(uint16_t us);

//Used to get a specific bit from an integer
#define GET_Bit(byte,bit)	(byte >> bit)
#define LOW 0
#define HIGH 1

//C really should have a Bool type
typedef enum {
	false, true
} bool;

#endif /* INC_UTILITIES_H_ */
