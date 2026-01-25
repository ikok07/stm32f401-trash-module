//
// Created by Kok on 1/14/26.
//

#include "sensor.h"

#include "app_state.h"
#include "utils.h"
#include "VL53L1X_api.h"

Sensor_Error_e wait_for_boot(uint32_t timeout);
void config_gpio();

/**
 * @brief Configures the sensor using the provided configuration structure
 * @param Sensor_Handle Sensor Handle
 * @param cfg Configuration struct
 */
Sensor_Error_e Sensor_Config(Sensor_Handle_t *Sensor_Handle, Sensor_Config_t cfg) {

    assert_param(cfg.MinVal <= cfg.MaxVal);
    assert_param(cfg.MaxVal >= cfg.MinVal);

    Sensor_Error_e status = SENSOR_ERROR_OK;
    Sensor_Handle->Address = CUSTOM_SENSOR_I2C_ADDRESS;
    Sensor_Handle->Config = cfg;

    if (Sensor_Handle->Config.InterMeasMs < Sensor_Handle->Config.TimingBudgetMs) {
        return SENSOR_ERROR_INVALID_CONFIG;
    }

    if ((status = wait_for_boot(1000)) != SENSOR_ERROR_OK) {
        return status;
    }

    if (VL53L1X_SensorInit(Sensor_Handle->Address) != VL53L1X_ERROR_NONE) {
        return SENSOR_ERROR_INIT_FAIL;
    }

    if (
        VL53L1X_SetDistanceThreshold(
            Sensor_Handle->Address,
            Sensor_Handle->Config.MinVal,
            Sensor_Handle->Config.MaxVal,
            Sensor_Handle->Config.Mode,
            0
        ) != VL53L1X_ERROR_NONE
    ) {
        return SENSOR_ERROR_THRES_FAIL;
    }

    if (
        VL53L1X_SetDistanceMode(
            Sensor_Handle->Address,
            Sensor_Handle->Config.DistanceMode
        ) != VL53L1X_ERROR_NONE
    ) {
        return SENSOR_ERROR_DIST_FAIL;
    }

    if (
        VL53L1X_SetInterMeasurementInMs(
            Sensor_Handle->Address,
            Sensor_Handle->Config.InterMeasMs
        ) != VL53L1X_ERROR_NONE
    ) {
        return SENSOR_ERROR_INTERMEAS_FAIL;
    }

    if (
        VL53L1X_SetTimingBudgetInMs(
            Sensor_Handle->Address,
            Sensor_Handle->Config.TimingBudgetMs
        ) != VL53L1X_ERROR_NONE
    ) {
        return SENSOR_ERROR_TB_FAIL;
    }

    if (
        VL53L1X_SetInterruptPolarity(
            Sensor_Handle->Address,
            Sensor_Handle->Config.InterPolPositive
        ) != VL53L1X_ERROR_NONE
    ) {
        return SENSOR_ERROR_INTER_FAIL;
    }

    if (Sensor_Handle->Config.InterGPIO) config_gpio();

    return status;
}

/**
 * @brief Reads the current sensor measurement
 * @param Sensor_Handle Sensor Handle
 * @param value Pointer where to write the sensor measurement
 */
Sensor_Error_e Sensor_Read(Sensor_Handle_t *Sensor_Handle, uint16_t *value) {
    assert_param(value != NULL);

    uint16_t distance = -1;
    #ifdef SENSOR_SIMULATE
        #ifdef SENSOR_DUMMY_VALUE
            distance = SENSOR_DUMMY_VALUE;
        #endif
    #else
        if (VL53L1X_GetDistance(Sensor_Handle->Address, &distance) != VL53L1X_ERROR_NONE) {
            return SENSOR_ERROR_READ_FAIL;
        }
    #endif

    if (value) *value = distance;
    return SENSOR_ERROR_OK;
}

Sensor_Error_e wait_for_boot(uint32_t timeout) {
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
        .Speed = GPIO_SPEED_MEDIUM
    };
    HAL_GPIO_Init(app_state.pSensorHandle->Config.InterGPIO, &GPIO_Config);

    // Enable interrupts
    IRQn_Type irq_number = Util_GPIO_Pin_to_IRQn(app_state.pSensorHandle->Config.InterGPIOPin);
    HAL_NVIC_SetPriority(irq_number, app_state.pSensorHandle->Config.InterNVICPriority, 0);
    HAL_NVIC_EnableIRQ(irq_number);
}