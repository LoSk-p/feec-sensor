#include <Arduino.h>
#include "blake/blake2.h"
#include "address.h"
#include <cstring>
#include <Ed25519.h>

const char *const ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
const int ALPHABET_MAP[128] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0,  1,  2,
    3,  4,  5,  6,  7,  8,  -1, -1, -1, -1, -1, -1, -1, 9,  10, 11, 12, 13, 14, 15, 16, -1, 17, 18, 19, 20,
    21, -1, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, -1, -1, -1, -1, -1, -1, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 43, -1, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, -1, -1, -1, -1, -1};

// result must be declared: char result[len * 137 / 100];
int EncodeBase58(const unsigned char *bytes, int len, unsigned char result[]) {
    unsigned char digits[512] = {0};
    int digitslen = 1;
    for (int i = 0; i < len; i++) {
        unsigned int carry = (unsigned int)bytes[i];
        for (int j = 0; j < digitslen; j++) {
            carry += (unsigned int)(digits[j]) << 8;
            digits[j] = (unsigned char)(carry % 58);
            carry /= 58;
        }
        while (carry > 0) {
            digits[digitslen++] = (unsigned char)(carry % 58);
            carry /= 58;
        }
    }
    int resultlen = 0;
    for (; resultlen < len && bytes[resultlen] == 0;)
        result[resultlen++] = '1';
    for (int i = 0; i < digitslen; i++)
        result[resultlen + i] = ALPHABET[digits[digitslen - 1 - i]];
    result[digitslen + resultlen] = 0;
    return digitslen + resultlen;
}

// result must be declared (for the worst case): char result[len * 2];
int DecodeBase58(const unsigned char *str, int len, unsigned char *result) {
    result[0] = 0;
    int resultlen = 1;
    for (int i = 0; i < len; i++) {
        unsigned int carry = (unsigned int)ALPHABET_MAP[str[i]];
        for (int j = 0; j < resultlen; j++) {
            carry += (unsigned int)(result[j]) * 58;
            result[j] = (unsigned char)(carry & 0xff);
            carry >>= 8;
        }
        while (carry > 0) {
            result[resultlen++] = (unsigned int)(carry & 0xff);
            carry >>= 8;
        }
    }

    for (int i = 0; i < len && str[i] == '1'; i++)
        result[resultlen++] = 0;

    // Poorly coded, but guaranteed to work.
    for (int i = resultlen - 1, z = (resultlen >> 1) + (resultlen & 1); i >= z; i--) {
        int k = result[i];
        result[i] = result[resultlen - i - 1];
        result[resultlen - i - 1] = k;
    }
    return resultlen;
}


char* getAddrFromPublicKey(RobonomicsPublicKey &pubKey) {
    unsigned char plainAddr[SR25519_PUBLIC_SIZE + 3] = {ROBONOMICS_PREFIX};
    memcpy(plainAddr + 1, pubKey.bytes, SR25519_PUBLIC_SIZE);

    uint8_t ssPrefixed[SR25519_PUBLIC_SIZE + 8] = {0x53, 0x53, 0x35, 0x38, 0x50, 0x52, 0x45};
    memcpy(ssPrefixed + 7, plainAddr, SR25519_PUBLIC_SIZE + 1);

    unsigned char blake2bHashed[64] = {0};
    blake2(blake2bHashed, 64, ssPrefixed, SR25519_PUBLIC_SIZE + 8, NULL, 0);
    plainAddr[1 + PUBLIC_KEY_LENGTH] = blake2bHashed[0];
    plainAddr[2 + PUBLIC_KEY_LENGTH] = blake2bHashed[1];

    unsigned char addrCh[SR25519_PUBLIC_SIZE * 2] = {0};

    int encodedLen = EncodeBase58(plainAddr, SR25519_PUBLIC_SIZE + 3, addrCh);

    // std::string result((char*)addrCh, encodedLen);
    char* result = new char[encodedLen + 1]; // +1 for null terminator
    strncpy(result, (char*)addrCh, encodedLen);
    result[encodedLen] = '\0';

    return result;
}

char* getAddrFromPrivateKey(uint8_t *private_key) {
    uint8_t robonomicsPublicKey[32];
    RobonomicsPublicKey public_key;
    Ed25519::derivePublicKey(robonomicsPublicKey, private_key);
    memcpy(public_key.bytes, robonomicsPublicKey, PUBLIC_KEY_LENGTH);
    char* robonomicsSs58Address = getAddrFromPublicKey(public_key);
    return robonomicsSs58Address;
}


RobonomicsPublicKey getPublicKeyFromAddr(const char *addrStr) {
    Address addr;
    memcpy(addr.symbols, addrStr, ADDRESS_LENGTH);
    RobonomicsPublicKey pubk{0};

    unsigned char bs58decoded[ADDRESS_LENGTH];
    int len = DecodeBase58(addr.symbols, ADDRESS_LENGTH, bs58decoded);
    if (len == 35) {
        // Check the address checksum
        // Add SS58RPE prefix, remove checksum (2 bytes)
        uint8_t ssPrefixed[PUBLIC_KEY_LENGTH + 8] = {0x53, 0x53, 0x35, 0x38, 0x50, 0x52, 0x45};
        memcpy(ssPrefixed + 7, bs58decoded, PUBLIC_KEY_LENGTH + 1);

        unsigned char blake2bHashed[64] = {0};
        blake2(blake2bHashed, 64, ssPrefixed, PUBLIC_KEY_LENGTH + 8, NULL, 0);
        if (bs58decoded[1 + PUBLIC_KEY_LENGTH] != blake2bHashed[0] || 
            bs58decoded[2 + PUBLIC_KEY_LENGTH] != blake2bHashed[1] ) {
            Serial.println("Address checksum is wrong.");
        }

        memcpy(pubk.bytes, bs58decoded + 1, PUBLIC_KEY_LENGTH);
    }

    return pubk;
}
