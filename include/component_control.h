#ifndef __COMPONENT_CONTROL_H__
#define __COMPONENT_CONTROL_H__

// #include <HardwareSerial.h>
#include <Arduino.h>
#include <ESP32Servo.h>
#include <AccelStepper.h>
#include <Wire.h>

#include "global.h"

void lcd_setup();
void lcdTask(void *pvParameters);

void step_setup();
void stepControl(int state);
void stepperTask(void *pvParameters);

void servo_setup();
void servoControl(int state);

void fan_setup();
void fanControl(int state);


#endif