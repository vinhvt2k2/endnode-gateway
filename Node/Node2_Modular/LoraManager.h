#ifndef LORA_MANAGER_H
#define LORA_MANAGER_H

#include <Arduino.h>

void initLoRa();
void goToSleep();

bool sendLoRaJson(const String &payload);
bool waitForEncryptedAck(const char *ackFor, uint32_t expectedRxCounter, unsigned long timeoutMs);

String buildConfigPayload();
bool performConfigHandshake();

bool sendSecurePayloadWithRetry(const String &plainPayload, const char *ackFor, unsigned long retryDelayMs);

#endif
