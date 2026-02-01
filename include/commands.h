//
// Created by Kok on 1/30/26.
//

#ifndef COMMANDS_H
#define COMMANDS_H

#include "cmd_handler.h"
#include "stm32f4xx_hal.h"

#define TOTAL_COMMANDS_COUNT            3
#define CMD_RESP_USART_TX_TIMEOUT       2000

#define RESPONSE_BUFFER_LEN             1024

#define COMMANDS_ENABLE_PORT            GPIOA
#define COMMANDS_ENABLE_PIN             GPIO_PIN_11

extern CHANDLER_CommandTypeDef gSupportedCommands[TOTAL_COMMANDS_COUNT];

HAL_StatusTypeDef COMMANDS_ConfigRx(uint8_t *RxBuffer, uint16_t Len);
HAL_StatusTypeDef COMMANDS_StartRx();
void COMMANDS_Execute(char *text);
uint8_t COMMANDS_Enabled();

#endif //COMMANDS_H
