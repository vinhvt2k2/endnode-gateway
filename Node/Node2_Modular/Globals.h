#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <Preferences.h>
#include "DHT.h"
#include "SdsDustSensor.h"
#include <KalmanFilter.h>
#include <VN_AQI.h>
#include "NodeConfig.h"

extern String macAddress;
extern const uint8_t NODE_AES_KEY[AES_KEY_SIZE];

extern Preferences preferences;

extern DHT dht;
extern SdsDustSensor sds;
extern KalmanFilter filterMQ7;
extern KalmanFilter filterUV;
extern VN_AQI aqiCalculator;

extern bool configConfirmed;
extern uint32_t securityCounter;

extern float array_pm25[12];
extern float array_pm10[12];
extern float aqi_history_features[3];
extern int lastAQI;
extern int lastPreAQI;

#endif
