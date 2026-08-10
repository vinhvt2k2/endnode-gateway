#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include "NodeConfig.h"

float getCO_mg_m3(float filtered_adc);
float getUVIndex(float filtered_adc);
void getFilteredDHT22(float &tempOut, float &humOut);
void updatePmHistory(float pm25, float pm10);
void updateAqiHistory(int currentAqi);

bool validateSensorData(const SensorData &data);
bool readSensorData(SensorData &data);
bool readValidSensorData(SensorData &data);
String buildValuePayload(const SensorData &data);

#endif
