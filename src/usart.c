//
// Created by Kok on 1/30/26.
//

#include "usart.h"

#include "app_state.h"

void HAL_USART_MspInit(USART_HandleTypeDef *husart) {
    GPIO_InitTypeDef GPIO_Config = {
        .Mode = GPIO_MODE_AF_PP,
        .Pull = GPIO_PULLUP,
        .Speed = GPIO_SPEED_LOW
    };

    if (husart->Instance == USART1) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_USART1_CLK_ENABLE();

        GPIO_Config.Alternate = GPIO_AF7_USART1;
        GPIO_Config.Pin = GPIO_PIN_9;

        HAL_GPIO_Init(GPIOA, &GPIO_Config);
    }
}

void HAL_USART_MspDeInit(USART_HandleTypeDef *husart) {
    if (husart->Instance == USART1) {
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);
    }
}

HAL_StatusTypeDef USART_Config() {
    app_state.pUSARTHandle->Instance = USART1;
    app_state.pUSARTHandle->Init = (USART_InitTypeDef) {
        .Mode = USART_MODE_TX_RX,
        .BaudRate = 9600,
        .CLKPhase = USART_PHASE_1EDGE,
        .CLKPolarity = USART_POLARITY_LOW,
        .Parity = USART_PARITY_NONE,
        .CLKLastBit = USART_LASTBIT_DISABLE,
        .StopBits = USART_STOPBITS_1,
        .WordLength = USART_WORDLENGTH_8B
    };
    return HAL_USART_Init(app_state.pUSARTHandle);
}

HAL_StatusTypeDef USART_DeInit() {
    return HAL_USART_DeInit(app_state.pUSARTHandle);
}
