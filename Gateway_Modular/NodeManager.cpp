#include "NodeManager.h"
#include "Globals.h"

int findNodeIndex(const String &mac) {
  for (int i = 0; i < NODE_COUNT; i++) {
    if (allowedNodes[i].mac == mac) return i;
  }
  return -1;
}

bool isAllowedNode(const String &mac) {
  int idx = findNodeIndex(mac);
  return idx >= 0 && allowedNodes[idx].enabled;
}

String getLocationFromMac(const String &mac) {
  int index = findNodeIndex(mac);
  if (index >= 0) return allowedNodes[index].location;
  return "unknown_location";
}
