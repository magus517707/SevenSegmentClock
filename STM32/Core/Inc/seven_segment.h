/*
 * seven_segment.h
 *
 *  Created on: Mar 24, 2021
 *      Author: Jonathan
 */

#ifndef INC_SEVEN_SEGMENT_H_
#define INC_SEVEN_SEGMENT_H_

#include "main.h"
#include "clock.h"
#include "pca9685.h"
#include "utilities.h"
#include "ws2812b.h"

#define H10 0
#define H1 1
#define M10 2
#define M1 3

typedef struct ServoOffsets{
	// All values represent degrees
	uint8_t min;
	uint8_t max;
}ServoOffsets;

typedef struct Digit{
	/**
	 * Channel order is alphabetical to zero index
	 * 			 ___a___
	 * 			|		|
	 * 			f		b
	 * 			|___g___|
	 * 			|		|
	 * 			e		c
	 * 			|___d___|
	 */

	LightStrip *LS;
	uint8_t light_digit[LED_COUNT];
	ServoOffsets offsets[7];// May need to use as offsets for the servos to sync displacement
	uint8_t *chan_array; 	// List of channel #'s associated to digit; see display above for order
	uint8_t set_array[7]; 	// Stores what servos should be moved to display the value assigned
	uint8_t value;			// Value assigned to digit
	uint8_t dig_pos;
	I2C_HandleTypeDef *pca9685_i2c;
}Digit;

Digit D_HrTen;
Digit D_HrOne;
Digit D_MnTen;
Digit D_MnOne;

void init_Digit(Digit *D, LightStrip *LS, uint8_t *chan_arr, I2C_HandleTypeDef *i2c, uint8_t dig_pos);
uint8_t parse_tens(uint8_t val);
uint8_t parse_ones(uint8_t val);
void update_Digits(Digit *h1,Digit *h2,Digit *m1,Digit *m2, sgTime *Time);
void transform_Digit(Digit *D);
void render_Digits(Digit *h1,Digit *h2,Digit *m1,Digit *m2);
void render_sep(LightStrip *LS, uint8_t h, uint8_t s, uint8_t l);
void get_SetLSArray(Digit *D);
void display_LEDValue(Digit *D,uint8_t h, uint8_t s, uint8_t l);
void get_SetArray(Digit *D);
void set_Digit_value(Digit *D, uint8_t val);
void set_Digit_servo(Digit *D);
void all_Digit_servo(Digit *D, uint8_t l);
void set_Digit_servo_angle(Digit *D, uint16_t ang);
void init_offsets(Digit *D, uint8_t *min, uint8_t *max);
#endif /* INC_SEVEN_SEGMENT_H_ */
