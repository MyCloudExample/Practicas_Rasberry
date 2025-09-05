#ifndef OLED_H
#define OLED_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Configuración básica
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_PAGE_HEIGHT 8

// Comandos SSD1306
#define OLED_SET_CONTRAST 0x81
#define OLED_DISPLAY_ALL_ON_RESUME 0xA4
#define OLED_DISPLAY_ALL_ON 0xA5
#define OLED_NORMAL_DISPLAY 0xA6
#define OLED_INVERT_DISPLAY 0xA7
#define OLED_DISPLAY_OFF 0xAE
#define OLED_DISPLAY_ON 0xAF
#define OLED_SET_DISPLAY_OFFSET 0xD3
#define OLED_SET_COM_PINS 0xDA
#define OLED_SET_VCOM_DETECT 0xDB
#define OLED_SET_DISPLAY_CLOCK_DIV 0xD5
#define OLED_SET_PRECHARGE 0xD9
#define OLED_SET_MULTIPLEX 0xA8
#define OLED_SET_LOW_COLUMN 0x00
#define OLED_SET_HIGH_COLUMN 0x10
#define OLED_SET_START_LINE 0x40
#define OLED_MEMORY_MODE 0x20
#define OLED_COLUMN_ADDR 0x21
#define OLED_PAGE_ADDR 0x22
#define OLED_COM_SCAN_INC 0xC0
#define OLED_COM_SCAN_DEC 0xC8
#define OLED_SEG_REMAP 0xA0
#define OLED_CHARGE_PUMP 0x8D
#define OLED_EXTERNAL_VCC 0x1
#define OLED_SWITCH_CAP_VCC 0x2

// Estructura para el display OLED
typedef struct {
    i2c_inst_t *i2c_port;
    uint8_t address;
    uint8_t width;
    uint8_t height;
    uint8_t buffer[OLED_WIDTH * OLED_HEIGHT / 8];
} oled_t;

// Prototipos de funciones
void oled_init(oled_t *oled, i2c_inst_t *i2c_port, uint8_t address, uint8_t width, uint8_t height);
void oled_write_command(oled_t *oled, uint8_t command);
void oled_write_data(oled_t *oled, const uint8_t *data, size_t size);
void oled_clear(oled_t *oled);
void oled_draw_pixel(oled_t *oled, uint8_t x, uint8_t y, bool on);
void oled_display(oled_t *oled);
void oled_draw_char(oled_t *oled, uint8_t x, uint8_t y, char c);
void oled_draw_string(oled_t *oled, uint8_t x, uint8_t y, const char *str);

#endif // OLED_H
