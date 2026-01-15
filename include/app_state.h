//
// Created by Kok on 1/14/26.
//

#ifndef APP_STATE_H
#define APP_STATE_H

#include "stm32f4xx_hal.h"
#include "sensor.h"

typedef struct {
    uint32_t HLCK;
    uint32_t PCLK1;
    uint32_t PCLK1_Tim;
    uint32_t PCLK2;
    uint32_t PCLK2_Tim;
    I2C_HandleTypeDef *pI2CHandle;
    TIM_HandleTypeDef *pTIM2Handle;
    Sensor_Handle_t *pSensorHandle;
} AppState_t;

extern AppState_t app_state;

#endif //APP_STATE_H
