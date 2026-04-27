#include "temp_humi.h"

// Khai báo biến toàn cục

DHT dht(DHTPIN, DHTTYPE);

void setup_monitoring_system()
{
    dht.begin();
    Serial.println("---DHT22 sensor ready---");
}

void temp_humi(void *pvParameters)
{

    while (1)
    {
        float temperature = dht.readTemperature();
        float humidity = dht.readHumidity();

        if (isnan(temperature) || isnan(humidity))
        {
            Serial.println("Failed to read DHT!");
            // Nếu lỗi, gán đại giá trị 0
            temperature = 0;
            humidity = 0;
        }
        else
        {
            // Cập nhật biến toàn cục
            glob_temperature = temperature;
            glob_humidity = humidity;
        }
        
        
        // --- IN RA SERIAL ---
        Serial.printf("Hum: %.1f%%  Temp: %.1fC\n", humidity, temperature);

        // Delay nằm ngoài IF để đảm bảo Task luôn ngủ, tránh treo CPU
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void Webserver_send_sensor(float temp, float humi);
