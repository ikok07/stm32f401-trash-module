//
// Created by Kok on 1/29/26.
//

#include "log.h"

#include <stdio.h>
#include <string.h>

#include "stm32f4xx_hal.h"

static LOGGER_TypeDef hlogger;

/**
 * @brief This method should setup basic peripherals that are most likely to not fail during initialization.
 *        This ensures that there will be some sign of error even if more complex peripherals are still not running.
 */
void LOGGER_InitBasic() {
    hlogger.Initialized = 1;
    LOGGER_InitBasicCB();
}

/**
 * @brief This method calls ONLY the LOGGER_LogBasicCB() which should never return error.
 *        This ensures that there will be some sign of error even if more complex peripherals are still not running.
 */
void LOGGER_LogBasic() {
    LOGGER_LogBasicCB();
}

/**
 * @brief This method initializes the logger with all the required peripherals even if they can return error.
 * @note LOGGER_LogInitBasic() is called inside this method
 */
LOGGER_ErrorTypeDef LOGGER_Init() {
    hlogger.Initialized = 1;
    hlogger.Enabled = 0;
    hlogger.FatalOccurred = 0;

    if (LOGGER_InitCB() != 0) return LOGGER_ERROR_IMPLEMENTATION;
    return LOGGER_ERROR_OK;
}

LOGGER_ErrorTypeDef LOGGER_DeInit() {
    hlogger = (LOGGER_TypeDef) {0};
    if (LOGGER_DeInitCB() != 0) return LOGGER_ERROR_IMPLEMENTATION;
    return LOGGER_ERROR_OK;
}

LOGGER_ErrorTypeDef LOGGER_ReInit() {
    LOGGER_ErrorTypeDef error = LOGGER_DeInit();
    if (error != LOGGER_ERROR_OK) return error;
    return LOGGER_Init();
}

LOGGER_ErrorTypeDef LOGGER_Log(LOGGER_LevelTypeDef level, char *msg) {
    if (!hlogger.Initialized) return LOGGER_ERROR_UNINITIALIZED;
    if (!hlogger.Enabled) return LOGGER_ERROR_DISABLED;

    if (hlogger.ActiveLevel > level) {
        // Logger's active level is higher than the provided one.
        return LOGGER_ERROR_OK;
    }

    if (strlen(msg) > LOGGER_MSG_MAX_LEN) return LOGGER_ERROR_MESSAGE_LEN;

    char formatted_msg[LOGGER_MSG_MAX_LEN];
    LOGGER_EventTypeDef event = {
        .Level = level,
        .msg = msg
    };
    if (LOGGER_FormatCB(&event, formatted_msg, sizeof(formatted_msg)) != 0) return LOGGER_ERROR_IMPLEMENTATION;

    event.msg = formatted_msg;

    if (level == LOGGER_LEVEL_FATAL) {
        if (LOGGER_FatalCB(&event) != 0) return LOGGER_ERROR_IMPLEMENTATION;
    } else {
        if (LOGGER_LogCB(&event) != 0) return LOGGER_ERROR_IMPLEMENTATION;
    }

    return LOGGER_ERROR_OK;
}

LOGGER_ErrorTypeDef LOGGER_Enable() {
    if (!hlogger.Initialized) return LOGGER_ERROR_UNINITIALIZED;
    hlogger.Enabled = 1;
    return LOGGER_ERROR_OK;
}

LOGGER_ErrorTypeDef LOGGER_Disable() {
    if (!hlogger.Initialized) return LOGGER_ERROR_UNINITIALIZED;
    hlogger.Enabled = 0;
    return LOGGER_ERROR_OK;
}

/**
 * @brief Configures the level of the messages which the logger will output
 * @param Level Logger level
 */
LOGGER_ErrorTypeDef LOGGER_SetLevel(LOGGER_LevelTypeDef Level) {
    if (!hlogger.Initialized) return LOGGER_ERROR_UNINITIALIZED;
    hlogger.ActiveLevel = Level;
    return LOGGER_ERROR_OK;
}

/**
 * @brief Setup peripherals required for logger to function properly. This method is called inside LOGGER_Init()
 * @return 0 -> OK\n 1 -> ERROR
 */
__weak uint8_t LOGGER_InitCB() {return 0;}

/**
 * @brief Use the configured peripherals to output log message
 * @param Event Logger event
 * @return 0 -> OK\n 1 -> ERROR
 */
__weak uint8_t LOGGER_LogCB(LOGGER_EventTypeDef *Event) {return 0;}

/**
 * @brief Handle cases where the error is fatal. This method is called when LOGGER_Log() is called with fatal log.
 * @param Event Logger event
 * @return 0 -> OK\n 1 -> ERROR
 */
__weak uint8_t LOGGER_FatalCB(LOGGER_EventTypeDef *Event) {return 0;}

__weak uint8_t LOGGER_FormatCB(LOGGER_EventTypeDef *Event, char *Buffer, uint16_t Len) {
    return snprintf(Buffer, Len, "%s - %s\n", LOGGER_GetLevelLabel(Event->Level), Event->msg) >= Len;
}

char *LOGGER_GetLevelLabel(LOGGER_LevelTypeDef Level) {
    switch (Level) {
        case LOGGER_LEVEL_DEBUG: return "DEBUG";
        case LOGGER_LEVEL_INFO: return "INFO";
        case LOGGER_LEVEL_WARNING: return "WARNING";
        case LOGGER_LEVEL_ERROR: return "ERROR";
        case LOGGER_LEVEL_FATAL: return "FATAL";
        default: return "UNKNOWN";
    };
}
