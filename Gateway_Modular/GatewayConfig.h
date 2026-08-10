#ifndef GATEWAY_CONFIG_H
#define GATEWAY_CONFIG_H

#include <Arduino.h>

#define WIFI_SSID       ""
#define WIFI_PASSWORD   ""
#define DATABASE_URL    ""
#define API_KEY         ""


#define LORA_SCK        5
#define LORA_MISO       19
#define LORA_MOSI       27
#define LORA_SS         18
#define LORA_RST        23
#define LORA_DIO0       26

#define LORA_FREQUENCY  433E6
#define LORA_SYNC_WORD  0xF3
#define LORA_SF         7
#define LORA_BW         125E3
#define LORA_CR         5

// =========================================================
// OLED CONFIG
// =========================================================
#define OLED_ENABLE      1
#define OLED_WIDTH       128
#define OLED_HEIGHT      64
#define OLED_SDA         21
#define OLED_SCL         22
#define OLED_RST         -1
#define OLED_ADDR        0x3C
#define OLED_ROTATION    0

#define SERVER_SEND_INTERVAL_MS  15000UL
#define MAX_QUEUE_SIZE           50
#define MAX_HISTORY_RECORDS      12


#define AES_KEY_SIZE       16
#define CCM_TAG_SIZE       16
#define CCM_NONCE_SIZE     12
#define MAX_PLAIN_SIZE     512
#define MAX_CIPHER_SIZE    512
#define MAX_B64_SIZE       768

#define DIR_UPLINK         'U'   // Node -> Gateway
#define DIR_DOWNLINK       'D'   // Gateway -> Node

#define TEMP_MIN        0.0f
#define TEMP_MAX        60.0f
#define HUM_MIN         0.0f
#define HUM_MAX         100.0f
#define CO_MIN          0.0f
#define CO_MAX          1000.0f
#define PM25_MIN        0.0f
#define PM25_MAX        1000.0f
#define PM10_MIN        0.0f
#define PM10_MAX        1000.0f
#define UV_MIN          0.0f
#define UV_MAX          15.0f
#define AQI_MIN         0
#define AQI_MAX         500

#endif
