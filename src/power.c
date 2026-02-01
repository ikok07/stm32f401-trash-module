//
// Created by Kok on 1/23/26.
//

#include "power.h"

#include "commands.h"
#include "stm32f4xx_hal.h"

uint8_t PWR_CheckDeviceWasSTBY() {
    // Enable clock access
    __HAL_RCC_PWR_CLK_ENABLE();

    return __HAL_PWR_GET_FLAG(PWR_FLAG_WU);
}

void PWR_EnterStandbyMode() {
    if (COMMANDS_Enabled()) return;

    // Enable clock access
    __HAL_RCC_PWR_CLK_ENABLE();

    // // Clear any pending wakeup flags before entering standby
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);  // Clear wakeup flag
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);  // Clear standby flag

    // Enable WKUP pin and enter standby mode
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
    HAL_PWR_EnterSTANDBYMode();
}
