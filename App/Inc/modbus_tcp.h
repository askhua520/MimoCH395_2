#ifndef __MODBUS_TCP_H
#define __MODBUS_TCP_H

#include <stdint.h>

/**
 * @brief  Initialize Modbus TCP server
 */
void ModbusTcp_Init(void);

/**
 * @brief  Non-blocking Modbus TCP process (call in main loop)
 */
void ModbusTcp_Process(void);

#endif /* __MODBUS_TCP_H */
