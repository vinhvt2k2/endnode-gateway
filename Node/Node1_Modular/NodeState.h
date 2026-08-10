#ifndef NODE_STATE_H
#define NODE_STATE_H

#include <Arduino.h>

void loadPersistentState();
void saveConfigState(bool confirmed);
uint32_t nextSecurityCounter();

#endif
