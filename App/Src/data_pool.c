/**
 * @file    data_pool.c
 * @brief   Sensor data pool and Modbus register mapping
 */

#include "data_pool.h"
#include "config.h"
#include "config_types.h"
#include "config_storage.h"
#include <string.h>

/* Raw 32-bit values: [sensor][dp] */
static int32_t s_values[CFG_MAX_SENSORS][CFG_MAX_DATA_POINTS];

/* Register map: linear array of 16-bit register values */
#define MAX_TOTAL_REGS  (CFG_MAX_SENSORS * CFG_MAX_DATA_POINTS * 2)
static uint16_t s_regs[MAX_TOTAL_REGS];
static uint16_t s_total_regs = 0;

/* Mapping: reg_index -> (sensor_idx, dp_idx, high_word_flag) */
typedef struct {
    uint8_t sensor_idx;
    uint8_t dp_idx;
    uint8_t is_high;   /* 0 = low 16 bits, 1 = high 16 bits */
} RegMap_t;

static RegMap_t s_reg_map[MAX_TOTAL_REGS];

void DataPool_Init(void)
{
    memset(s_values, 0, sizeof(s_values));
    memset(s_regs, 0, sizeof(s_regs));
    DataPool_RebuildMap();
}

void DataPool_SetValue(uint8_t sensor_idx, uint8_t dp_idx, int32_t raw_value)
{
    if (sensor_idx >= CFG_MAX_SENSORS || dp_idx >= CFG_MAX_DATA_POINTS) return;
    s_values[sensor_idx][dp_idx] = raw_value;

    /* Update register map too */
    for (uint16_t i = 0; i < s_total_regs; i++) {
        if (s_reg_map[i].sensor_idx == sensor_idx && s_reg_map[i].dp_idx == dp_idx) {
            uint32_t uval = (uint32_t)raw_value;
            if (s_reg_map[i].is_high) {
                s_regs[i] = (uint16_t)(uval >> 16);
            } else {
                s_regs[i] = (uint16_t)(uval & 0xFFFF);
            }
        }
    }
}

int32_t DataPool_GetValue(uint8_t sensor_idx, uint8_t dp_idx)
{
    if (sensor_idx >= CFG_MAX_SENSORS || dp_idx >= CFG_MAX_DATA_POINTS) return 0;
    return s_values[sensor_idx][dp_idx];
}

float DataPool_GetScaledValue(uint8_t sensor_idx, uint8_t dp_idx)
{
    if (sensor_idx >= CFG_MAX_SENSORS || dp_idx >= CFG_MAX_DATA_POINTS) return 0.0f;
    const SystemConfig_t *cfg = ConfigStorage_GetConfig();
    if (cfg == NULL) return (float)s_values[sensor_idx][dp_idx];
    if (sensor_idx >= cfg->sensor_count) return 0.0f;
    if (dp_idx >= cfg->sensors[sensor_idx].dp_count) return 0.0f;

    uint16_t div = cfg->sensors[sensor_idx].dp[dp_idx].scale_divisor;
    if (div == 0) div = 1;
    return (float)s_values[sensor_idx][dp_idx] / (float)div;
}

uint16_t DataPool_ReadReg(uint16_t reg_addr)
{
    if (reg_addr >= s_total_regs) return 0;
    return s_regs[reg_addr];
}

void DataPool_WriteReg(uint16_t reg_addr, uint16_t value)
{
    if (reg_addr >= s_total_regs) return;
    s_regs[reg_addr] = value;

    /* Write back to raw value */
    if (reg_addr < s_total_regs) {
        uint8_t si = s_reg_map[reg_addr].sensor_idx;
        uint8_t di = s_reg_map[reg_addr].dp_idx;
        uint32_t uval = (uint32_t)s_values[si][di];
        if (s_reg_map[reg_addr].is_high) {
            uval = (uval & 0x0000FFFF) | ((uint32_t)value << 16);
        } else {
            uval = (uval & 0xFFFF0000) | (uint32_t)value;
        }
        s_values[si][di] = (int32_t)uval;
    }
}

uint16_t DataPool_GetTotalRegs(void)
{
    return s_total_regs;
}

void DataPool_RebuildMap(void)
{
    const SystemConfig_t *cfg = ConfigStorage_GetConfig();
    s_total_regs = 0;

    if (cfg == NULL) return;

    for (uint8_t si = 0; si < cfg->sensor_count && si < CFG_MAX_SENSORS; si++) {
        for (uint8_t di = 0; di < cfg->sensors[si].dp_count && di < CFG_MAX_DATA_POINTS; di++) {
            /* Low word */
            if (s_total_regs < MAX_TOTAL_REGS) {
                s_reg_map[s_total_regs].sensor_idx = si;
                s_reg_map[s_total_regs].dp_idx     = di;
                s_reg_map[s_total_regs].is_high     = 0;
                s_regs[s_total_regs] = (uint16_t)(s_values[si][di] & 0xFFFF);
                s_total_regs++;
            }
            /* High word */
            if (s_total_regs < MAX_TOTAL_REGS) {
                s_reg_map[s_total_regs].sensor_idx = si;
                s_reg_map[s_total_regs].dp_idx     = di;
                s_reg_map[s_total_regs].is_high     = 1;
                s_regs[s_total_regs] = (uint16_t)((uint32_t)s_values[si][di] >> 16);
                s_total_regs++;
            }
        }
    }
}
