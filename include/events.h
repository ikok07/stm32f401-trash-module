//
// Created by Kok on 1/15/26.
//

#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "stm32f4xx_hal.h"

__weak void Event_NewSensorData(uint16_t value);

#endif //EVENT_HANDLER_H
