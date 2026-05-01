/**
 * @file    modbus_tcp.c
 * @brief   Modbus TCP server on port 502 via CH395
 */

#include "modbus_tcp.h"
#include "config.h"
#include "config_types.h"
#include "config_storage.h"
#include "data_pool.h"
#include "utils.h"
#include "log_uart.h"
#include "ch395_drv.h"
#include <string.h>

/* MBAP header size */
#define MBAP_HEADER_SIZE   7   /* TransactionID(2) + ProtocolID(2) + Length(2) + UnitID(1) */

/* Sockets for Modbus TCP */
#define MODBUS_TCP_SOCK_BASE  0
#define MODBUS_TCP_SOCK_COUNT CFG_TCP_MAX_CLIENTS

#define TCP_RX_BUF_SIZE  256
#define TCP_TX_BUF_SIZE  256

static uint8_t  s_tcp_rx[TCP_RX_BUF_SIZE];
static uint8_t  s_tcp_tx[TCP_TX_BUF_SIZE];
static uint8_t  s_listen_sock;
static uint8_t  s_client_sock[MODBUS_TCP_SOCK_COUNT];
static uint8_t  s_initialized;

void ModbusTcp_Init(void)
{
    s_listen_sock = MODBUS_TCP_SOCK_BASE;
    memset(s_client_sock, 0xFF, sizeof(s_client_sock));
    s_initialized = 0;
}

/**
 * @brief  Process one Modbus TCP request and build response
 */
static uint16_t process_modbus_pdu(uint8_t *pdu, uint16_t pdu_len, uint8_t *resp)
{
    if (pdu_len < 2) return 0;

    uint8_t func = pdu[0];
    uint16_t resp_len = 0;

    switch (func) {
    case 0x03: {
        if (pdu_len < 5) break;
        uint16_t start = ((uint16_t)pdu[1] << 8) | pdu[2];
        uint16_t qty   = ((uint16_t)pdu[3] << 8) | pdu[4];
        uint16_t total = DataPool_GetTotalRegs();

        if (start + qty > total) {
            resp[0] = func | 0x80;
            resp[1] = 0x02;
            resp_len = 2;
            break;
        }
        resp[0] = func;
        resp[1] = (uint8_t)(qty * 2);
        resp_len = 2;
        for (uint16_t i = 0; i < qty; i++) {
            uint16_t val = DataPool_ReadReg(start + i);
            resp[resp_len++] = (uint8_t)(val >> 8);
            resp[resp_len++] = (uint8_t)(val & 0xFF);
        }
        break;
    }
    case 0x06: {
        if (pdu_len < 5) break;
        uint16_t reg   = ((uint16_t)pdu[1] << 8) | pdu[2];
        uint16_t value = ((uint16_t)pdu[3] << 8) | pdu[4];
        if (reg >= DataPool_GetTotalRegs()) {
            resp[0] = func | 0x80;
            resp[1] = 0x02;
            resp_len = 2;
            break;
        }
        DataPool_WriteReg(reg, value);
        memcpy(resp, pdu, 5);
        resp_len = 5;
        break;
    }
    default:
        resp[0] = func | 0x80;
        resp[1] = 0x01;
        resp_len = 2;
        break;
    }

    return resp_len;
}

void ModbusTcp_Process(void)
{
    /* Try to accept connections */
    if (!s_initialized) {
        /* Try to set up TCP listen on first socket */
        if (CH395_TcpListen(s_listen_sock, CFG_TCP_PORT) == 0) {
            s_initialized = 1;
            Log_Printf("ModbusTCP: Listening on port %d\r\n", CFG_TCP_PORT);
        }
        return;
    }

    /* Check each client socket for data */
    for (int i = 0; i < MODBUS_TCP_SOCK_COUNT; i++) {
        uint8_t sock = MODBUS_TCP_SOCK_BASE + i;
        if (sock == s_listen_sock) continue;

        int16_t rlen = CH395_TcpRecv(sock, s_tcp_rx, sizeof(s_tcp_rx));
        if (rlen <= 0) continue;
        if (rlen < (int16_t)(MBAP_HEADER_SIZE + 2)) continue;   /* Minimum: MBAP + func + 1 byte */

        /* Parse MBAP header */
        uint16_t trans_id  = ((uint16_t)s_tcp_rx[0] << 8) | s_tcp_rx[1];
        uint16_t proto_id  = ((uint16_t)s_tcp_rx[2] << 8) | s_tcp_rx[3];
        uint16_t length    = ((uint16_t)s_tcp_rx[4] << 8) | s_tcp_rx[5];
        uint8_t  unit_id   = s_tcp_rx[6];

        if (proto_id != 0x0000) continue;
        if (length > rlen - 6) continue;

        /* Process PDU */
        uint16_t pdu_len = length - 1;   /* Exclude unit ID */
        uint16_t resp_pdu_len = process_modbus_pdu(&s_tcp_rx[MBAP_HEADER_SIZE], pdu_len, &s_tcp_tx[MBAP_HEADER_SIZE]);

        if (resp_pdu_len > 0) {
            /* Build MBAP header for response */
            s_tcp_tx[0] = (uint8_t)(trans_id >> 8);
            s_tcp_tx[1] = (uint8_t)(trans_id & 0xFF);
            s_tcp_tx[2] = 0x00;
            s_tcp_tx[3] = 0x00;
            uint16_t resp_length = resp_pdu_len + 1;
            s_tcp_tx[4] = (uint8_t)(resp_length >> 8);
            s_tcp_tx[5] = (uint8_t)(resp_length & 0xFF);
            s_tcp_tx[6] = unit_id;

            CH395_TcpSend(sock, s_tcp_tx, MBAP_HEADER_SIZE + resp_pdu_len);
        }
    }
}
