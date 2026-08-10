#include "DemoPrinter.h"
#include "Globals.h"
#include "ValidationManager.h"

void printAllowedNodes() {
  Serial.println("\n[*] Allowed node list:");
  for (int i = 0; i < NODE_COUNT; i++) {
    Serial.print("    ");
    Serial.print(allowedNodes[i].mac);
    Serial.print(" -> ");
    Serial.print(allowedNodes[i].location);
    Serial.print(" | kich hoat=");
    Serial.print(allowedNodes[i].enabled ? "co" : "khong");
    Serial.print(" | Counter cuoi=");
    Serial.println(allowedNodes[i].lastCounter);
  }
}

void printOuterPacketInfo(const String &mac, uint32_t counter, const String &cipherB64, const String &tagB64, int nodeIndex) {
  Serial.println("---------------- GOI BAO MAT NHAN DUOC ----------------");
  Serial.println("DINH DANG NGOAI: {m, c, d, t}");
  Serial.println("m / MAC      : " + mac);
  Serial.println("Vi tri       : " + allowedNodes[nodeIndex].location);
  Serial.println("c / Counter  : " + String(counter));
  Serial.println("Counter cuoi : " + String(allowedNodes[nodeIndex].lastCounter));
  Serial.println("d / Du lieu  : " + cipherB64);
  Serial.println("t / Ma xac thuc: " + tagB64);
  Serial.println("Che do AES   : AES-128-CCM");
  Serial.println("--------------------------------------------------");
}

void printDecryptedPayload(const String &plainPayload) {
  Serial.println("---------------- PAYLOAD SAU GIAI MA ----------------");
  Serial.println(plainPayload);
  Serial.println("---------------------------------------------------");
}

void printSensorDataFromDoc(const String &mac, JsonDocument &doc) {
  Serial.println("---------------- DU LIEU VALUE HOP LE ----------------");
  Serial.println("MAC      : " + mac);
  Serial.println("TEMP     : " + String(readFloat(doc, "TEMP"), 1) + " C");
  Serial.println("HUM      : " + String(readFloat(doc, "HUM"), 1) + " %");
  Serial.println("CO       : " + String(readFloat(doc, "CO"), 2) + " mg/m3");
  Serial.println("PM2.5    : " + String(readFloat(doc, "PM25"), 1) + " ug/m3");
  Serial.println("PM10     : " + String(readFloat(doc, "PM10"), 1) + " ug/m3");
  Serial.println("UV       : " + String(readFloat(doc, "UV"), 2));
  Serial.println("AQIndex  : " + String(readIntWithAlias(doc, "AQI", "AQIndex")));
  Serial.println("PreAQI   : " + String(readIntWithAlias(doc, "PRED_AQI", "PreAQI")));
  Serial.println("--------------------------------------------------");
}

void printAckDemo(const String &plainAck, const String &encryptedAck) {
  Serial.println("---------------- GUI ACK DA MA HOA ----------------");
  Serial.println("[ACK GOC]    " + plainAck);
  Serial.println("[ACK MA HOA] " + encryptedAck);
  Serial.println("--------------------------------------------------");
}

void printFirebaseData(const SensorData &data) {
  Serial.println("---------------- DU LIEU GUI FIREBASE ----------------");
  Serial.println("Duong dan : smartcity/" + data.location);
  Serial.println("MAC      : " + data.mac);
  Serial.println("TEMP     : " + String(data.temperature, 1) + " C");
  Serial.println("HUM      : " + String(data.humidity, 1) + " %");
  Serial.println("CO       : " + String(data.co, 2) + " mg/m3");
  Serial.println("PM2.5    : " + String(data.pm25, 1) + " ug/m3");
  Serial.println("PM10     : " + String(data.pm10, 1) + " ug/m3");
  Serial.println("UV       : " + String(data.uvIndex, 2));
  Serial.println("AQI      : " + String(data.aqIndex));
  Serial.println("PreAQI   : " + String(data.preAQI));
  Serial.println("Thoi gian tu luc khoi dong: " + String(millis()) + " ms");
  Serial.println("--------------------------------------------------");
}
