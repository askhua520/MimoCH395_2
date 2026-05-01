/**
 * @file    config_storage.c
 * @brief   Configuration storage with EEPROM and Flash support
 */

#include "config_storage.h"
#include "config.h"
#include "config_types.h"
#include "utils.h"
#include "log_uart.h"
#include "stm32f1xx_hal.h"
#include <string.h>

/* External I2C handle (CubeMX) */
extern I2C_HandleTypeDef hi2c1;

static SystemConfig_t s_config;
static uint8_t s_eeprom_ok;   /* 1 if EEPROM detected */

/**
 * @brief  Check if EEPROM is present
 */
static uint8_t eeprom_probe(void)
{
    return (HAL_I2C_IsDeviceReady(&hi2c1, CFG_EEPROM_ADDR, 3, 100) == HAL_OK) ? 1 : 0;
}

/**
 * @brief  Write data to EEPROM
 */
static HAL_StatusTypeDef eeprom_write(uint16_t addr, uint8_t *data, uint16_t len)
{
    /* AT24C256 page write: max 64 bytes per page */
    uint16_t written = 0;
    while (written < len) {
        uint16_t page_remain = 64 - (addr % 64);
        uint16_t chunk = len - written;
        if (chunk > page_remain) chunk = page_remain;

        uint8_t buf[2 + 64];
        buf[0] = (uint8_t)(addr >> 8);
        buf[1] = (uint8_t)(addr & 0xFF);
        memcpy(&buf[2], data + written, chunk);

        if (HAL_I2C_Master_Transmit(&hi2c1, CFG_EEPROM_ADDR, buf, chunk + 2, 100) != HAL_OK) {
            return HAL_ERROR;
        }
        HAL_Delay(10);   /* Wait for EEPROM write cycle */

        addr += chunk;
        written += chunk;
    }
    return HAL_OK;
}

/**
 * @brief  Read data from EEPROM
 */
static HAL_StatusTypeDef eeprom_read(uint16_t addr, uint8_t *data, uint16_t len)
{
    uint8_t addr_buf[2];
    addr_buf[0] = (uint8_t)(addr >> 8);
    addr_buf[1] = (uint8_t)(addr & 0xFF);

    if (HAL_I2C_Master_Transmit(&hi2c1, CFG_EEPROM_ADDR, addr_buf, 2, 100) != HAL_OK) {
        return HAL_ERROR;
    }
    return HAL_I2C_Master_Receive(&hi2c1, CFG_EEPROM_ADDR, data, len, 100);
}

/**
 * @brief  Save config to internal Flash
 */
static void flash_save(const SystemConfig_t *cfg)
{
    HAL_FLASH_Unlock();

    /* Erase page */
    FLASH_EraseInitTypeDef erase;
    uint32_t error;
    erase.TypeErase   = FLASH_TYPEERASE_PAGES;
    erase.PageAddress = CFG_CONFIG_FLASH_ADDR;
    erase.NbPages     = 1;
    HAL_FLASHEx_Erase(&erase, &error);

    /* Write config word by word */
    const uint32_t *src = (const uint32_t *)cfg;
    uint32_t addr = CFG_CONFIG_FLASH_ADDR;
    uint16_t words = (sizeof(SystemConfig_t) + 3) / 4;

    for (uint16_t i = 0; i < words; i++) {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, src[i]);
        addr += 4;
    }

    HAL_FLASH_Lock();
}

/**
 * @brief  Load config from internal Flash
 */
static int flash_load(SystemConfig_t *cfg)
{
    const SystemConfig_t *flash_cfg = (const SystemConfig_t *)CFG_CONFIG_FLASH_ADDR;
    if (flash_cfg->magic != CFG_MAGIC) return -1;

    uint32_t crc_calc = Utils_Crc32((uint8_t *)flash_cfg, sizeof(SystemConfig_t) - 4);
    if (crc_calc != flash_cfg->crc32) return -1;

    memcpy(cfg, flash_cfg, sizeof(SystemConfig_t));
    return 0;
}

void ConfigStorage_Init(void)
{
    s_eeprom_ok = eeprom_probe();

    if (ConfigStorage_Load() != 0) {
        Log_Printf("Config: Using defaults\r\n");
        Config_GetDefault(&s_config);
        ConfigStorage_Save();
    } else {
        Log_Printf("Config: Loaded from storage\r\n");
    }
}

void ConfigStorage_Save(void)
{
    /* Update CRC */
    s_config.crc32 = Utils_Crc32((uint8_t *)&s_config, sizeof(SystemConfig_t) - 4);

    if (s_eeprom_ok) {
        if (eeprom_write(0, (uint8_t *)&s_config, sizeof(SystemConfig_t)) == HAL_OK) {
            Log_Printf("Config: Saved to EEPROM\r\n");
            return;
        }
        Log_Printf("Config: EEPROM write failed, using Flash\r\n");
    }

    flash_save(&s_config);
    Log_Printf("Config: Saved to Flash\r\n");
}

int ConfigStorage_Load(void)
{
    /* Try EEPROM first */
    if (s_eeprom_ok) {
        SystemConfig_t tmp;
        if (eeprom_read(0, (uint8_t *)&tmp, sizeof(SystemConfig_t)) == HAL_OK) {
            if (tmp.magic == CFG_MAGIC) {
                uint32_t crc = Utils_Crc32((uint8_t *)&tmp, sizeof(SystemConfig_t) - 4);
                if (crc == tmp.crc32) {
                    memcpy(&s_config, &tmp, sizeof(SystemConfig_t));
                    return 0;
                }
            }
        }
    }

    /* Try Flash */
    return flash_load(&s_config);
}

const SystemConfig_t *ConfigStorage_GetConfig(void)
{
    return &s_config;
}

void ConfigStorage_SetConfig(const SystemConfig_t *cfg)
{
    memcpy(&s_config, cfg, sizeof(SystemConfig_t));
}
