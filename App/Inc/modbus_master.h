#ifndef __MODBUS_MASTER_H
#define __MODBUS_MASTER_H

#include <stdint.h>

/**
 * @brief  Initialize Modbus master
 */
void ModbusMaster_Init(void);

/**
 * @brief  Non-blocking Modbus master polling process (call in main loop)
 */
void ModbusMaster_Process(void);

#endif /* __MODBUS_MASTER_H */
