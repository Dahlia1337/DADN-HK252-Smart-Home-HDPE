#include "global.h"
#include "task_camera.h"

#define PIN_FLASHLIGHT 33

void setup()
{
    Serial.begin(115200);
    delay(5000);
    Serial.println("\n--- ESP32-CAM START---");

    pinMode(PIN_FLASHLIGHT, OUTPUT);

    Serial.setDebugOutput(true);
    if (!cam_setup())
    {
        Serial.println("SYSTEM STOP.");
        while (true)
        {
            delay(1000);
        }
    }

    Serial.println("CAMERA START");

    xTaskCreate(task_cam, "Task Camera", 4096, NULL, 2, NULL);
}

void loop()
{
    // Không làm gì ở loop chính, để FreeRTOS tự quản lý các Task
    Serial.println("---ESP32cam are working---");
    vTaskDelay(5000);
}
