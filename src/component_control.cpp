#include "component_control.h"
#include "tinyml.h"

Servo door;

void fan_setup()
{
    pinMode(FAN_IN1, OUTPUT);
    pinMode(FAN_IN2, OUTPUT);
    pinMode(FAN_PIN, OUTPUT);
    digitalWrite(FAN_IN1, LOW);
    digitalWrite(FAN_IN2, HIGH);
};

void fan_control(int state)
{
    switch (state)
    {
    case 0:
        fan_speed = 0;
        analogWrite(FAN_PIN, 0);
        break;
    case 1:
        fan_speed = 150;
        Serial.printf("Fan speed: %d\n", fan_speed);
        analogWrite(FAN_PIN, 150);
        break;
    case 2:
        fan_speed = 255;
        Serial.printf("Fan speed: %d\n", fan_speed);
        analogWrite(FAN_PIN, 255);
        break;

    case 3:
        fan_speed = int(result * 255);
        Serial.printf("Fan speed: %d\n", fan_speed);
        analogWrite(FAN_PIN, fan_speed);
        break;

    default:
        fan_speed = 0;
        analogWrite(FAN_PIN, 0);
        break;
    }
};

void component_reset()
{
    door.detach();
    analogWrite(FAN_PIN, 0);
}