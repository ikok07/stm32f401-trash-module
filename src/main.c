#include <stdlib.h>
#include <string.h>

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

#define ERR_LED_GPIO                    GPIOC
#define ERR_LED_PIN                     GPIO_PIN_14

#define ERR_UART_TX_TIMEOUT            2000

#define SERVO_CLOSE_ANGLE               0
#define SERVO_OPEN_ANGLE                90
#define SENSOR_RANGE_OPEN_TRIGGER       500

volatile uint8_t gShouldStopDevice = 0;
uint8_t gUARTCommandInputBuffer[1024];
uint16_t gUARTCommandInputBufferLen = sizeof(gUARTCommandInputBuffer) / sizeof(gUARTCommandInputBuffer[0]);

void handle_new_value(uint16_t value) {
    (void)value;
    uint8_t opened = value < SENSOR_RANGE_OPEN_TRIGGER;
    if (opened) {
        Servo_SetPosition(app_state.pServoHandle, SERVO_OPEN_ANGLE);
        HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    } else {
        Servo_SetPosition(app_state.pServoHandle, SERVO_CLOSE_ANGLE);
        gShouldStopDevice = 1;
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

    // Start UART commands reception
    if (COMMANDS_ConfigRx(gUARTCommandInputBuffer, gUARTCommandInputBufferLen) != HAL_OK) {
        LOGGER_Log(LOGGER_LEVEL_FATAL, "Failed to configure UART commands reception!");
    };
    if (COMMANDS_StartRx() != HAL_OK) {
        LOGGER_Log(LOGGER_LEVEL_FATAL, "Failed to start UART commands reception!");
    };

    // Configure I2C
    if (I2C_Config() != HAL_OK) {
        LOGGER_Log(LOGGER_LEVEL_FATAL, "Failed to configure I2C peripheral!");
    };

    // Configure Servo
    Servo_ConfigTypeDef Servo_Cfg = {
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
        LOGGER_Log(LOGGER_LEVEL_FATAL, "Failed to configure servo!");
    }

    // Check if device was in standby mode
    if (PWR_CheckDeviceWasSTBY()) {
        uint16_t value = -1;
        if (VL53L1X_Read(app_state.pSensorHandle, &value) != SENSOR_ERROR_OK) {
            LOGGER_Log(LOGGER_LEVEL_ERROR, "Failed to read sensor data!");
        };
        handle_new_value(value);
    } else {
        // First boot: Configure VL53L1X Driver and enable device stop
        gShouldStopDevice = 1;

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
        //     LOGGER_Log(LOGGER_LEVEL_ERROR, "Failed to configure sensor!");
        // }
    };

    while (!gShouldStopDevice);

    // Enter standby mode
    // HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    if (huart->Instance == app_state.pUARTHandle->Instance) {
        CHANDLER_CommandTypeDef cmd;
        if (CHANDLER_ParseCmd((char*)huart->pRxBuffPtr, &cmd) == CHANDLER_ERROR_OK) {
            if (CHANDLER_Exec(&cmd) != CHANDLER_ERROR_OK) {
                LOGGER_Log(LOGGER_LEVEL_ERROR, "Command handler failed to execute command's method!");
            }
        } else {
            LOGGER_Log(LOGGER_LEVEL_ERROR, "Command handler failed to parse provided command!");
        };

        if (COMMANDS_StartRx() != HAL_OK) {
            LOGGER_Log(LOGGER_LEVEL_ERROR, "Failed to restart UART command reception!");
        };

        // HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFE);
    }
}

void LOGGER_InitBasicCB() {
    // Enable clock access
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Configure Error LED
    GPIO_InitTypeDef GPIO_Config = {
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pin = ERR_LED_PIN,
        .Pull = GPIO_PULLDOWN,
        .Speed = GPIO_SPEED_LOW
    };
    HAL_GPIO_Init(ERR_LED_GPIO, &GPIO_Config);
}

void LOGGER_LogBasicCB() {
    HAL_GPIO_WritePin(ERR_LED_GPIO, ERR_LED_PIN, ENABLE);
    HAL_Delay(2000);
    HAL_GPIO_WritePin(ERR_LED_GPIO, ERR_LED_PIN, DISABLE);
}

uint8_t LOGGER_DeInitCB() {
    HAL_GPIO_DeInit(ERR_LED_GPIO, ERR_LED_PIN);
    return 0;
}

uint8_t LOGGER_LogCB(LOGGER_EventTypeDef *Event) {
    HAL_StatusTypeDef status = HAL_UART_Transmit(
        app_state.pUARTHandle,
        (uint8_t*)Event->msg,
        strlen(Event->msg) + 1,
        ERR_UART_TX_TIMEOUT
    );
    if (status != HAL_OK) return 1;
    return 0;
}

uint8_t LOGGER_FatalCB(LOGGER_EventTypeDef *Event) {
    HAL_GPIO_WritePin(ERR_LED_GPIO, ERR_LED_PIN, ENABLE);

    // Ignore error
    HAL_UART_Transmit(
        app_state.pUARTHandle,
        (uint8_t*)Event->msg,
        strlen(Event->msg) + 1,
        ERR_UART_TX_TIMEOUT
    );

    HAL_Delay(3000);
    NVIC_SystemReset();

    return 0;
}

void HAL_GPIO_EXTI_Callback(uint16_t Pin) {
    if (Pin == GPIO_PIN_0) {
        // Sensor interrupt while device NOT in standby mode
        uint16_t value = -1;
        if (VL53L1X_Read(app_state.pSensorHandle, &value) != SENSOR_ERROR_OK) {
            LOGGER_Log(LOGGER_LEVEL_ERROR, "Failed to read sensor data!");
        };
        handle_new_value(value);
        return;
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