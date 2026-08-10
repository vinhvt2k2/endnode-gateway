#include <Arduino.h>
#include "NodeConfig.h"
#include "Globals.h"
#include "NodeState.h"
#include "Utils.h"
#include "SensorManager.h"
#include "LoraManager.h"

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n================ KHOI DONG NODE BAO MAT ================");
  printWakeupReason();
  Serial.println("MAC: " + macAddress);
  Serial.println("Thoi gian ngu: " + String(TIME_SLEEP_MINUTES) + " phut");
  Serial.println("Thoi gian cho ACK: " + String(ACK_TIMEOUT_MS) + " ms");
  Serial.println("COUNT_RETRY: " + String(COUNT_RETRY));

  loadPersistentState();

  Serial2.begin(9600, SERIAL_8N1, RX2_PIN, TX2_PIN);
  dht.begin();
  sds.begin();

  filterMQ7.setInitial(analogRead(MQ7_PIN));
  filterUV.setInitial(analogRead(UV_PIN));

  initLoRa();
}

void loop() {
  if (!configConfirmed) {
    Serial.println("\n[*] Node chua duoc cau hinh. Bat dau bat tay CONFIG da ma hoa...");

    if (performConfigHandshake()) {
      saveConfigState(true);
      Serial.println("[OK] Bat tay CONFIG thanh cong va da luu trang thai");
    } else {
      // Khong bat tay duoc thi ngu de tiet kiem pin, lan thuc day sau thu lai.
      goToSleep();
    }
  }

  SensorData data;
  if (!readValidSensorData(data)) {
    // Du lieu do khong hop le thi khong gui rac len Gateway.
    goToSleep();
  }

  String valuePayload = buildValuePayload(data);
  Serial.println("\n[*] Tao payload VALUE thanh cong");
  sendSecurePayloadWithRetry(valuePayload, "VALUE", RETRY_DELAY_MS);

  // Thanh cong hay that bai sau COUNT_RETRY thi van ngu de tiet kiem pin.
  goToSleep();
}
