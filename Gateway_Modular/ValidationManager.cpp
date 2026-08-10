#include <math.h>
#include "GatewayConfig.h"
#include "ValidationManager.h"

bool dataValidation(JsonDocument &doc, String &reason) {
  const char *requiredFields[] = {"TEMP", "HUM", "CO", "PM25", "PM10", "UV"};
  const int requiredCount = sizeof(requiredFields) / sizeof(requiredFields[0]);

  for (int i = 0; i < requiredCount; i++) {
    if (!hasNumber(doc, requiredFields[i])) {
      reason = String("Thieu hoac sai truong: ") + requiredFields[i];
      return false;
    }
  }

  if (!hasAqiField(doc)) {
    reason = "Thieu AQI/AQIndex";
    return false;
  }

  if (!hasPreAqiField(doc)) {
    reason = "Thieu PRED_AQI/PreAQI";
    return false;
  }

  float temp = readFloat(doc, "TEMP");
  float hum  = readFloat(doc, "HUM");
  float co   = readFloat(doc, "CO");
  float pm25 = readFloat(doc, "PM25");
  float pm10 = readFloat(doc, "PM10");
  float uv   = readFloat(doc, "UV");
  int aqi    = readIntWithAlias(doc, "AQI", "AQIndex");
  int preAqi = readIntWithAlias(doc, "PRED_AQI", "PreAQI");

  if (!(temp > TEMP_MIN && temp <= TEMP_MAX)) {
    reason = "TEMP ngoai nguong";
    return false;
  }
  if (!isInRangeFloat(hum, HUM_MIN, HUM_MAX)) {
    reason = "HUM ngoai nguong";
    return false;
  }
  if (!isInRangeFloat(co, CO_MIN, CO_MAX)) {
    reason = "CO ngoai nguong";
    return false;
  }
  if (!isInRangeFloat(pm25, PM25_MIN, PM25_MAX)) {
    reason = "PM25 ngoai nguong";
    return false;
  }
  if (!isInRangeFloat(pm10, PM10_MIN, PM10_MAX)) {
    reason = "PM10 ngoai nguong";
    return false;
  }
  if (!isInRangeFloat(uv, UV_MIN, UV_MAX)) {
    reason = "UV ngoai nguong";
    return false;
  }
  if (!isInRangeInt(aqi, AQI_MIN, AQI_MAX)) {
    reason = "AQI/AQIndex ngoai nguong";
    return false;
  }
  if (!isInRangeInt(preAqi, AQI_MIN, AQI_MAX)) {
    reason = "PRED_AQI/PreAQI ngoai nguong";
    return false;
  }

  return true;
}

bool hasNumber(JsonDocument &doc, const char *key) {
  if (doc[key].isNull()) return false;
  return doc[key].is<float>() || doc[key].is<int>() || doc[key].is<double>();
}

bool hasAqiField(JsonDocument &doc) {
  return hasNumber(doc, "AQI") || hasNumber(doc, "AQIndex");
}

bool hasPreAqiField(JsonDocument &doc) {
  return hasNumber(doc, "PRED_AQI") || hasNumber(doc, "PreAQI");
}

float readFloat(JsonDocument &doc, const char *key) {
  return doc[key].as<float>();
}

int readIntWithAlias(JsonDocument &doc, const char *key1, const char *key2) {
  if (!doc[key1].isNull()) return doc[key1].as<int>();
  return doc[key2].as<int>();
}

bool isInRangeFloat(float value, float minValue, float maxValue) {
  if (isnan(value)) return false;
  return value >= minValue && value <= maxValue;
}

bool isInRangeInt(int value, int minValue, int maxValue) {
  return value >= minValue && value <= maxValue;
}
