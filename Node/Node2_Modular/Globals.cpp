#include "Globals.h"

String macAddress = "NODE_02";

extern const uint8_t NODE_AES_KEY[AES_KEY_SIZE] = {
  0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x20
};

Preferences preferences;

DHT dht(DHTPIN, DHTTYPE);
SdsDustSensor sds(Serial2);
KalmanFilter filterMQ7(20.0, 20.0, 0.1);
KalmanFilter filterUV(10.0, 10.0, 0.01);
VN_AQI aqiCalculator;

RTC_DATA_ATTR bool configConfirmed = false;
RTC_DATA_ATTR uint32_t securityCounter = 0;

RTC_DATA_ATTR float array_pm25[12] = {
  -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
  -1.0, -1.0, -1.0, -1.0, -1.0, -1.0
};

RTC_DATA_ATTR float array_pm10[12] = {
  -1.0, -1.0, -1.0, -1.0, -1.0, -1.0,
  -1.0, -1.0, -1.0, -1.0, -1.0, -1.0
};

RTC_DATA_ATTR float aqi_history_features[3] = {50.0, 50.0, 50.0};
RTC_DATA_ATTR int lastAQI = 50;
RTC_DATA_ATTR int lastPreAQI = 50;
