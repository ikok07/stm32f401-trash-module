#include <stdlib.h>
#include <string.h>

#include "app_state.h"
#include "stm32f4xx_hal.h"
#include "clocks.h"
#include "events.h"
#include "i2c.h"
#include "power.h"
#include "sensor.h"
#include "servo.h"
#include "log.h"

#define ERR_LED_GPIO            GPIOC
#define ERR_LED_PIN             GPIO_PIN_14

#define ERR_USART_TX_TIMEOUT    5000

#define SERVO_CLOSE_ANGLE               0
#define SERVO_OPEN_ANGLE                90
#define SENSOR_RANGE_OPEN_TRIGGER       500

volatile uint8_t gShouldStopDevice = 0;

int main(void) {
    if (HAL_Init() != HAL_OK) {
        while (1);
    }

    // Configure and enable logger
    LOGGER_Init();
    LOGGER_Enable();

    // Configure the Clocks
    if (CLK_Config() != HAL_OK) {
        LOGGER_Log(LOGGER_LEVEL_FATAL, "Failed to configure MCU's clocks!");
    };

    // Reinit logger for new clocks
    LOGGER_ReInit();
    LOGGER_Enable();

    // Configure I2C
    if (I2C_Config() != HAL_OK) {
        LOGGER_Log(LOGGER_LEVEL_FATAL, "Failed to configure I2C peripheral!");
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
        LOGGER_Log(LOGGER_LEVEL_FATAL, "Failed to configure servo!");
    }

    // Check if device was in standby mode
    if (PWR_CheckDeviceWasSTBY()) {
        uint16_t value = -1;
        if (Sensor_Read(app_state.pSensorHandle, &value) != SENSOR_ERROR_OK) {
            LOGGER_Log(LOGGER_LEVEL_ERROR, "Failed to read sensor data!");
        };
        Event_NewSensorData(value);
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
    // PWR_EnterStandbyMode();
    while (1);
}

void Event_NewSensorData(uint16_t value) {
    (void)value;
    uint8_t opened = value < SENSOR_RANGE_OPEN_TRIGGER;
    if (opened) {
        Servo_SetPosition(app_state.pServoHandle, SERVO_OPEN_ANGLE);
        HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
    } else {
        Servo_SetPosition(app_state.pServoHandle, SERVO_CLOSE_ANGLE);
        gShouldStopDevice = 1;
    }
}

uint8_t LOGGER_InitCB() {
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

    // Configure USART
    app_state.pErrUSARTHandle->Instance = USART1;
    app_state.pErrUSARTHandle->Init = (USART_InitTypeDef) {
        .Mode = USART_MODE_TX,
        .BaudRate = 9600,
        .CLKPhase = USART_PHASE_1EDGE,
        .CLKPolarity = USART_POLARITY_LOW,
        .Parity = USART_PARITY_NONE,
        .CLKLastBit = USART_LASTBIT_DISABLE,
        .StopBits = USART_STOPBITS_1,
        .WordLength = USART_WORDLENGTH_8B
    };
    HAL_StatusTypeDef status = HAL_USART_Init(app_state.pErrUSARTHandle);

    return status;
}

uint8_t LOGGER_DeInitCB() {
    HAL_GPIO_DeInit(ERR_LED_GPIO, ERR_LED_PIN);
    HAL_StatusTypeDef status = HAL_USART_DeInit(app_state.pErrUSARTHandle);
    return status;
}

uint8_t LOGGER_LogCB(LOGGER_EventTypeDef *event) {
    HAL_StatusTypeDef status = HAL_USART_Transmit(
        app_state.pErrUSARTHandle,
        (uint8_t*)event->msg,
        strlen(event->msg) + 1,
        ERR_USART_TX_TIMEOUT
    );
    if (status != HAL_OK) return 1;
    return 0;
}

uint8_t LOGGER_FatalCB(LOGGER_EventTypeDef *event) {
    HAL_GPIO_WritePin(ERR_LED_GPIO, ERR_LED_PIN, ENABLE);

    // Ignore error
    HAL_USART_Transmit(
        app_state.pErrUSARTHandle,
        (uint8_t*)event->msg,
        strlen(event->msg) + 1,
        ERR_USART_TX_TIMEOUT
    );

    HAL_Delay(3000);
    NVIC_SystemReset();

    return 0;
}

void HAL_GPIO_EXTI_Callback(uint16_t Pin) {
    if (Pin == GPIO_PIN_0) {
        // Sensor interrupt while device NOT in standby mode
        uint16_t value = -1;
        if (Sensor_Read(app_state.pSensorHandle, &value) != SENSOR_ERROR_OK) {
            LOGGER_Log(LOGGER_LEVEL_ERROR, "Failed to read sensor data!");
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
            .Alternate = GPIO_AF1_TIM2,
            .Speed = GPIO_SPEED_LOW
        };
        HAL_GPIO_Init(GPIOB, &GPIO_Config);
    }
}

void HAL_USART_MspInit(USART_HandleTypeDef *husart) {
    GPIO_InitTypeDef GPIO_Config = {
        .Mode = GPIO_MODE_AF_PP,
        .Pull = GPIO_PULLUP,
        .Speed = GPIO_SPEED_LOW
    };

    if (husart->Instance == USART1) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_USART1_CLK_ENABLE();

        GPIO_Config.Alternate = GPIO_AF7_USART1;
        GPIO_Config.Pin = GPIO_PIN_9;

        HAL_GPIO_Init(GPIOA, &GPIO_Config);
    }
}

void HAL_USART_MspDeInit(USART_HandleTypeDef *husart) {
    if (husart->Instance == USART1) {
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);
    }
}