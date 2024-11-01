#include <WiFi.h>
#include <Arduino.h>
#include <WebServer.h>
#include "web_server.h"
#include "web_pages.h"

// Replace with your desired access point credentials
bool credentials_received = false;
UserData user_data;

// Create a WebServer object on port 80
WebServer server(80);


// Function to handle root URL
void handleRoot(const char* robonomicsSs58Address) {
    String htmlResponse = generateHtmlContent(robonomicsSs58Address);
    server.send(200, "text/html", htmlResponse);
}

// Function to handle saving Wi-Fi credentials
void handleSave() {
    user_data.ssid = server.arg("ssid");
    user_data.password = server.arg("password");
    user_data.server_ip = server.arg("server_ip");

    // Log the received SSID and password
    Serial.print("Received SSID: ");
    Serial.println(user_data.ssid.c_str());
    Serial.print("Received Password: ");
    Serial.println(user_data.password.c_str());
    Serial.print("Server IP Address: ");
    Serial.println(user_data.server_ip.c_str());
    credentials_received = true;

    // Send a response back to the client
    server.send(200, "text/html", "<h1>Credentials Saved!</h1><p>SSID: " + user_data.ssid + "</p><p>Password: " + user_data.password + "</p>");
}

UserData get_wifi_creds_from_user(const char* robonomicsSs58Address) {
    credentials_received = false;
    uint64_t chipId = ESP.getEfuseMac();
    
    char APssid[30];
    snprintf(APssid, sizeof(APssid), "%s%lu", AP_SSID_PREFIX, (uint32_t)chipId);
    WiFi.softAP(APssid, NULL);
    Serial.println("Access Point started");

    // Update route handlers to include the SS58 address
    server.on("/", [=]() { handleRoot(robonomicsSs58Address); });
    server.on("/save", HTTP_POST, handleSave);

    server.begin();

    while (!credentials_received)
    {
        server.handleClient();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    
    server.stop();
    Serial.println("Access Point stop");
    WiFi.softAPdisconnect(true);
    Serial.println("Access Point stopped");
    
    return user_data;
}