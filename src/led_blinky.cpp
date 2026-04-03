#include "led_blinky.h"
#include "component_control.h"

void led_setup()
{
    pinMode(LED_GPIO, OUTPUT);
};

void led_blinky(void *pvParameters)
{

    while (1)
    {
        digitalWrite(LED_GPIO, LOW);
        vTaskDelay(pdMS_TO_TICKS(500));
        digitalWrite(LED_GPIO, HIGH);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
};

void ledControl(int state)
{
    if (state == 0)
    {
        digitalWrite(LED_GPIO, LOW);
        servoControl(90);
        stepControl(0);
    }
    else
    {
        digitalWrite(LED_GPIO, HIGH);
        servoControl(0);
        stepControl(1);
    }
}
