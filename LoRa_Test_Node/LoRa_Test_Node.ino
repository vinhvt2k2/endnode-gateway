#include <SPI.h>
#include <LoRa.h>

// ================== PIN LORA TTGO LoRa32 T3 V1.6.1 ==================
#define LORA_SCK   5
#define LORA_MISO  19
#define LORA_MOSI  27
#define LORA_SS    18
#define LORA_RST   23
#define LORA_DIO0  26

// ================== CAU HINH LORA - CAU HINH 1 ==================
#define LORA_FREQUENCY        433E6
#define LORA_SYNC_WORD        0xF3
#define LORA_TX_POWER         17
#define LORA_SPREADING_FACTOR 12
#define LORA_BANDWIDTH        125E3
#define LORA_CODING_RATE      5      // 5 tuong ung 4/5

// ================== CAU HINH KIEM THU ==================
#define START_SEQ             0
#define END_SEQ               300    .
#define SEND_INTERVAL_MS      15000UL

int currentSeq = START_SEQ;
unsigned long lastSendTime = 0;
bool testFinished = false;

void initLoRa() {
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("[LOI] Khoi tao LoRa that bai!");
    while (true) {
      delay(1000);
    }
  }

  LoRa.setSyncWord(LORA_SYNC_WORD);
  LoRa.setTxPower(LORA_TX_POWER);
  LoRa.setSpreadingFactor(LORA_SPREADING_FACTOR);
  LoRa.setSignalBandwidth(LORA_BANDWIDTH);
  LoRa.setCodingRate4(LORA_CODING_RATE);
  LoRa.enableCrc();

  Serial.println("[OK] Khoi tao LoRa thanh cong");
  Serial.println("===== CAU HINH 1 =====");
  Serial.println("Tan so      : 433 MHz");
  Serial.println("Cong suat   : 17 dBm");
  Serial.println("SF          : 12");
  Serial.println("BW          : 125 kHz");
  Serial.println("Coding Rate : 4/5");
  Serial.println("======================");
}

void sendTestPacket(int seq) {
  // Format goi test: CFG1,NODE_01,seq
  String packet = "";
  packet += "CFG1";
  packet += ",";
  packet += "NODE_01";
  packet += ",";
  packet += String(seq);

  LoRa.beginPacket();
  LoRa.print(packet);
  LoRa.endPacket();

  Serial.print("[GUI] Goi thu tu: ");
  Serial.print(seq);
  Serial.print(" | Noi dung: ");
  Serial.println(packet);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("====================================");
  Serial.println(" NODE KIEM THU LORA - CAU HINH 1");
  Serial.println(" Gui goi tu 0 den 300, moi goi 15s");
  Serial.println(" Format: CFG1,NODE_01,seq");
  Serial.println("====================================");

  initLoRa();

  // De gui goi dau tien ngay sau khi setup xong.
  lastSendTime = millis() - SEND_INTERVAL_MS;
}

void loop() {
  if (testFinished) {
    return;
  }

  if (millis() - lastSendTime >= SEND_INTERVAL_MS) {
    lastSendTime = millis();

    sendTestPacket(currentSeq);

    if (currentSeq >= END_SEQ) {
      testFinished = true;
      Serial.println();
      Serial.println("[HOAN THANH] Da gui xong tat ca goi kiem thu.");
      Serial.print("Tong so goi da gui: ");
      Serial.println(END_SEQ - START_SEQ + 1);
      Serial.println("Ban co the dung Node hoac reset de test lai.");
      return;
    }

    currentSeq++;
  }
}
