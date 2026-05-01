/**
 * @file    config.c
 * @brief   Configuration default values and factory function
 */

#include "config.h"
#include "config_types.h"
#include <string.h>

/* Forward declaration */
extern uint32_t Utils_Crc32(uint8_t *data, uint32_t len);

/**
 * @brief  Fill default sensor data point
 */
static void fill_default_dp(DataPoint_t *dp, const char *name,
                            uint16_t reg_addr, uint8_t func_code,
                            uint8_t data_type, uint16_t scale_divisor)
{
    memset(dp, 0, sizeof(DataPoint_t));
    strncpy(dp->name, name, sizeof(dp->name) - 1);
    dp->reg_addr      = reg_addr;
    dp->func_code     = func_code;
    dp->data_type     = data_type;
    dp->scale_divisor = scale_divisor;
}

/**
 * @brief  Get default system configuration
 * @param  cfg: pointer to config struct to fill
 */
void Config_GetDefault(SystemConfig_t *cfg)
{
    memset(cfg, 0, sizeof(SystemConfig_t));

    cfg->magic           = CFG_MAGIC;
    cfg->sensor_count    = CFG_MAX_SENSORS;
    cfg->slave_addr      = CFG_SLAVE_ADDR;
    cfg->poll_interval_ms = CFG_SENSOR_POLL_MS;
    cfg->collect_baud    = CFG_COLLECT_BAUD;
    cfg->forward_baud    = CFG_FORWARD_BAUD;
    cfg->tcp_enable      = 1;
    cfg->web_enable      = 1;

    /* Sensor 0 - smoke detector (slave addr 3) */
    cfg->sensors[0].slave_addr = DEFAULT_SLAVE_ADDR_0;
    cfg->sensors[0].dp_count   = DEFAULT_DP_COUNT;
    fill_default_dp(&cfg->sensors[0].dp[DP_IDX_SMOKE], "smoke", 0x0000, 3, 0, 1);
    fill_default_dp(&cfg->sensors[0].dp[DP_IDX_WATER], "water", 0x0001, 3, 0, 1);
    fill_default_dp(&cfg->sensors[0].dp[DP_IDX_TEMP],  "temp",  0x0002, 3, 0, 10);
    fill_default_dp(&cfg->sensors[0].dp[DP_IDX_HUMI],  "humi",  0x0003, 3, 0, 10);

    /* Sensor 1 - water leak (slave addr 4) */
    cfg->sensors[1].slave_addr = DEFAULT_SLAVE_ADDR_1;
    cfg->sensors[1].dp_count   = DEFAULT_DP_COUNT;
    fill_default_dp(&cfg->sensors[1].dp[DP_IDX_SMOKE], "smoke", 0x0000, 3, 0, 1);
    fill_default_dp(&cfg->sensors[1].dp[DP_IDX_WATER], "water", 0x0001, 3, 0, 1);
    fill_default_dp(&cfg->sensors[1].dp[DP_IDX_TEMP],  "temp",  0x0002, 3, 0, 10);
    fill_default_dp(&cfg->sensors[1].dp[DP_IDX_HUMI],  "humi",  0x0003, 3, 0, 10);

    /* Sensor 2 - temperature (slave addr 5) */
    cfg->sensors[2].slave_addr = DEFAULT_SLAVE_ADDR_2;
    cfg->sensors[2].dp_count   = DEFAULT_DP_COUNT;
    fill_default_dp(&cfg->sensors[2].dp[DP_IDX_SMOKE], "smoke", 0x0000, 3, 0, 1);
    fill_default_dp(&cfg->sensors[2].dp[DP_IDX_WATER], "water", 0x0001, 3, 0, 1);
    fill_default_dp(&cfg->sensors[2].dp[DP_IDX_TEMP],  "temp",  0x0002, 3, 0, 10);
    fill_default_dp(&cfg->sensors[2].dp[DP_IDX_HUMI],  "humi",  0x0003, 3, 0, 10);

    /* Sensor 3 - humidity (slave addr 6) */
    cfg->sensors[3].slave_addr = DEFAULT_SLAVE_ADDR_3;
    cfg->sensors[3].dp_count   = DEFAULT_DP_COUNT;
    fill_default_dp(&cfg->sensors[3].dp[DP_IDX_SMOKE], "smoke", 0x0000, 3, 0, 1);
    fill_default_dp(&cfg->sensors[3].dp[DP_IDX_WATER], "water", 0x0001, 3, 0, 1);
    fill_default_dp(&cfg->sensors[3].dp[DP_IDX_TEMP],  "temp",  0x0002, 3, 0, 10);
    fill_default_dp(&cfg->sensors[3].dp[DP_IDX_HUMI],  "humi",  0x0003, 3, 0, 10);

    /* Default cameras (per spec) */
    strncpy(cfg->cameras[0].ip,   "192.168.100.200", 15);
    strncpy(cfg->cameras[0].user, "admin",            15);
    strncpy(cfg->cameras[0].pass, "anor0825",         15);
    cfg->cameras[0].channel = 1;

    strncpy(cfg->cameras[1].ip,   "192.168.100.201", 15);
    strncpy(cfg->cameras[1].user, "admin",            15);
    strncpy(cfg->cameras[1].pass, "anor0825",         15);
    cfg->cameras[1].channel = 1;

    strncpy(cfg->cameras[2].ip,   "192.168.100.202", 15);
    strncpy(cfg->cameras[2].user, "admin",            15);
    strncpy(cfg->cameras[2].pass, "anor0825",         15);
    cfg->cameras[2].channel = 1;

    strncpy(cfg->cameras[3].ip,   "192.168.100.203", 15);
    strncpy(cfg->cameras[3].user, "admin",            15);
    strncpy(cfg->cameras[3].pass, "anor0825",         15);
    cfg->cameras[3].channel = 1;

    /* Calculate CRC32 (excluding the crc32 field itself) */
    cfg->crc32 = Utils_Crc32((uint8_t *)cfg, sizeof(SystemConfig_t) - 4);
}
