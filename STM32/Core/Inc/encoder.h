/*
 * encoder.h
 *
 *  Created on: Mar 3, 2021
 *      Author: Jonathan
 */

#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_
#include <main.h>
#include <stdlib.h>
#include "utilities.h"
#include "stm32g4xx_hal_rtc.h"

typedef struct _encoder_ {
	GPIO_TypeDef *button_port;
	uint16_t button_pin;
	volatile uint32_t *counter;
	int pat;
	int32_t count;
	int32_t last_count;
	uint8_t selector;
	uint8_t button_state;
	uint8_t button_last;
	uint8_t change;
	uint8_t sel_change;
	uint32_t delay;
	float value;
} Encoder;


void init_encoder(Encoder *en, GPIO_TypeDef *port, volatile uint32_t *encoder_counter, uint16_t button_pin);

#endif /* INC_ENCODER_H_ */
