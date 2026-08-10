#include "Globals.h"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool firebaseReady = false;

#if OLED_ENABLE
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RST);
bool oledReady = false;
#endif

String oledLastStatus = "Dang khoi dong";
String oledLastMac = "-";
String oledLastLocation = "-";
int oledLastRssi = 0;
float oledLastSnr = 0.0;
unsigned long oledLastUpdate = 0;

NodeInfo allowedNodes[] = {
  {
    "NODE_01",
    "location1",
    {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00},
    0,
    true
  },
  {
    "NODE_02",
    "location2",
    {0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x20},
    0,
    true
  }
};

extern const int NODE_COUNT = sizeof(allowedNodes) / sizeof(allowedNodes[0]);

std::queue<SensorData> dataQueue;
unsigned long lastServerSendTime = 0;
