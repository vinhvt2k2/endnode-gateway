#include "GatewayConfig.h"
#include "Globals.h"
#include "NetworkManager.h"
#include "LoraManager.h"
#include "OledManager.h"
#include "DemoPrinter.h"
#include "FirebaseManager.h"

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n=================================================");
  Serial.println(" KHOI DONG GATEWAY - CHE DO BAO MAT CONFIG/VALUE ");
  Serial.println(" AES-128-CCM + khoa rieng tung Node + Counter ");
  Serial.println("=================================================");

  initOLED();
  oledShowStatus("Dang ket noi WiFi");

  initWiFi();
  oledShowStatus("Dang khoi tao Firebase");

  initFirebase();
  oledShowStatus("Dang khoi tao LoRa");

  initLoRa();
  printAllowedNodes();

  lastServerSendTime = millis();

  oledShowStatus("San sang nhan LoRa");
  Serial.println("\n[*] Gateway da san sang. Dang lang nghe goi LoRa da ma hoa...");
}

void loop() {
  handleLoRaPacket();

  if ((millis() - lastServerSendTime) >= SERVER_SEND_INTERVAL_MS) {
    sendToServer();
    lastServerSendTime = millis();
  }
}
