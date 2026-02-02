#include <stdlib.h>
#include <string.h>

#include "app_config.h"
#include "app_state.h"
#include "stm32f4xx_hal.h"
#include "clocks.h"
#include "i2c.h"
#include "power.h"
#include "sensor.h"
#include "servo.h"
#include "log.h"
#include "uart.h"
#include "commands.h"
#include "tests.h"

#define SERVO_CLOSE_ANGLE               0
#define SERVO_OPEN_ANGLE                90
#define SENSOR_RANGE_OPEN_TRIGGER       500

#define UART_COMMAND_INPUT_BUFFER_SIZE  1024

uint8_t gUARTCommandInputBuffer[UART_COMMAND_INPUT_BUFFER_SIZE];

volatile uint8_t gReadSensorValueTrigger = 0;
volatile uint8_t gExecCommandTrigger = 0;
volatile uint8_t gEnterStandbyModeTrigger = 1;

void handle_new_value(uint16_t value) {
    (void)value;
    uint8_t openAllowed = value < SENSOR_RANGE_OPEN_TRIGGER;
    if (openAllowed) {
        SERVO_SetPosition(app_state.pServoHandle, SERVO_OPEN_ANGLE);
        gEnterStandbyModeTrigger = 0;
    } else {
        // Entering STANDBY mode will stop the PWM timer which will reset the servo
        gEnterStandbyModeTrigger = 1;
    }
}

int main(void) {
    if (HAL_Init() != HAL_OK) {
        while (1);
    }

    // Initialize basic logger and enable it
    LOGGER_InitBasic();
    LOGGER_Enable();

    // Configure the Clocks
    if (CLK_Config() != HAL_OK) {
        LOGGER_LogBasic();
    };

    // Configure UART
    if (UART1_Config() != HAL_OK) {
        LOGGER_LogBasic();
    };

    // Initialize the full logger and enable it
    LOGGER_Init();
    LOGGER_Enable();

    // Configure I2C
    if (I2C_Config() != HAL_OK) {
        LOGGER_Log(LOGGER_LEVEL_FATAL, "Failed to configure I2C peripheral!");
    };

    // Configure Servo
    SERVO_ConfigTypeDef Servo_Cfg = {
        .xTIM = TIM2,
        .Tim_Chan = TIM_CHANNEL_3,
        .Tim_Ck_Hz = app_state.PCLK1,
        .Period_Ms = 20,
        .Max_Deg = 180,
        .Start_Deg = 0
    };

    if (
        SERVO_Config(
            app_state.pServoHandle,
            &Servo_Cfg
        ) != SERVO_OK
    ) {
        LOGGER_Log(LOGGER_LEVEL_FATAL, "Failed to configure servo!");
    }

    if (!PWR_CheckDeviceWasSTBY()) {
        // Start UART commands reception
        if (COMMANDS_ConfigRx(gUARTCommandInputBuffer, UART_COMMAND_INPUT_BUFFER_SIZE) != HAL_OK) {
            LOGGER_Log(LOGGER_LEVEL_FATAL, "Failed to configure UART commands reception!");
        };
        if (COMMANDS_StartRx() != HAL_OK) {
            LOGGER_Log(LOGGER_LEVEL_FATAL, "Failed to start UART commands reception!");
        };

        VL53L1X_ConfigTypeDef Sensor_Cfg = {
            .Mode = VL53L1X_MODE_WITHING_DISTANCE,
            .DistanceMode = VL53L1X_DISTANCEMODE_SHORT,
            .InterMeasMs = 500,
            .TimingBudgetMs = VL53L1X_TIMINGBUDGET_100,
            .MinVal = 0,
            .MaxVal = SENSOR_RANGE_OPEN_TRIGGER,
            .InterGPIO = GPIOA,
            .InterGPIOPin = GPIO_PIN_0,
            .InterPolPositive = ENABLE,
            .InterNVICPriority = CUSTOM_VL53L1X_INT_PRIORITY
        };
        (void)Sensor_Cfg;
        // if (
        // VL53L1X_Config(
        //     app_state.pSensorHandle,
        //     Sensor_Cfg
        // ) != VL53L1X_ERROR_OK
        // ) {
        //     LOGGER_Log(LOGGER_LEVEL_ERROR, "Failed to configure sensor!");
        // }

        // Run tests
        if (TESTS_Run() != TEST_RESULT_PASS) {
            LOGGER_Log(LOGGER_LEVEL_FATAL, "Not all tests passed! Restarting MCU...");
        };
    } else {
        gReadSensorValueTrigger = 1;
    }

    while (1) {
        if (gReadSensorValueTrigger) {
            gReadSensorValueTrigger = 0;

            // Sensor read while device NOT in STANDBY mode
            uint16_t value = -1;
            if (VL53L1X_Read(app_state.pSensorHandle, &value) != VL53L1X_ERROR_OK) {
                LOGGER_Log(LOGGER_LEVEL_ERROR, "Failed to read sensor data!");
            };
            handle_new_value(value);
        }

        if (gExecCommandTrigger) {
            gExecCommandTrigger = 0;
            COMMANDS_Execute((char*)app_state.pUARTHandle->pRxBuffPtr);
        }

        if (gEnterStandbyModeTrigger) {
            gEnterStandbyModeTrigger = 0;
            PWR_EnterStandbyMode();
        }
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    if (huart->Instance == app_state.pUARTHandle->Instance) {
        gExecCommandTrigger = 1;
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t Pin) {
    if (Pin == GPIO_PIN_0) {
        gReadSensorValueTrigger = 1;
    }
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        // Enable Clock Access
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_TIM2_CLK_ENABLE();

        // Configure PB10 for TIM2 CH3
        GPIO_InitTypeDef GPIO_Config = {
            .Pin = GPIO_PIN_10,
            .Mode = GPIO_MODE_AF_PP,
            .Alternate = GPIO_AF1_TIM2,
            .Speed = GPIO_SPEED_LOW
        };
        HAL_GPIO_Init(GPIOB, &GPIO_Config);
    }
}