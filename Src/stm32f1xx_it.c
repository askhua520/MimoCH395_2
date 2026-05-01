/**
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines
  */

#include "main.h"
#include "stm32f1xx_it.h"
#include "rs485.h"
#include "ch395_drv.h"

/* External handles */
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/

/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
    while (1) { }
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
    while (1) { }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
    while (1) { }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
    while (1) { }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
    while (1) { }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                     */
/* Add here the Interrupt Handlers for the used peripherals.                   */
/******************************************************************************/

/**
  * @brief This function handles EXTI line0 interrupt (CH395 INT on PB0).
  */
void EXTI0_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_0) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
        CH395_OnInterrupt();
    }
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart2);
}

/**
  * @brief This function handles USART3 global interrupt.
  */
void USART3_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart3);
}

/**
  * @brief This function handles UART4 global interrupt.
  */
void UART4_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart4);
}

/**
  * @brief This function handles UART5 global interrupt.
  */
void UART5_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart5);
}

/******************************************************************************/
/* HAL Callback Functions                                                     */
/******************************************************************************/

/**
  * @brief  HAL UART RX Complete Callback
  *         Called by HAL after receiving 1 byte in IT mode
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    Rs485_UartRxCpltCallback(huart);
}
