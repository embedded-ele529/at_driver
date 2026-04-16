#ifndef AT_COMMAND_HAL_H_
#define AT_COMMAND_HAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <board.h>

// Sürücünün kullanacağı donanım makrolarını board.h'a bağlar
#define AT_HW_USART         AT_MODEM_USART_INSTANCE
#define AT_HW_BAUDRATE      AT_MODEM_BAUDRATE

// HAL kütüphanesindeki UART Handle'ı extern olarak alıyoruz (main.c'de tanımlı olan)
extern UART_HandleTypeDef huart3; // Kendi UART handle'ın neyse onu yaz (örn: huart2, huart3)

#ifdef __cplusplus
}
#endif

#endif /* AT_COMMAND_HAL_H_ */
