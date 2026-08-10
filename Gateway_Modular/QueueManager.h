#ifndef QUEUE_MANAGER_H
#define QUEUE_MANAGER_H
#include <ArduinoJson.h>
#include <Arduino.h>
void enqueueSensorData(const String &mac, JsonDocument &doc);
#endif
