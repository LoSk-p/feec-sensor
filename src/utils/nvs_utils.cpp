#include "nvs_utils.h"

Preferences preferences;

void writeStringToNVS(const char *key, const char *value) {
    Serial.printf("Write string %s to nvs key %s\r\n", value, key);
    preferences.begin(ROBONOMICS_STORAGE, false);
    preferences.putString(key, value);
    preferences.end();
}

// Function to read a string from NVS
String readStringFromNVS(const char *key) {
    Serial.printf("Read string from nvs key %s\r\n", key);
    preferences.begin(ROBONOMICS_STORAGE, true);
    String value = preferences.getString(key, "");
    preferences.end();
    Serial.printf("Got string %s from nvs key %s\r\n", value.c_str(), key);
    return value;
}

// Function to write bytes (blob) to NVS
void writeBytesToNVS(const char *key, const uint8_t *data, size_t data_length) {
    Serial.printf("Write bytes to nvs key %s\r\n", key);
    preferences.begin(ROBONOMICS_STORAGE, false);
    preferences.putBytes(key, data, data_length);
    preferences.end();
}

// Function to read bytes (blob) from NVS
bool readBytesFromNVS(const char *key, uint8_t *buffer, size_t buffer_size) {
    Serial.printf("Read bytes to nvs key %s\r\n", key);
    preferences.begin(ROBONOMICS_STORAGE, true);
    size_t required_size = preferences.getBytesLength(key);

    if (required_size == 0 || required_size > buffer_size) {
        preferences.end();
        return false;
    }

    preferences.getBytes(key, buffer, required_size);
    preferences.end();
    return true;
}
