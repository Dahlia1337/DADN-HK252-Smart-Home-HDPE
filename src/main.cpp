#include "global.h"
#include "led_blinky.h"
#include "neo_blinky.h"
#include "task_espnow.h"
#include "stepper_control.h"

void setup()
{
    Serial.begin(115200);
    delay(5000);
    Serial.println("---Starting setup---");

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    Serial.print("DIA CHI MAC CUA MACH QUAT: ");
    Serial.println(WiFi.macAddress());

    neo_setup();
    task_espnow();
    step_setup();

    xTaskCreate(neo_animation, "Task NEO Animation", 2048, NULL, 5, NULL);
    xTaskCreate(task_stepper, "Task Stepper Control", 4096, NULL, 5, NULL);
}

void loop()
{
    // Serial.println("Main loop is running...");
    vTaskDelay(pdMS_TO_TICKS(1000));
}
