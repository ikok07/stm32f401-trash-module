//
// Created by Kok on 1/30/26.
//

#ifndef CMD_HANDLER_H
#define CMD_HANDLER_H

#include <stdint.h>

typedef enum {
    CHANDLER_ERROR_OK,
    CHANDLER_ERROR_DISABLED,
    CHANDLER_INVALID_ARG,
    CHANDLER_CMD_NOT_FOUND,
    CHANDLER_ERROR_IMPLEMENTATION    // An error occurred in the system-specific implementation
} CHANDLER_ErrorTypeDef;

typedef char*(*CommandMethod)(void);
typedef struct {
    char *id;
    CommandMethod method;
    char *help;
} CHANDLER_CommandTypeDef;

typedef struct {
    uint8_t Enabled;
    CHANDLER_CommandTypeDef *CommandsMap;
    uint8_t Len;
} CHANDLER_StateTypeDef;

/* ------ Main methods ------ */

CHANDLER_ErrorTypeDef CHANDLER_SetCommands(CHANDLER_CommandTypeDef *CommandsMap, uint8_t Len);
CHANDLER_ErrorTypeDef CHANDLER_ParseCmd(char *Text, CHANDLER_CommandTypeDef *OutputCmd);
CHANDLER_ErrorTypeDef CHANDLER_Exec(CHANDLER_CommandTypeDef *Cmd);

/* ------ Controls ------ */

void CHANDLER_Enable();
void CHANDLER_Disable();

/* ------ Callbacks ------ */

uint8_t CHANDLER_ResponseCB(char *response);

#endif //CMD_HANDLER_H
