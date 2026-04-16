#include "at_command.h"
#include "at_command_hal.h"
#include <string.h>
#include "cmsis_os.h"

static uint8_t isDriverOpen = 0;

#define RX_BUFFER_SIZE 256 // Buffer to hold incoming data from the modem
static char rx_buffer[RX_BUFFER_SIZE];
static volatile uint16_t rx_index = 0;
static uint8_t rx_byte;

// Initialize the driver and start listening for incoming data
atCommandErrorCodes_t AtCommand_Open(void* vpParam)
{
    if (isDriverOpen) return E_AT_ERR_NONE;

    // Clean the buffer and reset index
    memset(rx_buffer, 0, RX_BUFFER_SIZE);
    rx_index = 0;

    // Start listening for incoming data (1 byte at a time)
    HAL_UART_Receive_IT(&AT_HW_USART, &rx_byte, 1);

    isDriverOpen = 1;
    return E_AT_ERR_NONE;
}

// Core function to execute an AT command and wait for the response
static atCommandErrorCodes_t ExecuteCommand(AtCommandReq_t* req)
{
    if (req == NULL || req->resp_buffer == NULL) return E_AT_ERR_HW_ERROR;

    // Cleaning
    memset(rx_buffer, 0, RX_BUFFER_SIZE);
    rx_index = 0;
    req->line_count = 0;
    for(int i=0; i < AT_MAX_LINES; i++) req->lines[i] = NULL;

    // Send the AT command to the modem (with CRLF)
    HAL_UART_Transmit(&AT_HW_USART, (uint8_t*)req->command, strlen(req->command), 100);
    HAL_UART_Transmit(&AT_HW_USART, (uint8_t*)"\r\n", 2, 100);

    // Wait for the expected response or timeout
    uint32_t start_tick = HAL_GetTick();
    atCommandErrorCodes_t result = E_AT_ERR_TIMEOUT;

    while ((HAL_GetTick() - start_tick) < req->timeout_ms) {
        if (strstr(rx_buffer, req->expected_resp) != NULL) {
            result = E_AT_ERR_NONE;
            break;
        }
        if (strstr(rx_buffer, "ERROR") != NULL) {
            result = E_AT_ERR_UNEXPECTED_RESPONSE;
            break;
        }
        osDelay(10); // Release CPU to other tasks while waiting
    }

    // Parsing
    if (result == E_AT_ERR_NONE) {
        strncpy(req->resp_buffer, rx_buffer, req->resp_buffer_len);

        // Remove CRLF and split into lines
        char* token = strtok(req->resp_buffer, "\r\n");
        while (token != NULL && req->line_count < AT_MAX_LINES) {

            // Exclude the command echo and empty lines
            if (strstr(token, req->command) == NULL) {
                req->lines[req->line_count++] = token;
            }
            token = strtok(NULL, "\r\n");
        }
    }

    return result;
}

// Handle IOCTL commands from the application
atCommandErrorCodes_t AtCommand_Ioctl(AT_COMMAND_IOCTL_COMMANDS_T eCommand, void* vpParam)
{
    if (!isDriverOpen) return E_AT_ERR_HW_ERROR;

    switch (eCommand) {
        case E_AT_IOCTL_SEND_CMD:
            return ExecuteCommand((AtCommandReq_t*)vpParam);

        default:
            return E_AT_ERR_WRONG_IOCTL_CMD;
    }

    return E_AT_ERR_NONE;
}

// Stop listening
atCommandErrorCodes_t AtCommand_Close(void* vpParam)
{
    isDriverOpen = 0;
    HAL_UART_AbortReceive_IT(&AT_HW_USART);
    return E_AT_ERR_NONE;
}

// INTERRUPT CALLBACK)
// This function is called by the HAL when a byte is received over UART.
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == AT_HW_USART.Instance) {

        // Add the received byte to the buffer if there's space
        if (rx_index < RX_BUFFER_SIZE - 1) {
            rx_buffer[rx_index] = (char)rx_byte;
            rx_index++;
            rx_buffer[rx_index] = '\0';
        }

        // Continue listening for the next byte
        HAL_UART_Receive_IT(&AT_HW_USART, &rx_byte, 1);
    }
}
