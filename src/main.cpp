#include <Arduino.h>
#include "web_server.h"
#include "robonomics_utils/Robonomics.h"
#include "utils/nvs_utils.h"
#include "main.h"
#include "server_requests.h"
#include "temp_sensor.h"

Robonomics robonomics;
TempSensorData sensor_data;
unsigned long last_datalog_time = 0;

void get_or_generate_private_key(uint8_t *robonomicsPrivateKey) {
    char* robonomicsSs58Address;
    bool res = readBytesFromNVS(ROBONOMICS_PRIVATE_KEY_NVS_KEY, robonomicsPrivateKey, sizeof(robonomicsPrivateKey));
    if (res) {
        robonomicsSs58Address = getAddrFromPrivateKey(robonomicsPrivateKey);
        ESP_LOGI(TAG, "Robonomics Address: %s", robonomicsSs58Address);
    } else {
        Ed25519::generatePrivateKey(robonomicsPrivateKey);
        writeBytesToNVS(ROBONOMICS_PRIVATE_KEY_NVS_KEY, robonomicsPrivateKey, sizeof(robonomicsPrivateKey));
        robonomicsSs58Address = getAddrFromPrivateKey(robonomicsPrivateKey);
        ESP_LOGI(TAG, "Robonomics Address: %s", robonomicsSs58Address);
    }
    delete[] robonomicsSs58Address;
}

void get_wifi_creds() {
    String ssid_buffer;
    String password_buffer;
    String server_ip_buffer;
    bool res = true;
    ssid_buffer = readStringFromNVS(WIFI_SSID_NVS_KEY);
    if (ssid_buffer != "") {
        password_buffer = readStringFromNVS(WIFI_PASSWORD_NVS_KEY);
        if (password_buffer != "") {
            server_ip_buffer = readStringFromNVS(SERVER_IP_NVS_KEY);
            user_data.ssid = ssid_buffer;
            user_data.password = password_buffer;
            user_data.server_ip = server_ip_buffer;
        } else {
            res = false;
        }
    } else {
        res = false;
    }
    if (!res) {
        user_data = get_wifi_creds_from_user(robonomics.getSs58Address());
        Serial.println("After server");
        writeStringToNVS(WIFI_SSID_NVS_KEY, user_data.ssid.c_str());
        writeStringToNVS(WIFI_PASSWORD_NVS_KEY, user_data.password.c_str());
        writeStringToNVS(SERVER_IP_NVS_KEY, user_data.server_ip.c_str());
    }
}


void setup() {
    Serial.begin(115200);
    delay(3000);
    uint8_t robonomicsPrivateKey[PRIVATE_KEY_LENGTH];
    get_or_generate_private_key(robonomicsPrivateKey);
    robonomics.setPrivateKey(robonomicsPrivateKey);
    sensor_setup();
    get_wifi_creds();
}


void loop() {
    get_temp_data(& sensor_data);
    WiFi.begin(user_data.ssid, user_data.password);
    while ( WiFi.status() != WL_CONNECTED ) {
        vTaskDelay(500 /portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "." );
    }
    if (user_data.server_ip != "") {
        Serial.println("Sending to server");
        sendSensorDataToServer(user_data.server_ip, & sensor_data, robonomics.getSs58Address());
    }
    if (millis() - last_datalog_time > DATALOG_SENDING_TIMEOUT) {
        robonomics.setup();
        String datalog_data = get_data_string(& sensor_data);
        robonomics.sendDatalogRecord(datalog_data.c_str());
        robonomics.disconnectWebsocket();
        last_datalog_time = millis();
    }
    WiFi.disconnect(true);
    delay(SERVER_SENDING_TIMEOUT);
}