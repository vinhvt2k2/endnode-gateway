#include "LoraManager.h"
#include "NodeConfig.h"
#include "Globals.h"
#include "SecurityManager.h"
#include "NodeState.h"
#include "Utils.h"

#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>
#include "esp_sleep.h"

void initLoRa() {
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("[LOI] Khoi tao LoRa that bai!");
    while (true) delay(1000);
  }

  LoRa.setSyncWord(LORA_SYNC_WORD);
  LoRa.setSpreadingFactor(LORA_SPREADING_FACTOR);
  LoRa.setSignalBandwidth(LORA_BANDWIDTH);
  LoRa.setCodingRate4(LORA_CODING_RATE);
  LoRa.enableCrc();
  LoRa.setTxPower(LORA_TX_POWER);

  Serial.println("[OK] Khoi tao LoRa thanh cong");
}

void goToSleep() {
  Serial.println("\n[*] Node chuyen sang deep sleep trong " + String(TIME_SLEEP_MINUTES) + " phut...");
  Serial.flush();

  LoRa.sleep();
  delay(100);

  preferences.end();

  esp_sleep_enable_timer_wakeup(SLEEP_TIME_US);
  esp_deep_sleep_start();
}

bool sendLoRaJson(const String &payload) {
  LoRa.beginPacket();
  LoRa.print(payload);
  int result = LoRa.endPacket();
  return result == 1;
}

bool waitForEncryptedAck(const char *ackFor, uint32_t expectedRxCounter, unsigned long timeoutMs) {
  unsigned long startTime = millis();

  while (millis() - startTime < timeoutMs) {
    int packetSize = LoRa.parsePacket();

    if (packetSize) {
      String incoming = "";
      while (LoRa.available()) {
        incoming += (char)LoRa.read();
      }

      Serial.println("[NHAN ACK MA HOA] " + incoming);

      StaticJsonDocument<1024> outerDoc;
      DeserializationError outerErr = deserializeJson(outerDoc, incoming);
      if (outerErr) {
        Serial.println("[CANH_BAO] Loi parse JSON goi ACK ben ngoai");
        continue;
      }

      String mac = outerDoc["m"] | "";
      uint32_t counter = outerDoc["c"] | 0;
      String cipherB64 = outerDoc["d"] | "";
      String tagB64 = outerDoc["t"] | "";

      if (mac != macAddress || counter != expectedRxCounter) {
        Serial.println("[CANH_BAO] ACK sai MAC hoac counter");
        Serial.println("       MAC nhan=" + mac + ", mong doi=" + macAddress);
        Serial.println("       counter nhan=" + String(counter) + ", mong doi=" + String(expectedRxCounter));
        continue;
      }

      String plainAck;
      if (!decryptPayload(NODE_AES_KEY, macAddress, counter, DIR_DOWNLINK, cipherB64, tagB64, plainAck)) {
        Serial.println("[CANH_BAO] Giai ma/xac thuc ACK that bai");
        continue;
      }

      Serial.println("[ACK SAU GIAI MA] " + plainAck);

      StaticJsonDocument<160> ackDoc;
      DeserializationError ackErr = deserializeJson(ackDoc, plainAck);
      if (ackErr) {
        Serial.println("[CANH_BAO] Loi parse JSON ACK sau giai ma");
        continue;
      }

      String type = ackDoc["type"] | "";
      String ackForRx = ackDoc["ack_for"] | "";
      uint32_t rxCtr = ackDoc["rx_ctr"] | 0;

      if (type == "ACK" && ackForRx == ackFor && rxCtr == expectedRxCounter) {
        Serial.println("[OK] Da nhan ACK hop le cho goi " + String(ackFor));
        return true;
      }

      Serial.println("[CANH_BAO] Noi dung ACK khong khop");
      Serial.println("       type=" + type + ", ack_for=" + ackForRx + ", rx_ctr=" + String(rxCtr));
    }

    delay(10);
  }

  return false;
}

String buildConfigPayload() {
  StaticJsonDocument<160> doc;
  doc["type"] = "CONFIG";
  doc["config"] = "DHT22,SDS011,MQ7,UV";

  String output;
  serializeJson(doc, output);
  return output;
}

bool performConfigHandshake() {
  String payload = buildConfigPayload();
  return sendSecurePayloadWithRetry(payload, "CONFIG", CONFIG_RETRY_DELAY_MS);
}

bool sendSecurePayloadWithRetry(const String &plainPayload, const char *ackFor, unsigned long retryDelayMs) {
  uint32_t txCounter = nextSecurityCounter();

  String encryptedPacket;
  if (!buildEncryptedPacket(macAddress, NODE_AES_KEY, txCounter, DIR_UPLINK, plainPayload, encryptedPacket)) {
    Serial.println("[LOI] Ma hoa goi tin that bai");
    return false;
  }

  printSecurityPacketInfo(plainPayload, encryptedPacket, txCounter, ackFor);

  for (int attempt = 1; attempt <= COUNT_RETRY; attempt++) {
    Serial.println("\n[" + String(ackFor) + "] Lan gui " + String(attempt) + "/" + String(COUNT_RETRY));
    Serial.println("[GUI] Dang gui goi tin da ma hoa den Gateway...");

    if (sendLoRaJson(encryptedPacket)) {
      if (waitForEncryptedAck(ackFor, txCounter, ACK_TIMEOUT_MS)) {
        Serial.println("[OK] Gateway da xac nhan goi " + String(ackFor));
        return true;
      }
    } else {
      Serial.println("[LOI] Gui LoRa that bai");
    }

    Serial.println("[CANH_BAO] Khong nhan ACK " + String(ackFor) + ", gui lai cung goi da ma hoa...");
    delay(retryDelayMs);
  }

  Serial.println("[LOI] Gui goi " + String(ackFor) + " that bai sau so lan thu toi da");
  return false;
}
