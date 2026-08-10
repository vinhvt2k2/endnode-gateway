#ifndef OLED_MANAGER_H
#define OLED_MANAGER_H
#include <Arduino.h>
void initOLED();
void oledShowStatus(const String &status);
void oledShowPacket(const String &status, const String &mac, const String &location, int rssi, float snr);
void oledShowFirebase(const String &status, size_t queueSize);
#endif
