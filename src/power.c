//
// Created by Kok on 1/23/26.
//

#include "power.h"

#include "stm32f4xx_hal.h"

void PWR_EnterStandbyMode() {
    // Enter stop mode
    HAL_PWR_EnterSTANDBYMode();
}
