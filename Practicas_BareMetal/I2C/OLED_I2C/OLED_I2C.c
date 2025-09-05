#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "oled.h"

#define I2C_PORT i2c0
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5
#define OLED_ADDRESS 0x3C

int main() {
    stdio_init_all();
    
    // Inicializar I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);
    
    // Inicializar OLED
    oled_t oled;
    oled_init(&oled, I2C_PORT, OLED_ADDRESS, 128, 64);
    
    // Dibujar en el display
    oled_draw_string(&oled, 10, 10, "Hola, Pico!");
    oled_draw_string(&oled, 10, 30, "OLED funciona");
    oled_display(&oled);
    
    while(1) {
        tight_loop_contents();
    }
    
    return 0;
}