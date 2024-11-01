#include "server_requests.h"
#include <HTTPClient.h>

void sendSensorDataToServer(String server_ip, TempSensorData *sensor_data, const char *address) {
    if(WiFi.status()== WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;
        http.begin(client, server_ip, SERVER_PORT, SERVER_URI);
        http.addHeader("Content-Type", "application/json");
        String httpRequestData = get_data_string(sensor_data, address);
        int httpResponseCode = http.POST(httpRequestData);
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        http.end();
    } else {
        Serial.println("Can't send request to server: WiFi Disconnected");
    }
}