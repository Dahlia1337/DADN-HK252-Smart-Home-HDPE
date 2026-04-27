#include "task_espnow.h"
#include "stepper_control.h"

struct_message myData;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&myData, incomingData, sizeof(myData));
    step_control(myData.direction);
}

void task_espnow() {
    if (esp_now_init() != ESP_OK) {
        Serial.println("Lỗi khởi tạo ESP-NOW");
        return;
    }

    esp_now_register_recv_cb(OnDataRecv);
    
    Serial.println("Mạch Quạt đã sẵn sàng nghe lệnh!");
}