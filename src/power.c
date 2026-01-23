//
// Created by Kok on 1/23/26.
//

#include "power.h"

#include "stm32f4xx_hal.h"

void PWR_EnterStopMode() {
    // Disable SysTick
    HAL_SuspendTick();

    // Enter stop mode
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}
