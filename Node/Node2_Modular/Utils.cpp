#include "Utils.h"
#include "Globals.h"
#include <math.h>
#include "esp_sleep.h"

bool isValidFloat(float value) {
  return !isnan(value) && !isinf(value);
}

bool inRangeFloat(float value, float minValue, float maxValue) {
  return isValidFloat(value) && value >= minValue && value <= maxValue;
}

bool inRangeInt(int value, int minValue, int maxValue) {
  return value >= minValue && value <= maxValue;
}

void printSensorData(const SensorData &data) {
  Serial.println("---------------- DU LIEU CAM BIEN ----------------");
  Serial.println("Nhiet do     : " + String(data.temperature, 1) + " C");
  Serial.println("Do am        : " + String(data.humidity, 1) + " %");
  Serial.println("Khi CO       : " + String(data.co, 2) + " mg/m3");
  Serial.println("Bui PM2.5    : " + String(data.pm25, 1) + " ug/m3");
  Serial.println("Bui PM10     : " + String(data.pm10, 1) + " ug/m3");
  Serial.println("Chi so UV    : " + String(data.uvIndex, 2));
  Serial.println("AQI hien tai : " + String(data.aqIndex));
  Serial.println("AQI du doan  : " + String(data.preAQI));
  Serial.println("---------------------------------------------");
}

void printSecurityPacketInfo(const String &plainPayload, const String &encryptedPacket, uint32_t txCounter, const char *ackFor) {
  Serial.println("---------------- GOI TIN BAO MAT GUI DI ----------------");
  Serial.println("Loai goi tin : " + String(ackFor));
  Serial.println("MAC          : " + macAddress);
  Serial.println("Bo dem       : " + String(txCounter));
  Serial.println("Che do AES   : AES-128-CCM");
  Serial.println("Dinh dang goi ngoai: {m,c,d,t}");
  Serial.println("[DU LIEU GOC]    : " + plainPayload);
  Serial.println("[DU LIEU MA HOA] : " + encryptedPacket);
  Serial.println("---------------------------------------------");
}

void printWakeupReason() {
  esp_sleep_wakeup_cause_t wakeupReason = esp_sleep_get_wakeup_cause();

  Serial.print("Ly do Node thuc day: ");
  switch (wakeupReason) {
    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println("Bo hen gio");
      break;
    default:
      Serial.println("Cap nguon/Reset/Ly do khac");
      break;
  }
}
