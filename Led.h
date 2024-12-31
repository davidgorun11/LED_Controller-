#ifndef LED_H
#define LED_H

#include <FastLED.h>
#include <HomeSpan.h>

#define LED_PIN 16         // Pin for connecting the LED strip
#define NUM_LEDS 144       // Number of LEDs in the strip
#define COLOR_ORDER GRB    // Color order for the strip
#define CHIPSET WS2812B    // Type of LED chipset

CRGB leds[NUM_LEDS];      // Array for controlling LEDs

struct LED : Service::LightBulb {
    SpanCharacteristic *power;
    SpanCharacteristic *brightness;
    SpanCharacteristic *hue;
    SpanCharacteristic *saturation;

    // New characteristics for effects
    SpanCharacteristic *effectIntensity; // Effect intensity
    SpanCharacteristic *effectSelection; // Effect selection

    LED() : Service::LightBulb() {
        power = new Characteristic::On();
        brightness = new Characteristic::Brightness(100);
        hue = new Characteristic::Hue(0);
        saturation = new Characteristic::Saturation(0);

        // Create custom characteristics with unique UUIDs
        effectIntensity = new SpanCharacteristic(
            "00000004-0000-1000-8000-0026BB765291", // Custom UUID
            PR | PW                                   // Permissions
        );
        effectIntensity->setRange(0, 100, 1);        // Range: 0-100

        effectSelection = new SpanCharacteristic(
            "00000005-0000-1000-8000-0026BB765291", // Custom UUID
            PR | PW                                   // Permissions
        );
        effectSelection->setRange(0, 2, 1);          // Range: 0-2 (0 - No effect, 1 - Blinking, 2 - Rainbow)

        FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
        FastLED.clear();
        FastLED.show();
    }

    boolean update() override {
        if (power->getNewVal()) {
            applySettings();

            // Effect selection
            int effect = effectSelection->getNewVal();
            int intensity = effectIntensity->getNewVal(); // Get intensity
            switch (effect) {
                case 1:
                    applyBlinkingEffect(intensity);
                    break;
                case 2:
                    applyRainbowEffect(intensity);
                    break;
                default:
                    applyStaticColor(); // No effect, just static color
                    break;
            }
        } else {
            FastLED.clear();
            FastLED.show();
        }
        return true;
    }

    void applySettings() {
        int br = map(brightness->getNewVal(), 0, 100, 0, 255);
        float h = hue->getNewVal();
        float s = saturation->getNewVal();

        CHSV hsvColor = CHSV(h / 360.0 * 255, s / 100.0 * 255, br);
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = hsvColor;
        }
        FastLED.show();
    }

    void applyStaticColor() {
        applySettings();
    }

    void applyBlinkingEffect(int intensity) {
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CRGB::White;
        }
        FastLED.show();
        delay(1000 / intensity); // Blinking speed depends on intensity
        FastLED.clear();
        FastLED.show();
        delay(1000 / intensity);
    }

    void applyRainbowEffect(int intensity) {
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = CHSV((i * 256 / NUM_LEDS + millis() / intensity) % 256, 255, 255);
        }
        FastLED.show();
    }
};

#endif
