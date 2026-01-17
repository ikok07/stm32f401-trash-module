//
// Created by Kok on 1/14/26.
//

#include "app_state.h"

I2C_HandleTypeDef I2C_Handle = {};
Sensor_Handle_t Sensor_Handle = {};
Servo_Handle_t Servo_Handle = {};

AppState_t app_state = {
    .HLCK = 0,
    .PCLK1 = 0,
    .PCLK1_Tim = 0,
    .PCLK2 = 0,
    .PCLK2_Tim = 0,
    .pI2CHandle = &I2C_Handle,
    .pSensorHandle = &Sensor_Handle,
    .pServoHandle = &Servo_Handle
};
