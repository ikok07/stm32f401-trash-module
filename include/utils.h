//
// Created by Kok on 1/15/26.
//

#ifndef UTILS_H
#define UTILS_H

#include "stm32f4xx_hal.h"

IRQn_Type Util_GPIO_Pin_to_IRQn(uint16_t GPIO_Pin);

#endif //UTILS_H
