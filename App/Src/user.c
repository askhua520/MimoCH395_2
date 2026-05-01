/**
 * @file    user.c
 * @brief   Main user entry points (called from main.c USER CODE sections)
 */

#include "user.h"
#include "rs485.h"
#include "modbus_master.h"
#include "modbus_slave.h"
#include "modbus_tcp.h"
#include "isapi_client.h"
#include "web_server.h"
#include "config_storage.h"
#include "data_pool.h"
#include "log_uart.h"
#include "tasks.h"

void User_Setup(void)
{
    Log_Init();
    Log_Printf("System starting...\r\n");

    ConfigStorage_Init();
    Rs485_Init();
    DataPool_Init();

    ModbusMaster_Init();
    ModbusSlave_Init();
    ModbusTcp_Init();
    IsapiClient_Init();
    WebServer_Init();

    Log_Printf("All modules initialized.\r\n");
}

void User_Loop(void)
{
    ModbusMaster_Process();
    ModbusSlave_Process();
    ModbusTcp_Process();
    IsapiClient_Process();
    WebServer_Process();
}
