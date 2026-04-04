#include "component_control.h"
#include <LiquidCrystal_I2C.h>

#define IN1 1
#define IN2 2
#define IN3 3
#define IN4 4

const int stepsPerRevolution = 2048;

LiquidCrystal_I2C lcd(33, 16, 2);
AccelStepper stepper(AccelStepper::FULL4WIRE, IN1, IN3, IN2, IN4);
Servo door;

void lcd_setup()
{
    lcd.begin();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("System Booting..");
};

void lcdTask(void *pvParameters)
{
    lcd.setCursor(0, 0);
    lcd.print("--Assignment----");
    lcd.setCursor(1, 0);
    lcd.print("-------Project--");
    
    vTaskDelay(5000);

    while (1){
        lcd.setCursor(0, 0);
        lcd.print("Smart-Home  HDPE");

        lcd.setCursor(1, 0);
        char buffer[17];
        snprintf(buffer, sizeof(buffer), "T:%.1f H:%.1f%%", glob_temperature, glob_humidity);
        lcd.print(buffer);

        vTaskDelay(200);
    }
};

void step_setup()
{
    stepper.setMaxSpeed(400.0);
    stepper.setAcceleration(500.0);
};

void step_control(int state)
{
    switch (state)
    {
    case 0:
        Serial.println("---rotate---");
        stepper.move(stepsPerRevolution / 4);
        break;
    case 1:
        Serial.println("---reverse---");
        stepper.move(-(stepsPerRevolution / 4));
        break;

    default:
        break;
    }
};

void stepperTask(void *pvParameters)
{
    stepper.moveTo(0);
    while (1)
    {
        stepper.run();
        vTaskDelay(1);
    }
}

void door_setup()
{
    door.attach(SERVO_PIN);
};

void fan_setup()
{
    pinMode(FAN_PIN, OUTPUT);
};

void door_control(int state)
{
    door.write(state);
};

void fan_control(int state)
{
    switch (state)
    {
    case 0:
        analogWrite(FAN_PIN, 0);
        break;

    case 1:
        analogWrite(FAN_PIN, 100);
        break;

    case 2:
        analogWrite(FAN_PIN, 255);
        break;

    case 3:
        analogWrite(FAN_PIN, 0);
        break;

    default:
        analogWrite(FAN_PIN, 0);
        break;
    }
};