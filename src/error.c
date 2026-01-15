//
// Created by Kok on 1/15/26.
//

#include "error.h"

volatile uint8_t error_active = 0;

/**
 * @brief Configures the error LED pin
 */
void Error_Config() {
    // Enable clock access
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_Config = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pin = CUSTOM_ERR_PIN,
        .Pull = GPIO_PULLDOWN,
        .Speed = GPIO_SPEED_LOW
    };
    HAL_GPIO_Init(CUSTOM_ERR_GPIO, &GPIO_Config);
}

/**
 * @brief Checks whether there is an active non-fatal error
 * @return 0 - Error is not active, 1 - error is active
 */
uint8_t Error_GetState() {
    return error_active;
}

/**
 * @brief Flashes the error LED (indicating non-fatal error) for a specific period
 * @param timeout Timeout after which the error LED will stop flashing
 */
void Error_Trigger(uint32_t timeout) {
    if (error_active) return;
    error_active = 1;

    uint16_t step = 250;
    for (int i = 0; i < timeout; i += step) {
        HAL_GPIO_TogglePin(CUSTOM_ERR_GPIO, CUSTOM_ERR_PIN);
        HAL_Delay(step);
    }

    HAL_GPIO_WritePin(CUSTOM_ERR_GPIO, CUSTOM_ERR_PIN, DISABLE);

    error_active = 0;
}

/**
 * @brief Activates the error LED (indicating fatal error), waits some time and resets the system
 */
void Error_TriggerFatal() {
    __disable_irq();
    HAL_GPIO_WritePin(CUSTOM_ERR_GPIO, CUSTOM_ERR_PIN, ENABLE);
    HAL_Delay(3000);
    NVIC_SystemReset();
}
