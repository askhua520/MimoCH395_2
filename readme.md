# Modbus Gateway - STM32F103RGT6

## Overview

A Modbus gateway device based on STM32F103RGT6 (Cortex-M3, 72MHz) that bridges:
- **Modbus RTU Master** → polls sensors on USART4 (collect bus)
- **Modbus RTU Slave** → responds on USART1 (forward bus)
- **Modbus TCP Server** → serves register data via CH395 Ethernet (port 502)
- **ISAPI OSD** → pushes sensor data to IP cameras via HTTP
- **Web Server** → configuration and status page on port 80

## Architecture

```
Sensors (RS485) ──→ USART4 ──→ Modbus Master ──→ DataPool
                                                    │
                                            ┌───────┼───────┐
                                            ↓       ↓       ↓
                                     RTU Slave  TCP Server  ISAPI
                                     (USART1)   (CH395)    (CH395)
                                     port 502    port 80    camera
```

## Pin Assignment

| Function       | Pin      | Description                    |
|---------------|----------|--------------------------------|
| USART1 TX     | PA9      | Modbus RTU forward bus         |
| USART1 RX     | PA10     |                                |
| USART1 DE     | PA8      | RS485 direction control        |
| USART2 TX     | PA2      | Reserved                       |
| USART2 RX     | PA3      |                                |
| USART2 DE     | PC9      | RS485 direction control        |
| USART3 TX     | PB10     | CH395 Ethernet                 |
| USART3 RX     | PB11     |                                |
| UART4 TX      | PC10     | Modbus RTU collect bus         |
| UART4 RX      | PC11     |                                |
| UART4 DE      | PA15     | RS485 direction control        |
| UART5 TX      | PC12     | Debug log output               |
| UART5 RX      | PD2      |                                |
| UART5 DE      | PB3      | RS485 direction control        |
| I2C1 SCL      | PB6      | EEPROM (AT24C256)              |
| I2C1 SDA      | PB7      |                                |
| CH395 INT     | PB0      | EXTI0, falling edge            |
| CH395 RST     | PB1      | Output, active low             |

## CubeMX Configuration

1. Create new project for STM32F103RGTx
2. Configure peripherals as shown in pin table
3. Enable all USART global interrupts in NVIC
4. Enable EXTI0 interrupt for CH395
5. Set system clock to 72MHz (HSE + PLL)
6. Generate code with HAL library
7. Copy `App/` directory into project root
8. Add `USER CODE` sections from `Src/main.c` and `Src/stm32f1xx_it.c`
9. Add all `App/` source files to Keil project

## Building

### Requirements
- Keil MDK v5.x
- STM32F1xx HAL library (included by CubeMX)
- STM32F1xx DFP (Device Family Pack)

### Steps
1. Generate code with CubeMX
2. Open `.uvprojx` in Keil
3. Add `App/Inc`, `App/Config`, `App/CH395`, `App/Web` to include paths
4. Add all `.c` files from `App/Src/`, `App/Config/`, `App/CH395/`, `App/Web/` to project
5. Build (F7)

## File Structure

```
ModbusGateway/
├── App/
│   ├── Config/
│   │   ├── config.h          # System macros
│   │   ├── config_types.h    # Struct definitions
│   │   └── config.c          # Default config factory
│   ├── Inc/
│   │   ├── user.h
│   │   ├── rs485.h
│   │   ├── utils.h
│   │   ├── log_uart.h
│   │   ├── data_pool.h
│   │   ├── modbus_master.h
│   │   ├── modbus_slave.h
│   │   ├── modbus_tcp.h
│   │   ├── isapi_client.h
│   │   ├── web_server.h
│   │   ├── config_storage.h
│   │   └── tasks.h
│   ├── Src/
│   │   ├── user.c
│   │   ├── rs485.c
│   │   ├── utils.c
│   │   ├── log_uart.c
│   │   ├── data_pool.c
│   │   ├── modbus_master.c
│   │   ├── modbus_slave.c
│   │   ├── modbus_tcp.c
│   │   ├── isapi_client.c
│   │   ├── web_server.c
│   │   ├── config_storage.c
│   │   └── tasks.c
│   ├── CH395/
│   │   ├── ch395_inc.h       # CH395 command definitions
│   │   ├── ch395_drv.h
│   │   └── ch395_drv.c       # CH395 UART driver
│   └── Web/
│       ├── index.html         # Web configuration page
│       ├── web_resources.h
│       └── web_resources.c    # Embedded HTML
├── Drivers/                   # CubeMX HAL drivers
├── Inc/                       # CubeMX generated headers
├── Src/                       # CubeMX generated sources
│   ├── main.c
│   └── stm32f1xx_it.c
├── MDK-ARM/                   # Keil project files
└── STM32F103RGTx_FLASH.ld     # Linker script (GCC reference)
```

## Design Notes

- **Cooperative multitasking**: All modules use non-blocking state machines
- **No RTOS required**: Bare-metal with HAL_GetTick() timing
- **Register mapping**: Each data point maps to 2 Modbus registers (32-bit)
- **Config storage**: EEPROM (AT24C256) primary, internal Flash fallback
- **CH395 communication**: Via USART3 at 115200 baud
- **All identifiers are pure ASCII** (comments may contain Chinese)
