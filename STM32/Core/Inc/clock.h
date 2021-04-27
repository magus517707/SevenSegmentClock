/*
 * clock.h
 *
 *  Created on: Mar 31, 2021
 *      Author: Jonathan
 */

#ifndef INC_CLOCK_H_
#define INC_CLOCK_H_

#include "main.h"
#include "rtc.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

#define Hour	0
#define Min		1

typedef struct sgTime{
	RTC_TimeTypeDef sTime;	//Time Setter
	RTC_DateTypeDef sDate;	//Date Setter
	RTC_TimeTypeDef gTime;	//Time Getter
	RTC_DateTypeDef gDate;	//Date Getter
	char time[10];			//Time String space
	char date[10];			//Date String space
}sgTime;

void init_clock (sgTime *Time);
void set_time(sgTime *Time, uint8_t value, uint8_t pos);
void set_time_all(sgTime *Time, uint8_t h, uint8_t m, uint8_t s);
void get_time(sgTime *Time);
void serialTime(sgTime *Time);

#endif /* INC_CLOCK_H_ */
