#include <task_handler.h>

void handleWebSocketMessage(String message)
{
    Serial.println(message);
    StaticJsonDocument<256> doc;

    DeserializationError error = deserializeJson(doc, message);
    if (error)
    {
        Serial.println("❌ Lỗi parse JSON!");
        return;
    }
    JsonObject value = doc["value"];
    if (doc["page"] == "device")
    {
        int gpio = doc["value"]["gpio"];
        String status = doc["value"]["status"];

        Serial.printf("Web Control -> GPIO: %d | Status: %s\n", gpio, status.c_str());

        // Chỉ xử lý nếu trạng thái là ON (người dùng chọn chế độ đó)
        // Logic 4 chế độ quạt dựa trên GPIO ảo (4, 5, 18, 19)
        if (status == "ON") {
            switch (gpio) {
                case 4:  // Web gửi GPIO 4 -> Chế độ Tắt
                    fan_state = 0;
                    Serial.println("=> MODE: Fan OFF");
                    break;
                case 5:  // Web gửi GPIO 5 -> Mức 1
                    fan_state = 1;
                    Serial.println("=> MODE: Fan Level 1");
                    break;
                case 18: // Web gửi GPIO 18 -> Mức 2
                    fan_state = 2;
                    Serial.println("=> MODE: Fan Level 2");
                    break;
                case 19: // Web gửi GPIO 19 -> Auto
                    fan_state = 3;
                    Serial.println("=> MODE: Fan Auto");
                    break;
                default:
                    Serial.println("=> Unknown GPIO command");
                    break;
            }
        }
    }
    else if (doc["page"] == "setting")
    {
        String WIFI_SSID = doc["value"]["ssid"].as<String>();
        String WIFI_PASS = doc["value"]["password"].as<String>();
        String CORE_IOT_TOKEN = doc["value"]["token"].as<String>();
        String CORE_IOT_SERVER = doc["value"]["server"].as<String>();
        String CORE_IOT_PORT = doc["value"]["port"].as<String>();

        Serial.println("📥 Nhận cấu hình từ WebSocket:");
        Serial.println("SSID: " + WIFI_SSID);
        Serial.println("PASS: " + WIFI_PASS);
        Serial.println("TOKEN: " + CORE_IOT_TOKEN);
        Serial.println("SERVER: " + CORE_IOT_SERVER);
        Serial.println("PORT: " + CORE_IOT_PORT);

        // Gọi hàm lưu cấu hình
        Save_info_File(WIFI_SSID, WIFI_PASS, CORE_IOT_TOKEN, CORE_IOT_SERVER, CORE_IOT_PORT);

        // Phản hồi lại client (tùy chọn)
        String msg = "{\"status\":\"ok\",\"page\":\"setting_saved\"}";
        ws.textAll(msg);
    }
}
