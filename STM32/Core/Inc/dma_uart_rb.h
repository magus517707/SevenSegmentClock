/*
 * dma_uart_rb.h
 *
 *  Created on: Jun 24, 2020
 *      Author: Jonathan
 */

#ifndef INC_DMA_UART_RB_H_
#define INC_DMA_UART_RB_H_

#include "ringbuff.h"
#include "usart.h"
#include "main.h"

extern UART_HandleTypeDef hlpuart1;
extern UART_HandleTypeDef huart4;
extern DMA_HandleTypeDef hdma_lpuart1_rx;
extern DMA_HandleTypeDef hdma_lpuart1_tx;
extern DMA_HandleTypeDef hdma_uart4_rx;
extern DMA_HandleTypeDef hdma_uart4_tx;

#define TX_BUFF_SIZE 64
#define TX_MAX_MSG_SIZE 32

#define RX_BUFF_SIZE 64
#define RX_MAX_MSG_SIZE 32

uint8_t tx_buffer[TX_BUFF_SIZE];
uint8_t tx_len;
ringbuff_t tx_rb;

uint8_t rx_buffer[RX_BUFF_SIZE];
uint16_t rx_last_pos, rx_current_pos;
ringbuff_t rx_rb;

uint8_t process_buffer[RX_BUFF_SIZE];

void TX_Init();
void RX_Init();
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void TX_Buffer_Add(const void *data, size_t len);

#endif /* INC_DMA_UART_RB_H_ */
