#include "temp_humi.h"

// Khai báo biến toàn cục
DHT20 dht20;
// LiquidCrystal_I2C lcd(33, 16, 2); // Lưu ý: Kiểm tra kỹ địa chỉ 33 (0x21)

// Hàm setup riêng cho phần này (được gọi 1 lần từ main.cpp)
void setup_monitoring_system()
{

    Wire.begin(11, 12);
    dht20.begin();

}

void temp_humi(void *pvParameters)
{

    while (1)
    {
        dht20.read();
        float temperature = dht20.getTemperature();
        float humidity = dht20.getHumidity();

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
