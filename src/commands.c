//
// Created by Kok on 1/30/26.
//

#include "commands.h"

#include <stdio.h>
#include <string.h>

#include "app_config.h"
#include "app_state.h"
#include "log.h"
#include "tests.h"
#include "tests_handler.h"

static uint8_t *gRxBuffer;
static uint16_t gRxLen;

void handle_test_all(char *Buf, uint16_t Len);
void handle_help(char *Buf, uint16_t Len);

static char gResponseBuffer[RESPONSE_BUFFER_LEN];
static uint8_t gCommandsEnabled = 0;

CHANDLER_CommandTypeDef gSupportedCommands[TOTAL_COMMANDS_COUNT] = {
    {.id = "test-all", .method = handle_test_all, .Buf = gResponseBuffer, .BufLen = RESPONSE_BUFFER_LEN, .help = "Runs all peripheral tests"},
    {.id = "help", .method = handle_help, .Buf = gResponseBuffer, .BufLen = RESPONSE_BUFFER_LEN, .help = "Get information about supported commands"}
};

uint8_t check_cmd_enabled() {
    // Enable clock access
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Config enable BTN
    GPIO_InitTypeDef GPIO_Config = {
        .Pin = COMMANDS_ENABLE_PIN,
        .Mode = GPIO_MODE_INPUT,
        .Pull = GPIO_PULLDOWN,
        .Speed = GPIO_SPEED_LOW,
    };
    HAL_GPIO_Init(COMMANDS_ENABLE_PORT, &GPIO_Config);

    return HAL_GPIO_ReadPin(COMMANDS_ENABLE_PORT, COMMANDS_ENABLE_PIN);
}

/**
 * @warning To enable commands reception the COMMANDS_ENABLE PORT should be HIGH
 */
HAL_StatusTypeDef COMMANDS_ConfigRx(uint8_t *RxBuffer, uint16_t Len) {
    // Check if commands are enabled
    gCommandsEnabled = check_cmd_enabled();
    if (!gCommandsEnabled) return HAL_OK;

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
    if (!gCommandsEnabled) return HAL_OK;
    return HAL_UARTEx_ReceiveToIdle_DMA(app_state.pUARTHandle, gRxBuffer, gRxLen);
}

void COMMANDS_Execute(char *text) {
    if (!gCommandsEnabled) return;

    CHANDLER_CommandTypeDef cmd;
    if (CHANDLER_ParseCmd(text, &cmd) == CHANDLER_ERROR_OK) {
        if (CHANDLER_Exec(&cmd) != CHANDLER_ERROR_OK) {
            LOGGER_Log(LOGGER_LEVEL_ERROR, "Command handler failed to execute command's method!");
        }
    } else {
        LOGGER_Log(LOGGER_LEVEL_ERROR, "Command handler failed to parse provided command!");
    };

    if (COMMANDS_StartRx() != HAL_OK) {
        LOGGER_Log(LOGGER_LEVEL_ERROR, "Failed to restart UART command reception!");
    };
}

uint8_t COMMANDS_Enabled() {
    return gCommandsEnabled;
}

uint8_t CHANDLER_ResponseCB(CHANDLER_CommandTypeDef *Cmd) {
    HAL_StatusTypeDef status = HAL_UART_Transmit(
        app_state.pUARTHandle,
        (uint8_t*)Cmd->Buf,
        strlen(Cmd->Buf) + 1,
        CMD_RESP_USART_TX_TIMEOUT
    );

    if (status != HAL_OK) return 1;
    return 0;
}

void handle_test_all(char *Buf, uint16_t Len) {
    memset(Buf, 0, Len);
    char *ptr = Buf;
    char *end = Buf + Len - 1;

    ptr += snprintf(ptr, end - ptr, "------ START TESTING PERIPHERALS ------\n");

    THANDLER_CaseTypeDef *test_cases;
    uint16_t test_cases_len;
    TESTS_GetCases(&test_cases, &test_cases_len);

    THANDLER_RunTests(test_cases, test_cases_len);

    for (int i = 0; i < test_cases_len; i++) {
        if (test_cases[i].Result != TEST_RESULT_PASS) {
            ptr += snprintf(ptr, end - ptr, "%d - Test '%s' didn't pass!\n", i + 1, test_cases[i].Name);
        } else {
            ptr += snprintf(ptr, end - ptr, "%d - Test '%s' passed successfully!\n", i + 1, test_cases[i].Name);
        }
    }

    snprintf(ptr, end - ptr, "------ FINISHED TESTING PERIPHERALS ------\n");
}

void handle_help(char *Buf, uint16_t Len) {
    memset(Buf, 0, Len);

    int offset = 0;
    int count = sizeof(gSupportedCommands) / sizeof(gSupportedCommands[0]);

    for (int i = 0; i < count; i++) {
        uint16_t written = snprintf(
            Buf + offset,
            Len - offset,
            "%s - %s\n",
            gSupportedCommands[i].id, gSupportedCommands[i].help
        );
        if (written < 0 || offset + written >= sizeof(Buf)) {
            break;
        }
        offset += written;
    }
}
