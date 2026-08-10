#ifndef NODE_CONFIG_H
#define NODE_CONFIG_H

#include <Arduino.h>

// ================= CAU HINH CHAN =================
#define DHTPIN   4
#define DHTTYPE  DHT22
#define RX2_PIN  16
#define TX2_PIN  17
#define MQ7_PIN  34
#define UV_PIN   35

#define LORA_SCK   18
#define LORA_MISO  19
#define LORA_MOSI  23
#define LORA_SS    5
#define LORA_RST   14
#define LORA_DIO0  26

// ================= CAU HINH GIAO THUC LORA =================
#define LORA_FREQUENCY        433E6
#define LORA_SYNC_WORD        0xF3
#define LORA_SPREADING_FACTOR 7
#define LORA_BANDWIDTH        125E3
#define LORA_CODING_RATE      5
#define LORA_TX_POWER         20

#define COUNT_RETRY           20
#define ACK_TIMEOUT_MS        10000UL
#define RETRY_DELAY_MS        1000UL
#define CONFIG_RETRY_DELAY_MS 3000UL

// Dang demo: sleep 5 phut. Khi chay that co the doi thanh 15.
#define TIME_SLEEP_MINUTES    5
#define SLEEP_TIME_US         ((uint64_t)TIME_SLEEP_MINUTES * 60ULL * 1000000ULL)

#define SENSOR_READ_RETRY     3
#define SENSOR_RETRY_DELAY_MS 2000UL

// ================= CAU HINH BAO MAT =================
#define AES_KEY_SIZE       16
#define CCM_TAG_SIZE       16
#define CCM_NONCE_SIZE     12
#define MAX_PLAIN_SIZE     512
#define MAX_CIPHER_SIZE    512
#define MAX_B64_SIZE       768

#define DIR_UPLINK         'U'   // Node -> Gateway
#define DIR_DOWNLINK       'D'   // Gateway -> Node

// ================= NGUONG VALIDATION TRUOC KHI GUI =================
#define TEMP_MIN   0.0f
#define TEMP_MAX   60.0f
#define HUM_MIN    0.0f
#define HUM_MAX    100.0f
#define CO_MIN     0.0f
#define CO_MAX     1000.0f
#define PM25_MIN   0.0f
#define PM25_MAX   1000.0f
#define PM10_MIN   0.0f
#define PM10_MAX   1000.0f
#define UV_MIN     0.0f
#define UV_MAX     15.0f
#define AQI_MIN    0
#define AQI_MAX    500

#define VOLTAGE_REF     3.3f
#define ADC_RESOLUTION  4095.0f

struct SensorData {
  float temperature;
  float humidity;
  float co;
  float pm25;
  float pm10;
  float uvIndex;
  int aqIndex;
  int preAQI;
};

#endif
