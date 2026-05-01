/**
 * @file    modbus_master.c
 * @brief   Modbus RTU master - non-blocking polling state machine
 */

#include "modbus_master.h"
#include "config.h"
#include "config_types.h"
#include "config_storage.h"
#include "data_pool.h"
#include "rs485.h"
#include "utils.h"
#include "log_uart.h"

/* State machine */
typedef enum {
    MSTATE_IDLE,
    MSTATE_SEND_REQUEST,
    MSTATE_WAIT_RESPONSE,
    MSTATE_PROCESS_RESPONSE,
    MSTATE_NEXT
} MasterState_t;

static MasterState_t s_state;
static uint8_t  s_cur_sensor;
static uint8_t  s_cur_dp;
static uint8_t  s_retry;
static uint32_t s_send_tick;
static uint8_t  s_tx_buf[16];
static uint8_t  s_rx_buf[64];
static uint16_t s_rx_len;
static uint32_t s_last_poll_tick;

/* Build and send a Modbus RTU read request */
static void send_request(void)
{
    const SystemConfig_t *cfg = ConfigStorage_GetConfig();
    if (cfg == NULL) {
        s_state = MSTATE_IDLE;
        return;
    }

    uint8_t slave_addr = cfg->sensors[s_cur_sensor].slave_addr;
    uint8_t func_code  = cfg->sensors[s_cur_sensor].dp[s_cur_dp].func_code;
    uint16_t reg_addr  = cfg->sensors[s_cur_sensor].dp[s_cur_dp].reg_addr;

    /* Build RTU frame: [addr][func][reg_hi][reg_lo][qty_hi][qty_lo][crc_lo][crc_hi] */
    s_tx_buf[0] = slave_addr;
    s_tx_buf[1] = func_code;
    s_tx_buf[2] = (uint8_t)(reg_addr >> 8);
    s_tx_buf[3] = (uint8_t)(reg_addr & 0xFF);
    s_tx_buf[4] = 0x00;   /* Qty high */
    s_tx_buf[5] = 0x02;   /* Qty low = 2 registers (32-bit value) */

    uint16_t crc = Utils_Crc16Modbus(s_tx_buf, 6);
    s_tx_buf[6] = (uint8_t)(crc & 0xFF);
    s_tx_buf[7] = (uint8_t)(crc >> 8);

    Rs485_FlushRx(CFG_COLLECT_UART_IDX);
    Rs485_SendData(CFG_COLLECT_UART_IDX, s_tx_buf, 8);

    s_send_tick = Utils_GetTick();
    s_state = MSTATE_WAIT_RESPONSE;
}

/* Process received response */
static void process_response(void)
{
    /* Minimum RTU response: [addr][func][bytecount][data...][crc_lo][crc_hi] */
    if (s_rx_len < 5) {
        s_state = MSTATE_NEXT;
        return;
    }

    /* Validate CRC */
    uint16_t crc_recv = (uint16_t)s_rx_buf[s_rx_len - 2] |
                        ((uint16_t)s_rx_buf[s_rx_len - 1] << 8);
    uint16_t crc_calc = Utils_Crc16Modbus(s_rx_buf, s_rx_len - 2);
    if (crc_recv != crc_calc) {
        Log_Printf("MB_M: CRC error\r\n");
        s_state = MSTATE_NEXT;
        return;
    }

    /* Check for exception response */
    if (s_rx_buf[1] & 0x80) {
        Log_Printf("MB_M: Exception %02X\r\n", s_rx_buf[2]);
        s_state = MSTATE_NEXT;
        return;
    }

    /* Extract value (function 0x03/0x04 response) */
    if ((s_rx_buf[1] == 0x03 || s_rx_buf[1] == 0x04) && s_rx_buf[2] >= 4) {
        uint32_t raw = ((uint32_t)s_rx_buf[3] << 24) |
                       ((uint32_t)s_rx_buf[4] << 16) |
                       ((uint32_t)s_rx_buf[5] << 8)  |
                       ((uint32_t)s_rx_buf[6]);
        DataPool_SetValue(s_cur_sensor, s_cur_dp, (int32_t)raw);
    }

    s_state = MSTATE_NEXT;
}

void ModbusMaster_Init(void)
{
    s_state = MSTATE_IDLE;
    s_cur_sensor = 0;
    s_cur_dp = 0;
    s_retry = 0;
    s_last_poll_tick = 0;
}

void ModbusMaster_Process(void)
{
    const SystemConfig_t *cfg = ConfigStorage_GetConfig();
    if (cfg == NULL) return;

    switch (s_state) {
    case MSTATE_IDLE:
        /* Wait for poll interval */
        if (cfg->sensor_count == 0) break;
        if (Utils_IsElapsed(s_last_poll_tick, cfg->poll_interval_ms)) {
            s_cur_sensor = 0;
            s_cur_dp = 0;
            s_retry = 0;
            s_state = MSTATE_SEND_REQUEST;
            s_last_poll_tick = Utils_GetTick();
        }
        break;

    case MSTATE_SEND_REQUEST:
        if (s_cur_sensor >= cfg->sensor_count) {
            s_state = MSTATE_IDLE;
            break;
        }
        if (s_cur_dp >= cfg->sensors[s_cur_sensor].dp_count) {
            s_state = MSTATE_NEXT;
            break;
        }
        send_request();
        break;

    case MSTATE_WAIT_RESPONSE:
        Rs485_ReadData(CFG_COLLECT_UART_IDX, s_rx_buf, sizeof(s_rx_buf), &s_rx_len);
        if (s_rx_len > 0) {
            s_state = MSTATE_PROCESS_RESPONSE;
        } else if (Utils_IsElapsed(s_send_tick, CFG_MODBUS_TIMEOUT_MS)) {
            /* Timeout */
            s_retry++;
            if (s_retry >= CFG_MODBUS_MAX_RETRY) {
                Log_Printf("MB_M: Timeout sensor%d dp%d\r\n", s_cur_sensor, s_cur_dp);
                s_state = MSTATE_NEXT;
            } else {
                s_state = MSTATE_SEND_REQUEST;   /* Retry */
            }
        }
        break;

    case MSTATE_PROCESS_RESPONSE:
        process_response();
        break;

    case MSTATE_NEXT:
        /* Move to next data point */
        s_cur_dp++;
        s_retry = 0;
        if (s_cur_dp >= cfg->sensors[s_cur_sensor].dp_count) {
            s_cur_dp = 0;
            s_cur_sensor++;
            if (s_cur_sensor >= cfg->sensor_count) {
                s_state = MSTATE_IDLE;
            } else {
                s_state = MSTATE_SEND_REQUEST;
            }
        } else {
            s_state = MSTATE_SEND_REQUEST;
        }
        break;
    }
}
