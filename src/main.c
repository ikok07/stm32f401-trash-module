#include <stdlib.h>

#include "app_config.h"
#include "app_state.h"
#include "stm32f4xx_hal.h"
#include "clocks.h"
#include "error.h"
#include "events.h"
#include "i2c.h"
#include "power.h"
#include "sensor.h"
#include "servo.h"

#define SERVO_CLOSE_ANGLE               0
#define SERVO_OPEN_ANGLE                90
#define SENSOR_RANGE_OPEN_TRIGGER       500

volatile uint8_t should_stop_device = 0;

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

    // Configure Servo
    Servo_Config_t Servo_Cfg = {
        .xTIM = TIM2,
        .Tim_Chan = TIM_CHANNEL_3,
        .Tim_Ck_Hz = app_state.PCLK1,
        .Period_Ms = 20,
        .Max_Deg = 180,
        .Start_Deg = 0
    };

    if (
        Servo_Config(
            app_state.pServoHandle,
            &Servo_Cfg
        ) != SERVO_OK
    ) {
        Error_TriggerFatal();
    }

    // Check if device was in standby mode
    if (PWR_CheckDeviceWasSTBY()) {
        uint16_t value = -1;
        if (Sensor_Read(app_state.pSensorHandle, &value) != SENSOR_ERROR_OK) {
            Error_Trigger(3000);
        };
        Event_NewSensorData(value);
    } else {
        // First boot: Configure VL53L1X Driver and enable device stop
        should_stop_device = 1;

        // Sensor_Config_t Sensor_Cfg = {
        //     .Mode = SENSOR_MODE_OUT_OF_DISTANCE,
        //     .DistanceMode = SENSOR_DISTANCEMODE_SHORT,
        //     .InterMeasMs = 500,
        //     .TimingBudgetMs = SENSOR_TIMINGBUDGET_100,
        //     .MinVal = 0,
        //     .MaxVal = SENSOR_RANGE_OPEN_TRIGGER,
        //     .InterGPIO = GPIOA,
        //     .InterGPIOPin = GPIO_PIN_0,
        //     .InterPolPositive = ENABLE,
        //     .InterNVICPriority = CUSTOM_SENSOR_INT_PRIORITY
        // };
        //
        // if (
        // Sensor_Config(
        //     app_state.pSensorHandle,
        //     Sensor_Cfg
        // ) != SENSOR_ERROR_OK
        // ) {
        //     Error_TriggerFatal();
        // }
    };

    while (!should_stop_device);

    // Enter standby mode
    PWR_EnterStandbyMode();
}

void Event_NewSensorData(uint16_t value) {
    (void)value;
    uint8_t opened = value < SENSOR_RANGE_OPEN_TRIGGER;
    if (opened) {
        Servo_SetPosition(app_state.pServoHandle, SERVO_OPEN_ANGLE);
        HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
    } else {
        Servo_SetPosition(app_state.pServoHandle, SERVO_CLOSE_ANGLE);
        should_stop_device = 1;
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_0) {
        // Sensor interrupt while device NOT in standby mode
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
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_TIM2_CLK_ENABLE();

        // Configure PB10 for TIM2 CH3
        GPIO_InitTypeDef GPIO_Config = {
            .Pin = GPIO_PIN_10,
            .Mode = GPIO_MODE_AF_PP,
            .Alternate = GPIO_AF1_TIM2
        };
        HAL_GPIO_Init(GPIOB, &GPIO_Config);
    }
}