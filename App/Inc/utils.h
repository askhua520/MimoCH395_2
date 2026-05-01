#ifndef __UTILS_H
#define __UTILS_H

#include <stdint.h>

/**
 * @brief  Calculate CRC16/Modbus
 */
uint16_t Utils_Crc16Modbus(uint8_t *data, uint16_t len);

/**
 * @brief  Calculate CRC32
 */
uint32_t Utils_Crc32(uint8_t *data, uint32_t len);

/**
 * @brief  Get system tick (wraps HAL_GetTick)
 */
uint32_t Utils_GetTick(void);

/**
 * @brief  Non-blocking delay check: returns 1 if 'start + ms' has elapsed
 */
uint8_t Utils_IsElapsed(uint32_t start, uint32_t ms);

/**
 * @brief  Append string to destination, return pointer to end
 */
char *Utils_StrAppend(char *dst, const char *src);

/**
 * @brief  Convert uint16 to decimal string, return pointer to end
 */
char *Utils_Uint16ToStr(char *dst, uint16_t val);

/**
 * @brief  Convert int16 to decimal string, return pointer to end
 */
char *Utils_Int16ToStr(char *dst, int16_t val);

/**
 * @brief  Convert float to string with given decimal places, return pointer to end
 */
char *Utils_FloatToStr(char *dst, float val, uint8_t decimals);

#endif /* __UTILS_H */
