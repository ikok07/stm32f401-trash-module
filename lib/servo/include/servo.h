//
// Created by Kok on 1/17/26.
//

#ifndef SERVO_H
#define SERVO_H

#include "stm32f4xx_hal.h"

typedef enum {
    SERVO_OK,
    SERVO_TIM_ERR, // There was an error with the timer generating the PWM signal
    SERVO_POS_ERR // Desired servo position is not between the allowed 0 and 180 degrees
} SERVO_ErrorTypeDef;

typedef struct {
    TIM_TypeDef *xTIM;
    uint32_t Tim_Chan;
    uint32_t Tim_Ck_Hz; // Frequency of the TIM's clock in Hz
    uint32_t Period_Ms;
    uint32_t Max_Deg; // The maximum allowed angle of the servo
    uint32_t Start_Deg; // 0 - 180 Degrees
} SERVO_ConfigTypeDef;

typedef struct {
    TIM_HandleTypeDef Tim_Handle;
    uint32_t Tim_Chan;
    uint32_t Max_Deg;
} SERVO_HandleTypeDef;

SERVO_ErrorTypeDef SERVO_Config(SERVO_HandleTypeDef *hservo, SERVO_ConfigTypeDef *Cfg);
SERVO_ErrorTypeDef SERVO_SetPosition(SERVO_HandleTypeDef *hservo, uint16_t Degrees);

#endif //SERVO_H
