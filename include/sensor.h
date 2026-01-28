//
// Created by Kok on 1/14/26.
//

#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>
#include "stm32f4xx_hal.h"

#define CUSTOM_SENSOR_I2C_ADDRESS           (0x52)
#define CUSTOM_SENSOR_CONN_TRIES            (1)
#define CUSTOM_SENSOR_CONN_TIMEOUT          (3000)

// TEST ONLY
// #define SENSOR_SIMULATE                 1
// #define SENSOR_DUMMY_VALUE              200

typedef enum {
    SENSOR_ERROR_OK,
    SENSOR_ERROR_BOOT_FAIL,
    SENSOR_ERROR_INIT_FAIL,
    SENSOR_ERROR_THRES_FAIL,
    SENSOR_ERROR_DIST_FAIL,
    SENSOR_ERROR_INTERMEAS_FAIL,
    SENSOR_ERROR_TB_FAIL,
    SENSOR_ERROR_INTER_FAIL,
    SENSOR_ERROR_TIMEOUT,
    SENSOR_ERROR_INVALID_CONFIG,
    SENSOR_ERROR_READ_FAIL
} Sensor_Error_e;

typedef enum {
    SENSOR_MODE_BELOW,
    SENSOR_MODE_BEYOND,
    SENSOR_MODE_OUT_OF_DISTANCE,
    SENSOR_MODE_WITHING_DISTANCE
} Sensor_Mode_e;

typedef enum {
    SENSOR_DISTANCEMODE_SHORT = 1,
    SENSOR_DISTANCEMODE_LONG,
} Sensor_DistanceMode_e;

typedef enum {
    SENSOR_TIMINGBUDGET_15 = 15,
    SENSOR_TIMINGBUDGET_33 = 33,
    SENSOR_TIMINGBUDGET_50 = 50,
    SENSOR_TIMINGBUDGET_100 = 100,
    SENSOR_TIMINGBUDGET_200 = 200,
    SENSOR_TIMINGBUDGET_500 = 500
} Sensor_TimingBudget_e;

typedef struct {
    Sensor_Mode_e Mode;
    Sensor_DistanceMode_e DistanceMode;
    uint16_t MinVal;
    uint16_t MaxVal;
    uint32_t InterMeasMs;
    Sensor_TimingBudget_e TimingBudgetMs;
    GPIO_TypeDef *InterGPIO; // Can be left NULL if no interrupt is being used
    uint16_t InterGPIOPin;
    uint8_t InterPolPositive;
    uint32_t InterNVICPriority;
} Sensor_Config_t;

typedef struct {
    uint16_t Address;
    Sensor_Config_t Config;
} Sensor_Handle_t;

Sensor_Error_e Sensor_Config(Sensor_Handle_t *Sensor_Handle, Sensor_Config_t cfg);
Sensor_Error_e Sensor_Read(Sensor_Handle_t *Sensor_Handle, uint16_t *value);

#endif //SENSOR_H
