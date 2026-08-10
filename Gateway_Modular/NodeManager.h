#ifndef NODE_MANAGER_H
#define NODE_MANAGER_H
#include <Arduino.h>
int findNodeIndex(const String &mac);
bool isAllowedNode(const String &mac);
String getLocationFromMac(const String &mac);
#endif
