/**
 * @file    log_uart.c
 * @brief   Debug logging via UART5
 */

#include "log_uart.h"
#include "config.h"
#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

extern UART_HandleTypeDef huart5;

static char s_log_buf[256];

void Log_Init(void)
{
    /* UART5 is already initialized by CubeMX; nothing extra needed */
}

void Log_Printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(s_log_buf, sizeof(s_log_buf), fmt, ap);
    va_end(ap);

    if (len > 0) {
        HAL_UART_Transmit(&huart5, (uint8_t *)s_log_buf, (uint16_t)len, 1000);
    }
}

/**
 * @brief  Redirect fputc to UART5 for printf support
 */
int fputc(int ch, FILE *f)
{
    (void)f;
    HAL_UART_Transmit(&huart5, (uint8_t *)&ch, 1, 100);
    return ch;
}
