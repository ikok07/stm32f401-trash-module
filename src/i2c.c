//
// Created by Kok on 1/14/26.
//

#include "i2c.h"
#include "app_state.h"
#include "app_config.h"

void HAL_I2C_MspInit(I2C_HandleTypeDef *I2C_Handle) {
    // Enable Clock Access
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Configure PB6 and PB7
    GPIO_InitTypeDef GPIO_Config = {
        .Pin = GPIO_PIN_6 | GPIO_PIN_7,
        .Mode = GPIO_MODE_AF_OD,
        .Alternate = GPIO_AF4_I2C1,
        .Pull = GPIO_PULLUP // Use external pull up resistor
    };

    HAL_GPIO_Init(GPIOB, &GPIO_Config);
}

HAL_StatusTypeDef I2C_Config() {
    HAL_StatusTypeDef status;

    app_state.pI2CHandle->Instance = I2C1;
    app_state.pI2CHandle->Init = (I2C_InitTypeDef) {
        .DutyCycle = I2C_DUTYCYCLE_2,
        .ClockSpeed = CUSTOM_I2C_SPEED,
        .AddressingMode = I2C_ADDRESSINGMODE_7BIT
    };

    if ((status = HAL_I2C_Init(app_state.pI2CHandle)) != HAL_OK) {
        return status;
    };

    return status;
}
