#ifndef LED_H
#define LED_H

#include <FastLED.h>
#include <HomeSpan.h>

#define LED_PIN 16         // Пин для подключения ленты
#define NUM_LEDS 144       // Количество светодиодов в ленте
#define COLOR_ORDER GRB    // Порядок цветов для ленты
#define CHIPSET WS2812B    // Тип чипа ленты

CRGB leds[NUM_LEDS];      // Массив для управления светодиодами

struct LED : Service::LightBulb {
    SpanCharacteristic *power;
    SpanCharacteristic *brightness;
    SpanCharacteristic *hue;
    SpanCharacteristic *saturation;

    // Новые характеристики для эффектов
    SpanCharacteristic *effectIntensity; // Интенсивность эффекта
    SpanCharacteristic *effectSelection; // Выбор эффекта

    LED() : Service::LightBulb() {
        power = new Characteristic::On();
        brightness = new Characteristic::Brightness(100);
        hue = new Characteristic::Hue(0);
        saturation = new Characteristic::Saturation(0);

        // Создаем пользовательские характеристики
        effectIntensity = new SpanCharacteristic(
            "00000004-0000-1000-8000-0026BB765291", // Custom UUID
            PR | PW                                 // Permissions
        );
        effectIntensity->setRange(0, 100, 1);       // Диапазон: 0-100

        effectSelection = new SpanCharacteristic(
            "00000005-0000-1000-8000-0026BB765291", // Custom UUID
            PR | PW                                 // Permissions
        );
        effectSelection->setRange(0, 2, 1);         // Диапазон: 0-2 (0 - Нет эффекта, 1 - Мигание, 2 - Радуга)

        FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
        FastLED.clear();
        FastLED.show();
    }

    boolean update() override {
        if (power->getNewVal()) {
            applySettings();

            // Выбор эффекта
            int effect = effectSelection->getNewVal();
            int intensity = effectIntensity->getNewVal(); // Получение интенсивности
            switch (effect) {
                case 1:
                    applyBlinkingEffect(intensity);
                    break;
                case 2:
                    applyRainbowEffect(intensity);
                    break;
                default:
                    applyStaticColor(); // Без эффекта, только цвет
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
        delay(1000 / intensity); // Скорость мигания зависит от интенсивности
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
