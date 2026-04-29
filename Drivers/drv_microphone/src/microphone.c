#include "microphone.h"
#include "microphone_hal.h"
#include "pdm2pcm_glo.h"   // ST'nin PDM'den PCM'e dönüştürme kütüphanesi
#include <string.h>

/* --- İÇ DEĞİŞKENLER VE TAMPONLAR --- */
static uint8_t isMicOpen = 0;

// SES TANIMA İÇİN HESAPLAMALAR:
// 1 milisaniyelik ses (16 kHz frekansta) = 16 adet PCM örneği demektir.
// 16 adet PCM üretmek için kütüphane bizden 1024 bit (yani 64 adet 16-bit PDM verisi) ister.
// Ping-Pong (Çift Tampon) mantığı için bunu 2 ile çarpıyoruz: Toplam 128
#define PDM_BUFFER_SIZE 128
static uint16_t pdm_rx_buffer[PDM_BUFFER_SIZE];

// PDM2PCM Kütüphanesi Değişkenleri
static PDM_Filter_Handler_t PDM_FilterHandler[1];
static PDM_Filter_Config_t PDM_FilterConfig[1];

static volatile uint8_t data_half_ready = 0;
static volatile uint8_t data_full_ready = 0;

/* --- 1. SÜRÜCÜYÜ BAŞLATMA --- */
micErrorCodes_t Microphone_Open(void* vpParam)
{
    if (isMicOpen) return E_MIC_ERR_NONE;

    memset(pdm_rx_buffer, 0, sizeof(pdm_rx_buffer));
    data_half_ready = 0;
    data_full_ready = 0;

    // --- PDM FİLTRE KURULUMU (SİHİRLİ KISIM) ---
    // 1. İşleyici Ayarları
    PDM_FilterHandler[0].bit_order = PDM_FILTER_BIT_ORDER_LSB;
    PDM_FilterHandler[0].endianness = PDM_FILTER_ENDIANNESS_LE;
    PDM_FilterHandler[0].high_pass_tap = 2122358088; // Sesteki patlamaları/uğultuları kesen DC filtresi
    PDM_FilterHandler[0].out_ptr_channels = 1;       // Mono ses
    PDM_FilterHandler[0].in_ptr_channels = 1;        // Tek mikrofon
    PDM_Filter_Init(&PDM_FilterHandler[0]);

    // 2. Dönüşüm Konfigürasyonu
    PDM_FilterConfig[0].output_samples_number = 16;  // Her okumada 1 ms'lik (16 sample) ses çıkar
    PDM_FilterConfig[0].mic_gain = 24;               // Mikrofonun kazancı/ses seviyesi (Gerekirse artırılır)
    PDM_FilterConfig[0].decimation_factor = PDM_FILTER_DEC_FACTOR_64; // 16 kHz için 64x seyreltme
    PDM_Filter_setConfig(&PDM_FilterHandler[0], &PDM_FilterConfig[0]);

    // I2S'i başlat ve mikrofondan DMA ile PDM akışını başlat
    if (HAL_I2S_Receive_DMA(&MIC_HW_I2S, pdm_rx_buffer, PDM_BUFFER_SIZE) != HAL_OK) {
        return E_MIC_ERR_HW_ERROR;
    }

    isMicOpen = 1;
    return E_MIC_ERR_NONE;
}

/* --- 2. AYARLAR (IOCTL) --- */
micErrorCodes_t Microphone_Ioctl(MIC_IOCTL_COMMANDS_T eCommand, void* vpParam)
{
    if (!isMicOpen) return E_MIC_ERR_HW_ERROR;

    switch (eCommand) {
        case E_MIC_IOCTL_PAUSE:
            HAL_I2S_DMAPause(&MIC_HW_I2S);
            break;
        case E_MIC_IOCTL_RESUME:
            HAL_I2S_DMAResume(&MIC_HW_I2S);
            break;
        case E_MIC_IOCTL_GET_VERSION:
            if (vpParam != NULL) { *((float*)vpParam) = MICROPHONE_DRIVER_SW_VERSION; }
            break;
        default:
            return E_MIC_ERR_WRONG_IOCTL_CMD;
    }
    return E_MIC_ERR_NONE;
}

/* --- 3. VERİ OKUMA (READ) --- */
micErrorCodes_t Microphone_Read(int16_t* pBuffer, uint16_t size)
{
    if (!isMicOpen) return E_MIC_ERR_HW_ERROR;
    if (pBuffer == NULL) return E_MIC_ERR_BUFFER_NULL;

    uint8_t *pdm_ptr = NULL;

    // Hangi yarı dolduysa onun başlangıç adresini al
    if (data_half_ready) {
        pdm_ptr = (uint8_t*)&pdm_rx_buffer[0];
        data_half_ready = 0;
    }
    else if (data_full_ready) {
        pdm_ptr = (uint8_t*)&pdm_rx_buffer[PDM_BUFFER_SIZE / 2];
        data_full_ready = 0;
    }

    // Eğer yeni bir veri geldiyse, filtreye sok
    if (pdm_ptr != NULL) {
        // PDM verisini al, pBuffer içine gerçek PCM sesi olarak (16 adet) yaz!
        PDM_Filter(pdm_ptr, (uint16_t*)pBuffer, &PDM_FilterHandler[0]);
        return E_MIC_ERR_NONE;
    }

    // Yeni veri yoksa beklemesini söyle
    return E_MIC_ERR_UNKNOWN;
}

/* --- 4. SÜRÜCÜYÜ KAPATMA --- */
micErrorCodes_t Microphone_Close(void* vpParam)
{
    HAL_I2S_DMAStop(&MIC_HW_I2S);
    isMicOpen = 0;
    return E_MIC_ERR_NONE;
}

/* --- 5. DMA KESMELERİ --- */
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == MIC_HW_I2S.Instance) data_half_ready = 1;
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == MIC_HW_I2S.Instance) data_full_ready = 1;
}
