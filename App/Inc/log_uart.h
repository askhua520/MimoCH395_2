#ifndef __LOG_UART_H
#define __LOG_UART_H

#include <stdint.h>

/**
 * @brief  Initialize debug UART (UART5)
 */
void Log_Init(void);

/**
 * @brief  Printf-style debug output
 */
void Log_Printf(const char *fmt, ...);

#endif /* __LOG_UART_H */
