/**
 * @file    modbus_slave.c
 * @brief   Modbus RTU slave on USART1 - non-blocking
 */

#include "modbus_slave.h"
#include "config.h"
#include "config_types.h"
#include "config_storage.h"
#include "data_pool.h"
#include "rs485.h"
#include "utils.h"
#include "log_uart.h"

#define SLAVE_RX_BUF_SIZE  256
#define SLAVE_TX_BUF_SIZE  256

static uint8_t  s_rx_buf[SLAVE_RX_BUF_SIZE];
static uint16_t s_rx_len;
static uint8_t  s_tx_buf[SLAVE_TX_BUF_SIZE];

void ModbusSlave_Init(void)
{
    Rs485_FlushRx(CFG_FORWARD_UART_IDX);
}

/**
 * @brief  Build exception response
 */
static uint16_t build_exception(uint8_t *buf, uint8_t slave_addr, uint8_t func_code, uint8_t exception_code)
{
    buf[0] = slave_addr;
    buf[1] = func_code | 0x80;
    buf[2] = exception_code;
    uint16_t crc = Utils_Crc16Modbus(buf, 3);
    buf[3] = (uint8_t)(crc & 0xFF);
    buf[4] = (uint8_t)(crc >> 8);
    return 5;
}

/**
 * @brief  Handle FC 0x03: Read Holding Registers
 */
static uint16_t handle_read_regs(uint8_t *resp, uint8_t slave_addr,
                                  uint16_t start_addr, uint16_t qty)
{
    uint16_t total = DataPool_GetTotalRegs();
    if (start_addr + qty > total) {
        return build_exception(resp, slave_addr, 0x03, 0x02); /* Illegal data address */
    }

    resp[0] = slave_addr;
    resp[1] = 0x03;
    resp[2] = (uint8_t)(qty * 2);   /* Byte count */
    uint16_t pos = 3;

    for (uint16_t i = 0; i < qty; i++) {
        uint16_t val = DataPool_ReadReg(start_addr + i);
        resp[pos++] = (uint8_t)(val >> 8);
        resp[pos++] = (uint8_t)(val & 0xFF);
    }

    uint16_t crc = Utils_Crc16Modbus(resp, pos);
    resp[pos++] = (uint8_t)(crc & 0xFF);
    resp[pos++] = (uint8_t)(crc >> 8);
    return pos;
}

/**
 * @brief  Handle FC 0x06: Write Single Register
 */
static uint16_t handle_write_reg(uint8_t *resp, uint8_t slave_addr,
                                  uint16_t reg_addr, uint16_t value)
{
    uint16_t total = DataPool_GetTotalRegs();
    if (reg_addr >= total) {
        return build_exception(resp, slave_addr, 0x06, 0x02);
    }

    DataPool_WriteReg(reg_addr, value);

    /* Echo back the request as response */
    resp[0] = slave_addr;
    resp[1] = 0x06;
    resp[2] = (uint8_t)(reg_addr >> 8);
    resp[3] = (uint8_t)(reg_addr & 0xFF);
    resp[4] = (uint8_t)(value >> 8);
    resp[5] = (uint8_t)(value & 0xFF);
    uint16_t crc = Utils_Crc16Modbus(resp, 6);
    resp[6] = (uint8_t)(crc & 0xFF);
    resp[7] = (uint8_t)(crc >> 8);
    return 8;
}

void ModbusSlave_Process(void)
{
    /* Check for incoming data */
    Rs485_ReadData(CFG_FORWARD_UART_IDX, s_rx_buf, sizeof(s_rx_buf), &s_rx_len);

    if (s_rx_len < 8) return;   /* Minimum RTU frame: [addr][func][...][crc_lo][crc_hi] */

    const SystemConfig_t *cfg = ConfigStorage_GetConfig();
    if (cfg == NULL) return;

    uint8_t slave_addr = cfg->slave_addr;

    /* Validate CRC */
    uint16_t crc_recv = (uint16_t)s_rx_buf[s_rx_len - 2] |
                        ((uint16_t)s_rx_buf[s_rx_len - 1] << 8);
    uint16_t crc_calc = Utils_Crc16Modbus(s_rx_buf, s_rx_len - 2);
    if (crc_recv != crc_calc) return;

    /* Check address match (0 = broadcast) */
    if (s_rx_buf[0] != slave_addr && s_rx_buf[0] != 0) return;

    uint8_t func = s_rx_buf[1];
    uint16_t resp_len = 0;

    switch (func) {
    case 0x03: {   /* Read Holding Registers */
        uint16_t start = ((uint16_t)s_rx_buf[2] << 8) | s_rx_buf[3];
        uint16_t qty   = ((uint16_t)s_rx_buf[4] << 8) | s_rx_buf[5];
        resp_len = handle_read_regs(s_tx_buf, s_rx_buf[0], start, qty);
        break;
    }
    case 0x06: {   /* Write Single Register */
        uint16_t reg   = ((uint16_t)s_rx_buf[2] << 8) | s_rx_buf[3];
        uint16_t value = ((uint16_t)s_rx_buf[4] << 8) | s_rx_buf[5];
        resp_len = handle_write_reg(s_tx_buf, s_rx_buf[0], reg, value);
        break;
    }
    default:
        resp_len = build_exception(s_tx_buf, s_rx_buf[0], func, 0x01); /* Illegal function */
        break;
    }

    if (resp_len > 0 && s_rx_buf[0] != 0) {   /* Don't respond to broadcast */
        Rs485_SendData(CFG_FORWARD_UART_IDX, s_tx_buf, resp_len);
    }

    Rs485_FlushRx(CFG_FORWARD_UART_IDX);
}
