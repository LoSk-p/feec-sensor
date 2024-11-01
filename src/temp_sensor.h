#pragma once

#include "main.h"
#include <Arduino.h>

#ifdef DHT_USE
#define DHT22_PIN 21
#endif

struct TempSensorData {
    float temperature;
    float humidity;
    float pressure;
};

String get_data_string(TempSensorData *sensors_data);
String get_data_string(TempSensorData *sensors_data, const char *address);
void get_temp_data(TempSensorData *temp_data);
void sensor_setup();