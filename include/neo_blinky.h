#ifndef __NEO_BLINKY__
#define __NEO_BLINKY__
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "global.h"


void neo_setup();
void neo_animation(void *pvParameters);
void neo_blinky();

#endif