#include "neo_blinky.h"

Adafruit_NeoPixel strip(1, NEO_PIN, NEO_GRB + NEO_KHZ800);

void neo_setup()
{    
    strip.begin();
    strip.clear();
    strip.show();
    strip.setBrightness(20);
    // Serial.println("---NeoPixel ready!---");
};

void neo_animation(void *pvParameters)
{
    while (1)
    {
        for (long firstPixelHue = 0; firstPixelHue < 65536; firstPixelHue += 256)
        {
            int pixelHue = firstPixelHue + (0 * 65536L / strip.numPixels());
            strip.setPixelColor(0, strip.gamma32(strip.ColorHSV(pixelHue)));
            strip.show();

            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }
}

void neo_blinky()
{
    digitalWrite(NEO_PIN, !digitalRead(NEO_PIN));
}
