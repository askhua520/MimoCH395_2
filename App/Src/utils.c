/**
 * @file    utils.c
 * @brief   Utility functions: CRC, tick, string helpers
 */

#include "utils.h"
#include "stm32f1xx_hal.h"

/* ---- CRC16/Modbus ---- */

uint16_t Utils_Crc16Modbus(uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

/* ---- CRC32 ---- */

uint32_t Utils_Crc32(uint8_t *data, uint32_t len)
{
    uint32_t crc = 0xFFFFFFFF;
    for (uint32_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
    }
    return ~crc;
}

/* ---- Tick ---- */

uint32_t Utils_GetTick(void)
{
    return HAL_GetTick();
}

uint8_t Utils_IsElapsed(uint32_t start, uint32_t ms)
{
    return (HAL_GetTick() - start) >= ms;
}

/* ---- String helpers ---- */

char *Utils_StrAppend(char *dst, const char *src)
{
    while (*src) {
        *dst++ = *src++;
    }
    *dst = '\0';
    return dst;
}

char *Utils_Uint16ToStr(char *dst, uint16_t val)
{
    char tmp[6];
    int i = 0;
    if (val == 0) {
        *dst++ = '0';
        *dst = '\0';
        return dst;
    }
    while (val > 0 && i < 5) {
        tmp[i++] = '0' + (val % 10);
        val /= 10;
    }
    while (i > 0) {
        *dst++ = tmp[--i];
    }
    *dst = '\0';
    return dst;
}

char *Utils_Int16ToStr(char *dst, int16_t val)
{
    if (val < 0) {
        *dst++ = '-';
        val = -val;
    }
    return Utils_Uint16ToStr(dst, (uint16_t)val);
}

char *Utils_FloatToStr(char *dst, float val, uint8_t decimals)
{
    if (val < 0.0f) {
        *dst++ = '-';
        val = -val;
    }
    int32_t int_part = (int32_t)val;
    float frac = val - (float)int_part;

    /* Integer part */
    char tmp[12];
    int i = 0;
    if (int_part == 0) {
        *dst++ = '0';
    } else {
        while (int_part > 0 && i < 10) {
            tmp[i++] = '0' + (int_part % 10);
            int_part /= 10;
        }
        while (i > 0) {
            *dst++ = tmp[--i];
        }
    }

    /* Fractional part */
    if (decimals > 0) {
        *dst++ = '.';
        for (int d = 0; d < decimals; d++) {
            frac *= 10.0f;
            int digit = (int)frac;
            *dst++ = '0' + digit;
            frac -= (float)digit;
        }
    }
    *dst = '\0';
    return dst;
}
