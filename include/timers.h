//
// Created by Kok on 1/10/26.
//

#ifndef TIMER_H
#define TIMER_H

#define CUSTOM_TIM2_PRESC               1       // For 16MHz
#define CUSTOM_TIM2_ARR_VALUE           1000 - 1

HAL_StatusTypeDef TIMERS_ConfigTIM2CH1(uint16_t start_duty);
void TIMERS_SetTIM2CH1_DutyCycle(uint16_t value);
TIM_HandleTypeDef *TIMERS_TIM2CH1_GetHandle();

#endif //TIMER_H
