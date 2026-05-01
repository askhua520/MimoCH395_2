/**
 * @file    ch395_drv.c
 * @brief   CH395 Ethernet chip driver via USART3
 */

#include "ch395_drv.h"
#include "ch395_inc.h"
#include "config.h"
#include "log_uart.h"
#include "stm32f1xx_hal.h"
#include <string.h>

/* External UART handle (USART3) */
extern UART_HandleTypeDef huart3;

/* CH395 pins */
#define CH395_RST_PORT   GPIOB
#define CH395_RST_PIN    GPIO_PIN_1
#define CH395_INT_PORT   GPIOB
#define CH395_INT_PIN    GPIO_PIN_0

#define CH395_UART_TIMEOUT  500

static volatile uint8_t s_int_flag;
static uint8_t s_rx_byte;

/* ---- Low-level UART ---- */

static void ch395_send_cmd(uint8_t cmd)
{
    uint8_t buf[1];
    buf[0] = cmd;
    HAL_UART_Transmit(&huart3, buf, 1, CH395_UART_TIMEOUT);
}

static void ch395_send_data(const uint8_t *data, uint16_t len)
{
    HAL_UART_Transmit(&huart3, (uint8_t *)data, len, CH395_UART_TIMEOUT);
}

static void ch395_recv_data(uint8_t *data, uint16_t len)
{
    HAL_UART_Receive(&huart3, data, len, CH395_UART_TIMEOUT);
}

static uint8_t ch395_send_cmd_recv_byte(uint8_t cmd)
{
    ch395_send_cmd(cmd);
    uint8_t val = 0;
    HAL_Delay(1);
    ch395_recv_data(&val, 1);
    return val;
}

/* ---- Reset ---- */

static void ch395_reset(void)
{
    HAL_GPIO_WritePin(CH395_RST_PORT, CH395_RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(50);
    HAL_GPIO_WritePin(CH395_RST_PORT, CH395_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(100);
}

/* ---- Init ---- */

void CH395_Init(void)
{
    s_int_flag = 0;

    ch395_reset();

    /* Check chip existence */
    uint8_t ver = ch395_send_cmd_recv_byte(CMD_CHECK_EXIST);
    (void)ver;   /* Expected to return complement of sent byte */

    /* Set MAC address (default) */
    {
        uint8_t mac[6] = {0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
        ch395_send_cmd(CMD_SET_MAC_ADDR);
        ch395_send_data(mac, 6);
        HAL_Delay(2);
    }

    /* Set default IP: 192.168.1.100 */
    {
        uint8_t ip[4] = {192, 168, 1, 100};
        CH395_SetIPAddr(ip);
    }

    /* Set default mask: 255.255.255.0 */
    {
        uint8_t mask[4] = {255, 255, 255, 0};
        CH395_SetMask(mask);
    }

    /* Set default gateway: 192.168.1.1 */
    {
        uint8_t gw[4] = {192, 168, 1, 1};
        CH395_SetGateway(gw);
    }

    /* Initialize sockets to TCP mode */
    for (uint8_t i = 0; i < 4; i++) {
        ch395_send_cmd(CMD_SET_PROTO_TYPE);
        uint8_t data[2] = {i, PROTO_TYPE_TCP};
        ch395_send_data(data, 2);
        HAL_Delay(2);

        ch395_send_cmd(CMD_OPEN_SOCKET);
        ch395_send_data(&i, 1);
        HAL_Delay(5);
    }

    /* Start RX IT on USART3 for interrupt-driven communication */
    HAL_UART_Receive_IT(&huart3, &s_rx_byte, 1);

    Log_Printf("CH395: Initialized\r\n");
}

/* ---- Configuration ---- */

void CH395_GetMacAddr(uint8_t *mac)
{
    /* CH395 doesn't have a direct read-MAC command in SPI/UART mode;
       return the stored MAC (simplified) */
    mac[0] = 0x02; mac[1] = 0x03; mac[2] = 0x04;
    mac[3] = 0x05; mac[4] = 0x06; mac[5] = 0x07;
}

void CH395_SetIPAddr(uint8_t *ip)
{
    ch395_send_cmd(CMD_SET_IP_ADDR);
    ch395_send_data(ip, 4);
    HAL_Delay(2);
}

void CH395_SetMask(uint8_t *mask)
{
    ch395_send_cmd(CMD_SET_MASK_ADDR);
    ch395_send_data(mask, 4);
    HAL_Delay(2);
}

void CH395_SetGateway(uint8_t *gw)
{
    ch395_send_cmd(CMD_SET_GWIP_ADDR);
    ch395_send_data(gw, 4);
    HAL_Delay(2);
}

/* ---- TCP ---- */

int CH395_TcpConnect(uint8_t sock, uint8_t *ip, uint16_t port)
{
    if (sock >= 4) return -1;

    /* Set destination IP */
    ch395_send_cmd(CMD_SET_SOCKET_DEST_IP);
    uint8_t buf_ip[5] = {sock, ip[0], ip[1], ip[2], ip[3]};
    ch395_send_data(buf_ip, 5);
    HAL_Delay(2);

    /* Set destination port */
    ch395_send_cmd(CMD_SET_SOCKET_DEST_PORT);
    uint8_t buf_port[3];
    buf_port[0] = sock;
    buf_port[1] = (uint8_t)(port >> 8);
    buf_port[2] = (uint8_t)(port & 0xFF);
    ch395_send_data(buf_port, 3);
    HAL_Delay(2);

    /* Connect */
    ch395_send_cmd(CMD_TCP_CONNECT);
    ch395_send_data(&sock, 1);
    HAL_Delay(5);

    return 0;
}

int CH395_TcpListen(uint8_t sock, uint16_t port)
{
    if (sock >= 4) return -1;

    /* Set source port */
    ch395_send_cmd(CMD_SET_SOCKET_SOUR_PORT);
    uint8_t buf[3];
    buf[0] = sock;
    buf[1] = (uint8_t)(port >> 8);
    buf[2] = (uint8_t)(port & 0xFF);
    ch395_send_data(buf, 3);
    HAL_Delay(2);

    /* Open socket */
    ch395_send_cmd(CMD_OPEN_SOCKET);
    ch395_send_data(&sock, 1);
    HAL_Delay(5);

    /* Listen */
    ch395_send_cmd(CMD_TCP_LISTEN);
    ch395_send_data(&sock, 1);
    HAL_Delay(5);

    return 0;
}

int CH395_TcpSend(uint8_t sock, uint8_t *data, uint16_t len)
{
    if (sock >= 4) return -1;

    ch395_send_cmd(CMD_TCP_SEND);
    uint8_t hdr[3];
    hdr[0] = sock;
    hdr[1] = (uint8_t)(len >> 8);
    hdr[2] = (uint8_t)(len & 0xFF);
    ch395_send_data(hdr, 3);
    ch395_send_data(data, len);
    HAL_Delay(2);

    return 0;
}

int16_t CH395_TcpRecv(uint8_t sock, uint8_t *buf, uint16_t max_len)
{
    if (sock >= 4) return -1;

    /* Get received data length */
    ch395_send_cmd(CMD_GET_RECV_LEN);
    ch395_send_data(&sock, 1);
    HAL_Delay(1);

    uint8_t len_buf[2];
    ch395_recv_data(len_buf, 2);
    uint16_t avail = ((uint16_t)len_buf[0] << 8) | len_buf[1];

    if (avail == 0) return 0;

    uint16_t to_read = (avail < max_len) ? avail : max_len;

    /* Read data */
    ch395_send_cmd(CMD_GET_RECV_DATA);
    uint8_t req[3];
    req[0] = sock;
    req[1] = (uint8_t)(to_read >> 8);
    req[2] = (uint8_t)(to_read & 0xFF);
    ch395_send_data(req, 3);
    HAL_Delay(1);

    ch395_recv_data(buf, to_read);

    return (int16_t)to_read;
}

uint8_t CH395_GetSocketStatus(uint8_t sock)
{
    if (sock >= 4) return 0xFF;
    ch395_send_cmd(CMD_GET_SOCKET_STATUS);
    ch395_send_data(&sock, 1);
    HAL_Delay(1);
    uint8_t status = 0;
    ch395_recv_data(&status, 1);
    return status;
}

void CH395_Process(void)
{
    if (!s_int_flag) return;
    s_int_flag = 0;

    /* Read global interrupt flag */
    uint8_t gint = ch395_send_cmd_recv_byte(CMD_GET_GLOBAL_INT);

    /* Handle socket events (placeholder) */
    (void)gint;
}

void CH395_OnInterrupt(void)
{
    s_int_flag = 1;
}
