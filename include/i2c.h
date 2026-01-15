//
// Created by Kok on 1/14/26.
//

#ifndef I2C_H
#define I2C_H

#include "stm32f4xx_hal.h"

#define CUSTOM_I2C_SPEED           100'000

HAL_StatusTypeDef I2C_Config();

#endif //I2C_H
