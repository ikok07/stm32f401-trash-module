//
// Created by Kok on 1/14/26.
//

#include "sensor.h"

#include "app_state.h"
#include "utils.h"
#include "VL53L1X_api.h"

VL53L1X_ErrorTypeDef wait_for_boot(uint32_t timeout);
void config_gpio();

/**
 * @brief Configures the sensor using the provided configuration structure
 * @param hvl53l1x Sensor Handle
 * @param Cfg Configuration struct
 */
VL53L1X_ErrorTypeDef VL53L1X_Config(VL54L1X_HandleTypeDef *hvl53l1x, VL53L1X_ConfigTypeDef Cfg) {

    assert_param(Cfg.MinVal <= Cfg.MaxVal);
    assert_param(Cfg.MaxVal >= Cfg.MinVal);

    VL53L1X_ErrorTypeDef status = SENSOR_ERROR_OK;
    hvl53l1x->Address = VL53L1X_I2C_ADDRESS;
    hvl53l1x->Config = Cfg;

    if (hvl53l1x->Config.InterMeasMs < hvl53l1x->Config.TimingBudgetMs) {
        return SENSOR_ERROR_INVALID_CONFIG;
    }

    if ((status = wait_for_boot(1000)) != SENSOR_ERROR_OK) {
        return status;
    }

    if (VL53L1X_SensorInit(hvl53l1x->Address) != VL53L1X_ERROR_NONE) {
        return SENSOR_ERROR_INIT_FAIL;
    }

    if (
        VL53L1X_SetDistanceThreshold(
            hvl53l1x->Address,
            hvl53l1x->Config.MinVal,
            hvl53l1x->Config.MaxVal,
            hvl53l1x->Config.Mode,
            0
        ) != VL53L1X_ERROR_NONE
    ) {
        return SENSOR_ERROR_THRES_FAIL;
    }

    if (
        VL53L1X_SetDistanceMode(
            hvl53l1x->Address,
            hvl53l1x->Config.DistanceMode
        ) != VL53L1X_ERROR_NONE
    ) {
        return SENSOR_ERROR_DIST_FAIL;
    }

    if (
        VL53L1X_SetInterMeasurementInMs(
            hvl53l1x->Address,
            hvl53l1x->Config.InterMeasMs
        ) != VL53L1X_ERROR_NONE
    ) {
        return SENSOR_ERROR_INTERMEAS_FAIL;
    }

    if (
        VL53L1X_SetTimingBudgetInMs(
            hvl53l1x->Address,
            hvl53l1x->Config.TimingBudgetMs
        ) != VL53L1X_ERROR_NONE
    ) {
        return SENSOR_ERROR_TB_FAIL;
    }

    if (
        VL53L1X_SetInterruptPolarity(
            hvl53l1x->Address,
            hvl53l1x->Config.InterPolPositive
        ) != VL53L1X_ERROR_NONE
    ) {
        return SENSOR_ERROR_INTER_FAIL;
    }

    if (hvl53l1x->Config.InterGPIO) config_gpio();

    return status;
}

/**
 * @brief Reads the current sensor measurement
 * @param hvl53l1x Sensor Handle
 * @param Value Pointer where to write the sensor measurement
 */
VL53L1X_ErrorTypeDef VL53L1X_Read(VL54L1X_HandleTypeDef *hvl53l1x, uint16_t *Value) {
    assert_param(Value != NULL);

    uint16_t distance = -1;
    #ifdef VL53L1X_SIMULATE
        #ifdef VL53L1X_DUMMY_VALUE
            distance = VL53L1X_DUMMY_VALUE;
        #endif
    #else
        if (VL53L1X_GetDistance(hvl53l1x->Address, &distance) != VL53L1X_ERROR_NONE) {
            return SENSOR_ERROR_READ_FAIL;
        }
    #endif

    if (Value) *Value = distance;
    return SENSOR_ERROR_OK;
}

VL53L1X_ErrorTypeDef wait_for_boot(uint32_t timeout) {
    uint8_t state = 0;
    uint32_t start_tick = HAL_GetTick();

    while ((HAL_GetTick() - start_tick) < timeout) {
        if (VL53L1X_BootState(app_state.pSensorHandle->Address, &state) != VL53L1X_ERROR_NONE) {
            return SENSOR_ERROR_BOOT_FAIL;
        }
        if (state) return SENSOR_ERROR_OK;

        HAL_Delay(10);
    }

    return SENSOR_ERROR_TIMEOUT;
}

void config_gpio() {
    if (!app_state.pSensorHandle->Config.InterGPIO) return;

    // Enable clock access
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Config GPIO
    GPIO_InitTypeDef GPIO_Config = {
        .Mode = GPIO_MODE_INPUT,
        .Pin = app_state.pSensorHandle->Config.InterGPIOPin,
        .Pull = app_state.pSensorHandle->Config.InterPolPositive ? GPIO_PULLDOWN : GPIO_PULLUP,
        .Speed = GPIO_SPEED_LOW,
    };
    HAL_GPIO_Init(app_state.pSensorHandle->Config.InterGPIO, &GPIO_Config);

    // Enable interrupts
    IRQn_Type irq_number = Util_GPIO_Pin_to_IRQn(app_state.pSensorHandle->Config.InterGPIOPin);
    HAL_NVIC_SetPriority(irq_number, app_state.pSensorHandle->Config.InterNVICPriority, 0);
    HAL_NVIC_EnableIRQ(irq_number);
}