/*
 * uart_handler.c
 *
 *  Created on: Apr 7, 2021
 *      Author: Jonathan
 */

#include "uart_handler.h"
#include "stdlib.h"
#include "seven_segment.h"

//Alias for the DMA RX buffer
#define pbuff process_buffer


const uint8_t eom[] = { 'E', 'O', 'M', '\r' }; // Footer to check for on incoming messages

ClockVars cv = {0};
ServoCal sc = {0};


void init_handler() {
	cv.state = 99;
	ringbuff_set_evt_fn(&rx_rb, Process_Uart_In);
}


void Process_Uart_In(ringbuff_t *buff, ringbuff_evt_type_t type, size_t len) {
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	if (type == RINGBUFF_EVT_READ) {
		if (!(memcmp(&pbuff[len - 4], &eom, 4))) {
			switch(pbuff[0]){
				case 'H':	// Get value to set Hours
					cv.hms[0] = pbuff[1];
					cv.state = 1;
					break;
				case 'M':	// Get value to set Minutes
					cv.hms[1] = pbuff[1];
					cv.state = 2;
					break;
				case 'S':	// Get value to set Seconds
					cv.hms[2] = pbuff[1];
					cv.state = 3;
					break;
				case 'A':	// Get values to set all time values
					cv.hms[0] = pbuff[1];//H
					cv.hms[1] = pbuff[2];//M
					cv.hms[2] = pbuff[3];//S
					cv.state = 4;
					break;
				case 'T':
					clock_state = !clock_state; // Toggle Clock State
					break;
				case 'Z':						// Set servos. Can only work when Clock state is 0
					if(clock_state == 0){
						sc.d_srv[0] = pbuff[1]; //HR_Tens
						sc.d_srv[1] = pbuff[2]; //HR_Ones
						sc.d_srv[2] = pbuff[3]; //MIN_Tens
						sc.d_srv[3] = pbuff[4]; //MIN_Ones
						sc.angle = (pbuff[5]<<8)|(pbuff[6]&0x00FF); // Create 16 bit value from two 8 bit values
						cal_servos(&sc);
					}
			}
		}
		else {
			cv.state = 0;
		}
	}
}

void change_time(sgTime *Time, ClockVars *cv){
	switch(cv->state){
		case 1:
			set_time(Time, cv->hms[0], cv->state);// Set Hours
			break;
		case 2:
			set_time(Time, cv->hms[1], cv->state);// Set Minutes
			break;
		case 3:
			set_time(Time, cv->hms[2], cv->state);// Set Seconds
			break;
		case 4:
			set_time_all(Time, cv->hms[0], cv->hms[1], cv->hms[2]);// Set all time values
			break;
		case 99:
			return;
		default:
			break;
	}
	cv->state = 99;
	get_time(Time);
	serialTime(Time);
}

void cal_servos(ServoCal *s){
	for(int i = 0; i < 4; i++){
		uint8_t d = s->d_srv[i];
		if(d != 0){
			switch(i){
				case 0:
					for(int j = 0;j<7;j++)D_HrTen.set_array[i] = ((1<<i)&d);
					set_Digit_servo_angle(&D_HrTen, s->angle);
					break;
				case 1:
					for(int j = 0;j<7;j++)D_HrOne.set_array[i] = ((1<<i)&d);
					set_Digit_servo_angle(&D_HrOne, s->angle);
					break;
				case 2:
					for(int j = 0;j<7;j++)D_MnTen.set_array[i] = ((1<<i)&d);
					set_Digit_servo_angle(&D_MnTen, s->angle);
					break;
				case 3:
					for(int j = 0;j<7;j++)D_MnOne.set_array[i] = ((1<<i)&d);
					set_Digit_servo_angle(&D_MnOne, s->angle);
					break;
			}
		}
	}
}
