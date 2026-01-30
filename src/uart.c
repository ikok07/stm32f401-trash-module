//
// Created by Kok on 1/30/26.
//

#include "uart.h"

#include "app_config.h"
#include "app_state.h"

void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
    GPIO_InitTypeDef GPIO_Config = {
        .Mode = GPIO_MODE_AF_PP,
        .Pull = GPIO_PULLUP,
        .Speed = GPIO_SPEED_LOW
    };

    if (huart->Instance == USART1) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_USART1_CLK_ENABLE();

        // TX
        GPIO_Config.Alternate = GPIO_AF7_USART1;
        GPIO_Config.Pin = GPIO_PIN_9;

        HAL_GPIO_Init(GPIOA, &GPIO_Config);

        // RX
        GPIO_Config.Pin = GPIO_PIN_10;
        HAL_GPIO_Init(GPIOA, &GPIO_Config);

        HAL_NVIC_EnableIRQ(USART1_IRQn);
        HAL_NVIC_SetPriority(USART1_IRQn, CUSTOM_UART1_INT_PRIORITY, 0);
    }
}

void HAL_USART_MspDeInit(USART_HandleTypeDef *husart) {
    if (husart->Instance == USART1) {
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);
    }
}

HAL_StatusTypeDef UART1_Config() {
    app_state.pUARTHandle->Instance = USART1;
    app_state.pUARTHandle->Init = (UART_InitTypeDef) {
        .Mode = USART_MODE_TX_RX,
        .BaudRate = 9600,
        .Parity = USART_PARITY_NONE,
        .StopBits = USART_STOPBITS_1,
        .WordLength = USART_WORDLENGTH_8B,
        .HwFlowCtl = UART_HWCONTROL_NONE
    };
    return HAL_UART_Init(app_state.pUARTHandle);
}

HAL_StatusTypeDef UART1_DeInit() {
    return HAL_UART_DeInit(app_state.pUARTHandle);
}
