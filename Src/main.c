/**
 * @file    main.c
 * @brief   STM32F103RGT6 main program (CubeMX generated template)
 *
 * This file shows the USER CODE sections to add to the CubeMX-generated main.c.
 * CubeMX will generate the initialization code; you only need to add the
 * USER CODE blocks shown below.
 *
 * ===== CubeMX Configuration Checklist =====
 * 1. SYS: Debug = Serial Wire, Timebase = SysTick
 * 2. RCC: HSE = Crystal, LSE = Crystal
 * 3. USART1: Async, 115200 baud (Modbus RTU forward)
 * 4. USART2: Async, 115200 baud
 * 5. USART3: Async, 115200 baud (CH395 Ethernet)
 * 6. UART4:  Async, 115200 baud (Modbus RTU collect)
 * 7. UART5:  Async, 115200 baud (Debug log)
 * 8. I2C1:   Standard mode (EEPROM)
 * 9. GPIO:  PA8, PC9, PA15, PB3 = Output (RS485 DE)
 *           PB0 = Input (CH395 INT, EXTI0 falling edge)
 *           PB1 = Output (CH395 RST)
 * 10. NVIC: Enable USART1-5 global interrupts, EXTI0 interrupt
 */

/* Includes ---------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "user.h"
/* USER CODE END Includes */

/* Private variables ------------------------------------------------*/
/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes --------------------------------------*/
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/**
 * @brief  Main program entry
 * @note   CubeMX-generated init calls happen before User_Setup()
 */
int main(void)
{
    /* MCU Configuration ------------------------------------------------*/
    /* Reset of all peripherals, Initializes the Flash interface and Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();
    MX_USART3_UART_Init();
    MX_UART4_Init();
    MX_UART5_Init();
    MX_I2C1_Init();

    /* USER CODE BEGIN 2 */
    User_Setup();
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
        User_Loop();
    }
    /* USER CODE END 3 */
}
