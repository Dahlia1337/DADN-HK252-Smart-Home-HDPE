#include "task_toogle_boot.h"

#define BOOT 0

void Task_Toogle_BOOT(void *pvParameters)
{
    unsigned long buttonPressStartTime = 0;
    while (true)
    {
        if (digitalRead(BOOT) == LOW)
        {
            if (buttonPressStartTime == 0)
            {
                buttonPressStartTime = millis();
            }
            else if (millis() - buttonPressStartTime > 2000)
            {
                Serial.println("Boot button pressed for 2 seconds. Restarting...");
                component_reset();
                digitalWrite(LED1_PIN, LOW);
                vTaskDelay(pdMS_TO_TICKS(500));
                digitalWrite(LED1_PIN, HIGH);
                vTaskDelay(pdMS_TO_TICKS(500));
                vTaskDelete(NULL);
            }
        }
        else
        {
            buttonPressStartTime = 0;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}