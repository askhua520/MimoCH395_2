#ifndef __MODBUS_SLAVE_H
#define __MODBUS_SLAVE_H

#include <stdint.h>

/**
 * @brief  Initialize Modbus RTU slave
 */
void ModbusSlave_Init(void);

/**
 * @brief  Non-blocking Modbus slave process (call in main loop)
 */
void ModbusSlave_Process(void);

#endif /* __MODBUS_SLAVE_H */
