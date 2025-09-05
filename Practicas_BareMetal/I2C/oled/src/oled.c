#include "oled.h"
#include <string.h>

// Función para inicializar el display OLED
void oled_init(oled_t *oled, i2c_inst_t *i2c_port, uint8_t address, uint8_t width, uint8_t height) {
    oled->i2c_port = i2c_port;
    oled->address = address;
    oled->width = width;
    oled->height = height;
    memset(oled->buffer, 0, sizeof(oled->buffer));

    // Secuencia de inicialización
    oled_write_command(oled, OLED_DISPLAY_OFF);
    oled_write_command(oled, OLED_SET_DISPLAY_CLOCK_DIV);
    oled_write_command(oled, 0x80);
    oled_write_command(oled, OLED_SET_MULTIPLEX);
    oled_write_command(oled, height - 1);
    oled_write_command(oled, OLED_SET_DISPLAY_OFFSET);
    oled_write_command(oled, 0x00);
    oled_write_command(oled, OLED_SET_START_LINE | 0x00);
    oled_write_command(oled, OLED_CHARGE_PUMP);
    oled_write_command(oled, 0x14);
    oled_write_command(oled, OLED_MEMORY_MODE);
    oled_write_command(oled, 0x00);
    oled_write_command(oled, OLED_SEG_REMAP | 0x01);
    oled_write_command(oled, OLED_COM_SCAN_DEC);
    oled_write_command(oled, OLED_SET_COM_PINS);
    oled_write_command(oled, height == 32 ? 0x02 : 0x12);
    oled_write_command(oled, OLED_SET_CONTRAST);
    oled_write_command(oled, 0x7F);
    oled_write_command(oled, OLED_SET_PRECHARGE);
    oled_write_command(oled, 0xF1);
    oled_write_command(oled, OLED_SET_VCOM_DETECT);
    oled_write_command(oled, 0x40);
    oled_write_command(oled, OLED_DISPLAY_ALL_ON_RESUME);
    oled_write_command(oled, OLED_NORMAL_DISPLAY);
    oled_write_command(oled, OLED_DISPLAY_ON);

    oled_clear(oled);
    oled_display(oled);
}

// Función para enviar un comando al display
void oled_write_command(oled_t *oled, uint8_t command) {
    uint8_t buf[2] = {0x00, command}; // Co = 0, D/C = 0
    i2c_write_blocking(oled->i2c_port, oled->address, buf, 2, false);
}

// Función para enviar datos al display
void oled_write_data(oled_t *oled, const uint8_t *data, size_t size) {
    uint8_t buf[size + 1];
    buf[0] = 0x40; // Co = 0, D/C = 1

    for(size_t i = 0; i < size; i++) {
        buf[i + 1] = data[i];
    }

    i2c_write_blocking(oled->i2c_port, oled->address, buf, size + 1, false);
}

// Función para limpiar el buffer
void oled_clear(oled_t *oled) {
    memset(oled->buffer, 0, sizeof(oled->buffer));
}

// Función para dibujar un píxel en el buffer
void oled_draw_pixel(oled_t *oled, uint8_t x, uint8_t y, bool on) {
    if(x >= oled->width || y >= oled->height) return;

    if(on) {
        oled->buffer[x + (y / 8) * oled->width] |= (1 << (y % 8));
    } else {
        oled->buffer[x + (y / 8) * oled->width] &= ~(1 << (y % 8));
    }
}

// Función para enviar el buffer al display
void oled_display(oled_t *oled) {
    for(uint8_t page = 0; page < oled->height / 8; page++) {
        oled_write_command(oled, OLED_PAGE_ADDR);
        oled_write_command(oled, page);
        oled_write_command(oled, page);

        oled_write_command(oled, OLED_COLUMN_ADDR);
        oled_write_command(oled, 0);
        oled_write_command(oled, oled->width - 1);

        oled_write_data(oled, &oled->buffer[page * oled->width], oled->width);
    }
}

// Función para dibujar un carácter (implementación básica)
void oled_draw_char(oled_t *oled, uint8_t x, uint8_t y, char c) {
    // Fuente básica 5x7 (deberías implementar una mejor)
    const uint8_t font[95][5] = {
        {0x00, 0x00, 0x00, 0x00, 0x00}, // espacio
        // ... (aquí deberías incluir los datos de tu fuente)
    };

    if(c < 32 || c > 126) return;

    for(uint8_t i = 0; i < 5; i++) {
        uint8_t line = font[c - 32][i];
        for(uint8_t j = 0; j < 7; j++) {
            oled_draw_pixel(oled, x + i, y + j, line & (1 << j));
        }
    }
}

// Función para dibujar una cadena de texto
void oled_draw_string(oled_t *oled, uint8_t x, uint8_t y, const char *str) {
    uint8_t current_x = x;
    while(*str) {
        oled_draw_char(oled, current_x, y, *str++);
        current_x += 6; // Espacio entre caracteres
        if(current_x > oled->width - 5) {
            current_x = x;
            y += 8;
        }
    }
}
