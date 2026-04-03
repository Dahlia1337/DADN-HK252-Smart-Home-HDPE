#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <WiFi.h>

#define ADAFRUIT

#ifdef AT_LAB   
#define LED_GPIO        48
#define NEO_PIN         45
#else
#define LED_GPIO        48
#define NEO_PIN         45
#endif

#define RGB_PIXELS_PIN  8
#define FAN_PIN         6
#define SERVO_PIN       5

extern float glob_temperature;
extern float glob_humidity;
extern float led_state;
extern float fan_speed;
extern int fan_state;
extern bool system_state;

extern String WIFI_SSID;
extern String WIFI_PASS;

extern boolean isWifiConnected;
// extern SemaphoreHandle_t xBinarySemaphoreInternet;
// extern SemaphoreHandle_t xLedMutex;
// extern SemaphoreHandle_t xNeoPixelMutex;

#endif