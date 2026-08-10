#include <Wire.h>
#include "GatewayConfig.h"
#include "Globals.h"
#include "OledManager.h"

void initOLED() {
#if OLED_ENABLE
  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    oledReady = false;
    Serial.println("[-] Khong khoi tao duoc man hinh OLED");
    return;
  }

  oledReady = true;
  display.clearDisplay();
  display.setRotation(OLED_ROTATION);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("GATEWAY LORA");
  display.println("Dang khoi dong...");
  display.display();

  Serial.println("[+] Da khoi tao man hinh OLED");
#else
  Serial.println("[*] OLED dang tat trong cau hinh");
#endif
}

void oledShowStatus(const String &status) {
  oledLastStatus = status;
  oledLastUpdate = millis();

#if OLED_ENABLE
  if (!oledReady) return;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("GATEWAY LORA");

  display.setCursor(0, 12);
  display.println("TT: " + status);

  display.setCursor(0, 24);
  display.println("WiFi: " + String(WiFi.status() == WL_CONNECTED ? "OK" : "LOI"));

  display.setCursor(0, 36);
  display.println("Firebase: " + String(firebaseReady ? "OK" : "CHUA"));

  display.setCursor(0, 48);
  display.println("Queue: " + String(dataQueue.size()));

  display.display();
#endif
}

void oledShowPacket(const String &status, const String &mac, const String &location, int rssi, float snr) {
  oledLastStatus = status;
  oledLastMac = mac;
  oledLastLocation = location;
  oledLastRssi = rssi;
  oledLastSnr = snr;
  oledLastUpdate = millis();

#if OLED_ENABLE
  if (!oledReady) return;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("GATEWAY LORA");

  display.setCursor(0, 10);
  display.println("TT: " + status);

  display.setCursor(0, 20);
  display.println("Node: " + mac);

  display.setCursor(0, 30);
  display.println("Vi tri: " + location);

  display.setCursor(0, 40);
  display.println("RSSI: " + String(rssi) + " dBm");

  display.setCursor(0, 50);
  display.println("SNR: " + String(snr, 1) + " dB Q:" + String(dataQueue.size()));

  display.display();
#endif
}

void oledShowFirebase(const String &status, size_t queueSize) {
  oledLastStatus = status;
  oledLastUpdate = millis();

#if OLED_ENABLE
  if (!oledReady) return;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("GATEWAY LORA");

  display.setCursor(0, 12);
  display.println("TT: " + status);

  display.setCursor(0, 24);
  display.println("Node: " + oledLastMac);

  display.setCursor(0, 36);
  display.println("RSSI:" + String(oledLastRssi) + " SNR:" + String(oledLastSnr, 1));

  display.setCursor(0, 48);
  display.println("Queue: " + String(queueSize));

  display.display();
#endif
}
