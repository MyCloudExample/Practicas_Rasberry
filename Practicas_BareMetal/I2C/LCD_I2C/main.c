#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Configuración para LCD 20x4
#define LCD_I2C_ADDR 0x27  // Dirección común (0x27 o 0x3F)
#define I2C_SDA_PIN 6
#define I2C_SCL_PIN 7

// Comandos LCD
#define LCD_CLEARDISPLAY   0x01
#define LCD_RETURNHOME     0x02
#define LCD_ENTRYMODESET   0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_FUNCTIONSET    0x20
#define LCD_SETDDRAMADDR   0x80

// Flags para LCD_ENTRYMODESET
#define LCD_ENTRYLEFT      0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// Flags para LCD_DISPLAYCONTROL
#define LCD_DISPLAYON     0x04
#define LCD_DISPLAYOFF    0x00
#define LCD_CURSORON      0x02
#define LCD_CURSOROFF     0x00
#define LCD_BLINKON       0x01
#define LCD_BLINKOFF      0x00

// Flags para LCD_FUNCTIONSET
#define LCD_8BITMODE      0x10
#define LCD_4BITMODE      0x00
#define LCD_2LINE         0x08
#define LCD_1LINE         0x00
#define LCD_5x10DOTS      0x04
#define LCD_5x8DOTS       0x00

// Mascaras para los pines del PCF8574
#define LCD_RS            0x01
#define LCD_RW            0x02
#define LCD_EN            0x04
#define LCD_BACKLIGHT     0x08
#define LCD_DATA          0xF0

void lcd_send_nibble(uint8_t nibble, uint8_t rs) {
    uint8_t data = (nibble << 4) | (rs ? LCD_RS : 0) | LCD_BACKLIGHT;
    uint8_t data_en = data | LCD_EN;
    uint8_t data_no_en = data & ~LCD_EN;
    
    uint8_t buf[4] = {data_en, data_en, data_no_en, data_no_en};
    i2c_write_blocking(i2c1, LCD_I2C_ADDR, buf, 4, false);
    sleep_us(50);
}

void lcd_send_byte(uint8_t byte, uint8_t rs) {
    lcd_send_nibble(byte >> 4, rs);
    lcd_send_nibble(byte & 0x0F, rs);
}

void lcd_command(uint8_t cmd) {
    lcd_send_byte(cmd, 0);
    // Comandos de limpieza y home necesitan más tiempo
    if (cmd == LCD_CLEARDISPLAY || cmd == LCD_RETURNHOME) {
        sleep_ms(2);
    } else {
        sleep_us(100);
    }
}

void lcd_write_char(char c) {
    lcd_send_byte(c, 1);
    sleep_us(100);
}

void lcd_init() {
    // Espera inicial para que el LCD se estabilice
    sleep_ms(50);
    
    // Secuencia de inicialización en modo 4 bits (3 veces 0x03)
    lcd_send_nibble(0x03, 0);
    sleep_ms(5);
    lcd_send_nibble(0x03, 0);
    sleep_us(100);
    lcd_send_nibble(0x03, 0);
    sleep_us(100);
    lcd_send_nibble(0x02, 0); // Cambio a modo 4 bits
    
    // Configuración del LCD
    lcd_command(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);
    lcd_command(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);
    lcd_command(LCD_CLEARDISPLAY);
    lcd_command(LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT);
    
    sleep_ms(100);
}

void lcd_write_string(const char *str) {
    while (*str) {
        lcd_write_char(*str++);
    }
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
    uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54}; // Direcciones para LCD 20x4
    if (row > 3) row = 3;
    lcd_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

int main() {
    stdio_init_all();
    
    // Inicializa I2C
    i2c_init(i2c1, 100000); // 100 kHz
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    
    // Inicializa el LCD
    lcd_init();
    
    // Escribe en las 4 líneas
    lcd_write_string("Linea 1: Funciona!");
    lcd_set_cursor(0, 1);
    lcd_write_string("Linea 2: 20x4 LCD");
    lcd_set_cursor(0, 2);
    lcd_write_string("Linea 3: Pines 6-7");
    lcd_set_cursor(0, 3);
    lcd_write_string("Linea 4: RPi Pico");
    
    while (1) {
        tight_loop_contents();
    }
    
    return 0;
}