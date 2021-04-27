/*
 * dma_uart_rb.c
 *
 *  Created on: Jun 24, 2020
 *      Author: Jonathan
 */

#include "dma_uart_rb.h"

#define DMA_WRITE_PTR ((RX_BUFF_SIZE - huart4.hdmarx->Instance->CNDTR) & (RX_BUFF_SIZE - 1))

void TX_Init(void) {
	ringbuff_init(&tx_rb, tx_buffer, TX_BUFF_SIZE);
}

static void TX_Data(void) {
	if (tx_len) return; /*If length > 0, DMA transfer is on-going. This function will be called again at transfer completion */
	tx_len = ringbuff_get_linear_block_read_length(&tx_rb); /*Get maximum length of buffer to read data as linear memory */
	if (tx_len) {
		uint8_t *ring_data = ringbuff_get_linear_block_read_address(&tx_rb); /* Get pointer to read memory */
		HAL_UART_Transmit_DMA(&huart4, ring_data, tx_len); /* Start DMA transfer */
	}
}

void TX_Buffer_Add(const void *data, size_t len) {
	ringbuff_write(&tx_rb, data, len);
	TX_Data();
}

void TX_Complete(void) {
	if (tx_len) {
		ringbuff_skip(&tx_rb, tx_len); /* Now skip the data (move read pointer) as they were successfully transferred over DMA */
		tx_len = 0; /* Reset length. Implies DMA is not active */
		TX_Data(); /* Try to send more */
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == LPUART1) {
		CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE)); /* Disable TXEIE and TCIE interrupts */
		huart->gState = HAL_UART_STATE_READY; /* TX process is ended, restore huart->gState to Ready */
		TX_Complete();
	}
}

void RX_Init(void) {
	ringbuff_init(&rx_rb, rx_buffer, RX_BUFF_SIZE);
	rx_last_pos = 0;
	rx_current_pos = 0;
	__HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE); /* Enable idle line interrupt */
	__HAL_DMA_DISABLE_IT(&hdma_lpuart1_rx, DMA_IT_HT); /* Disable UART Half RX interrupt */
	HAL_UART_Receive_DMA(&huart4, rx_buffer, RX_BUFF_SIZE);
}

void RX_Complete(void) {
	uint16_t len;
	rx_current_pos = DMA_WRITE_PTR; /* Get current write pointer */
	len = (rx_current_pos - rx_last_pos + RX_BUFF_SIZE) % RX_BUFF_SIZE; /* Calculate how far the DMA write pointer has moved */
	if (len <= RX_MAX_MSG_SIZE) { /* Check message size */
		ringbuff_advance(&rx_rb, len); /* Move the ring buffer write pointer */
		rx_last_pos = rx_current_pos;
		ringbuff_read(&rx_rb, process_buffer, len); /* read out the data to an array for processing */
	}
	else while (1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == UART4) {
		if (__HAL_UART_GET_FLAG(&huart4, UART_FLAG_IDLE)) {
			RX_Complete();
		}
	}
}
