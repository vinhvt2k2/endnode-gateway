#include "QueueManager.h"
#include "Globals.h"
#include "NodeManager.h"
#include "ValidationManager.h"

void enqueueSensorData(const String &mac, JsonDocument &doc) {
  SensorData data;
  data.mac         = mac;
  data.location    = getLocationFromMac(mac);
  data.temperature = doc["TEMP"].as<float>();
  data.humidity    = doc["HUM"].as<float>();
  data.co          = doc["CO"].as<float>();
  data.pm25        = doc["PM25"].as<float>();
  data.pm10        = doc["PM10"].as<float>();
  data.uvIndex     = doc["UV"].as<float>();
  data.aqIndex     = readIntWithAlias(doc, "AQI", "AQIndex");
  data.preAQI      = readIntWithAlias(doc, "PRED_AQI", "PreAQI");

  if (dataQueue.size() >= MAX_QUEUE_SIZE) {
    Serial.println("[!] Queue day. Xoa ban ghi cu nhat.");
    dataQueue.pop();
  }

  dataQueue.push(data);
  Serial.println("[QUEUE] Da dua du lieu vao Queue cho " + data.location + ". Kich thuoc Queue hien tai: " + String(dataQueue.size()));
}
