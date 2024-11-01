#include "temp_sensor.h"
#include <Arduino.h>

#define SERVER_PORT 8000
#define SERVER_URI "/esp_data"

void sendSensorDataToServer(String server_ip, TempSensorData *sensor_data, const char *address);