#ifndef _STEPPER_CONTROL_H
#define _STEPPER_CONTROL_H

#include "global.h"
#include <AccelStepper.h>



void step_setup();
void step_control(int state);
void task_stepper(void *pvParameters);


#endif // _STEPPER_CONTROL_H