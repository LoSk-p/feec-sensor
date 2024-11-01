#include <Preferences.h>

#define ROBONOMICS_STORAGE "robonomics"
#define ROBONOMICS_PRIVATE_KEY_NVS_KEY "priv_key"
#define WIFI_SSID_NVS_KEY "wifi_ssid"
#define WIFI_PASSWORD_NVS_KEY "wifi_password"
#define SERVER_IP_NVS_KEY "server_ip"

void writeStringToNVS(const char *key, const char *value);
String readStringFromNVS(const char *key);
void writeBytesToNVS(const char *key, const uint8_t *data, size_t data_length);
bool readBytesFromNVS(const char *key, uint8_t *buffer, size_t buffer_size);