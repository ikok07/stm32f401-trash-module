//
// Created by Kok on 7/17/25.
//

#include "stm32f4xx_hal.h"
#include <stdio.h>

#include "../lib/logger/include/log.h"


#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line) {
    char buffer[255];
    snprintf(buffer, sizeof(buffer), "Assertion failed in file: %s on line: %ld\n", (char*)file, line);
    LOGGER_Log(LOGGER_LEVEL_FATAL, buffer);
}

#endif
