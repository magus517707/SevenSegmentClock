/*
 * clock.c
 *
 *  Created on: Mar 31, 2021
 *      Author: Jonathan
 */
#include "clock.h"

//Initialize the Clock
void init_clock (sgTime *Time){
	Time->sTime.Hours = 0x00; // set hours
	Time->sTime.Minutes = 0x00; // set minutes
	Time->sTime.Seconds = 0x00; // set seconds
	Time->sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	Time->sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	Time->sTime.TimeFormat = RTC_HOURFORMAT_24;
	HAL_RTC_SetTime(&hrtc, &Time->sTime, RTC_FORMAT_BCD);
	Time->sDate.WeekDay = RTC_WEEKDAY_THURSDAY; // day
	Time->sDate.Month = RTC_MONTH_JANUARY; // month
	Time->sDate.Date = 0x1; // date
	Time->sDate.Year = 0x18; // year
	HAL_RTC_SetDate(&hrtc, &Time->sDate, RTC_FORMAT_BCD);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2); // backup register
}

//Set single time unit
void set_time(sgTime *Time, uint8_t value, uint8_t pos){
	get_time(Time);
	switch (pos){
	case 1:
		Time->sTime.Hours = value; // set hours
		Time->sTime.Minutes = Time->gTime.Minutes;
		Time->sTime.Seconds = Time->gTime.Seconds;
	break;
	case 2:
		Time->sTime.Hours = Time->gTime.Hours;
		Time->sTime.Minutes = value; // set minutes
		Time->sTime.Seconds = Time->gTime.Seconds;
	break;
	case 3:
		Time->sTime.Minutes = Time->gTime.Minutes;
		Time->sTime.Hours = Time->gTime.Hours;
		Time->sTime.Seconds = value; // set seconds
	break;
	}
	HAL_RTC_SetTime(&hrtc, &Time->sTime, RTC_FORMAT_BCD);
	HAL_RTC_SetDate(&hrtc, &Time->sDate, RTC_FORMAT_BCD);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2); // backup register
}

//Set total clock
void set_time_all(sgTime *Time, uint8_t h, uint8_t m, uint8_t s){
	Time->sTime.Hours = h; 		// set hours
	Time->sTime.Minutes = m;	// set minutes
	Time->sTime.Seconds = s;	// set seconds
	HAL_RTC_SetTime(&hrtc, &Time->sTime, RTC_FORMAT_BCD);
	HAL_RTC_SetDate(&hrtc, &Time->sDate, RTC_FORMAT_BCD);
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2); // backup register
}

void get_time(sgTime *Time){
	/* Get the RTC current Time */
	 HAL_RTC_GetTime(&hrtc, &Time->gTime, RTC_FORMAT_BIN);
	/* Get the RTC current Date */
	 HAL_RTC_GetDate(&hrtc, &Time->gDate, RTC_FORMAT_BIN);
}

// Send time over Serial
void serialTime(sgTime *Time){
	/* Display time Format: hh:mm:ss */
	  sprintf((char*)Time->time,"%02d:%02d:%02d",Time->gTime.Hours, Time->gTime.Minutes, Time->gTime.Seconds);
	  //HAL_UART_Transmit(&hlpuart1, (uint8_t*) Time->time, strlen(Time->time), 1000);
	  //HAL_UART_Transmit(&hlpuart1, (uint8_t*)"\n", 1, 1000);
	  HAL_UART_Transmit(&huart4, (uint8_t*) Time->time, strlen(Time->time),1000);
	  HAL_UART_Transmit(&huart4, (uint8_t*)"\n", 1, 1000);
}

void serialDate(sgTime *Time){
	/* Display date Format: dd-mm-yy */
	sprintf((char*)Time->date,"%02d-%02d-%2d",Time->gDate.Date, Time->gDate.Month, 2000 + Time->gDate.Year);
	HAL_UART_Transmit(&hlpuart1, (uint8_t*) Time->date, strlen(Time->date), 1000);
	HAL_UART_Transmit(&hlpuart1, (uint8_t*)"\n", 1, 1000);
}


