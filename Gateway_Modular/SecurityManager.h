#ifndef SECURITY_MANAGER_H
#define SECURITY_MANAGER_H
#include <Arduino.h>
#include "GatewayConfig.h"

uint32_t fnv1a32(const String &text);
void buildNonce(const String &mac, uint32_t counter, char direction, uint8_t nonce[CCM_NONCE_SIZE]);
String buildAad(const String &mac, uint32_t counter, char direction);
bool base64Encode(const uint8_t *input, size_t inputLen, String &output);
bool base64Decode(const String &input, uint8_t *output, size_t outputMaxLen, size_t &outputLen);
bool encryptPayload(const uint8_t key[AES_KEY_SIZE], const String &mac, uint32_t counter, char direction,
                    const String &plainPayload, String &cipherB64, String &tagB64);
bool decryptPayload(const uint8_t key[AES_KEY_SIZE], const String &mac, uint32_t counter, char direction,
                    const String &cipherB64, const String &tagB64, String &plainPayload);
bool buildEncryptedPacket(const String &mac, const uint8_t key[AES_KEY_SIZE], uint32_t counter, char direction,
                          const String &plainPayload, String &encryptedPacket);

#endif
