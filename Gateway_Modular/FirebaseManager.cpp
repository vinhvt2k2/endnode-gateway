#include <Firebase_ESP_Client.h>
#include <ArduinoJson.h>
#include "GatewayConfig.h"
#include "Globals.h"
#include "FirebaseManager.h"
#include "DemoPrinter.h"
#include "OledManager.h"

void sendToServer() {
  if (!firebaseReady) {
    Serial.println("[-] Firebase chua san sang. Bo qua sendToServer().");
    return;
  }

  if (dataQueue.empty()) {
    Serial.println("[*] Queue rong. Khong can gui Server.");
    oledShowFirebase("Queue rong", dataQueue.size());
    return;
  }

  SensorData &data = dataQueue.front();
  Serial.println("\n[*] sendToServer(): Dang gui 1 ban ghi len Firebase...");
  printFirebaseData(data);

  bool ok = pushSensorDataToFirebase(data);

  if (ok) {
    dataQueue.pop();
    Serial.print("[+] Gui Firebase thanh cong. Kich thuoc Queue: ");
    Serial.println(dataQueue.size());
    oledShowFirebase("Firebase OK", dataQueue.size());
  } else {
    Serial.println("[-] Gui Firebase that bai. Giu ban ghi trong Queue de gui lai.");
    Serial.println("    Loi: " + fbdo.errorReason());
    oledShowFirebase("Firebase LOI", dataQueue.size());
  }
}

bool pushSensorDataToFirebase(const SensorData &data) {
  String basePath = "smartcity/" + data.location;
  String currentPath = basePath + "/current";
  String historyPath = basePath + "/history";
  String historyIndexPath = basePath + "/historyIndex";  


  FirebaseJson json;
  json.set("AQI", data.aqIndex);
  json.set("PreAQI", data.preAQI);
  json.set("PM25", data.pm25);
  json.set("PM10", data.pm10);
  json.set("CO", data.co);
  json.set("temperature", data.temperature);
  json.set("humidity", data.humidity);
  json.set("uvIndex", data.uvIndex);
  json.set("createdAt/.sv", "timestamp");  

  bool ok = true;

  // 1. Ghi du lieu moi nhat:
  // smartcity/locationX/current/...
  ok &= Firebase.RTDB.setJSON(&fbdo, currentPath, &json);

  String historyKey = "";
  if (ok) {
    ok &= Firebase.RTDB.pushJSON(&fbdo, historyPath, &json);
    if (ok) {
      historyKey = fbdo.pushName();
    }
  }

  if (ok && historyKey.length() > 0) {
    ok &= Firebase.RTDB.setBool(&fbdo, historyIndexPath + "/" + historyKey, true);
  }

  if (ok) {
    keepLastHistoryRecords(historyPath, MAX_HISTORY_RECORDS);
  }

  return ok;
}

uint64_t stringToUint64(const String &s) {
  uint64_t value = 0;
  for (int i = 0; i < s.length(); i++) {
    char c = s.charAt(i);
    if (c < '0' || c > '9') break;
    value = value * 10ULL + (uint64_t)(c - '0');
  }
  return value;
}

bool isNumericString(const String &s) {
  if (s.length() == 0) return false;
  for (int i = 0; i < s.length(); i++) {
    char c = s.charAt(i);
    if (c < '0' || c > '9') return false;
  }
  return true;
}

int compareHistoryKeys(const String &a, const String &b) {
  bool aNum = isNumericString(a);
  bool bNum = isNumericString(b);

  if (aNum && bNum) {
    uint64_t av = stringToUint64(a);
    uint64_t bv = stringToUint64(b);
    if (av < bv) return -1;
    if (av > bv) return 1;
    return 0;
  }

  // Firebase push key duoc thiet ke de sap xep tang dan theo thoi gian khi so sanh chuoi.
  if (a < b) return -1;
  if (a > b) return 1;
  return 0;
}

void sortKeysAscending(String *keys, int count) {
  for (int i = 0; i < count - 1; i++) {
    for (int j = i + 1; j < count; j++) {
      if (compareHistoryKeys(keys[j], keys[i]) < 0) {
        String tmp = keys[i];
        keys[i] = keys[j];
        keys[j] = tmp;
      }
    }
  }
}

void keepLastHistoryRecords(const String &historyPath, int maxRecords) {
  if (maxRecords <= 0) return;

  String historyIndexPath = historyPath;
  historyIndexPath.replace("/history", "/historyIndex");

  if (!Firebase.RTDB.getJSON(&fbdo, historyIndexPath)) {
    Serial.println("[CANH_BAO] Khong doc duoc historyIndex de cat bot du lieu.");
    Serial.println("           Loi: " + fbdo.errorReason());
    return;
  }

  FirebaseJson *json = fbdo.to<FirebaseJson *>();
  size_t count = json->iteratorBegin();

  if (count <= (size_t)maxRecords) {
    Serial.println("[HISTORY] So ban ghi hien tai: " + String(count) +
                   "/" + String(maxRecords) + ". Khong can xoa.");
    json->iteratorEnd();
    return;
  }

  String *keys = new String[count];

  for (size_t i = 0; i < count; i++) {
    int type = 0;
    String key;
    String value;
    json->iteratorGet(i, type, key, value);
    keys[i] = key;
  }

  json->iteratorEnd();

  sortKeysAscending(keys, (int)count);

  int deleteCount = (int)count - maxRecords;

  for (int i = 0; i < deleteCount; i++) {
    String oldKey = keys[i];

    bool okHistory = Firebase.RTDB.deleteNode(&fbdo, historyPath + "/" + oldKey);
    bool okIndex = Firebase.RTDB.deleteNode(&fbdo, historyIndexPath + "/" + oldKey);

    if (okHistory && okIndex) {
      Serial.println("[HISTORY] Da xoa ban ghi cu: " + oldKey);
    } else {
      Serial.println("[CANH_BAO] Xoa ban ghi cu that bai: " + oldKey);
      Serial.println("           Loi: " + fbdo.errorReason());
    }
  }

  delete[] keys;

  Serial.println("[HISTORY] Da giu lai " + String(maxRecords) + " ban ghi gan nhat.");
}
