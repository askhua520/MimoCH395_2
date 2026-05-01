#ifndef __DATA_POOL_H
#define __DATA_POOL_H

#include <stdint.h>

/**
 * @brief  Initialize data pool
 */
void DataPool_Init(void);

/**
 * @brief  Set raw value for a sensor data point
 */
void DataPool_SetValue(uint8_t sensor_idx, uint8_t dp_idx, int32_t raw_value);

/**
 * @brief  Get raw value for a sensor data point
 */
int32_t DataPool_GetValue(uint8_t sensor_idx, uint8_t dp_idx);

/**
 * @brief  Get scaled value (raw / scale_divisor)
 */
float DataPool_GetScaledValue(uint8_t sensor_idx, uint8_t dp_idx);

/**
 * @brief  Read register from linear register space
 */
uint16_t DataPool_ReadReg(uint16_t reg_addr);

/**
 * @brief  Write register in linear register space
 */
void DataPool_WriteReg(uint16_t reg_addr, uint16_t value);

/**
 * @brief  Get total number of registers
 */
uint16_t DataPool_GetTotalRegs(void);

/**
 * @brief  Rebuild register mapping table from config
 */
void DataPool_RebuildMap(void);

#endif /* __DATA_POOL_H */
