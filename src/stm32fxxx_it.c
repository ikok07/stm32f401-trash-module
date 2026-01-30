//
// Created by Kok on 7/17/25.
//

#include "app_state.h"
#include "stm32f4xx_hal.h"

void SysTick_Handler() {
    HAL_IncTick();
}

void EXTI9_5_IRQHandler() {
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_5) != RESET) {
        HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
    }
}

void USART1_IRQHandler() {
    HAL_UART_IRQHandler(app_state.pUARTHandle);
}

void DMA2_Stream2_IRQHandler() {
    HAL_DMA_IRQHandler(app_state.pDMAUART1RXHandle);
}
