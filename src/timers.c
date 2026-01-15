//
// Created by Kok on 1/10/26.
//

#include "stm32f4xx_hal.h"
#include "timers.h"

#include "app_state.h"

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *TIM_Handle) {
    if (TIM_Handle->Instance == TIM2) {
        // Enable Clock Access
        __HAL_RCC_TIM2_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        // Configure PA0 for TIM2 CH1
        GPIO_InitTypeDef GPIO_Config = {
            .Pin = GPIO_PIN_0,
            .Mode = GPIO_MODE_AF_PP,
            .Alternate = GPIO_AF1_TIM2
        };
        HAL_GPIO_Init(GPIOA, &GPIO_Config);
    }
}

HAL_StatusTypeDef TIMERS_ConfigTIM2CH1(uint16_t start_duty) {
    HAL_StatusTypeDef status = HAL_OK;

    app_state.pTIM2Handle->Instance = TIM2;
    app_state.pTIM2Handle->Init = (TIM_Base_InitTypeDef) {
        .CounterMode = TIM_COUNTERMODE_UP,
        .ClockDivision = TIM_CLOCKDIVISION_DIV1,
        .Prescaler = CUSTOM_TIM2_PRESC,
        .Period = CUSTOM_TIM2_ARR_VALUE,
        .AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE
    };

    // Init TIM2
    if ((status = HAL_TIM_PWM_Init(app_state.pTIM2Handle)) != HAL_OK) {
        return status;
    }

    // Configure TIM Channel 1
    TIM_OC_InitTypeDef TIM2_OC_Config = {
        .OCMode = TIM_OCMODE_PWM1,
        .Pulse = start_duty - 1
    };
    if ((status = HAL_TIM_PWM_ConfigChannel(app_state.pTIM2Handle, &TIM2_OC_Config, TIM_CHANNEL_1)) != HAL_OK) {
        return status;
    }

    // Start TIM2
    if ((status = HAL_TIM_PWM_Start(app_state.pTIM2Handle, TIM_CHANNEL_1)) != HAL_OK) {
        return status;
    }

    return status;
}

void TIMERS_SetTIM2CH1_DutyCycle(uint16_t value) {
    __HAL_TIM_SET_COMPARE(app_state.pTIM2Handle, TIM_CHANNEL_1, value > 0 ? value - 1 : 0);
}

TIM_HandleTypeDef *TIMERS_TIM2CH1_GetHandle() {
    return app_state.pTIM2Handle;
}
