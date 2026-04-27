#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <WiFi.h>

#define ADAFRUIT

#define LED1_PIN        43
#define LED2_PIN        44
#define NEO_PIN         38

#define FAN_PIN         5
#define FAN_IN1         6
#define FAN_IN2         7

#define DHTPIN          4
#define DHTTYPE         DHT22

extern float glob_temperature;
extern float glob_humidity;
extern float led_state;
extern int fan_speed;
extern int fan_state;
extern bool system_state;

extern String WIFI_SSID;
extern String WIFI_PASS;

extern boolean isWifiConnected;
// extern SemaphoreHandle_t xBinarySemaphoreInternet;
// extern SemaphoreHandle_t xLedMutex;
// extern SemaphoreHandle_t xNeoPixelMutex;

#endif