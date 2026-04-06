#ifndef SSD1306_H
#define SSD1306_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define SSD1306_I2C_ADDR _u(0x3C)
#define SSD1306_WIDTH    128
#define SSD1306_HEIGHT   64

typedef struct {
    uint8_t buffer[SSD1306_WIDTH * (SSD1306_HEIGHT / 8) + 1];
    i2c_inst_t *i2c_port;
} ssd1306_t;

void ssd1306_init(ssd1306_t *disp, i2c_inst_t *i2c);
void ssd1306_clear(ssd1306_t *disp);
void ssd1306_show(ssd1306_t *disp);
void ssd1306_draw_pixel(ssd1306_t *disp, uint8_t x, uint8_t y, bool on);
void ssd1306_draw_string(ssd1306_t *disp, uint8_t x, uint8_t y, const char *str);

void ssd1306_draw_icon_scaled(ssd1306_t *disp, uint8_t x, uint8_t y, const uint8_t *bitmap);

#endif 