//
// Created by Kok on 1/30/26.
//

#ifndef COMMANDS_H
#define COMMANDS_H

#include "cmd_handler.h"
#include "stm32f4xx_hal.h"

#define TOTAL_COMMANDS_COUNT            1
#define CMD_RESP_USART_TX_TIMEOUT       2000

extern CHANDLER_CommandTypeDef gSupportedCommands[TOTAL_COMMANDS_COUNT];

HAL_StatusTypeDef COMMANDS_ConfigRx(uint8_t *RxBuffer, uint16_t Len);
HAL_StatusTypeDef COMMANDS_StartRx();

#endif //COMMANDS_H
