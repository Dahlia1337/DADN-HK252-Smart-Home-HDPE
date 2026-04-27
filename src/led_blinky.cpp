#include "led_blinky.h"

void led_setup()
{
    pinMode(LED_PIN, OUTPUT);
};

void led_blinky(void *pvParameters)
{

    while (1)
    {
        digitalWrite(LED_PIN, LOW);
        vTaskDelay(pdMS_TO_TICKS(500));
        digitalWrite(LED_PIN, HIGH);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
};

