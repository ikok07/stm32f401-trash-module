//
// Created by Kok on 1/17/26.
//

#include "servo.h"

#include "timers.h"

/**
 * @brief Configures the required timer for servo control
 * @param Servo_Handle Servo Handle
 * @param cfg Sensor configuration structure
 */
Servo_Error_e Servo_Config(Servo_Handle_t *Servo_Handle, Servo_Config_t *cfg) {

    assert_param(IS_TIM_INSTANCE(cfg->xTIM));
    assert_param(IS_TIM_CHANNELS(cfg->Tim_Chan));
    assert_param(cfg->Start_Deg <= cfg->Max_Deg);

    // Configure timer for PWM generation
    Servo_Handle->Tim_Handle.Instance = cfg->xTIM;
    Servo_Handle->Tim_Chan = cfg->Tim_Chan;
    Servo_Handle->Max_Deg = cfg->Max_Deg;
    TIMERS_PWMConfig_t PWM_Config = {
        .Tim_Ck_Hz = cfg->Tim_Ck_Hz,
        .Period_Ms = cfg->Period_Ms,
        .Channel = cfg->Tim_Chan,
        .Duty = (cfg->Start_Deg * 100) / Servo_Handle->Max_Deg
    };

    if (TIMERS_ConfigPWM(&Servo_Handle->Tim_Handle, &PWM_Config) != TIMERS_OK) {
        return SERVO_TIM_ERR;
    };

    // Start the timer
    if (TIMERS_StartPWM(&Servo_Handle->Tim_Handle, Servo_Handle->Tim_Chan) != TIMERS_OK) {
        return SERVO_TIM_ERR;
    }

    return SERVO_OK;
}

/**
 * @brief Sets the position of the servo
 * @param Servo_Handle Servo Handle
 * @param deg 0 - 180 Degrees
 */
Servo_Error_e Servo_SetPosition(Servo_Handle_t *Servo_Handle, uint16_t deg) {
    if (deg < 0 || deg > Servo_Handle->Max_Deg) {
        return SERVO_POS_ERR;
    }

    if (
        TIMERS_PWMSetDutyCycle(
            &Servo_Handle->Tim_Handle,
            Servo_Handle->Tim_Chan,
            (deg * 100) / Servo_Handle->Max_Deg
        ) != TIMERS_OK
    ) {
        return SERVO_TIM_ERR;
    }

    return SERVO_OK;
}