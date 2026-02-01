//
// Created by Kok on 1/30/26.
//

#include "cmd_handler.h"

#include <stddef.h>
#include <string.h>

#include "stm32f4xx_hal.h"

static CHANDLER_StateTypeDef hchandler;

/**
 * @brief This methods sets the supported commands. Any other commands will be discarded with error.
 * @param CommandsMap Array of commands
 * @param Len The length of the commands array
 */
CHANDLER_ErrorTypeDef CHANDLER_SetCommands(CHANDLER_CommandTypeDef *CommandsMap, uint8_t Len) {
    if (CommandsMap == NULL || Len <= 0) return CHANDLER_INVALID_ARG;

    hchandler.CommandsMap = CommandsMap;
    hchandler.Len = Len;
    return CHANDLER_ERROR_OK;
}

/**
 * @brief This method gets the command structure from the commands map. If there is not match found, it returns error
 * @param Text Input text containing command
 * @param OutputCmd Command structure
 */
CHANDLER_ErrorTypeDef CHANDLER_ParseCmd(char *Text, CHANDLER_CommandTypeDef *OutputCmd) {
    for (int i = 0; i < hchandler.Len; i++) {
        if (strcmp(hchandler.CommandsMap[i].id, Text) == 0) {
            *OutputCmd = hchandler.CommandsMap[i];
            return CHANDLER_ERROR_OK;
        }
    }

    return CHANDLER_CMD_NOT_FOUND;
}

/**
 * @brief This method executes the method inside the command structure and outputs the response through the CHANDLER_ResponseCB()
 * @param Cmd Command structure
 */
CHANDLER_ErrorTypeDef CHANDLER_Exec(CHANDLER_CommandTypeDef *Cmd) {
    if (Cmd == NULL) return CHANDLER_INVALID_ARG;
    if (!hchandler.Enabled) return CHANDLER_ERROR_DISABLED;

    Cmd->method(Cmd->Buf, Cmd->BufLen);

    if (CHANDLER_ResponseCB(Cmd) != 0) return CHANDLER_ERROR_IMPLEMENTATION;
    return CHANDLER_ERROR_OK;
}

void CHANDLER_Enable() {
    hchandler.Enabled = 1;
}

void CHANDLER_Disable() {
    hchandler.Enabled = 0;
}

/**
 * @brief This callback should transmit the response to the user using peripheral of choice
 * @param response Response content to be transmitted
 * @return 0 -> OK\n 1 -> ERROR
 */
__weak uint8_t CHANDLER_ResponseCB(CHANDLER_CommandTypeDef *Cmd) {return 0;}
