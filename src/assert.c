//
// Created by Kok on 7/17/25.
//

#include "stm32f4xx_hal.h"
#include <stdio.h>

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line) {
    printf("Assertion failed in file: %s on line: %ld\n", (char*)file, line);
    while (1);
}

#endif
