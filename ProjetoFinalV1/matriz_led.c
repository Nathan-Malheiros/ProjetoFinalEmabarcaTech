#include "matriz_led.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"

void matriz_led_init() {
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, MATRIZ_LED_PIN, 800000, false);
    matriz_led_clear();
}

void matriz_led_set_color(uint8_t r, uint8_t g, uint8_t b) {
    uint32_t color_grb = ((uint32_t)(g) << 16) | ((uint32_t)(r) << 8) | (uint32_t)(b);
    for (int i = 0; i < NUM_LEDS; i++) {
        pio_sm_put_blocking(pio0, 0, color_grb << 8u);
    }
}

void matriz_led_clear() {
    matriz_led_set_color(0, 0, 0);
}


void matriz_led_draw_pattern(const uint8_t *pattern, uint8_t r, uint8_t g, uint8_t b) {
    uint32_t color_grb = ((uint32_t)(g) << 16) | ((uint32_t)(r) << 8) | (uint32_t)(b);
    for (int i = 0; i < NUM_LEDS; i++) {
        if (pattern[i]) {
            pio_sm_put_blocking(pio0, 0, color_grb << 8u);
        } else {
            pio_sm_put_blocking(pio0, 0, 0); // Desligado
        }
    }
}