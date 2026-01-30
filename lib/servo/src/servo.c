//
// Created by Kok on 1/17/26.
//

#include "servo.h"

#include "timers.h"

/**
 * @brief Configures the required timer for servo control
 * @param hservo Servo Handle
 * @param Cfg Sensor configuration structure
 */
Servo_ErrorTypeDef Servo_Config(Servo_HandleTypeDef *hservo, Servo_ConfigTypeDef *Cfg) {

    assert_param(IS_TIM_INSTANCE(Cfg->xTIM));
    assert_param(IS_TIM_CHANNELS(Cfg->Tim_Chan));
    assert_param(Cfg->Start_Deg <= Cfg->Max_Deg);

    // Configure timer for PWM generation
    hservo->Tim_Handle.Instance = Cfg->xTIM;
    hservo->Tim_Chan = Cfg->Tim_Chan;
    hservo->Max_Deg = Cfg->Max_Deg;
    TIMERS_PWMConfig_t PWM_Config = {
        .Tim_Ck_Hz = Cfg->Tim_Ck_Hz,
        .Period_Ms = Cfg->Period_Ms,
        .Channel = Cfg->Tim_Chan,
        .Duty = (Cfg->Start_Deg * 100) / hservo->Max_Deg
    };

    if (TIMERS_ConfigPWM(&hservo->Tim_Handle, &PWM_Config) != TIMERS_OK) {
        return SERVO_TIM_ERR;
    };

    // Start the timer
    if (TIMERS_StartPWM(&hservo->Tim_Handle, hservo->Tim_Chan) != TIMERS_OK) {
        return SERVO_TIM_ERR;
    }

    return SERVO_OK;
}

/**
 * @brief Sets the position of the servo
 * @param hservo Servo Handle
 * @param Degrees 0 - 180 Degrees
 */
Servo_ErrorTypeDef Servo_SetPosition(Servo_HandleTypeDef *hservo, uint16_t Degrees) {
    if (Degrees < 0 || Degrees > hservo->Max_Deg) {
        return SERVO_POS_ERR;
    }

    if (
        TIMERS_PWMSetDutyCycle(
            &hservo->Tim_Handle,
            hservo->Tim_Chan,
            (Degrees * 100) / hservo->Max_Deg
        ) != TIMERS_OK
    ) {
        return SERVO_TIM_ERR;
    }

    return SERVO_OK;
}