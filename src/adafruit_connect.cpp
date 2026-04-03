#include "adafruit_connect.h"
#include "led_blinky.h"
#include "component_control.h"

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_KEY);

// --- Setup feed ---
//  Subscribe
Adafruit_MQTT_Subscribe led_control = Adafruit_MQTT_Subscribe(&mqtt, MQTT_USERNAME "/feeds/led-state");
Adafruit_MQTT_Subscribe fan_control = Adafruit_MQTT_Subscribe(&mqtt, MQTT_USERNAME "/feeds/fan-state");
//  Publish
Adafruit_MQTT_Publish temp_feed = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish humi_feed = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/feeds/humidity");

unsigned long lastPublishTime = 0;

void mqtt_setup()
{
    mqtt.subscribe(&led_control);
    mqtt.subscribe(&fan_control);
};

void mqtt_reconnect()
{
    int8_t code_error_connect;

    Serial.print("Connecting to MQTT... ");

    int count_error_times = 10;

    while (((code_error_connect = mqtt.connect()) != 0) && (count_error_times > 0))
    {

        Serial.println(mqtt.connectErrorString(code_error_connect));
        Serial.println("Retry MQTT connection");
        mqtt.disconnect();
        count_error_times--;
        vTaskDelay(100);
    }
    if (mqtt.connected())
        Serial.println("MQTT Connected!");
    else
        Serial.println("Can't connect to MQTT!");
};

void mqtt_task(void *pvParameters)
{
    // Đợi 5s ban đầu để Wifi ổn định hoàn toàn
    vTaskDelay(pdMS_TO_TICKS(5000));
    Serial.println(" --- MQTT Task Started --- ");

    while(1)
    {
        if (!mqtt.connected())
        {
            mqtt_reconnect();
        }

        //  Subscribe
        Adafruit_MQTT_Subscribe *subscription;
        while ((subscription = mqtt.readSubscription(5))) 
        {
            if (subscription == &led_control)
            {
                Serial.printf("Receive: led = %s\n", (char *)led_control.lastread);
                ledControl(atoi((char *)led_control.lastread));
            }
            else if (subscription == &fan_control)
            {
                Serial.printf("Receive: fan = %s\n", (char *)fan_control.lastread);
                fanControl(atoi((char *)fan_control.lastread));
            }
        }

        //  Publish
        if (millis() - lastPublishTime > 5000)
        {
            if (!isnan(glob_temperature) && !isnan(glob_humidity))
            {
                Serial.printf("Publishing: T=%.2f, H=%.2f\n", glob_temperature, glob_humidity);
                temp_feed.publish(glob_temperature);
                humi_feed.publish(glob_humidity);
            }
            lastPublishTime = millis();
        }

        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
}