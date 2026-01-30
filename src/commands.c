//
// Created by Kok on 1/30/26.
//

#include "commands.h"

#include <stdio.h>
#include <string.h>

#include "app_config.h"
#include "app_state.h"

static char gHelpBuffer[1024];
static uint8_t *gRxBuffer;
static uint16_t gRxLen;

char *handle_help();

CHANDLER_CommandTypeDef gSupportedCommands[TOTAL_COMMANDS_COUNT] = {
    {.id = "help", .method = handle_help, .help = "Get information about supported commands"}
};

HAL_StatusTypeDef COMMANDS_ConfigRx(uint8_t *RxBuffer, uint16_t Len) {
    CHANDLER_SetCommands(gSupportedCommands, TOTAL_COMMANDS_COUNT);
    CHANDLER_Enable();

    assert(RxBuffer != NULL);
    assert(Len > 0);

    gRxBuffer = RxBuffer;
    gRxLen = Len;

    __HAL_RCC_DMA2_CLK_ENABLE();

    // Configure DMA
    app_state.pDMAUART1RXHandle->Instance = DMA2_Stream2;
    app_state.pDMAUART1RXHandle->Init = (DMA_InitTypeDef) {
        .Mode = DMA_NORMAL,
        .Direction = DMA_PERIPH_TO_MEMORY,
        .FIFOMode = DMA_FIFOMODE_DISABLE,
        .MemInc = DMA_MINC_ENABLE,
        .MemDataAlignment = DMA_MDATAALIGN_BYTE,
        .PeriphInc = DMA_PINC_DISABLE,
        .PeriphDataAlignment =DMA_PDATAALIGN_BYTE,
        .Priority = DMA_PRIORITY_HIGH,
        .Channel = DMA_CHANNEL_4
    };

    HAL_StatusTypeDef status;
    if ((status = HAL_DMA_Init(app_state.pDMAUART1RXHandle)) != HAL_OK) return status;

    __HAL_LINKDMA(app_state.pUARTHandle, hdmarx, *app_state.pDMAUART1RXHandle);

    // Enable DMA interrupts
    HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
    HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, CUSTOM_DMA_UART1RX_INT_PRIORITY, 0);

    return status;
}

HAL_StatusTypeDef COMMANDS_StartRx() {
    return HAL_UARTEx_ReceiveToIdle_DMA(app_state.pUARTHandle, gRxBuffer, gRxLen);
}

uint8_t CHANDLER_ResponseCB(char *response) {
    HAL_StatusTypeDef status = HAL_UART_Transmit(
        app_state.pUARTHandle,
        (uint8_t*)response,
        strlen(response) + 1,
        CMD_RESP_USART_TX_TIMEOUT
    );

    if (status != HAL_OK) return 1;
    return 0;
}

char *handle_help() {
    gHelpBuffer[0] = '\0';
    int offset = 0;
    int count = sizeof(gSupportedCommands) / sizeof(gSupportedCommands[0]);

    for (int i = 0; i < count; i++) {
        uint16_t written = snprintf(
            gHelpBuffer + offset,
            sizeof(gHelpBuffer) - offset,
            "%s - %s\n",
            gSupportedCommands[i].id, gSupportedCommands[i].help
        );
        if (written < 0 || offset + written >= sizeof(gHelpBuffer)) {
            break;
        }
        offset += written;
    }

    return gHelpBuffer;
}
