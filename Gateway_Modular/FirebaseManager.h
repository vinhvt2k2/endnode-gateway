#ifndef FIREBASE_MANAGER_H
#define FIREBASE_MANAGER_H
#include <Arduino.h>
#include "DataTypes.h"

void sendToServer();
bool pushSensorDataToFirebase(const SensorData &data);
void keepLastHistoryRecords(const String &historyPath, int maxRecords);
uint64_t stringToUint64(const String &s);
bool isNumericString(const String &s);
int compareHistoryKeys(const String &a, const String &b);
void sortKeysAscending(String *keys, int count);

#endif
