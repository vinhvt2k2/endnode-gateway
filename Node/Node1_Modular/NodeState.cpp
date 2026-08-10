#include "NodeState.h"
#include "Globals.h"

void loadPersistentState() {
  preferences.begin("node-sec", false);

  configConfirmed = preferences.getBool("cfg", false);
  securityCounter = preferences.getUInt("ctr", 0);

  Serial.println("[TRANG_THAI] Da cau hinh: " + String(configConfirmed ? "true" : "false"));
  Serial.println("[TRANG_THAI] Bo dem bao mat: " + String(securityCounter));
}

void saveConfigState(bool confirmed) {
  configConfirmed = confirmed;
  preferences.putBool("cfg", configConfirmed);
}

uint32_t nextSecurityCounter() {
  securityCounter++;
  if (securityCounter == 0) {
    // Tranh counter = 0 vi gateway coi 0 la invalid.
    securityCounter = 1;
  }
  preferences.putUInt("ctr", securityCounter);
  return securityCounter;
}
