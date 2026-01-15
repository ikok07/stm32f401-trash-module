#include <stdlib.h>

#include "app_config.h"
#include "app_state.h"
#include "stm32f4xx_hal.h"
#include "clocks.h"
#include "error.h"
#include "events.h"
#include "i2c.h"
#include "sensor.h"

int main(void) {
    if (HAL_Init() != HAL_OK) {
        while (1);
    }

    // Configure error handling
    Error_Config();

    // Configure the Clocks
    if (CLK_Config() != HAL_OK) {
        Error_TriggerFatal();
    };

    // Configure I2C
    if (I2C_Config() != HAL_OK) {
        Error_TriggerFatal();
    };

    // Configure VL53L1X Driver
    Sensor_Config_t Sensor_Cfg = {
        .Mode = SENSOR_MODE_BELOW,
        .DistanceMode = SENSOR_DISTANCEMODE_SHORT,
        .InterMeasMs = 500,
        .TimingBudgetMs = SENSOR_TIMINGBUDGET_100,
        .MinVal = 200,
        .InterGPIO = GPIOB,
        .InterGPIOPin = GPIO_PIN_5,
        .InterPolPositive = ENABLE,
        .InterNVICPriority = CUSTOM_SENSOR_INT_PRIORITY
    };
    if (
    Sensor_Config(
        app_state.pSensorHandle,
        Sensor_Cfg
    ) != SENSOR_ERROR_OK
    ) {
        Error_TriggerFatal();
    }

    // // Configure TIM2 CH1
    // TIMERS_ConfigTIM2CH1(500);

    while (1);
}

void Event_NewSensorData(uint16_t value) {
    (void)value;
    // Todo: Update servo based on value...
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_5) {
        uint16_t value = -1;
        if (Sensor_Read(app_state.pSensorHandle, &value) != SENSOR_ERROR_OK) {
            Error_Trigger(3000);
        };
        Event_NewSensorData(value);
        return;
    }
}