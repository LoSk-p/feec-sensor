#include "temp_sensor.h"
#include <Adafruit_Sensor.h>
#include <Arduino_JSON.h>

#ifdef BME_USE
#include <Adafruit_BME280.h>
#endif 

#ifdef DHT_USE
#include <DHT.h>
#endif

#ifdef DHT_USE
DHT dht22(DHT22_PIN, DHT11);
#endif
#ifdef BME_USE
Adafruit_BME280 bme;
#endif 

void sensor_setup() {
    bool status;
#ifdef DHT_USE
    dht22.begin();  
#endif
#ifdef BME_USE
    bool status;
    status = bme.begin(0x76); 
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
    }
#endif
}

String get_data_string(TempSensorData *sensors_data) {
    JSONVar json_data;
    json_data["temperature"] = String(sensors_data->temperature, 2);
    json_data["humidity"] = String(sensors_data->humidity, 2);
    return JSON.stringify(json_data);
}

String get_data_string(TempSensorData *sensors_data, const char *address) {
    JSONVar json_data;
    json_data["temperature"] = String(sensors_data->temperature, 2);
    json_data["humidity"] = String(sensors_data->humidity, 2);
    json_data["address"] = address;
    return JSON.stringify(json_data);
}

void print_values(TempSensorData *temp_data) {
    Serial.print("Temperature = ");
    Serial.print(temp_data->temperature);
    Serial.println(" *C");

    Serial.print("Humidity = ");
    Serial.print(temp_data->humidity);
    Serial.println(" %");

    if (temp_data->pressure > 0) {
        Serial.print("Pressure = ");
        Serial.print(temp_data->pressure);
        Serial.println(" hPa");
    }
}

void get_temp_data(TempSensorData *temp_data) {
#ifdef TEST_SENSOR_USE
    temp_data->humidity = 17;
    temp_data->temperature = 15;
    temp_data->pressure = -1;
#endif
#ifdef DHT_USE
    temp_data->humidity = dht22.readHumidity();
    temp_data->temperature = dht22.readTemperature();
    temp_data->pressure = -1;
#endif
#ifdef BME_USE
    temp_data->humidity = bme.readHumidity();
    temp_data->temperature = bme.readTemperature();
    temp_data->pressure = bme.readPressure() / 100.0F;
#endif
    print_values(temp_data);
}

