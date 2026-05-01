#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdint.h>

/* Sensor config */
#define CFG_MAX_SENSORS             4
#define CFG_MAX_DATA_POINTS         8
#define CFG_SENSOR_POLL_MS          4000
#define CFG_OSD_UPDATE_MIN_MS       500
#define CFG_MODBUS_TIMEOUT_MS       1000
#define CFG_MODBUS_MAX_RETRY        3

/* Collect bus (USART4) */
#define CFG_COLLECT_UART_IDX        3       /* USART4 index */
#define CFG_COLLECT_BAUD            115200

/* RTU slave forward bus (USART1) */
#define CFG_FORWARD_UART_IDX        0       /* USART1 index */
#define CFG_FORWARD_BAUD            115200
#define CFG_SLAVE_ADDR              1

/* Modbus TCP */
#define CFG_TCP_PORT                502
#define CFG_TCP_MAX_CLIENTS         2

/* Web server */
#define CFG_WEB_PORT                80

/* Camera */
#define CFG_MAX_CAMERAS             CFG_MAX_SENSORS

/* Storage */
#define CFG_EEPROM_ADDR             0xA0
#define CFG_CONFIG_FLASH_ADDR       0x0801F800
#define CFG_MAGIC                   0x434F4E46

/* UART index definitions */
#define UART_IDX_1      0
#define UART_IDX_2      1
#define UART_IDX_3      2
#define UART_IDX_4      3
#define UART_IDX_5      4
#define UART_IDX_COUNT  5

/* Default data points */
#define DEFAULT_DP_COUNT    4
#define DP_IDX_SMOKE    0
#define DP_IDX_WATER    1
#define DP_IDX_TEMP     2
#define DP_IDX_HUMI     3

#define DEFAULT_SLAVE_ADDR_0    3
#define DEFAULT_SLAVE_ADDR_1    4
#define DEFAULT_SLAVE_ADDR_2    5
#define DEFAULT_SLAVE_ADDR_3    6

#endif /* __CONFIG_H */
