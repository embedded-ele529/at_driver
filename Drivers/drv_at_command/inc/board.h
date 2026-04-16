#ifndef BOARD_H_
#define BOARD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32f4xx_hal.h"

// --- AT COMMAND MODEM BOARD DEFINITIONS ---
// Modemin bağlı olduğu UART portu (Örnek olarak USART3 diyelim)
#define AT_MODEM_USART_INSTANCE    USART3
#define AT_MODEM_BAUDRATE          115200


#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
