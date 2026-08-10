#include "Globals.h"

String macAddress = "NODE_01";

extern const uint8_t NODE_AES_KEY[AES_KEY_SIZE] = {
  0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00
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
