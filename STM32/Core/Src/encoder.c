/*
 * encoder.c
 *
 *  Created on: Mar 3, 2021
 *      Author: Jonathan
 */

#include "encoder.h"

uint8_t selector_step = 16;
void init_encoder(Encoder *en, GPIO_TypeDef *port, volatile uint32_t *encoder_counter, uint16_t button_pin) {

	en->button_port = port;
	en->button_pin = button_pin;
	en->counter = encoder_counter;
	en->count = 0;
	en->last_count = 0;
	en->selector = 0;
	en->value = 0.0;
	en->change = 0;
	en->sel_change = 0;
}


void check_encoder(Encoder *en) {
	en->button_state = HAL_GPIO_ReadPin(en->button_port, en->button_pin); //read the button state of the encoder
	en->count = TIM2->CNT >> 2;
	uint8_t d = abs(en->count - en->last_count);// determine the delta between the last read and the current read
	if (d > 32) d = 1; //crappy way to deal with under/over flow but still allow for a delta multiplier.
	//Determine if the button was pressed and change menu selector
	if ((en->button_state == LOW) && (en->button_last == HIGH)) {
		en->selector += selector_step;
		en->sel_change = 1;
	}
	//Direction Detection
	if (!(en->count == en->last_count)) {
		if (en->count - en->last_count < 0) {
			en->value = -d;
		}
		else {
			en->value = d;
		}
		en->change = 1;
	}
	//Set past states for next cycle
	en->last_count = en->count;
	en->button_last = en->button_state;
}
