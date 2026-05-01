#ifndef __RS485_H
#define __RS485_H

#include <stdint.h>

#define RS485_RING_BUF_SIZE  256

/**
 * @brief  Initialize all RS485 UART peripherals
 */
void Rs485_Init(void);

/**
 * @brief  Send data on specified UART with DE pin control
 * @param  uart_idx: UART index (UART_IDX_1 .. UART_IDX_5)
 * @param  data: pointer to data buffer
 * @param  len: data length
 */
void Rs485_SendData(uint8_t uart_idx, uint8_t *data, uint16_t len);

/**
 * @brief  Set RS485 direction
 * @param  uart_idx: UART index
 * @param  tx_en: 1=TX enable, 0=RX enable
 */
void Rs485_SetDir(uint8_t uart_idx, uint8_t tx_en);

/**
 * @brief  Read data from ring buffer
 * @param  uart_idx: UART index
 * @param  buf: destination buffer
 * @param  max_len: max bytes to read
 * @param  actual_len: actual bytes read
 */
void Rs485_ReadData(uint8_t uart_idx, uint8_t *buf, uint16_t max_len, uint16_t *actual_len);

/**
 * @brief  Flush receive ring buffer
 * @param  uart_idx: UART index
 */
void Rs485_FlushRx(uint8_t uart_idx);

/**
 * @brief  UART RX complete callback (called from ISR)
 * @param  huart: UART handle
 */
void Rs485_UartRxCpltCallback(void *huart);

#endif /* __RS485_H */
