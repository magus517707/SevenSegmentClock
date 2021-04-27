/*
 * ws2812b.c
 *
 *  Created on: Mar 25, 2021
 *      Author: Jonathan
 */
#include "ws2812b.h"

LightStrip *GLS; //Hacky way to be able to manipulate a lightstrip during DMA transfer

//Initialize LightStrip
void LS_init(LightStrip *L, uint8_t c, DMA_HandleTypeDef *dma, TIM_HandleTypeDef *tim, uint32_t tchan){
	L->count = c;
	L->wb_pos = 0;
	L->htim = tim;
	L->hdma_tim_chan = dma;
	L->timer_channel = tchan;
	for(int i = 0; i< NUM_BYTES;i++){
		L->color_buffer[i]=0;
	}
	for(int i = 0; i< WRB_LEN;i++){
		L->write_buffer[i]=0;
	}
}

//Set RGB value to specific LED
void set_RGB(LightStrip *L, uint8_t index, uint8_t r, uint8_t g, uint8_t b){
	L->color_buffer[BPP*index    ] = g;
	L->color_buffer[BPP*index + 1] = r;
	L->color_buffer[BPP*index + 2] = b;
}

//Set all LEDs to specific value
void set_all_RGB(LightStrip *L, uint8_t r, uint8_t g, uint8_t b){
	for(uint_fast8_t i = 0; i < L->count; ++i){
		set_RGB(L, i, r, g, b);
	}
}

//Render RGB data on the LightStrip
void write_RGB(LightStrip *L){
	if(L->wb_pos != 0 || L->hdma_tim_chan->State != HAL_DMA_STATE_READY){
		// Ongoing Transfer, abort
		for(uint8_t i = 0; i < WRB_LEN; ++i){
			L->write_buffer[i] = 0;
		}
		L->wb_pos = 0;
		HAL_TIM_PWM_Stop_DMA(L->htim, L->timer_channel);
		return;
	}
	for(uint_fast8_t i = 0; i < 8; ++i){
	    L->write_buffer[i     ] = DUTY_0 << (((L->color_buffer[0] << i) & 0x80) > 0);
	    L->write_buffer[i +  8] = DUTY_0 << (((L->color_buffer[1] << i) & 0x80) > 0);
	    L->write_buffer[i + 16] = DUTY_0 << (((L->color_buffer[2] << i) & 0x80) > 0);
	    L->write_buffer[i + 24] = DUTY_0 << (((L->color_buffer[3] << i) & 0x80) > 0);
	    L->write_buffer[i + 32] = DUTY_0 << (((L->color_buffer[4] << i) & 0x80) > 0);
	    L->write_buffer[i + 40] = DUTY_0 << (((L->color_buffer[5] << i) & 0x80) > 0);
	}
	HAL_TIM_PWM_Start_DMA(L->htim, L->timer_channel, (uint32_t *)L->write_buffer, WRB_LEN);
	L->wb_pos = 2;
	GLS = L;
}

//Half-transfer of DMA buffer callback
void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim) {
  // DMA buffer set from LED(write_buff_p) to LED(write_buff_p + 1)
  if(GLS->wb_pos < LED_COUNT) {
    // Fill the even buffer
    for(uint_fast8_t i = 0; i < 8; ++i) {
      GLS->write_buffer[i     ] = DUTY_0 << (((GLS->color_buffer[3 * GLS->wb_pos    ] << i) & 0x80) > 0);
      GLS->write_buffer[i +  8] = DUTY_0 << (((GLS->color_buffer[3 * GLS->wb_pos + 1] << i) & 0x80) > 0);
      GLS->write_buffer[i + 16] = DUTY_0 << (((GLS->color_buffer[3 * GLS->wb_pos + 2] << i) & 0x80) > 0);
    }
    GLS->wb_pos++;
  } else if (GLS->wb_pos < LED_COUNT + 2) {
    // First half reset zero fill
    for(uint8_t i = 0; i < WRB_LEN / 2; ++i) GLS->write_buffer[i] = 0;
    GLS->wb_pos++;
  }
}

//Complete transfer of DMA buffer callback
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
  // DMA buffer set from LED(write_buff_p) to LED(write_buff_p + 1)
  if(GLS->wb_pos < LED_COUNT) {
    // Fill the odd buffer
    for(uint_fast8_t i = 0; i < 8; ++i) {
      GLS->write_buffer[i + 24] = DUTY_0 << (((GLS->color_buffer[3 * GLS->wb_pos    ] << i) & 0x80) > 0);
      GLS->write_buffer[i + 32] = DUTY_0 << (((GLS->color_buffer[3 * GLS->wb_pos + 1] << i) & 0x80) > 0);
      GLS->write_buffer[i + 40] = DUTY_0 << (((GLS->color_buffer[3 * GLS->wb_pos + 2] << i) & 0x80) > 0);
    }
    GLS->wb_pos++;
  } else if (GLS->wb_pos < LED_COUNT + 2) {
    // Second half reset zero fill
    for(uint8_t i = WRB_LEN / 2; i < WRB_LEN; ++i) GLS->write_buffer[i] = 0;
    ++GLS->wb_pos;
  } else {
	GLS->wb_pos = 0;
    HAL_TIM_PWM_Stop_DMA(GLS->htim, GLS->timer_channel);
  }
}

// Fast hsl2rgb algorithm: https://stackoverflow.com/questions/13105185/fast-algorithm-for-rgb-hsl-conversion
uint32_t hsl_to_rgb(uint8_t h, uint8_t s, uint8_t l) {
	if(l == 0) return 0;

	volatile uint8_t  r, g, b, lo, c, x, m;
	volatile uint16_t h1, l1, H;
	l1 = l + 1;
	if (l < 128)    c = ((l1 << 1) * s) >> 8;
	else            c = (512 - (l1 << 1)) * s >> 8;

	H = h * 6;              // 0 to 1535 (actually 1530)
	lo = H & 255;           // Low byte  = primary/secondary color mix
	h1 = lo + 1;

	if ((H & 256) == 0)   x = h1 * c >> 8;          // even sextant, like red to yellow
	else                  x = (256 - h1) * c >> 8;  // odd sextant, like yellow to green

	m = l - (c >> 1);
	switch(H >> 8) {       // High byte = sextant of colorwheel
	 case 0 : r = c; g = x; b = 0; break; // R to Y
	 case 1 : r = x; g = c; b = 0; break; // Y to G
	 case 2 : r = 0; g = c; b = x; break; // G to C
	 case 3 : r = 0; g = x; b = c; break; // C to B
	 case 4 : r = x; g = 0; b = c; break; // B to M
	 default: r = c; g = 0; b = x; break; // M to R
	}

	return (((uint32_t)r + m) << 16) | (((uint32_t)g + m) << 8) | ((uint32_t)b + m);
}

//Snippets
//	for(uint8_t i = 0; i < LED_COUNT;) {
//			// Calculate color
//			uint32_t rgb_color = hsl_to_rgb(angle + (i * angle_difference), 255, 10);
//			// Set color
//			set_RGB(&l_1,i, (rgb_color >> 16) & 0xFF, (rgb_color >> 8) & 0xFF, rgb_color & 0xFF);
//			set_RGB(&l_2,i, (rgb_color >> 16) & 0xFF, (rgb_color >> 8) & 0xFF, rgb_color & 0xFF);
//			i++;
//		}
//	++angle;
