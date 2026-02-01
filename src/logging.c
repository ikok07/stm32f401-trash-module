//
// Created by Kok on 2/1/26.
//

#include "logging.h"

#include <string.h>

#include "app_state.h"
#include "log.h"

void LOGGER_InitBasicCB() {
    // Enable clock access
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Configure Error LED
    GPIO_InitTypeDef GPIO_Config = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pin = ERR_LED_PIN,
        .Pull = GPIO_PULLDOWN,
        .Speed = GPIO_SPEED_LOW
    };
    HAL_GPIO_Init(ERR_LED_GPIO, &GPIO_Config);
}

void LOGGER_LogBasicCB() {
    HAL_GPIO_WritePin(ERR_LED_GPIO, ERR_LED_PIN, ENABLE);
    HAL_Delay(2000);
    HAL_GPIO_WritePin(ERR_LED_GPIO, ERR_LED_PIN, DISABLE);
}

uint8_t LOGGER_DeInitCB() {
    HAL_GPIO_DeInit(ERR_LED_GPIO, ERR_LED_PIN);
    return 0;
}

uint8_t LOGGER_LogCB(LOGGER_EventTypeDef *Event) {
    HAL_StatusTypeDef status = HAL_UART_Transmit(
        app_state.pUARTHandle,
        (uint8_t*)Event->msg,
        strlen(Event->msg) + 1,
        ERR_UART_TX_TIMEOUT
    );
    if (status != HAL_OK) return 1;
    return 0;
}

uint8_t LOGGER_FatalCB(LOGGER_EventTypeDef *Event) {
    HAL_GPIO_WritePin(ERR_LED_GPIO, ERR_LED_PIN, ENABLE);

    // Ignore error
    LOGGER_LogCB(Event);

    HAL_Delay(3000);
    NVIC_SystemReset();

    return 0;
}