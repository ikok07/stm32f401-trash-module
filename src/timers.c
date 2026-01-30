//
// Created by Kok on 1/10/26.
//

#include "stm32f4xx_hal.h"
#include "timers.h"

#include "app_state.h"

/**
 * @brief Configure timer for PWM
 * @warning HAL_TIM_PWM_MspInit should be implemented separately
 * @param Tim_Handle Timer Handle
 * @param cfg Config structure
 */
TIMERS_Error_e TIMERS_ConfigPWM(TIM_HandleTypeDef *Tim_Handle, TIMERS_PWMConfig_t *cfg) {
    assert_param(IS_TIM_CHANNELS(cfg->Channel));

    uint32_t prescaler = (cfg->Tim_Ck_Hz / 1'000'000); // Get 1Mhz clock
    uint32_t period = (cfg->Period_Ms * 1000); // Adjusted for 1Mhz clock

    Tim_Handle->Init = (TIM_Base_InitTypeDef) {
        .CounterMode = TIM_COUNTERMODE_UP,
        .ClockDivision = TIM_CLOCKDIVISION_DIV1,
        .Prescaler = prescaler - 1,
        .Period = period - 1,
        .AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE
    };

    // Init TIM2
    if (HAL_TIM_PWM_Init(Tim_Handle) != HAL_OK) {
        return TIMERS_ERR;
    }

    // Configure TIM Channel 1
    TIM_OC_InitTypeDef TIM2_OC_Config = {
        .OCMode = TIM_OCMODE_PWM1,
        .OCPolarity = TIM_OCPOLARITY_HIGH,
        .Pulse = ((cfg->Duty * period) / 100)
    };
    if (HAL_TIM_PWM_ConfigChannel(Tim_Handle, &TIM2_OC_Config, cfg->Channel) != HAL_OK) {
        return TIMERS_CHAN_CFG_ERR;
    }

    return TIMERS_OK;
}

/**
 * @brief Starts PWM Timer
 * @param Tim_Handle Timer Handle
 * @param chan Timer Channel
 *        This parameter can be one of the following values:
 *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
 *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
 *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
 *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
 */
TIMERS_Error_e TIMERS_StartPWM(TIM_HandleTypeDef *Tim_Handle, uint32_t chan) {
    if (HAL_TIM_PWM_Start(Tim_Handle, chan) != HAL_OK) {
        return TIMERS_ERR;
    }

    return TIMERS_OK;
}

/**
 * @brief Sets PWM Duty Cycle
 * @param Tim_Handle Timer Handle
 * @param chan Timer Channel
 *        This parameter can be one of the following values:
 *            @arg TIM_CHANNEL_1: TIM Channel 1 selected
 *            @arg TIM_CHANNEL_2: TIM Channel 2 selected
 *            @arg TIM_CHANNEL_3: TIM Channel 3 selected
 *            @arg TIM_CHANNEL_4: TIM Channel 4 selected
 * @param duty PWM Duty Cycle (between 0 and 100)
 */
TIMERS_Error_e TIMERS_PWMSetDutyCycle(TIM_HandleTypeDef *Tim_Handle, uint32_t chan, uint32_t duty) {
    if (duty > 100) return TIMERS_INVALID_DUTY;

    uint32_t period = Tim_Handle->Init.Period + 1;
    uint32_t new_duty = ((duty * period) / 100);
    __HAL_TIM_SET_COMPARE(Tim_Handle, chan, new_duty > 0 ? new_duty - 1 : 0);

    return TIMERS_OK;
}