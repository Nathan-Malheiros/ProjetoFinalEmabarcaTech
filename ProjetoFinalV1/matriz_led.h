#ifndef MATRIZ_LED_H
#define MATRIZ_LED_H

#include "pico/stdlib.h"

#define MATRIZ_LED_PIN 7
#define NUM_LEDS 25

void matriz_led_init();
void matriz_led_set_color(uint8_t r, uint8_t g, uint8_t b);
void matriz_led_clear();


void matriz_led_draw_pattern(const uint8_t *pattern, uint8_t r, uint8_t g, uint8_t b);

#endif 