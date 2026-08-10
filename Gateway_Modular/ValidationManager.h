#ifndef VALIDATION_MANAGER_H
#define VALIDATION_MANAGER_H
#include <Arduino.h>
#include <ArduinoJson.h>
bool dataValidation(JsonDocument &doc, String &reason);
bool hasNumber(JsonDocument &doc, const char *key);
bool hasAqiField(JsonDocument &doc);
bool hasPreAqiField(JsonDocument &doc);
float readFloat(JsonDocument &doc, const char *key);
int readIntWithAlias(JsonDocument &doc, const char *key1, const char *key2);
bool isInRangeFloat(float value, float minValue, float maxValue);
bool isInRangeInt(int value, int minValue, int maxValue);
#endif
