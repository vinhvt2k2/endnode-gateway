#ifndef LORA_MANAGER_H
#define LORA_MANAGER_H
#include <Arduino.h>
#include <ArduinoJson.h>

void initLoRa();
void handleLoRaPacket();
void processConfig(const String &mac, uint32_t counter, JsonDocument &plainDoc, bool duplicate);
void processValue(const String &mac, uint32_t counter, JsonDocument &plainDoc, bool duplicate);
void sendAckEncrypted(const String &mac, const String &ackFor, uint32_t rxCounter);

#endif
