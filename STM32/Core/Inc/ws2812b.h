/*
 * ws2812b.h
 *
 *  Created on: Mar 25, 2021
 *      Author: Jonathan
 */

#ifndef INC_WS2812B_H_
#define INC_WS2812B_H_

#include "dma.h"
#include "tim.h"

#define LED_COUNT 36

#define TIMER_AAR 209
#define DUTY_0 67 // (~32% of 209)
#define DUTY_1 130 // (~62% of 209)
#define DUTY_RESET 0
#define BPP 3
#define NUM_BYTES (BPP*LED_COUNT)
#define WRB_LEN (BPP * 8 * 2)

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern DMA_HandleTypeDef hdma_tim3_ch1;
extern DMA_HandleTypeDef hdma_tim3_ch2;
extern DMA_HandleTypeDef hdma_tim3_ch3;
extern DMA_HandleTypeDef hdma_tim4_ch1;
extern DMA_HandleTypeDef hdma_tim4_ch2;

typedef struct LightStrip{
	uint8_t count;
	uint8_t color_buffer[NUM_BYTES];
	uint8_t write_buffer[WRB_LEN];
	uint_fast8_t wb_pos;
	DMA_HandleTypeDef *hdma_tim_chan;
	TIM_HandleTypeDef *htim;
	uint32_t timer_channel;
}LightStrip;

void LS_init(LightStrip *L, uint8_t c, DMA_HandleTypeDef *dma, TIM_HandleTypeDef *tim, uint32_t tchan);
void set_RGB(LightStrip *L, uint8_t index, uint8_t r, uint8_t g, uint8_t b);
void set_all_RGB(LightStrip *L, uint8_t r, uint8_t g, uint8_t b);
void write_RGB(LightStrip *L);
uint32_t hsl_to_rgb(uint8_t h, uint8_t s, uint8_t l);
#endif /* INC_WS2812B_H_ */
