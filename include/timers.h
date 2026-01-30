//
// Created by Kok on 1/10/26.
//

#ifndef TIMER_H
#define TIMER_H

#define CUSTOM_TIM2_PRESC               1
#define CUSTOM_TIM2_ARR_VALUE           1000 - 1

typedef enum {
    TIMERS_OK,
    TIMERS_ERR, // Generic timer error
    TIMERS_CHAN_CFG_ERR, // Error during channel configuration
    TIMERS_INVALID_ACTIVE_CHAN, // The active channel in the TIM's handle is not valid
    TIMERS_INVALID_DUTY // Duty cycle is outside the allowed boundaries
} TIMERS_Error_e;

typedef struct {
    uint32_t Channel;
    uint32_t Period_Ms;
    uint32_t Duty; // Value between 0 and 100
    uint32_t Tim_Ck_Hz; // Frequency of the TIM's clock in Hz
} TIMERS_PWMConfig_t;

TIMERS_Error_e TIMERS_ConfigPWM(TIM_HandleTypeDef *Tim_Handle, TIMERS_PWMConfig_t *cfg);
TIMERS_Error_e TIMERS_StartPWM(TIM_HandleTypeDef *Tim_Handle, uint32_t chan);
TIMERS_Error_e TIMERS_PWMSetDutyCycle(TIM_HandleTypeDef *Tim_Handle, uint32_t chan, uint32_t duty);

#endif //TIMER_H
