#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <Arduino.h>
#include "GatewayConfig.h"

struct NodeInfo {
  String mac;
  String location;
  uint8_t key[AES_KEY_SIZE];
  uint32_t lastCounter;
  bool enabled;
};

struct SensorData {
  String mac;
  String location;
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
