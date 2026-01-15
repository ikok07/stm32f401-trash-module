//
// Created by Kok on 1/15/26.
//

#ifndef ERROR_H
#define ERROR_H

#include "stm32f4xx_hal.h"

#define CUSTOM_ERR_GPIO         GPIOC
#define CUSTOM_ERR_PIN          GPIO_PIN_14

void Error_Config();
uint8_t Error_GetState();
void Error_Trigger(uint32_t timeout);
void Error_TriggerFatal();

#endif //ERROR_H
