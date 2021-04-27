/*
 * uart_handler.h
 *
 *  Created on: Apr 7, 2021
 *      Author: Jonathan
 */

#ifndef INC_UART_HANDLER_H_
#define INC_UART_HANDLER_H_
#include "dma_uart_rb.h"
#include "clock.h"
#include "pca9685.h"

uint8_t clock_state;

typedef struct ServoCal{
	uint8_t d_srv[4];
	uint16_t angle;
}ServoCal;

typedef struct clock_vars{
	uint8_t hms[3];
	uint8_t state;
}ClockVars;

extern ClockVars cv;

void Process_Uart_In(ringbuff_t *buff, ringbuff_evt_type_t type, size_t len);
void init_handler();
void change_time(sgTime *Time, ClockVars *cv);
void cal_servos(ServoCal *s);
#endif /* INC_UART_HANDLER_H_ */
