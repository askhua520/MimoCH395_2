/**
  * @file    system_stm32f1xx.c
  * @brief   CMSIS Cortex-M3 Device Peripheral Access Layer System Source File
  *
  * This file provides basic system initialization after reset.
  */

#include "stm32f1xx.h"

/* Uncomment this line to enable global clock configuration via SystemInit() */
/* #define VECT_TAB_SRAM */
#define VECT_TAB_OFFSET  0x0U

uint32_t SystemCoreClock = 72000000U;

const uint8_t AHBPrescTable[16U] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t APBPrescTable[8U]  = {0, 0, 0, 0, 1, 2, 3, 4};

/**
  * @brief  Setup the microcontroller system (init FPU, vector table, etc.)
  *         Called before main() by the startup code.
  */
void SystemInit(void)
{
    /* Reset the RCC clock configuration to the default reset state */
    RCC->CR   |= 0x00000001U;   /* Set HSION bit */
    RCC->CFGR &= 0xF8FF0000U;   /* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE, MCO */
    RCC->CR   &= 0xFEF6FFFFU;   /* Reset HSEON, CSSON, PLLON */
    RCC->CR   &= 0xFFFBFFFFU;   /* Reset HSEBYP */
    RCC->CFGR &= 0xFF80FFFFU;   /* Reset PLLSRC, PLLXTPRE, PLLMUL, USBPRE */
    RCC->CIR   = 0x009F0000U;   /* Disable all RCC interrupts */

#ifdef VECT_TAB_SRAM
    SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET;
#else
    SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET;
#endif
}
