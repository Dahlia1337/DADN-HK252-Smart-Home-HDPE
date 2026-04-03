#include "neo_blinky.h"
#include "global.h"

// Adafruit_NeoPixel pixels(1, RGB_PIXELS_PIN, NEO_GRB + NEO_KHZ800);
// pixels.begin();
// pixels.setBrightness(20);

// void rgb_control(int state){

// };

void neo_animation(void *pvParameters)
{
    Adafruit_NeoPixel pixel(1, NEO_PIN, NEO_GRB + NEO_KHZ800);
    pixel.begin();
    pixel.setBrightness(0);
    pixel.setPixelColor(0,pixel.Color(255,255,255));
    vTaskDelay(2000);

    Adafruit_NeoPixel strip(4, RGB_PIXELS_PIN, NEO_GRB + NEO_KHZ800);

    strip.begin();
    strip.setBrightness(10);

    while (true)
    {
        // Vòng lặp tạo màu cầu vồng (0 -> 65535)
        for (long firstPixelHue = 0; firstPixelHue < 65536; firstPixelHue += 256)
        {
            // Chuyển đổi giá trị Hue sang màu RGB
            int pixelHue = firstPixelHue + (0 * 65536L / strip.numPixels());
            for (int i = 0; i < 4; i++)
                strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));

            strip.show();

            vTaskDelay(pdMS_TO_TICKS(20));
        }
        
    }
};
