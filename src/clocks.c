//
// Created by Kok on 1/10/26.
//

#include "clocks.h"

#include "app_state.h"
#include "stm32f4xx_hal.h"

HAL_StatusTypeDef CLK_Config() {

    // Enable the HSI and use it for SYSCLK
    RCC_OscInitTypeDef Config_Osc = {
        .OscillatorType = RCC_OSCILLATORTYPE_HSI,
        .HSIState = RCC_HSI_ON,
        .HSICalibrationValue =  RCC_HSICALIBRATION_DEFAULT,
        .PLL.PLLState = RCC_PLL_NONE
    };

    HAL_StatusTypeDef status = HAL_OK;

    if ((status = HAL_RCC_OscConfig(&Config_Osc)) != HAL_OK) {
        return status;
    };

    // Configure clocks
    RCC_ClkInitTypeDef Config_Clk = {
        .ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2,
        .SYSCLKSource = RCC_SYSCLKSOURCE_HSI,
        .AHBCLKDivider = RCC_SYSCLK_DIV4, // HCLK = 4 MHz
        .APB1CLKDivider = RCC_HCLK_DIV1,
        .APB2CLKDivider = RCC_HCLK_DIV1
    };

    if ((status = HAL_RCC_ClockConfig(&Config_Clk, FLASH_LATENCY_0)) != HAL_OK) {
        return status;
    };

    app_state.HLCK = 4'000'000;
    app_state.PCLK1 = 4'000'000;
    app_state.PCLK1_Tim = 4'000'000;
    app_state.PCLK2 = 4'000'000;
    app_state.PCLK2_Tim = 4'000'000;

    return status;
}
