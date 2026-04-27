#include "stepper_control.h"

#define IN1 15
#define IN2 16
#define IN3 17
#define IN4 18

AccelStepper stepper(AccelStepper::FULL4WIRE, IN1, IN3, IN2, IN4);

int stepsPerRevolution = 2048;
float max_speed = 400.0;

const long max_left_pos = -512;
const long max_right_pos = 512;

float tracking_speed = 50.0;
float search_speed = 30.0;

volatile int current_state = 3;

volatile unsigned long last_ai_command_time = 0;

volatile unsigned long time_person_lost = 0;

int sweep_direction = 1;

void step_setup()
{
    stepper.setMaxSpeed(600.0);
    stepper.setCurrentPosition(0);
    Serial.println("---Stepper Ready---");
}

void step_control(int state)
{
    last_ai_command_time = millis();

    if (state != 3)
    {
        time_person_lost = 0;
    }

    if (current_state != state)
    {
        Serial.print("Đã nhận lệnh AI: ");
        Serial.print(state);

        current_state = state;
        if (current_state == 0)
            Serial.println(" -> Yêu cầu quay TRÁI");
        else if (current_state == 1)
            Serial.println(" -> Yêu cầu ĐỨNG IM (GIỮA)");
        else if (current_state == 2)
            Serial.println(" -> Yêu cầu quay PHẢI");
        else
            Serial.println(" -> KHÔNG CÓ NGƯỜI");
    }
};

void task_stepper(void *pvParameters)
{

    while (1)
    {
        long pos = stepper.currentPosition();

        if (last_ai_command_time > 0 && (millis() - last_ai_command_time > 500))
        {
            current_state = 1;
        }

        switch (current_state)
        {
        case 0:
            if (pos < max_right_pos)
            {
                stepper.setSpeed(tracking_speed);
            }
            else
                stepper.setSpeed(0);
            break;
        case 2:
            if (pos > max_left_pos)
            {
                stepper.setSpeed(-tracking_speed);
            }
            else
                stepper.setSpeed(0);
            break;

        case 1:
            stepper.setSpeed(0);
            break;

        case 3:
            if (time_person_lost == 0)
            {
                time_person_lost = millis();
                stepper.setSpeed(0);
            }
            else if (millis() - time_person_lost > 3000)
            {
                if (pos >= max_right_pos)
                    sweep_direction = -1;
                if (pos <= max_left_pos)
                    sweep_direction = 1;
                stepper.setSpeed(sweep_direction * search_speed);
            }
            else
            {
                stepper.setSpeed(0);
            }
            break;
        default:
            stepper.setSpeed(0);
            break;
        }

        stepper.runSpeed();
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}