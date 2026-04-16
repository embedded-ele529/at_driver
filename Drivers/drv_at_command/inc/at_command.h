#ifndef AT_COMMAND_H_
#define AT_COMMAND_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define AT_COMMAND_DRIVER_SW_VERSION	(1.0)

// Hata Kodları
typedef enum {
    E_AT_ERR_NONE = 0,
    E_AT_ERR_TIMEOUT,
    E_AT_ERR_UNEXPECTED_RESPONSE,
    E_AT_ERR_HW_ERROR,
    E_AT_ERR_WRONG_IOCTL_CMD,
    E_AT_ERR_UNKNOWN
} atCommandErrorCodes_t;

// IOCTL Komutları (Sadece temel ihtiyaçlar)
typedef enum {
    E_AT_IOCTL_NONE = 0,
    E_AT_IOCTL_GET_VERSION,
    E_AT_IOCTL_SEND_CMD           // AT Komutu gönderme ve okuma işlemi
} AT_COMMAND_IOCTL_COMMANDS_T;

// IOCTL "E_AT_IOCTL_SEND_CMD" için vpParam üzerinden iletilecek Struct
typedef struct {
    const char* command;         // Gönderilecek komut (Örn: "AT+CPIN?")
    const char* expected_resp;   // Beklenen cevap (Örn: "OK")
    uint32_t    timeout_ms;      // Maksimum bekleme süresi
    char* out_buffer;      // Dönen cevabı almak istiyorsak (Örn: "+CSQ: 18,99")
    uint16_t    out_buffer_len;  // out_buffer'ın maksimum boyutu
} AtCommandReq_t;

// --- API FONKSİYONLARI --- (Sadece 3 temel fonksiyon)
atCommandErrorCodes_t AtCommand_Open(void* vpParam);

atCommandErrorCodes_t AtCommand_Ioctl(AT_COMMAND_IOCTL_COMMANDS_T eCommand, void* vpParam);

atCommandErrorCodes_t AtCommand_Close(void* vpParam);

#ifdef __cplusplus
}
#endif

#endif /* AT_COMMAND_H_ */
