/**
 * @file    tasks.c
 * @brief   Bare-metal cooperative task scheduler
 */

#include "tasks.h"
#include "modbus_master.h"
#include "modbus_slave.h"
#include "modbus_tcp.h"
#include "isapi_client.h"
#include "web_server.h"
#include "log_uart.h"

void Tasks_Init(void)
{
    /* All modules initialized in User_Setup */
}

void Tasks_Process(void)
{
    /* Round-robin processing of all non-blocking state machines */
    ModbusMaster_Process();
    ModbusSlave_Process();
    ModbusTcp_Process();
    IsapiClient_Process();
    WebServer_Process();
}
