;/**
;  * @file    startup_stm32f103xg.s
;  * @brief   STM32F103RGT6 startup code for Keil MDK (Cortex-M3)
;  *
;  * - Set the initial SP
;  * - Set the initial PC == Reset_Handler
;  * - Set the vector table
;  * - Branch to main in the C library (which eventually calls main())
;  */

                PRESERVE8
                THUMB

; Vector Table Mapped to Address 0 at Reset
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

                IMPORT  |Image$$ARM_LIB_STACK$$ZI$$Limit|

__Vectors       DCD     |Image$$ARM_LIB_STACK$$ZI$$Limit|  ; Top of Stack
                DCD     Reset_Handler                       ; Reset Handler
                DCD     NMI_Handler                         ; NMI Handler
                DCD     HardFault_Handler                   ; Hard Fault Handler
                DCD     MemManage_Handler                   ; MPU Fault Handler
                DCD     BusFault_Handler                    ; Bus Fault Handler
                DCD     UsageFault_Handler                  ; Usage Fault Handler
                DCD     0                                   ; Reserved
                DCD     0                                   ; Reserved
                DCD     0                                   ; Reserved
                DCD     0                                   ; Reserved
                DCD     SVC_Handler                         ; SVCall Handler
                DCD     DebugMon_Handler                    ; Debug Monitor Handler
                DCD     0                                   ; Reserved
                DCD     PendSV_Handler                      ; PendSV Handler
                DCD     SysTick_Handler                     ; SysTick Handler

                ; External Interrupts
                DCD     WWDG_IRQHandler                     ;  0: Window Watchdog
                DCD     PVD_IRQHandler                      ;  1: PVD through EXTI Line detect
                DCD     TAMPER_IRQHandler                   ;  2: Tamper
                DCD     RTC_IRQHandler                      ;  3: RTC
                DCD     FLASH_IRQHandler                    ;  4: Flash
                DCD     RCC_IRQHandler                      ;  5: RCC
                DCD     EXTI0_IRQHandler                    ;  6: EXTI Line 0
                DCD     EXTI1_IRQHandler                    ;  7: EXTI Line 1
                DCD     EXTI2_IRQHandler                    ;  8: EXTI Line 2
                DCD     EXTI3_IRQHandler                    ;  9: EXTI Line 3
                DCD     EXTI4_IRQHandler                    ; 10: EXTI Line 4
                DCD     DMA1_Channel1_IRQHandler            ; 11: DMA1 Channel 1
                DCD     DMA1_Channel2_IRQHandler            ; 12: DMA1 Channel 2
                DCD     DMA1_Channel3_IRQHandler            ; 13: DMA1 Channel 3
                DCD     DMA1_Channel4_IRQHandler            ; 14: DMA1 Channel 4
                DCD     DMA1_Channel5_IRQHandler            ; 15: DMA1 Channel 5
                DCD     DMA1_Channel6_IRQHandler            ; 16: DMA1 Channel 6
                DCD     DMA1_Channel7_IRQHandler            ; 17: DMA1 Channel 7
                DCD     ADC1_2_IRQHandler                   ; 18: ADC1 & ADC2
                DCD     USB_HP_CAN1_TX_IRQHandler           ; 19: USB High Priority or CAN1 TX
                DCD     USB_LP_CAN1_RX0_IRQHandler          ; 20: USB Low  Priority or CAN1 RX0
                DCD     CAN1_RX1_IRQHandler                 ; 21: CAN1 RX1
                DCD     CAN1_SCE_IRQHandler                 ; 22: CAN1 SCE
                DCD     EXTI9_5_IRQHandler                  ; 23: EXTI Line 9..5
                DCD     TIM1_BRK_IRQHandler                 ; 24: TIM1 Break
                DCD     TIM1_UP_IRQHandler                  ; 25: TIM1 Update
                DCD     TIM1_TRG_COM_IRQHandler             ; 26: TIM1 Trigger and Commutation
                DCD     TIM1_CC_IRQHandler                  ; 27: TIM1 Capture Compare
                DCD     TIM2_IRQHandler                     ; 28: TIM2
                DCD     TIM3_IRQHandler                     ; 29: TIM3
                DCD     TIM4_IRQHandler                     ; 30: TIM4
                DCD     I2C1_EV_IRQHandler                  ; 31: I2C1 Event
                DCD     I2C1_ER_IRQHandler                  ; 32: I2C1 Error
                DCD     I2C2_EV_IRQHandler                  ; 33: I2C2 Event
                DCD     I2C2_ER_IRQHandler                  ; 34: I2C2 Error
                DCD     SPI1_IRQHandler                     ; 35: SPI1
                DCD     SPI2_IRQHandler                     ; 36: SPI2
                DCD     USART1_IRQHandler                   ; 37: USART1
                DCD     USART2_IRQHandler                   ; 38: USART2
                DCD     USART3_IRQHandler                   ; 39: USART3
                DCD     EXTI15_10_IRQHandler                ; 40: EXTI Line 15..10
                DCD     RTCAlarm_IRQHandler                 ; 41: RTC Alarm through EXTI Line
                DCD     USBWakeUp_IRQHandler                ; 42: USB Wakeup from suspend
                DCD     TIM8_BRK_IRQHandler                 ; 43: TIM8 Break
                DCD     TIM8_UP_IRQHandler                  ; 44: TIM8 Update
                DCD     TIM8_TRG_COM_IRQHandler             ; 45: TIM8 Trigger and Commutation
                DCD     TIM8_CC_IRQHandler                  ; 46: TIM8 Capture Compare
                DCD     ADC3_IRQHandler                     ; 47: ADC3
                DCD     FSMC_IRQHandler                     ; 48: FSMC
                DCD     SDIO_IRQHandler                     ; 49: SDIO
                DCD     TIM5_IRQHandler                     ; 50: TIM5
                DCD     SPI3_IRQHandler                     ; 51: SPI3
                DCD     UART4_IRQHandler                    ; 52: UART4
                DCD     UART5_IRQHandler                    ; 53: UART5
                DCD     TIM6_IRQHandler                     ; 54: TIM6
                DCD     TIM7_IRQHandler                     ; 55: TIM7
                DCD     DMA2_Channel1_IRQHandler            ; 56: DMA2 Channel 1
                DCD     DMA2_Channel2_IRQHandler            ; 57: DMA2 Channel 2
                DCD     DMA2_Channel3_IRQHandler            ; 58: DMA2 Channel 3
                DCD     DMA2_Channel4_5_IRQHandler          ; 59: DMA2 Channel 4 & 5

__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

; Reset Handler
Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  __main
                IMPORT  SystemInit
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP

; Dummy Exception Handlers (infinite loop which can be modified)
NMI_Handler     PROC
                EXPORT  NMI_Handler               [WEAK]
                B       .
                ENDP
HardFault_Handler PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
MemManage_Handler PROC
                EXPORT  MemManage_Handler         [WEAK]
                B       .
                ENDP
BusFault_Handler PROC
                EXPORT  BusFault_Handler          [WEAK]
                B       .
                ENDP
UsageFault_Handler PROC
                EXPORT  UsageFault_Handler        [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler               [WEAK]
                B       .
                ENDP
DebugMon_Handler PROC
                EXPORT  DebugMon_Handler          [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler           [WEAK]
                B       .
                ENDP

Default_Handler PROC
                EXPORT  WWDG_IRQHandler           [WEAK]
                EXPORT  PVD_IRQHandler            [WEAK]
                EXPORT  TAMPER_IRQHandler         [WEAK]
                EXPORT  RTC_IRQHandler            [WEAK]
                EXPORT  FLASH_IRQHandler          [WEAK]
                EXPORT  RCC_IRQHandler            [WEAK]
                EXPORT  EXTI0_IRQHandler          [WEAK]
                EXPORT  EXTI1_IRQHandler          [WEAK]
                EXPORT  EXTI2_IRQHandler          [WEAK]
                EXPORT  EXTI3_IRQHandler          [WEAK]
                EXPORT  EXTI4_IRQHandler          [WEAK]
                EXPORT  DMA1_Channel1_IRQHandler  [WEAK]
                EXPORT  DMA1_Channel2_IRQHandler  [WEAK]
                EXPORT  DMA1_Channel3_IRQHandler  [WEAK]
                EXPORT  DMA1_Channel4_IRQHandler  [WEAK]
                EXPORT  DMA1_Channel5_IRQHandler  [WEAK]
                EXPORT  DMA1_Channel6_IRQHandler  [WEAK]
                EXPORT  DMA1_Channel7_IRQHandler  [WEAK]
                EXPORT  ADC1_2_IRQHandler         [WEAK]
                EXPORT  USB_HP_CAN1_TX_IRQHandler [WEAK]
                EXPORT  USB_LP_CAN1_RX0_IRQHandler [WEAK]
                EXPORT  CAN1_RX1_IRQHandler       [WEAK]
                EXPORT  CAN1_SCE_IRQHandler       [WEAK]
                EXPORT  EXTI9_5_IRQHandler        [WEAK]
                EXPORT  TIM1_BRK_IRQHandler       [WEAK]
                EXPORT  TIM1_UP_IRQHandler        [WEAK]
                EXPORT  TIM1_TRG_COM_IRQHandler   [WEAK]
                EXPORT  TIM1_CC_IRQHandler        [WEAK]
                EXPORT  TIM2_IRQHandler           [WEAK]
                EXPORT  TIM3_IRQHandler           [WEAK]
                EXPORT  TIM4_IRQHandler           [WEAK]
                EXPORT  I2C1_EV_IRQHandler        [WEAK]
                EXPORT  I2C1_ER_IRQHandler        [WEAK]
                EXPORT  I2C2_EV_IRQHandler        [WEAK]
                EXPORT  I2C2_ER_IRQHandler        [WEAK]
                EXPORT  SPI1_IRQHandler           [WEAK]
                EXPORT  SPI2_IRQHandler           [WEAK]
                EXPORT  USART1_IRQHandler         [WEAK]
                EXPORT  USART2_IRQHandler         [WEAK]
                EXPORT  USART3_IRQHandler         [WEAK]
                EXPORT  EXTI15_10_IRQHandler      [WEAK]
                EXPORT  RTCAlarm_IRQHandler       [WEAK]
                EXPORT  USBWakeUp_IRQHandler      [WEAK]
                EXPORT  TIM8_BRK_IRQHandler       [WEAK]
                EXPORT  TIM8_UP_IRQHandler        [WEAK]
                EXPORT  TIM8_TRG_COM_IRQHandler   [WEAK]
                EXPORT  TIM8_CC_IRQHandler        [WEAK]
                EXPORT  ADC3_IRQHandler           [WEAK]
                EXPORT  FSMC_IRQHandler           [WEAK]
                EXPORT  SDIO_IRQHandler           [WEAK]
                EXPORT  TIM5_IRQHandler           [WEAK]
                EXPORT  SPI3_IRQHandler           [WEAK]
                EXPORT  UART4_IRQHandler          [WEAK]
                EXPORT  UART5_IRQHandler          [WEAK]
                EXPORT  TIM6_IRQHandler           [WEAK]
                EXPORT  TIM7_IRQHandler           [WEAK]
                EXPORT  DMA2_Channel1_IRQHandler  [WEAK]
                EXPORT  DMA2_Channel2_IRQHandler  [WEAK]
                EXPORT  DMA2_Channel3_IRQHandler  [WEAK]
                EXPORT  DMA2_Channel4_5_IRQHandler [WEAK]

WWDG_IRQHandler
PVD_IRQHandler
TAMPER_IRQHandler
RTC_IRQHandler
FLASH_IRQHandler
RCC_IRQHandler
EXTI0_IRQHandler
EXTI1_IRQHandler
EXTI2_IRQHandler
EXTI3_IRQHandler
EXTI4_IRQHandler
DMA1_Channel1_IRQHandler
DMA1_Channel2_IRQHandler
DMA1_Channel3_IRQHandler
DMA1_Channel4_IRQHandler
DMA1_Channel5_IRQHandler
DMA1_Channel6_IRQHandler
DMA1_Channel7_IRQHandler
ADC1_2_IRQHandler
USB_HP_CAN1_TX_IRQHandler
USB_LP_CAN1_RX0_IRQHandler
CAN1_RX1_IRQHandler
CAN1_SCE_IRQHandler
EXTI9_5_IRQHandler
TIM1_BRK_IRQHandler
TIM1_UP_IRQHandler
TIM1_TRG_COM_IRQHandler
TIM1_CC_IRQHandler
TIM2_IRQHandler
TIM3_IRQHandler
TIM4_IRQHandler
I2C1_EV_IRQHandler
I2C1_ER_IRQHandler
I2C2_EV_IRQHandler
I2C2_ER_IRQHandler
SPI1_IRQHandler
SPI2_IRQHandler
USART1_IRQHandler
USART2_IRQHandler
USART3_IRQHandler
EXTI15_10_IRQHandler
RTCAlarm_IRQHandler
USBWakeUp_IRQHandler
TIM8_BRK_IRQHandler
TIM8_UP_IRQHandler
TIM8_TRG_COM_IRQHandler
TIM8_CC_IRQHandler
ADC3_IRQHandler
FSMC_IRQHandler
SDIO_IRQHandler
TIM5_IRQHandler
SPI3_IRQHandler
UART4_IRQHandler
UART5_IRQHandler
TIM6_IRQHandler
TIM7_IRQHandler
DMA2_Channel1_IRQHandler
DMA2_Channel2_IRQHandler
DMA2_Channel3_IRQHandler
DMA2_Channel4_5_IRQHandler

                B       .
                ENDP

                ALIGN
                END
