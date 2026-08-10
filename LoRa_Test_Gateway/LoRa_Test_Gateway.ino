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
#define LORA_SPREADING_FACTOR 12
#define LORA_BANDWIDTH        125E3
#define LORA_CODING_RATE      5      // 5 tuong ung 4/5

// ================== CAU HINH KIEM THU ==================
#define START_SEQ             0
#define END_SEQ               299    

bool firstPacketReceived = false;
int lastSeq = -1;

int receivedCount = 0;
int lostCount = 0;
int duplicateCount = 0;
int invalidCount = 0;

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
  LoRa.setSpreadingFactor(LORA_SPREADING_FACTOR);
  LoRa.setSignalBandwidth(LORA_BANDWIDTH);
  LoRa.setCodingRate4(LORA_CODING_RATE);
  LoRa.enableCrc();

  Serial.println("[OK] Khoi tao LoRa thanh cong");
  Serial.println("===== CAU HINH 1 =====");
  Serial.println("Tan so      : 433 MHz");
  Serial.println("SF          : 12");
  Serial.println("BW          : 125 kHz");
  Serial.println("Coding Rate : 4/5");
  Serial.println("======================");
}

int parseSeqFromPacket(String packet) {
  // Format: CFG1,NODE_01,seq
  int firstComma = packet.indexOf(',');
  if (firstComma < 0) return -1;

  int secondComma = packet.indexOf(',', firstComma + 1);
  if (secondComma < 0) return -1;

  String configName = packet.substring(0, firstComma);
  configName.trim();

  String nodeId = packet.substring(firstComma + 1, secondComma);
  nodeId.trim();

  String seqStr = packet.substring(secondComma + 1);
  seqStr.trim();

  if (configName != "CFG1") return -1;
  if (nodeId.length() == 0) return -1;
  if (seqStr.length() == 0) return -1;

  return seqStr.toInt();
}

void printSummary() {
  int expectedUntilNow = 0;

  if (firstPacketReceived) {
    expectedUntilNow = lastSeq - START_SEQ + 1;
  }

  float lossRatio = 0.0;
  if (expectedUntilNow > 0) {
    lossRatio = ((float)lostCount / (float)expectedUntilNow) * 100.0;
  }

  Serial.println("------------- THONG KE HIEN TAI -------------");
  Serial.print("Goi nhan duoc      : ");
  Serial.println(receivedCount);

  Serial.print("Goi bi mat         : ");
  Serial.println(lostCount);

  Serial.print("Goi lap lai        : ");
  Serial.println(duplicateCount);

  Serial.print("Goi loi dinh dang  : ");
  Serial.println(invalidCount);

  Serial.print("Seq cuoi da nhan   : ");
  Serial.println(lastSeq);

  Serial.print("Ti le mat goi      : ");
  Serial.print(lossRatio, 2);
  Serial.println(" %");
  Serial.println("---------------------------------------------");
}

void printFinalSummary() {
  Serial.println();
  Serial.println("=============== KET THUC KIEM THU ===============");

  int totalExpected = END_SEQ - START_SEQ + 1;

  // Neu den goi cuoi thi co the tinh tong mat goi theo tong mong doi.
  int finalLost = totalExpected - receivedCount;
  if (finalLost < 0) finalLost = 0;

  float finalLossRatio = ((float)finalLost / (float)totalExpected) * 100.0;

  Serial.print("Tong so goi mong doi : ");
  Serial.println(totalExpected);

  Serial.print("Tong so goi nhan duoc: ");
  Serial.println(receivedCount);

  Serial.print("Tong so goi mat      : ");
  Serial.println(finalLost);

  Serial.print("Goi lap lai          : ");
  Serial.println(duplicateCount);

  Serial.print("Goi loi dinh dang    : ");
  Serial.println(invalidCount);

  Serial.print("Ti le mat goi cuoi   : ");
  Serial.print(finalLossRatio, 2);
  Serial.println(" %");

  Serial.println("==================================================");
}

void processPacket(String packet, int rssi, float snr) {
  int seq = parseSeqFromPacket(packet);

  if (seq < START_SEQ || seq > END_SEQ) {
    invalidCount++;
    Serial.println("[LOI] Goi sai dinh dang hoac seq ngoai pham vi");
    Serial.println(packet);
    return;
  }

  Serial.println();
  Serial.println("=============== NHAN GOI LORA ===============");
  Serial.print("Noi dung : ");
  Serial.println(packet);

  Serial.print("Seq      : ");
  Serial.println(seq);

  Serial.print("RSSI     : ");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("SNR      : ");
  Serial.print(snr, 2);
  Serial.println(" dB");

  if (!firstPacketReceived) {
    firstPacketReceived = true;

    if (seq > START_SEQ) {
      int missingBeforeFirst = seq - START_SEQ;
      lostCount += missingBeforeFirst;
      Serial.print("[CANH BAO] Mat cac goi truoc seq dau tien: ");
      Serial.println(missingBeforeFirst);
    }

    lastSeq = seq;
    receivedCount++;
  } else {
    if (seq == lastSeq) {
      duplicateCount++;
      Serial.println("[CANH BAO] Goi lap lai");
    } else if (seq > lastSeq) {
      int missing = seq - lastSeq - 1;

      if (missing > 0) {
        lostCount += missing;
        Serial.print("[CANH BAO] Phat hien mat goi: ");
        Serial.println(missing);
      }

      lastSeq = seq;
      receivedCount++;
    } else {
      duplicateCount++;
      Serial.println("[CANH BAO] Goi cu den muon hoac bi lap");
    }
  }

  printSummary();

  if (seq >= END_SEQ) {
    printFinalSummary();
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("====================================");
  Serial.println(" GATEWAY KIEM THU LORA - CAU HINH 1");
  Serial.println(" Nhan goi va tinh ti le mat goi");
  Serial.println(" Format: CFG1,NODE_01,seq");
  Serial.println("====================================");

  initLoRa();

  Serial.println("[SAN SANG] Dang cho goi tu Node...");
}

void loop() {
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    String packet = "";

    while (LoRa.available()) {
      packet += (char)LoRa.read();
    }

    int rssi = LoRa.packetRssi();
    float snr = LoRa.packetSnr();

    processPacket(packet, rssi, snr);
  }
}
