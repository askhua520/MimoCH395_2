/**
 * @file    stm32f1xx_it.c
 * @brief   Interrupt Service Routines
 *
 * This file shows the USER CODE additions for the CubeMX-generated stm32f1xx_it.c.
 * Add the following handlers into the USER CODE sections.
 */

#include "stm32f1xx_hal.h"
#include "rs485.h"
#include "ch395_drv.h"

/* External UART handles (CubeMX) */
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;

/**
 * @brief  USART1 global interrupt handler
 * @note   Add to USER CODE section in stm32f1xx_it.c
 */
/* USER CODE BEGIN USART1_IRQHandler */
void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}
/* USER CODE END USART1_IRQHandler */

/**
 * @brief  USART2 global interrupt handler
 */
/* USER CODE BEGIN USART2_IRQHandler */
void USART2_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart2);
}
/* USER CODE END USART2_IRQHandler */

/**
 * @brief  USART3 global interrupt handler
 */
/* USER CODE BEGIN USART3_IRQHandler */
void USART3_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart3);
}
/* USER CODE END USART3_IRQHandler */

/**
 * @brief  UART4 global interrupt handler
 */
/* USER CODE BEGIN UART4_IRQHandler */
void UART4_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart4);
}
/* USER CODE END UART4_IRQHandler */

/**
 * @brief  UART5 global interrupt handler
 */
/* USER CODE BEGIN UART5_IRQHandler */
void UART5_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart5);
}
/* USER CODE END UART5_IRQHandler */

/**
 * @brief  HAL UART RX complete callback
 * @note   Add to USER CODE section in stm32f1xx_hal_uart.c or override here
 */
/* USER CODE BEGIN HAL_UART_RxCpltCallback */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    Rs485_UartRxCpltCallback(huart);
}
/* USER CODE END HAL_UART_RxCpltCallback */

/**
 * @brief  EXTI Line0 interrupt handler (CH395 INT on PB0)
 * @note   Add to USER CODE section in stm32f1xx_it.c
 */
/* USER CODE BEGIN EXTI0_IRQHandler */
void EXTI0_IRQHandler(void)
{
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_0) != RESET)
    {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
        CH395_OnInterrupt();
    }
}
/* USER CODE END EXTI0_IRQHandler */
