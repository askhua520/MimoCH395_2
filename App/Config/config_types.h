#ifndef __CONFIG_TYPES_H
#define __CONFIG_TYPES_H

#include "config.h"

#pragma pack(push, 1)

/* Data point configuration */
typedef struct {
    char     name[16];          /* Data point name, e.g. "smoke" */
    uint16_t reg_addr;          /* Modbus register address */
    uint8_t  func_code;         /* Modbus function code (3 or 4) */
    uint8_t  data_type;         /* 0=uint16, 1=int16, 2=uint32, 3=int32, 4=float */
    uint16_t scale_divisor;     /* Scale divisor (value / scale_divisor = actual) */
    uint8_t  reserved[2];
} DataPoint_t;

/* Sensor configuration */
typedef struct {
    uint8_t     slave_addr;                     /* Modbus slave address */
    uint8_t     dp_count;                       /* Data point count */
    uint8_t     reserved[2];
    DataPoint_t dp[CFG_MAX_DATA_POINTS];        /* Data point array */
} SensorConfig_t;

/* Camera configuration */
typedef struct {
    char     ip[16];        /* Camera IP address */
    uint8_t  channel;       /* Video channel number */
    char     user[16];      /* Username */
    char     pass[16];      /* Password */
    uint8_t  reserved[3];
} CameraConfig_t;

/* System configuration */
typedef struct {
    uint32_t       magic;                       /* Magic number for validation */
    uint8_t        sensor_count;                /* Number of sensors */
    uint8_t        slave_addr;                  /* This device Modbus slave address */
    uint16_t       poll_interval_ms;            /* Poll interval in ms */
    uint32_t       collect_baud;                /* Collect bus baud rate */
    uint32_t       forward_baud;                /* Forward bus baud rate */
    uint8_t        tcp_enable;                  /* Modbus TCP enable */
    uint8_t        web_enable;                  /* Web server enable */
    uint8_t        reserved[2];
    SensorConfig_t sensors[CFG_MAX_SENSORS];     /* Sensor configs */
    CameraConfig_t cameras[CFG_MAX_CAMERAS];     /* Camera configs */
    uint32_t       crc32;                       /* CRC32 checksum */
} SystemConfig_t;

#pragma pack(pop)

#endif /* __CONFIG_TYPES_H */
