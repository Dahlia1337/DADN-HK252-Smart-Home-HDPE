#ifndef __COMPONENT_CONTROL_H__
#define __COMPONENT_CONTROL_H__

// #include <HardwareSerial.h>
#include <Arduino.h>
#include <ESP32Servo.h>
#include <Wire.h>

#include "global.h"

void component_reset();

void lcd_setup();
void lcdTask(void *pvParameters);

void door_setup();
void door_control(int state);

void fan_setup();
void fan_control(int state);


#endif