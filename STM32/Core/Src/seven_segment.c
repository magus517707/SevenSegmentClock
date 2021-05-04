/*
 * seven_segment.c
 *
 *  Created on: Mar 24, 2021
 *      Author: Jonathan
 */


#include "seven_segment.h"
#define SERVO_DEL 100

#define LEDH 2
#define LEDS 255
#define LEDL 127

// Initialize a single digit structure on the clock
void init_Digit(Digit *D, LightStrip *LS, uint8_t *chan_arr, I2C_HandleTypeDef *i2c, uint8_t dig_pos){
	D->LS = LS;
	D->chan_array = chan_arr;
	D->dig_pos = dig_pos;
	D->pca9685_i2c = i2c;
	for(int i = 0; i < 7;i++){
		D->set_array[i] = 0;
	}
}

uint8_t parse_ones(uint8_t val){
	return val%10;
}

uint8_t parse_tens(uint8_t val){
	return (val/10);
}

//Update all Digits from the Clock values
void update_Digits(Digit *h1,Digit *h2,Digit *m1,Digit *m2, sgTime *Time){
	get_time(Time);
	h1->value = parse_tens(Time->gTime.Hours);
	get_SetArray(h1);
	h2->value = parse_ones(Time->gTime.Hours);
	get_SetArray(h2);
	m1->value = parse_tens(Time->gTime.Minutes);
	get_SetArray(m1);
	m2->value = parse_ones(Time->gTime.Minutes);
	get_SetArray(m2);
}

// Physically change the servos and LEDS on the Clock
void transform_Digit(Digit *D){
	get_SetArray(D);
	get_SetLSArray(D);
	set_Digit_servo(D);
	display_LEDValue(D, LEDH, LEDS, LEDL);
	HAL_Delay(10);
}

// Basically sued to update the physical Clock
void render_Digits(Digit *h1,Digit *h2,Digit *m1,Digit *m2){
	transform_Digit(h1);
	transform_Digit(h2);
	transform_Digit(m1);
	transform_Digit(m2);
}

// Setup the separator color
void render_sep(LightStrip *LS, uint8_t h, uint8_t s, uint8_t l){
	uint32_t rgb_color = hsl_to_rgb(h, s, l);
	for(int i = 0; i<LS->count;i++){
		set_RGB(LS,i, (rgb_color >> 16) & 0xFF, (rgb_color >> 8) & 0xFF, rgb_color & 0xFF);
	}
	write_RGB(LS);
}

// For the Digit LEDS select the LED segments that need to be set to display the number.
void get_SetLSArray(Digit *D){
	/**
	 * LED Segment Order
	 * 			 ___1___
	 * 			|		|
	 * 			2		4
	 * 			|__3/5__|
	 * 			|		|
	 * 			6		8
	 * 			|___7___|
	 */
	uint8_t count = 0;
	uint8_t iters[8] = {4,5,4,5,4,5,4,5};
	static const uint8_t LUT[16][8] = {
			{1,1,0,1,0,1,1,1},//0
			{0,0,0,1,0,0,0,1},//1
			{1,0,1,1,1,1,1,0},//2
			{1,0,1,1,1,0,1,1},//3
			{0,1,1,1,1,0,0,1},//4
			{1,1,1,0,1,0,1,1},//5
			{1,1,1,0,1,1,1,1},//6
			{1,0,0,1,0,0,0,1},//7
			{1,1,1,1,1,1,1,1},//8
			{1,1,1,1,1,0,0,1},//9
			{1,1,1,1,1,1,0,1},//A
			{0,1,1,0,1,1,1,1},//b
			{1,1,0,0,0,1,1,0},//C
			{0,0,1,1,1,1,1,1},//d
			{1,1,1,0,1,1,1,0},//E
			{1,1,1,0,1,1,0,0},//F
	};
	for(int i = 0; i < 8; i++){
		if(LUT[D->value][i] == 1){
			for(int j = 0; j < iters[i];j++){
				D->light_digit[count] = 1;
				count++;
			}
		}
		else{
			for(int j = 0; j < iters[i];j++){
				D->light_digit[count] = 0;
				count++;
			}
		}
	}
}

// Once the LED Digit value has been captured, change the LEDS
void display_LEDValue(Digit *D,uint8_t h, uint8_t s, uint8_t l){
	uint32_t rgb_color = hsl_to_rgb(h, s, l);
	for(int i = 0; i<D->LS->count;i++){
		if(D->light_digit[i]==1){
			set_RGB(D->LS,i, (rgb_color >> 16) & 0xFF, (rgb_color >> 8) & 0xFF, rgb_color & 0xFF);
		}
		else{
			set_RGB(D->LS,i, 0,0,0);
		}
	}
	write_RGB(D->LS);
}

// Like the LED array but this is for the Servos that need to be set to display the digit.
void get_SetArray(Digit *D){
	static const uint8_t LUT[16][7] = {
			{1,1,1,1,1,1,0},//0
			{0,1,1,0,0,0,0},//1
			{1,1,0,1,1,0,1},//2
			{1,1,1,1,0,0,1},//3
			{0,1,1,0,0,1,1},//4
			{1,0,1,1,0,1,1},//5
			{1,0,1,1,1,1,1},//6
			{1,1,1,0,0,0,0},//7
			{1,1,1,1,1,1,1},//8
			{1,1,1,0,0,1,1},//9
			{1,1,1,0,1,1,1},//A
			{0,0,1,1,1,1,1},//b
			{1,0,0,1,1,1,0},//C
			{0,1,1,1,1,0,1},//d
			{1,0,0,1,1,1,1},//E
			{1,0,0,0,1,1,1},//F
	};
	for(int i = 0;i<7;i++){
		D->set_array[i] = LUT[D->value][i];
	}
}

//Set the Digit Value and update the Servo array
void set_Digit_value(Digit *D, uint8_t val){
	D->value = val;
	get_SetArray(D);
}

//Render the digit by moving the servos
void set_Digit_servo(Digit *D){
	for(int i = 0; i < 7; i++){
		if(D->set_array[i] != 0){
			PCA9685_SetServoAngle (D->pca9685_i2c, D->chan_array[i],D->offsets[i].min);
			delay_us(SERVO_DEL);
		}
		else{
			PCA9685_SetServoAngle (D->pca9685_i2c, D->chan_array[i],D->offsets[i].max);
			delay_us(SERVO_DEL);
		}
	}
}

//Set all Servos in a Digit to a specific angle
void all_Digit_servo(Digit *D, uint8_t l){
	if(l == 1){
		for(int i = 0; i < 7; i++){
			PCA9685_SetServoAngle (D->pca9685_i2c, D->chan_array[i],D->offsets[i].max);
			delay_us(SERVO_DEL);

		}
	}
	else{
		for(int i = 0; i < 7; i++){
			PCA9685_SetServoAngle (D->pca9685_i2c, D->chan_array[i],D->offsets[i].min);
			delay_us(SERVO_DEL);

		}
	}
}

// This is used specifically for servo calibration
void set_Digit_servo_angle(Digit *D, uint16_t ang){
	for(int i = 0; i < 7; i++){
		if(D->set_array[i] != 0){
			PCA9685_SetPin(D->pca9685_i2c, D->chan_array[i], ang, 0);
			delay_us(SERVO_DEL);
		}
	}
}

void init_offsets(Digit *D, uint8_t *min, uint8_t *max){
	for(int i = 0; i < 7; i++){
		D->offsets[i].min = min[i];
		D->offsets[i].max = max[i];
	}
}

