#include <Arduino.h>

#define AP_SSID_PREFIX "RobonomicsSensor-"

struct UserData {
    String ssid;
    String password;
    String server_ip;
};

extern UserData user_data;

UserData get_wifi_creds_from_user(const char* robonomicsSs58Address);
