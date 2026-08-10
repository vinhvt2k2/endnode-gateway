#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <queue>
#include <Firebase_ESP_Client.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "GatewayConfig.h"
#include "DataTypes.h"

extern FirebaseData fbdo;
extern FirebaseAuth auth;
extern FirebaseConfig config;
extern bool firebaseReady;

#if OLED_ENABLE
extern Adafruit_SSD1306 display;
extern bool oledReady;
#endif

extern String oledLastStatus;
extern String oledLastMac;
extern String oledLastLocation;
extern int oledLastRssi;
extern float oledLastSnr;
extern unsigned long oledLastUpdate;

extern NodeInfo allowedNodes[];
extern const int NODE_COUNT;

extern std::queue<SensorData> dataQueue;
extern unsigned long lastServerSendTime;

#endif
