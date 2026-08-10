#include <ArduinoJson.h>
#include "mbedtls/ccm.h"
#include "mbedtls/base64.h"
#include "SecurityManager.h"

uint32_t fnv1a32(const String &text) {
  uint32_t hash = 2166136261UL;
  for (int i = 0; i < text.length(); i++) {
    hash ^= (uint8_t)text[i];
    hash *= 16777619UL;
  }
  return hash;
}

void buildNonce(const String &mac, uint32_t counter, char direction, uint8_t nonce[CCM_NONCE_SIZE]) {
  uint32_t h1 = fnv1a32(mac);
  uint32_t h2 = fnv1a32(mac + String(direction));

  nonce[0] = (uint8_t)direction;
  nonce[1] = (uint8_t)(h1 >> 24);
  nonce[2] = (uint8_t)(h1 >> 16);
  nonce[3] = (uint8_t)(h1 >> 8);
  nonce[4] = (uint8_t)(h1);
  nonce[5] = (uint8_t)(h2 >> 16);
  nonce[6] = (uint8_t)(h2 >> 8);
  nonce[7] = (uint8_t)(h2);
  nonce[8] = (uint8_t)(counter >> 24);
  nonce[9] = (uint8_t)(counter >> 16);
  nonce[10] = (uint8_t)(counter >> 8);
  nonce[11] = (uint8_t)(counter);
}

String buildAad(const String &mac, uint32_t counter, char direction) {
  return String(direction) + "|" + mac + "|" + String(counter);
}

bool base64Encode(const uint8_t *input, size_t inputLen, String &output) {
  unsigned char encoded[MAX_B64_SIZE];
  size_t olen = 0;

  int ret = mbedtls_base64_encode(encoded, sizeof(encoded), &olen, input, inputLen);
  if (ret != 0) return false;

  encoded[olen] = '\0';
  output = String((char *)encoded);
  return true;
}

bool base64Decode(const String &input, uint8_t *output, size_t outputMaxLen, size_t &outputLen) {
  int ret = mbedtls_base64_decode(output, outputMaxLen, &outputLen,
                                  (const unsigned char *)input.c_str(), input.length());
  return ret == 0;
}

bool encryptPayload(const uint8_t key[AES_KEY_SIZE], const String &mac, uint32_t counter, char direction,
                    const String &plainPayload, String &cipherB64, String &tagB64) {
  size_t plainLen = plainPayload.length();
  if (plainLen == 0 || plainLen > MAX_PLAIN_SIZE) return false;

  uint8_t nonce[CCM_NONCE_SIZE];
  buildNonce(mac, counter, direction, nonce);

  String aad = buildAad(mac, counter, direction);

  uint8_t cipher[MAX_CIPHER_SIZE];
  uint8_t tag[CCM_TAG_SIZE];

  mbedtls_ccm_context ctx;
  mbedtls_ccm_init(&ctx);

  int ret = mbedtls_ccm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, key, 128);
  if (ret != 0) {
    mbedtls_ccm_free(&ctx);
    return false;
  }

  ret = mbedtls_ccm_encrypt_and_tag(&ctx,
                                    plainLen,
                                    nonce, CCM_NONCE_SIZE,
                                    (const unsigned char *)aad.c_str(), aad.length(),
                                    (const unsigned char *)plainPayload.c_str(),
                                    cipher,
                                    tag, CCM_TAG_SIZE);

  mbedtls_ccm_free(&ctx);

  if (ret != 0) return false;

  return base64Encode(cipher, plainLen, cipherB64) && base64Encode(tag, CCM_TAG_SIZE, tagB64);
}

bool decryptPayload(const uint8_t key[AES_KEY_SIZE], const String &mac, uint32_t counter, char direction,
                    const String &cipherB64, const String &tagB64, String &plainPayload) {
  uint8_t cipher[MAX_CIPHER_SIZE];
  uint8_t tag[CCM_TAG_SIZE];
  uint8_t plain[MAX_PLAIN_SIZE + 1];
  size_t cipherLen = 0;
  size_t tagLen = 0;

  if (!base64Decode(cipherB64, cipher, sizeof(cipher), cipherLen)) return false;
  if (!base64Decode(tagB64, tag, sizeof(tag), tagLen)) return false;
  if (cipherLen == 0 || cipherLen > MAX_PLAIN_SIZE || tagLen != CCM_TAG_SIZE) return false;

  uint8_t nonce[CCM_NONCE_SIZE];
  buildNonce(mac, counter, direction, nonce);

  String aad = buildAad(mac, counter, direction);

  mbedtls_ccm_context ctx;
  mbedtls_ccm_init(&ctx);

  int ret = mbedtls_ccm_setkey(&ctx, MBEDTLS_CIPHER_ID_AES, key, 128);
  if (ret != 0) {
    mbedtls_ccm_free(&ctx);
    return false;
  }

  ret = mbedtls_ccm_auth_decrypt(&ctx,
                                 cipherLen,
                                 nonce, CCM_NONCE_SIZE,
                                 (const unsigned char *)aad.c_str(), aad.length(),
                                 cipher,
                                 plain,
                                 tag, CCM_TAG_SIZE);

  mbedtls_ccm_free(&ctx);

  if (ret != 0) return false;

  plain[cipherLen] = '\0';
  plainPayload = String((char *)plain);
  return true;
}

bool buildEncryptedPacket(const String &mac, const uint8_t key[AES_KEY_SIZE], uint32_t counter, char direction,
                          const String &plainPayload, String &encryptedPacket) {
  String cipherB64;
  String tagB64;

  if (!encryptPayload(key, mac, counter, direction, plainPayload, cipherB64, tagB64)) {
    return false;
  }

  StaticJsonDocument<1024> outerDoc;
  outerDoc["m"] = mac;
  outerDoc["c"] = counter;
  outerDoc["d"] = cipherB64;
  outerDoc["t"] = tagB64;

  serializeJson(outerDoc, encryptedPacket);
  return true;
}
