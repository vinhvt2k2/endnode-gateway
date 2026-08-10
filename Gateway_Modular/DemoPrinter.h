#ifndef DEMO_PRINTER_H
#define DEMO_PRINTER_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include "DataTypes.h"

void printAllowedNodes();
void printOuterPacketInfo(const String &mac, uint32_t counter, const String &cipherB64, const String &tagB64, int nodeIndex);
void printDecryptedPayload(const String &plainPayload);
void printSensorDataFromDoc(const String &mac, JsonDocument &doc);
void printAckDemo(const String &plainAck, const String &encryptedAck);
void printFirebaseData(const SensorData &data);

#endif
