//
// Created by Kok on 1/14/26.
//

#include "app_state.h"

I2C_HandleTypeDef I2CHandle = {};
VL54L1X_HandleTypeDef SensorHandle = {};
SERVO_HandleTypeDef ServoHandle = {};
UART_HandleTypeDef ErrUSARTHandle = {};
DMA_HandleTypeDef pDMAUSART1RXHandle = {};

AppState_t app_state = {
    .HLCK = 0,
    .PCLK1 = 0,
    .PCLK1_Tim = 0,
    .PCLK2 = 0,
    .PCLK2_Tim = 0,
    .pI2CHandle = &I2CHandle,
    .pSensorHandle = &SensorHandle,
    .pServoHandle = &ServoHandle,
    .pUARTHandle = &ErrUSARTHandle,
    .pDMAUART1RXHandle = &pDMAUSART1RXHandle
};
