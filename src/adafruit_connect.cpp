#include "adafruit_connect.h"
#include "led_blinky.h"
#include "component_control.h"
#include "neo_blinky.h"

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_KEY);

// --- Setup feed ---
//  Subscribe
Adafruit_MQTT_Subscribe led_feed = Adafruit_MQTT_Subscribe(&mqtt, MQTT_USERNAME "/feeds/led-state");
Adafruit_MQTT_Subscribe fan_feed = Adafruit_MQTT_Subscribe(&mqtt, MQTT_USERNAME "/feeds/fan-state");
Adafruit_MQTT_Subscribe rgb_feed = Adafruit_MQTT_Subscribe(&mqtt, MQTT_USERNAME "/feeds/rgb-state");

//  Publish
Adafruit_MQTT_Publish temp_feed = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish humi_feed = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/feeds/humidity");
Adafruit_MQTT_Publish fan_speed_feed = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/feeds/fan-speed");

unsigned long lastPublishTime = 0;

void mqtt_setup()
{
    mqtt.subscribe(&led_feed);
    mqtt.subscribe(&fan_feed);
    mqtt.subscribe(&rgb_feed);
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
        vTaskDelay(1000);
    }
    if (mqtt.connected())
        Serial.println("MQTT Connected!");
    else
        Serial.println("Can't connect to MQTT!");
};

void mqtt_task(void *pvParameters)
{
    vTaskDelay(pdMS_TO_TICKS(5000));
    Serial.println(" --- MQTT Task Started --- ");

    float temp_last_publish = glob_temperature;
    float humi_last_publish = glob_humidity;
    float fan_speed_last_publish = fan_speed;

    if (!mqtt.connected())
    {
        mqtt_reconnect();
    }

    Serial.printf("Publishing: Temp=%.2f, Humi=%.2f, Fspd=%d\n", glob_temperature, glob_humidity, fan_speed);
    temp_feed.publish(glob_temperature);
    humi_feed.publish(glob_humidity);
    fan_speed_feed.publish(fan_speed);

    while (1)
    {
        if (!mqtt.connected())
        {
            mqtt_reconnect();
        }

        //  Subscribe
        Adafruit_MQTT_Subscribe *subscription;
        while ((subscription = mqtt.readSubscription(5)))
        {
            if (subscription == &led_feed)
            {
                Serial.printf("Receive: led = %s\n", (char *)led_feed.lastread);
                ledControl(atoi((char *)led_feed.lastread));
            }
            else if (subscription == &fan_feed)
            {
                Serial.printf("Receive: fan = %s\n", (char *)fan_feed.lastread);
                fan_state = atoi((char *)fan_feed.lastread);
                fan_control(fan_state);
            }

            else if (subscription == &rgb_feed)
            {
                Serial.printf("Receive: led = %s\n", (char *)rgb_feed.lastread);
                rgb_control(atoi((char *)rgb_feed.lastread));
            }
        }

        //  Publish
        if (millis() - lastPublishTime > 5000)
        {
            if (!isnan(glob_temperature) && !isnan(glob_humidity))
            {
                // Serial.printf("Publishing: Temp=%.2f, Humi=%.2f, Fspd=%d\n", glob_temperature, glob_humidity, fan_speed);
                if (abs(glob_temperature * 100- temp_last_publish * 100) > 20)
                {
                    Serial.printf("Publishing: Temp=%.2f\n", glob_temperature);
                    temp_feed.publish(glob_temperature);
                    temp_last_publish = glob_temperature;
                }
                if (abs(glob_humidity * 100 - humi_last_publish * 100) > 20)
                {
                    Serial.printf("Publishing: Humi=%.2f\n", glob_humidity);
                    humi_feed.publish(glob_humidity);
                    humi_last_publish = glob_humidity;
                }
                if (abs(fan_speed - fan_speed_last_publish) > 5)
                {
                    Serial.printf("Publishing: Fspd=%d\n", fan_speed);
                    fan_speed_feed.publish(fan_speed);
                    fan_speed_last_publish = fan_speed;
                }
            }
            lastPublishTime = millis();
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}