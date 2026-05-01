/**
 * @file    ch395_drv.h
 * @brief   CH395 Ethernet chip driver
 */

#ifndef __CH395_DRV_H
#define __CH395_DRV_H

#include <stdint.h>

/**
 * @brief  Initialize CH395 chip (reset, configure IP)
 */
void CH395_Init(void);

/**
 * @brief  Get MAC address
 * @param  mac: 6-byte buffer
 */
void CH395_GetMacAddr(uint8_t *mac);

/**
 * @brief  Set IP address
 */
void CH395_SetIPAddr(uint8_t *ip);

/**
 * @brief  Set subnet mask
 */
void CH395_SetMask(uint8_t *mask);

/**
 * @brief  Set gateway IP
 */
void CH395_SetGateway(uint8_t *gw);

/**
 * @brief  Open a TCP connection to remote IP:port
 * @param  sock: socket index (0-3)
 * @param  ip: remote IP (4 bytes)
 * @param  port: remote port
 * @return 0 on success, -1 on error
 */
int CH395_TcpConnect(uint8_t sock, uint8_t *ip, uint16_t port);

/**
 * @brief  Listen on a TCP port
 * @param  sock: socket index
 * @param  port: local port
 * @return 0 on success, -1 on error
 */
int CH395_TcpListen(uint8_t sock, uint16_t port);

/**
 * @brief  Send data over TCP
 * @param  sock: socket index
 * @param  data: data buffer
 * @param  len: data length
 * @return 0 on success
 */
int CH395_TcpSend(uint8_t sock, uint8_t *data, uint16_t len);

/**
 * @brief  Receive data from TCP
 * @param  sock: socket index
 * @param  buf: receive buffer
 * @param  max_len: max bytes to read
 * @return number of bytes received, 0 if none, -1 on error
 */
int16_t CH395_TcpRecv(uint8_t sock, uint8_t *buf, uint16_t max_len);

/**
 * @brief  Get socket status
 * @param  sock: socket index
 * @return status byte
 */
uint8_t CH395_GetSocketStatus(uint8_t sock);

/**
 * @brief  Process CH395 events (poll interrupts)
 */
void CH395_Process(void);

/**
 * @brief  Notify CH395 driver of interrupt (call from EXTI ISR)
 */
void CH395_OnInterrupt(void);

#endif /* __CH395_DRV_H */
