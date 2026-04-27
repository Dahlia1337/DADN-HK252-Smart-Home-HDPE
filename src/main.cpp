#include "global.h"
#include "task_button.h"
#include "temp_humi.h"
// #include "tinyml.h"

// include task
#include "task_wifi.h"
#include "led_blinky.h"
#include "neo_blinky.h"
#include "task_check_info.h"
#include "task_toogle_boot.h"
#include "task_webserver.h"
#include "adafruit_connect.h"
#include "component_control.h"
#include "tinyml.h"

void setup()
{
    Serial.begin(115200);
    delay(5000);
    Serial.println("------ ESP32 start ------");
    check_info_File(0);
    // Delete_info_File();
    
    setup_monitoring_system();
    rgb_setup();
    led_setup();
    fan_setup();
    mqtt_setup();
    

    // xTaskCreate(Task_Toogle_BOOT, "Task Toogle Boot", 1024, NULL, 3, NULL);
    xTaskCreate(led_blinky, "Task LED Blink", 2048, NULL, 2, NULL);
    xTaskCreate(neo_animation, "Task NEO Blink", 2048, NULL, 2, NULL);
    xTaskCreate(temp_humi, "Task TEMP HUMI", 4096, NULL, 2, NULL);
    xTaskCreate( tiny_ml_task, "Tiny ML Task" , 8192, NULL, 2, NULL);
    xTaskCreate(mqtt_task, "MQTT Task", 8192, NULL, 2, NULL);
    
}

void loop()
{
    if (check_info_File(1))
    {
        if (!Wifi_reconnect())
        {
            Webserver_stop();
        }
        else
        {
            // CORE_IOT_reconnect();
        }
    }
    Webserver_reconnect();
}
