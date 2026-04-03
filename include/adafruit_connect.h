#ifndef __TASK_ADAFRUIT_H__
#define __TASK_ADAFRUIT_H__

#include "global.h"
#include "adafruit_key.h"

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <HTTPClient.h>

void mqtt_setup();
void mqtt_reconnect();
void mqtt_task(void *pvParameters);

#endif