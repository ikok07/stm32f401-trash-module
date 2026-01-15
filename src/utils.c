//
// Created by Kok on 1/15/26.
//

#include "utils.h"

IRQn_Type Util_GPIO_Pin_to_IRQn(uint16_t GPIO_Pin) {
    switch (GPIO_Pin) {
        case GPIO_PIN_0:  return EXTI0_IRQn;
        case GPIO_PIN_1:  return EXTI1_IRQn;
        case GPIO_PIN_2:  return EXTI2_IRQn;
        case GPIO_PIN_3:  return EXTI3_IRQn;
        case GPIO_PIN_4:  return EXTI4_IRQn;
        case GPIO_PIN_5:
        case GPIO_PIN_6:
        case GPIO_PIN_7:
        case GPIO_PIN_8:
        case GPIO_PIN_9:  return EXTI9_5_IRQn;
        case GPIO_PIN_10:
        case GPIO_PIN_11:
        case GPIO_PIN_12:
        case GPIO_PIN_13:
        case GPIO_PIN_14:
        case GPIO_PIN_15: return EXTI15_10_IRQn;
        default: return (IRQn_Type)-1; // Invalid
    }
}