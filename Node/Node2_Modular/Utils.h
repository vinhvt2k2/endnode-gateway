#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include "NodeConfig.h"

bool isValidFloat(float value);
bool inRangeFloat(float value, float minValue, float maxValue);
bool inRangeInt(int value, int minValue, int maxValue);

void printSensorData(const SensorData &data);
void printSecurityPacketInfo(const String &plainPayload, const String &encryptedPacket, uint32_t txCounter, const char *ackFor);
void printWakeupReason();

#endif
