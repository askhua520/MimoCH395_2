#ifndef __CONFIG_STORAGE_H
#define __CONFIG_STORAGE_H

#include <stdint.h>
#include "config_types.h"

/**
 * @brief  Initialize config storage, load or use defaults
 */
void ConfigStorage_Init(void);

/**
 * @brief  Save config to storage
 */
void ConfigStorage_Save(void);

/**
 * @brief  Load config from storage
 * @return 0 on success, -1 on error (CRC mismatch or empty)
 */
int ConfigStorage_Load(void);

/**
 * @brief  Get pointer to current config
 */
const SystemConfig_t *ConfigStorage_GetConfig(void);

/**
 * @brief  Update current config (in-memory)
 */
void ConfigStorage_SetConfig(const SystemConfig_t *cfg);

#endif /* __CONFIG_STORAGE_H */
