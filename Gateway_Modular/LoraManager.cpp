#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>
#include "GatewayConfig.h"
#include "Globals.h"
#include "LoraManager.h"
#include "SecurityManager.h"
#include "NodeManager.h"
#include "ValidationManager.h"
#include "QueueManager.h"
#include "DemoPrinter.h"
#include "OledManager.h"

void initLoRa() {
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("[-] LOI: Khong tim thay module LoRa");
    while (true) delay(1000);
  }

  LoRa.setSyncWord(LORA_SYNC_WORD);
  LoRa.setSpreadingFactor(LORA_SF);
  LoRa.setSignalBandwidth(LORA_BW);
  LoRa.setCodingRate4(LORA_CR);
  LoRa.enableCrc();

  Serial.println("[+] Da khoi tao LoRa");
  Serial.println("    Tan so: 433 MHz");
}

void handleLoRaPacket() {
  int packetSize = LoRa.parsePacket();
  if (!packetSize) return;

  String incoming = "";
  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  int packetRssi = LoRa.packetRssi();
  float packetSnr = LoRa.packetSnr();

  Serial.println("\n[NHAN] Goi LoRa da ma hoa:");
  Serial.println("[TIN_HIEU] RSSI = " + String(packetRssi) + " dBm, SNR = " + String(packetSnr, 1) + " dB");
  Serial.println(incoming);

  StaticJsonDocument<1024> outerDoc;
  DeserializationError error = deserializeJson(outerDoc, incoming);
  if (error) {
    Serial.print("[-] Loi parse JSON ben ngoai: ");
    Serial.println(error.c_str());
    return;
  }

  String mac = outerDoc["m"] | "";
  uint32_t counter = outerDoc["c"] | 0;
  String cipherB64 = outerDoc["d"] | "";
  String tagB64 = outerDoc["t"] | "";

  if (mac.length() == 0 || counter == 0 || cipherB64.length() == 0 || tagB64.length() == 0) {
    Serial.println("[-] Thieu truong bao mat trong packet. Bo qua.");
    return;
  }

  int nodeIndex = findNodeIndex(mac);
  if (nodeIndex < 0) {
    Serial.println("[-] MAC Node khong hop le. Bo qua: " + mac);
    oledShowPacket("Node khong hop le", mac, "-", packetRssi, packetSnr);
    return;
  }

  NodeInfo &node = allowedNodes[nodeIndex];

  if (!node.enabled) {
    Serial.println("[-] Node dang bi vo hieu hoa. Bo qua: " + mac);
    oledShowPacket("Node bi khoa", mac, node.location, packetRssi, packetSnr);
    return;
  }

  printOuterPacketInfo(mac, counter, cipherB64, tagB64, nodeIndex);
  oledShowPacket("Dang giai ma", mac, node.location, packetRssi, packetSnr);

  if (counter < node.lastCounter) {
    Serial.println("[-] Phat hien goi phat lai. counter < lastCounter. Bo qua.");
    oledShowPacket("Replay - bo qua", mac, node.location, packetRssi, packetSnr);
    return;
  }

  bool duplicate = (counter == node.lastCounter && node.lastCounter != 0);

  String plainPayload;
  if (!decryptPayload(node.key, mac, counter, DIR_UPLINK, cipherB64, tagB64, plainPayload)) {
    Serial.println("[-] Giai ma/xac thuc that bai. Khong gui ACK.");
    oledShowPacket("Giai ma loi", mac, node.location, packetRssi, packetSnr);
    return;
  }

  Serial.println("[OK] Giai ma/xac thuc thanh cong");
  oledShowPacket("Giai ma OK", mac, node.location, packetRssi, packetSnr);
  printDecryptedPayload(plainPayload);

  StaticJsonDocument<512> plainDoc;
  DeserializationError plainError = deserializeJson(plainDoc, plainPayload);
  if (plainError) {
    Serial.print("[-] Loi parse JSON payload sau giai ma: ");
    Serial.println(plainError.c_str());
    return;
  }

  String type = plainDoc["type"] | "";
  if (type.length() == 0) {
    Serial.println("[-] Thieu truong type trong payload.");
    return;
  }

  Serial.println("[*] Loai goi tin = " + type);

  if (type == "CONFIG") {
    processConfig(mac, counter, plainDoc, duplicate);
  } else if (type == "VALUE") {
    processValue(mac, counter, plainDoc, duplicate);
  } else {
    Serial.println("[-] Khong ho tro loai goi sau giai ma: " + type);
  }
}

void processConfig(const String &mac, uint32_t counter, JsonDocument &plainDoc, bool duplicate) {
  Serial.println("[*] Xu ly CONFIG tu Node: " + mac);

  int nodeIndex = findNodeIndex(mac);
  if (nodeIndex < 0 || !allowedNodes[nodeIndex].enabled) {
    Serial.println("[-] Tu choi CONFIG. Khong gui ACK.");
    return;
  }

  if (!duplicate) {
    allowedNodes[nodeIndex].lastCounter = counter;
    Serial.println("[+] Chap nhan CONFIG");
    Serial.println("    Vi tri: " + getLocationFromMac(mac));
    Serial.println("    Cap nhat Counter cuoi = " + String(counter));
    oledShowPacket("CONFIG OK", mac, getLocationFromMac(mac), oledLastRssi, oledLastSnr);
  } else {
    Serial.println("[*] Phat hien CONFIG gui lai. Chi gui lai ACK.");
    oledShowPacket("CONFIG gui lai", mac, getLocationFromMac(mac), oledLastRssi, oledLastSnr);
  }

  sendAckEncrypted(mac, "CONFIG", counter);
}

void processValue(const String &mac, uint32_t counter, JsonDocument &plainDoc, bool duplicate) {
  Serial.println("[*] Xu ly VALUE tu Node: " + mac);

  int nodeIndex = findNodeIndex(mac);
  if (nodeIndex < 0 || !allowedNodes[nodeIndex].enabled) {
    Serial.println("[-] Bo qua VALUE. Khong gui ACK.");
    return;
  }

  if (duplicate) {
    Serial.println("[*] Phat hien VALUE gui lai. Chi gui lai ACK, khong dua vao Queue lan nua.");
    sendAckEncrypted(mac, "VALUE", counter);
    return;
  }

  String reason = "";
  if (!dataValidation(plainDoc, reason)) {
    Serial.println("[-] Du lieu VALUE khong hop le: " + reason);
    Serial.println("[-] Khong gui ACK. Node se tu dong gui lai.");
    oledShowPacket("VALUE khong hop le", mac, getLocationFromMac(mac), oledLastRssi, oledLastSnr);
    return;
  }

  printSensorDataFromDoc(mac, plainDoc);

  enqueueSensorData(mac, plainDoc);
  allowedNodes[nodeIndex].lastCounter = counter;
  Serial.println("[+] Cap nhat Counter cuoi = " + String(counter));
  sendAckEncrypted(mac, "VALUE", counter);

  Serial.print("[+] Chap nhan VALUE va dua vao Queue. Kich thuoc Queue: ");
  Serial.println(dataQueue.size());
  oledShowPacket("VALUE OK", mac, getLocationFromMac(mac), oledLastRssi, oledLastSnr);
}

void sendAckEncrypted(const String &mac, const String &ackFor, uint32_t rxCounter) {
  int nodeIndex = findNodeIndex(mac);
  if (nodeIndex < 0) {
    Serial.println("[-] sendAckEncrypted(): Khong tim thay Node");
    return;
  }

  StaticJsonDocument<160> ackDoc;
  ackDoc["type"] = "ACK";
  ackDoc["ack_for"] = ackFor;
  ackDoc["rx_ctr"] = rxCounter;

  String plainAck;
  serializeJson(ackDoc, plainAck);

  String encryptedAck;
  if (!buildEncryptedPacket(mac, allowedNodes[nodeIndex].key, rxCounter, DIR_DOWNLINK, plainAck, encryptedAck)) {
    Serial.println("[-] Ma hoa ACK that bai");
    return;
  }

  LoRa.beginPacket();
  LoRa.print(encryptedAck);
  LoRa.endPacket();

  printAckDemo(plainAck, encryptedAck);
}
