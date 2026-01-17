#include <stdlib.h>

#include "app_config.h"
#include "app_state.h"
#include "stm32f4xx_hal.h"
#include "clocks.h"
#include "error.h"
#include "events.h"
#include "i2c.h"
#include "sensor.h"
#include "servo.h"

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

    // Configure Servo
    Servo_Config_t Servo_Cfg = {
        .xTIM = TIM2,
        .Tim_Chan = TIM_CHANNEL_3,
        .Tim_Ck_Hz = app_state.PCLK1,
        .Period_Ms = 20,
        .Start_Deg = 90
    };

    if (
        Servo_Config(
            app_state.pServoHandle,
            &Servo_Cfg
        ) != SERVO_OK
    ) {
        Error_TriggerFatal();
    }

    uint8_t i = 1;
    while (1) {
        uint32_t val = 20 * i++;
        if (val > 180) val = 0, i = 0;
        Servo_SetPosition(app_state.pServoHandle, val);
        HAL_Delay(1000);
    };
}

void Event_NewSensorData(uint16_t value) {
    (void)value;
    // Todo: Update servo based on value...
    // Servo_SetPosition(app_state.pServoHandle, 90);
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

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *Tim_Handle) {
    if (Tim_Handle->Instance == TIM2) {
        // Enable Clock Access
        __HAL_RCC_TIM2_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        // Configure PA0 for TIM2 CH1
        GPIO_InitTypeDef GPIO_Config = {
            .Pin = GPIO_PIN_10,
            .Mode = GPIO_MODE_AF_PP,
            .Alternate = GPIO_AF1_TIM2
        };
        HAL_GPIO_Init(GPIOB, &GPIO_Config);
    }
}