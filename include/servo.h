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
} Servo_Error_e;

typedef struct {
    TIM_TypeDef *xTIM;
    uint32_t Tim_Chan;
    uint32_t Tim_Ck_Hz; // Frequency of the TIM's clock in Hz
    uint32_t Period_Ms;
    uint32_t Start_Deg; // 0 - 180 Degrees
} Servo_Config_t;

typedef struct {
    TIM_HandleTypeDef Tim_Handle;
    uint32_t Tim_Chan;
} Servo_Handle_t;

Servo_Error_e Servo_Config(Servo_Handle_t *Servo_Handle, Servo_Config_t *cfg);
Servo_Error_e Servo_SetPosition(Servo_Handle_t *Servo_Handle, uint16_t deg);

#endif //SERVO_H
