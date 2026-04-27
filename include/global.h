#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <WiFi.h>

#define NEO_PIN     48
// #define LED_COUNT   1 
// #define LED_GPIO    44

#define RX_FROM_CAM 16
#define TX_TO_CAM   17

typedef struct struct_message {
    uint8_t direction; // 0: Trái, 1: Giữa, 2: Phải, 3: Không người
} struct_message;

extern struct_message myData;

#endif