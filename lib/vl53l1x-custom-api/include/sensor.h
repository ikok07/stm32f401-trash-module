//
// Created by Kok on 1/14/26.
//

#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>
#include "stm32f4xx_hal.h"

#define VL53L1X_I2C_ADDRESS                 (0x52)
#define VL53L1X_CONN_TRIES                  (1)
#define VL53L1X_CONN_TIMEOUT                (3000)

// TEST ONLY
// #define VL53L1X_SIMULATE                 (1)
// #define VL53L1X_DUMMY_VALUE              (200)

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
} VL53L1X_ErrorTypeDef;

typedef enum {
    SENSOR_MODE_BELOW,
    SENSOR_MODE_BEYOND,
    SENSOR_MODE_OUT_OF_DISTANCE,
    SENSOR_MODE_WITHING_DISTANCE
} VL53L1X_ModeTypeDef;

typedef enum {
    SENSOR_DISTANCEMODE_SHORT = 1,
    SENSOR_DISTANCEMODE_LONG,
} VL53L1X_DistanceTypeDef;

typedef enum {
    SENSOR_TIMINGBUDGET_15 = 15,
    SENSOR_TIMINGBUDGET_33 = 33,
    SENSOR_TIMINGBUDGET_50 = 50,
    SENSOR_TIMINGBUDGET_100 = 100,
    SENSOR_TIMINGBUDGET_200 = 200,
    SENSOR_TIMINGBUDGET_500 = 500
} VL53L1X_TimingBudgetTypeDef;

typedef struct {
    VL53L1X_ModeTypeDef Mode;
    VL53L1X_DistanceTypeDef DistanceMode;
    uint16_t MinVal;
    uint16_t MaxVal;
    uint32_t InterMeasMs;
    VL53L1X_TimingBudgetTypeDef TimingBudgetMs;
    GPIO_TypeDef *InterGPIO; // Can be left NULL if no interrupt is being used
    uint16_t InterGPIOPin;
    uint8_t InterPolPositive;
    uint32_t InterNVICPriority;
} VL53L1X_ConfigTypeDef;

typedef struct {
    uint16_t Address;
    VL53L1X_ConfigTypeDef Config;
} VL54L1X_HandleTypeDef;

VL53L1X_ErrorTypeDef VL53L1X_Config(VL54L1X_HandleTypeDef *hvl53l1x, VL53L1X_ConfigTypeDef Cfg);
VL53L1X_ErrorTypeDef VL53L1X_Read(VL54L1X_HandleTypeDef *hvl53l1x, uint16_t *Value);

#endif //SENSOR_H
